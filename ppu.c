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