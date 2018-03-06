#ifndef HELPER_H
#define HELPER_H


typedef struct reg {      // keeps a dynamic list of registers; 

    char * regType;
    char * physicalReg; 
    int occurrences; 
    struct reg *next; 
    int spilled; 
    int spillOffset;
    int liveRange; 
    int startLine; 
    int endLine;  
    int mark; 
    int isMapped; 
    int NextUse; 
    
}myRegisters;


typedef struct regList{

    myRegisters * head; 
    myRegisters * tail; 
    
}regList;                // linked list of registers; 


typedef struct regNode {

    char * reg; 
    struct regNode * next;

}regNode;

typedef struct regQueue{

    regNode * head; 
    regNode * tail;

} regQueue; 

// note on opcodes   C is some integer >= 0

// //*LOADI immediate       => target Register
// //*STORE     First           => target Register
// //*LOAD  First           => target Register
// //ADD        First,Second    => target Register
// //MULT       First,Second    => target Register
// //SUB        First,Second    => target Register
// //LSHIFT First,Second    => target Register
// //RSHIFT First,Second    => target Register


// DIV      First,Second    => target Register
// OUTPUT   IMMIDIATE   

//STOREAI   First               => Target, immidiate
//LOADAI    First, immidiate    => Target  

/*

                newInstr->next = ptr->next; 
                ptr->next->previous = newInstr; 
                ptr->next = newInstr; 
                newInstr->previous = ptr;

    */ 

/*

                instr * temp = ptr->previous; 
                newInstr->next = ptr; 
                ptr->previous = newInstr; 
                temp->next = newInstr;
                newInstr->previous = temp;



*/


typedef enum {LOADI, LOADAI, LOAD, STOREAI, STORE, ADD, SUB, MULT, DIV, LSHIFT, RSHIFT, OUTPUTAI, OUTPUT } OpCode; // opcodes

typedef struct Instruction {        // for storing the instruction set 
    OpCode opcode; 
    char * First; 
    char * Second; 
    char * Target;
    int maxLive; 
    int immediate;  // can be offset or just an immidiate value
    regList * maxList;
    struct Instruction * next;     // next instruction
    struct Instruction * previous; // previous instruction 
}instr;

typedef struct instructionList {    // a list  of instructions 
    instr * head; 
    instr * tail; 
}myInstrList; 

// feasible registers



regList * currentlyMapped; // for bottom up, keeps track of the registers currently mapped to a physical register


char * f1; 
char * f2;   

int instructionLength = 0; 
 
regList * spilledSet;     // holds the set of registers to be spilled

regQueue * MyRegQ; 

regList * myList;  // a list of all distinct registers including their occurrences 

regList * physical;  // a global list of virtual registers to be mapped to physical registers

myInstrList * InstructionList; // global list of instructions inputted into the program

int globalOffset = 0;  // a global variable for the spill offsets

int numPhysicalReg; 

//char ** physicalRegisters; // an array of strings for holding the physical registers;


// functions for populating list
size_t trimwhitespace(char *out, size_t len, const char *str);   // trims white spaces from a line
void popReg (regList * list, char * in);                         // populates the linkedList with new distinct registers and updates the occurrence of present once
int isAlreadPresent ( char * currentRegister, regList * myList); // check if a given register is already sorted; 
regList * parsefile(char * file, char * buffer);                 // parses throug an input file and fully populates the ll
void printList (regList * myList);


// functions for spill code
int isPhysicalReg ( char * reg );  // checks whether the given register should be mapped to a physical register
void getVirtualToPhysical (regList * all, int physicalNum); // returns a list of virtual registers that should be mapped to physical registers
int getSpillVal(); // returns the next available offset for spilling
int ispilled (myRegisters * reg); // tells whether a virtual register currently lives in memory 


//functions for dealing with instruction list 

myInstrList * LoadInstructions ( char * file, char * buffer ); // load instructions into a doubly linked list 
void popInstr ( myInstrList * InstrList, char * line );       // gets a line as input and loads the content of the line as a single instrction into a struct
void InsertToInstructionList (myInstrList * InstrList, instr * newInstr); // inserts an instruction to a doubly linked list of instructions
void printInstructionList (myInstrList * instructions);  // prints a list of instructions to standard out


// functions for liveRange top down
void printMaxLives ( instr * instructions ); 
int Spilled( char * reg );    // determines whether a given register is set to be spilled to memory 







#endif