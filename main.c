#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define HALT_OPCODE 0x19
#define NOP_OPCODE 0x18
#define EIGHT_BIT_MASK 0xff
#define SIXTEEN_BIT_MASK 0xffff
#define SIZE_OF_MEMORY 65536
void fetchNextInstruction(void);
void executeInstruction(void);

unsigned char memory[SIZE_OF_MEMORY];

// Accumulator (8 bit) used to operate on data
uint8_t ACC = 0;

// Instruction register (8 bit) used to hold the current instruction being
// executed
uint8_t IR = 0;

// Memory Address Register (16 bit) used to hold an address being used as a
// pointer, i.e., an indirect reference to data in memory
uint16_t MAR = 0;

// Program counter (16 bit) used to hold the address of the next instruction to
// execute. It is initialized to zero.
uint16_t PC = 0;

int main(int argc, char *argv[]) {
  // Execution loop. Continue fetching and executing
  // until PC points to a HALT instruction.
  //
  FILE *fp;
  char *filename;
  char ch;
  uint32_t i = 0;
  char file_name_buffer[100];
  int count;
  if (argc < 2) {
    printf("Missing mem_in.txt argument.");
    return 1;
  } else {
    // I think safe management of arguments? let me know if this is unsafe/wrong
    // - J
    count = strlen(argv[1]) + 1;
    filename = (char *)malloc(count);
    strncpy(filename, argv[1], count);
    filename[strlen(filename)] = '\0';
  }
  // Open file in read only mode
  fp = fopen(filename, "r");
  // If file opened successfully, then print the contents
  if (fp) {
    printf("File contents:\n");
    while (!feof(fp)) {
      ch = fgetc(fp);
      // fill memory array here
      // Conditions ignore blank space and new line character so we only get
      // data. Doesn't ignore 00 though, we shall see what kandel says for this.
      //
      if (isspace(ch) == 0 && ch != '\n') {
        memory[i] = ch;
        i++;
      }
      // printf("%c", ch);
    }
    fclose(fp);
  } else {
    printf("Failed to open the file: mem_in.txt.\n");
    return 1;
  }
  /*
  // Utility print out memory array
  for (i = 0; i < SIZE_OF_MEMORY; i++) {
    printf("%c", memory[i]);
  }
  */
  // memory has been fully loaded, and now we begin execution of the program.
  while (memory[PC] != HALT_OPCODE) {
    fetchNextInstruction();
    executeInstruction();
  }
}

void fetchNextInstruction() {}

void executeInstruction() {}
