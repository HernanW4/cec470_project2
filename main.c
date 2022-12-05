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
// Old PC
uint16_t old_PC = 0;

uint16_t operand = 0; // This avoids us reading out of the file.
int main(int argc, char *argv[]) {
  // Execution loop. Continue fetching and executing
  // until PC points to a HALT instruction.
  //
  FILE *fp;
  char *filename;
  char ch;
  uint32_t i = 0;
  uint32_t count = 11;

  if (argc >= 3) {
    printf("Too many arguments!\n");
    printf("Only allowed argument: mem_in_.txt");
    return 0;
  }
  if (argc < 2) {
    printf("Missing mem_in.txt argument.");
    return 0;
  } else {
    // Essentially, create enough space to safely copy the argument (1) into the
    // program. strncpy doesn't play nice with the null character, so we have to
    // add it. if someone can verify this for me that would be great!
    filename = (char *)malloc(count);
    strncpy(filename, argv[1], count);
    filename[strlen(filename)] = '\0';
  }
  // Open file in read only mode
  fp = fopen(filename, "rb");
  // If file opened successfully, then print the contents
  // This line is fire to me lmao
  if (fp && strcmp(filename, "mem_in.txt") != 1) {
    printf("File contents:\n");
    while (!feof(fp)) {
      ch = fgetc(fp);
      // fill memory array here
      // Conditions ignore blank space and new line character so we only get
      // data. Doesn't ignore 00 though, we shall see what kandel says for
      // this.
      //
      if (isspace(ch) == 0 && ch != '\n') {
        memory[i] = ch;
        i++;
      }
      // printf("%c", ch);
    }
    fclose(fp);
    free(filename);
  } else {
    free(filename);
    printf("Failed to open the file: mem_in.txt.\n");
    printf("Check Arguments.");
    return 0;
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
  // We need to add a mem_out.txt!
  FILE *mem_output_file;
  mem_output_file = fopen("mem_out.txt", "w");
  for (i = 0; i < 65536;) {
    for (int j = 0; j < 16; j++) {
      fprintf(mem_output_file, "%02x ", (memory[i]));
      i++;
    }
    fprintf(mem_output_file, "\n");
  }
  fclose(mem_output_file);
  return 0;
}

void fetchNextInstruction() {
  // So my understanding of how we keep track of the addresses of both the
  // opcode and MSB and LSB via the given global variables.
  // How we know to increment the pointer to the opcode may be need another
  // global variable here
  IR = memory[PC];
  old_PC = PC;
  PC++; // Increment PC
}

void executeInstruction() {
  int address;

  if ((IR & 0x80) == 0x80) { // Math or Logical OP!

    int dest;
    int source;
    switch (IR & 0x0c) { // Destination
    case 0x00:
      dest = memory[MAR]; // Indirect (MAR used as a pointer)
      break;
    case 0x04:
      dest = ACC; // Accumulator ACC
      break;
    case 0x08:
      dest = MAR; // Address register MAR
      break;
    case 0x0c:
      dest = memory[((memory[old_PC + 1] << 8) + memory[old_PC + 2])]; // Memory
      break;
    }

    switch(IR & 0x03){ //Source
        case 0x00: //Indirec (MAR used as pointer)
            source = memory[MAR];
            break;
        case 0x01://Accumulator ACC
            source = ACC;
            break;
        case 0x02://Constant*
                  // TODO
            break;
        case 0x03:
            //TODO
            break;
    }


  } else { // All else is either a "No Operation", "Halt" or and illegal opcode.
    if (IR == 0x18) // NOP
    {

    }

    else if (IR == 0x19) // Halt
    {

    }

    else // Illegal Op!
    {
    }
  }
}
