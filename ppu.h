#include <stdio.h>
#include "6502_emulator/cpu.h"

#define PPU_MEMORY_SIZE 0x4000
#define PPU_PAGE_SIZE 256

typedef struct {
	unsigned char *memory;
} PPU;