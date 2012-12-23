#include "nes.h"

int readFileBytes(const char *name, char **program)
{
    FILE *fl = fopen(name, "r");
    fseek(fl, 0, SEEK_END);
    int program_length = (int)ftell(fl);
	*program = malloc(program_length);
    fseek(fl, 0, SEEK_SET);
    fread(*program, 1, program_length, fl);
    fclose(fl);
    return program_length;
}

int isValidROM(char *rom, int rom_length) {	
	// .NES file format informations obtained from http://nesdev.com/NESDoc.pdf
	
	int conditions[] = {
		(rom[0] == 'N'),
		(rom[1] == 'E'),
		(rom[2] == 'S'),
		(rom[3] == 0x1A),
		(rom[9] == 0)
	};
	
	int i;
	for(i = 0; i < sizeof(conditions)/sizeof(int); i++) {
		if(!conditions[i]) return 0;
	}
	
	return 1;
}

int main(int argc, char *argv[]) {
	if(argc != 2) {
		printf("Usage: %s file.nes \n", argv[0]);
		return -1;
	}
	
	char *rom;
	int rom_length = readFileBytes(argv[1], &rom);
	
	if(!isValidROM(rom, rom_length)) {
		printf("Invalid ROM.\n");
		return -1;
	}
	
	printf("Valid ROM. Starting...\n");
	
	int rom_banks = rom[4];
	int vrom_banks = rom[5];
	
	printf("vrom_banks: %i\n", vrom_banks);
	
	char rom_mapper = (rom[6] >> 4) & rom[7]; // 4 lower bits in byte 6, 4 higher bits in byte 7
	if(rom_mapper != 0) {
		printf("Unsupported ROM mapper.\n");
		return -1;
	}
	
	CPU *cpu = malloc(sizeof(*cpu));
	initializeCPU(cpu);
	PPU *ppu = malloc(sizeof(*ppu));
	initializePPU(ppu);
	
	int i = 0, j = 0;
	int rom_index = 16;
	
	if((rom[6] & 0x4) != 0) { // has a 512-byte trainer to be loaded at 0x7000-0x71FF
		for(i = 0; i < 512; i++) {
			cpu->memory[0x7000 + i] = rom[rom_index++];
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
			cpu->memory[pgr_rom_index + (1024 * 16 * i) + j] = rom[rom_index++];
		}
	}
	
	// load rom banks into PPU's memory
	for(i = 0; i < vrom_banks; i++) {
		for(j = 0; j < 1024*8; j++) {
			ppu->memory[(i * 1024 * 8) + j] = rom[rom_index++];
		}
	}
	
	printf("rom_index: %i\n", rom_index);
	ppu_printMemory(ppu);
	
	cpu->pc = joinBytes(cpu->memory[0xFFFC], cpu->memory[0xFFFD]);
	printf("cpu->pc: %x\n", cpu->pc);
	
	for(;;) {
		step(cpu);
		// ppu_step(ppu);
	}
	
	freeCPU(cpu);
	free(cpu);
	freePPU(ppu);
	free(ppu);
	free(rom);
	
	return 0;
}