/*
This is a basict stating framework from the instruction file.

*/
#define HALT_OPCODE 0x19

void fetchNextInstruction(void);
void executeInstruction(void);

unsigned char memory[65536];
unsigned char ACC = 0;
unsigned char IR = 0;
unsigned int MAR = 0;
unsigned int PC = 0;

int main(int argc, char *argv[]){
    //Execution loop. Continue fetcvhing and executing 
    //until PC points to a HALT instruction.
    while (memory[PC] != HALT_OPCODE)
    {
        fetchNextInstruction();
        executeInstruction();
    }
    
}
