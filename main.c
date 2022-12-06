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
      // Only fills memory if the character retrieved is not a space, null
      // character, and new line character
      if (isspace(ch) == 0 && ch != '\n' && ch != '\0') {
        memory[i] = ch;
        i++;
      }
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
  for (i = 0; i < SIZE_OF_MEMORY;) {
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
  PC++; // increment here to move on from previous op
  // Here is a tip for this function, We need to ask what the current IR is so
  // if it is mathematical, logical, branch, or something with memory, then
  // after that is done, we simply have to increment the PC depending on the
  // operation's requirement for addressing, if the current IR op requires two
  // arguments, we must increment the PC by 2 so we dont use an address or
  // whatever is there as an operation command. Id say look at how we wrote
  // execution if and switch statements for reference

  operand = PC - old_PC - 1; // Move operand ptr
  PC &= 0xffff;              // Memory check
}

void executeInstruction() {
  int address;

  if ((IR & 0x80) == 0x80) { // Math or Logical OP!

    int dest;
    int source;

    switch (IR & 0x0c) { // Determine Destination
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
    default:
      break;
    }

    switch (IR & 0x03) { // Determine Source
    case 0x00:           // Indirec (MAR used as pointer)
      source = memory[MAR];
      break;
    case 0x01: // Accumulator ACC
      source = ACC;
      break;
    case 0x02: // used for constant
      if ((IR & 0x0c) == 0x8) {
        source = (memory[PC - 2] << 8) + memory[PC - 1];
      } else {
        source = memory[PC - 1];
      }
      break;
    case 0x03: // memory
      if ((IR & 0x0c) == 0x8) {
        address = ((memory[old_PC + 1] << 8) + memory[old_PC + 2]);
        source = (memory[address] << 8) + memory[address + 1];
      } else {
        source = memory[((memory[old_PC + 1] << 8) + memory[old_PC + 2])];
      }
      break;
    default:
      break;
    }

    switch (IR & 0x70) {
    case 0x00: // AND
      dest = dest & source;
      break;
    case 0x10: // OR
      dest = dest | source;
      break;
    case 0x20: // XOR
      dest = dest ^ source;
      break;
    case 0x30: // ADD
      dest = dest + source;
      break;
    case 0x40: // SUB
      dest = dest - source;
      break;
    case 0x50: // INC
      dest++;
      break;
    case 0x60: // DEC
      dest--;
      break;
    case 0x70: // NOT
      dest = !dest;
      break;
    default:
      break;
    }

    switch (IR & 0x0C) { // Isolates destination ID
    case 0x00:
      memory[MAR] = dest & 0xff; // indirect
      break;
    case 0x04:
      ACC = dest & 0xff; // Accumulator
      break;
    case 0x08:
      MAR = dest & 0xffff; // Address Register
      break;
    case 0x0C:

      memory[((memory[old_PC + 1] << 8) + memory[old_PC + 2])] =
          dest & 0xff; // Why yes this is a terrible line
      break;
    default:
      break;
    }
  } else if ((IR & 0xf0) ==
             0) { // This asks whether it is a store or a load op
                  // youre going to need another if else within this else if for
                  // defining which of the two it is, again, use
                  // maskng, ensure which bits you need to mask
  } else if ((IR & 0xF8) == 0x10) { // This took a hot minute to figure this out
                                    // LOL Asks if IR is for branches
    address = (memory[old_PC + 1] << 8) +
              memory[old_PC + 2]; // Retrieves branch address to jump to

    switch (IR & 0x07) {
    case 0: // BRA (Unconditional branch/branch always)
      PC = address;
      break;
    case 1: // BRZ (Branch if ACC = 0)
      if (ACC == 0)
        PC = address;
      break;
    case 2: // BNE (Branch if ACC != 0)
      if (ACC != 0)
        PC = address;
      break;
    case 3: // BLT (Branch if ACC < 0)
      if ((ACC & 0x80) != 0)
        PC = address;
      break;
    case 4: // BLE (Branch if ACC <= 0)
      if (((ACC & 0x80) != 0) || (ACC == 0))
        PC = address;
      break;
    case 5: // BGT (Branch if ACC > 0)
      if (((ACC & 0x80) == 0) && (ACC != 0))
        PC = address;
      break;
    case 6: // BGE (Branch if ACC >= 0)
      if ((ACC & 0x80) == 0)
        PC = address;
      break;
    default:
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
