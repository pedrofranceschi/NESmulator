#include "ppu.h"

void initializePPU(PPU *ppu) {
	ppu->memory = malloc(sizeof(unsigned char) * PPU_MEMORY_SIZE);
	if (!ppu->memory) {
		fprintf(stderr, "Failed to allocate PPU memory\n");
		exit(1);
	}
	memset(ppu->memory, 0, PPU_MEMORY_SIZE);
}

void ppu_writeMemory(PPU *ppu, char *buffer, int start, int offset) {
	if (!ppu || !buffer || start < 0 || offset < 0 || start + offset > PPU_MEMORY_SIZE)
		return;
	for(int i = 0; i < offset; i++)
		ppu->memory[start + i] = buffer[i];
}

void ppu_readMemory(PPU *ppu, char *buffer, int start, int offset) {
	if (!ppu || !buffer || start < 0 || offset < 0 || start + offset > PPU_MEMORY_SIZE)
		return;
	int i;
	for(i = 0; i < offset; i++)
		buffer[i] = ppu->memory[start + i];
}

void ppu_printMemory(PPU *ppu) {
	int i, j;
	for(i = 0; i < PPU_MEMORY_SIZE/PPU_PAGE_SIZE; i++) {
		printf("=== Page %i\n", i);
		for(j = 0;  j < PPU_PAGE_SIZE; j++)
			printf("%x ", ppu->memory[(i * PPU_PAGE_SIZE) + j]);
		printf("\n");
	}
}

void freePPU(PPU *ppu) {
	free(ppu->memory);
}

void ppu_step(PPU *ppu, CPU *cpu) {
	
}
