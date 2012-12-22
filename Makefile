FILES += nes.c
FILES += ppu.c
FILES += 6502_emulator/cpu.c
EXECUTABLE = NESmulator

all:
	gcc $(FILES) $(LIBRARIES) -o $(EXECUTABLE)

run: all
	./$(EXECUTABLE) super_mario_bros.nes
