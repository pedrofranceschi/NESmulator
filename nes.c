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


int main(int argc, char *argv[]) {
	if(argc != 2) {
		printf("Usage: %s file.nes \n", argv[0]);
		return -1;
	}
	
	char *program;
	int program_length = readFileBytes(argv[1], &program);
	
	printf("ok!\n");
	
	free(program);
	
	return 0;
}