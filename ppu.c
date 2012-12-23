#include "ppu.h"

void initializePPU(PPU *ppu) {
	ppu->memory = malloc(sizeof(unsigned char) * PPU_MEMORY_SIZE);
}

void ppu_writeMemory(PPU *ppu, char *buffer, int start, int offset) {
	int i;
	for(i = 0; i < offset; i++) {
		ppu->memory[start + i] = buffer[i];
	}
}

void ppu_readMemory(PPU *ppu, char *buffer, int start, int offset) {
	int i;
	for(i = 0; i < offset; i++) {
		buffer[i] = ppu->memory[start + i];
	}
}

void ppu_printMemory(PPU *ppu) {
	int i, j;

	for(i = 0; i < PPU_MEMORY_SIZE/PPU_PAGE_SIZE; i++) {
		printf("=== Page %i\n", i);
		for(j = 0;  j < PPU_PAGE_SIZE; j++) {
			printf("%x ", ppu->memory[(i * PPU_PAGE_SIZE) + j]);
		}
		printf("\n");
	}
}

void freePPU(PPU *ppu) {
	free(ppu->memory);
}