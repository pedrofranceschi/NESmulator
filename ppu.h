#include <stdio.h>

#define PPU_MEMORY_SIZE 0x4000

typedef struct {
	unsigned char *memory;
} PPU;