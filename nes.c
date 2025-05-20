#include "nes.h"

int readFileBytes(const char *name, char **program)
{
    FILE *fl = fopen(name, "r");
    if (!fl) {
		fprintf(stderr, "Failed to read file\n");
        return -1;
    }
    fseek(fl, 0, SEEK_END);
    int program_length = (int)ftell(fl);
    *program = malloc(program_length);
    if (!program || !*program) {
        fprintf(stderr, "Program is NULL or failed to allocate program memory");
        fclose(fl);
        return -1;
    }
    fseek(fl, 0, SEEK_SET);
    fread(*program, 1, program_length, fl);
    fclose(fl);
    return program_length;
}

int isValidROM(char *rom, int rom_length) {
	// .NES file format informations obtained from http://nesdev.com/NESDoc.pdf
    if (rom_length < 10) return 0;
	int conditions[] = {
		(rom[0] == 'N'),
		(rom[1] == 'E'),
		(rom[2] == 'S'),
		(rom[3] == 0x1A),
		(rom[9] == 0)
	};
	int i;
	for(i = 0; i < sizeof(conditions)/sizeof(int); i++)
		if(!conditions[i]) return 0;
	return 1;
}

void mirrorMemoryArray(unsigned char **memory, int start, int end, int mirror_start) {
	end += 1;
    if (start > end) return;
	size_t length = end - start;
    int i;
	for(i = 0; i < length && mirror_start + i < length; i++)
		memory[mirror_start + i] = memory[start + i];
}

int main(int argc, char *argv[]) {
	if(argc != 2) {
		printf("Usage: %s file.nes \n", argv[0]);
		return -1;
	}

	char *rom;
	int rom_length = readFileBytes(argv[1], &rom);

	if(rom_length < 0 || !rom) {
		printf("Failed to read ROM file.\n");
		return -1;
	}

	if(!isValidROM(rom, rom_length)) {
		printf("Invalid ROM.\n");
		free(rom);
		return -1;
	}

	printf("Valid ROM. Starting...\n");

	int rom_banks = rom[4];
	int vrom_banks = rom[5];

	printf("vrom_banks: %i\n", vrom_banks);

	char rom_mapper = (rom[6] >> 4) & rom[7]; // 4 lower bits in byte 6, 4 higher bits in byte 7
	if(rom_mapper) {
		printf("Unsupported ROM mapper.\n");
		free(rom);
		return -1;
	}

	CPU *cpu = malloc(sizeof(*cpu));
	if (!cpu) {
		printf("Failed to allocate CPU memory.\n");
		free(rom);
		return -1;
	}
	initializeCPU(cpu);

	PPU *ppu = malloc(sizeof(*ppu));
	if (!ppu) {
		printf("Failed to allocate PPU memory.\n");
		freeCPU(cpu);
		free(cpu);
		free(rom);
		return -1;
	}
	initializePPU(ppu);

	int i = 0, j = 0;
	int rom_index = 16;

	if((rom[6] & 0x4) != 0) { // has a 512-byte trainer to be loaded at 0x7000-0x71FF
		for(i = 0; i < 512; i++) {
			*cpu->memory[0x7000 + i] = rom[rom_index++];
		}
	}

	int pgr_rom_index = 0x8000;

	if(rom_banks == 1) {
		// loads the first rom bank in the end of the RAM (upper bank),
		// according to http://fms.komkon.org/EMUL8/NES.html
		pgr_rom_index += 1024*16;
	}

	// load rom banks into CPU's memory
	for(i = 0; i < rom_banks; i++) {
		for(j = 0; j < 1024*16; j++) { // 16kb per rom bank
			*cpu->memory[pgr_rom_index + (1024 * 16 * i) + j] = rom[rom_index++];
		}
	}

	// load rom banks into PPU's memory
	for(i = 0; i < vrom_banks; i++) {
		for(j = 0; j < 1024*8; j++) {
			ppu->memory[(i * 1024 * 8) + j] = rom[rom_index++];
		}
	}

	printf("rom_index: %i\n", rom_index);
	// ppu_printMemory(ppu);

	cpu->pc = joinBytes(*cpu->memory[0xFFFC], *cpu->memory[0xFFFD]);
	printf("cpu->pc: %x\n", cpu->pc);

	// *cpu->memory[0x2000] = 0x

	for(;;) {
		step(cpu);

		// handle memory mirrorings described at
		// http://wiki.nesdev.com/w/index.php/Mirroring#Memory_Mirroring

		mirrorMemoryArray(cpu->memory, 0x0000, 0x07FF, 0x0800);
		mirrorMemoryArray(cpu->memory, 0x0000, 0x07FF, 0x1000);
		mirrorMemoryArray(cpu->memory, 0x0000, 0x07FF, 0x1800);

		for(i = 0; i < 0x2000; i += 8) {
			if(i == 0) continue;
			mirrorMemoryArray(cpu->memory, 0x2000, 0x2007, 0x2000 + i);
		}

		// handle PPU memory mirrorings described at
		// http://wiki.nesdev.com/w/index.php/PPU_memory_map

		// 0x3f20
		// 0x3f40
		// 0x3f80
		// 0x3fc0
		//
		// mirrorMemoryArray(ppu->memory, 0x3f00, 0x3f1f, 0x3f20);
		// mirrorMemoryArray(ppu->memory, 0x3f00, 0x3f1f);
		// mirrorMemoryArray(ppu->memory, 0x3f00, 0x3f1f);
		// mirrorMemoryArray(ppu->memory, 0x3f00, 0x3f1f);

		printMemory(cpu);
		// ppu_printMemory(ppu);

		ppu_step(ppu, cpu);
		// ppu_step(ppu);
	}

	freeCPU(cpu);
	free(cpu);
	freePPU(ppu);
	free(ppu);
	free(rom);

	return 0;
}
