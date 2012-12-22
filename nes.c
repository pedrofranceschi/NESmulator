#include <stdio.h>
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
	
	free(rom);
	return 0;
}