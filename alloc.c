#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h> 
#include <ctype.h>
#include "helper.h"
#include "mergesort.h"
#include "CascadeSorter.h"


size_t trimwhitespace(char *out, size_t len, const char *str){

  if(len == 0)
    return 0;

  const char *end;
  size_t out_size;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
  {
    *out = 0;
    return 1;
  }

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;
  end++;

  // Set output size to minimum of trimmed string length and buffer size minus 1
  out_size = (end - str) < len-1 ? (end - str) : len-1;

  // Copy trimmed string and add null terminator
  memcpy(out, str, out_size);
  out[out_size] = 0;

  return out_size;
}


void popReg (regList * list, char * in){

    char * ptr = in; 

    if( strstr(ptr,"store") || strstr(ptr,"rshift") ){

        ptr = strstr(ptr, "r"); 

        ptr++; 
    }                  // if this operations are here, we ignore the first r

    char * seek; 

    while( (seek = strstr(ptr, "r")) != NULL ){

        ptr = seek;

        char * str = (char*)malloc(sizeof(char) * 5); 

        sscanf(ptr, "%5s", str);


        if( !isdigit(str[3]) ){
            sscanf(str,"%3s",str);  
        }       

        if( !isdigit(str[2]) ){
            sscanf(str,"%2s",str);  
        }

       // printf("%s\n", str );

        // check if the register is already present in the list
        if( isAlreadPresent(str, list) ){
            free(str); 
            ptr++; 
            continue; 
        }


        else{

            myRegisters * newReg = (myRegisters*)malloc(sizeof(myRegisters));
            newReg->regType = (char*)malloc(sizeof(char)*5); 
            newReg->occurrences = 1; 
            newReg->spillOffset = 0; 
            newReg->spilled = 0; 
            newReg->liveRange = 0; 
            newReg->endLine = 0;
            newReg->startLine = 0;  
            newReg->mark = 0; 
            newReg->isMapped = 0; 
            newReg->NextUse = 0;
            sscanf(str, "%s", newReg->regType); 
            free(str); 
            //insert the new node into the linked list
            if(list->head == NULL){   // at the head
                list->head = newReg; 
                list->tail = newReg; 
                list->tail->next = NULL; 
            }

            else{

                list->tail->next = newReg;    // at the tail 
                list->tail = newReg;
                list->tail->next = NULL; 
            }


        }

            ptr++;  // move the pointer 
    }

}



int isAlreadPresent ( char * currentRegister, regList * List){  // check if the current register is already present in the list 

    myRegisters * ptr = List->head; 

    while(ptr != NULL){

        if ( strcmp( currentRegister, ptr->regType ) == 0 ) {

            ptr->occurrences++; 

            return 1;
        }

        ptr = ptr->next; 
    }

    return 0; 
}



regList * parsefile(char * file, char * buffer){


    FILE * f = fopen(file, "r"); 

    if(!f){

        printf("Error reading file\n");
        exit(1); 
    }


    regList * returnList = (regList*)malloc(sizeof(regList));   // linked list to hold registers


    while ( fgets(buffer, 100, f) != NULL ){

        if( strstr(buffer, "//") != NULL ){

            continue;    // invalid line
        }     

        char newBuffer[100]; 

       int trim = trimwhitespace(newBuffer, strlen(buffer), buffer); 

        if (trim == 1){  // all spaces
        continue; 
       }        

        popReg(returnList, newBuffer);  // populate the list with the registers in that line
    }
    fclose(f); 
    return returnList; 


}


void printList (regList * List){

    myRegisters * ptr = List->head; 

    int j = 0; 

    while( ptr != NULL){

        printf(" Virtual Register : %s \n",  ptr->regType);
        j++; 
        ptr = ptr->next; 
    }
}



// gets the next offset of the register to be spiilled

int getSpillVal(){

    globalOffset -= 5; 

    return globalOffset; 
}

// gets the name of a register and returns its offset in memory 

int getOffset (char * reg){

    myRegisters * ptr = myList->head; 

    while( ptr != NULL ){

        if( strcmp(reg, ptr->regType) == 0 ){

            return ptr->spillOffset; 
        }

        ptr = ptr->next; 
    }


    return 0; 

}



// checks if a register is spilled or not. 

int isSpilled (char * reg){

    myRegisters * ptr = myList->head; 

    while( ptr != NULL ){

        if( strcmp(reg, ptr->regType) == 0 ){

            return ptr->spilled; 
        }

        ptr = ptr->next; 
    }

    return 0; 
}



// set spilled offset/ spilled val of a register

int setOffset ( char * reg ){

    myRegisters * ptr = myList->head; 

    while ( ptr != NULL ){

        if( strcmp(reg,ptr->regType) == 0 ){

            ptr->spilled = 1; 
            ptr->spillOffset = getSpillVal(); 
            return ptr->spillOffset; 
        }

        ptr = ptr->next; 
    }

    return 0; 
}



// checks if a register is the target Register of an instruction 

//int contains ()




// populates a list with the virtual registers to be mapped to physical register
// input: my list of registers and the amount of physical registers We have
//output: A list containing all the registers to be mapped to physical registers

void getVirtualToPhysical (regList * all, int physicalNum){

    physical = (regList*)malloc(sizeof(regList)); 

    myRegisters * ptr = all->head;

    myRegisters * newReg = (myRegisters*)malloc(sizeof(myRegisters)); 
    memcpy(newReg, ptr, sizeof(myRegisters)); 
    physical->head = newReg; 
    physical->tail = newReg; 
    physical->tail->next = NULL; 
    ptr = ptr->next; 

    while( physicalNum > 1  && ptr != NULL){

        myRegisters * newReg = (myRegisters*)malloc(sizeof(myRegisters)); 
        memcpy(newReg, ptr, sizeof(myRegisters));         
        physical->tail->next = newReg; 
        physical->tail = newReg;
        physical->tail->next = NULL;  

        ptr = ptr->next; 

        physicalNum--; 
    } 

}



// checks if a register is  to be mapped to a physical register

int isPhysicalReg ( char * reg ){

    myRegisters * ptr = physical->head; 

    while ( ptr != NULL ){

        if ( strcmp(reg , ptr->regType) == 0 ){

            return 1; 
        }

        ptr = ptr->next; 
    }

    return 0; 
}


// takes in a virtual register and returns its physical register mapping

char * getPhysicalReg ( char * reg ){

    myRegisters * ptr = physical->head; 

    while ( ptr != NULL ){

        if( strcmp(reg, ptr->regType) == 0 ){

            return ptr->physicalReg; 
        }

        ptr = ptr->next; 
    }

    return NULL;   // could not find the register
}

/*
char * getF1 (){

    return f1; 
}

char * getF2(){

    return f2; 
}

*/

// my two feasible registers should be numPhysicalReg+1, numPhysicalReg+2 respectively. 


// note on opcodes   C is some integer >= 0

// LOADI    immediate       => target Register
// STOREAI  First           => target Register
// LOAD     First           => target Register
// ADD      First,Second    => target Register
// MULT     First,Second    => target Register
// SUB      First,Second    => target Register
// LSHIFT   First,Second    => target Register
// RSHIFT   First,Second    => target Register
// DIV      First,Second    => target Register
// OUTPUT   IMMIDIATE

void InsertToInstructionList (myInstrList * InstrList, instr * newInstr){

    if ( InstrList->head == NULL ){  // insert at the head
        newInstr->previous = NULL; 
        InstrList->head = newInstr; 
        InstrList->tail = newInstr; 
        InstrList->tail->next = NULL; 
    }

    else{

        newInstr->previous = InstrList->tail;     // insert at the tail 
        InstrList->tail->next = newInstr;
        InstrList->tail = newInstr; 
        InstrList->tail->next = NULL;  
    }

}   


void popInstr ( myInstrList * InstrList, char * line ){
    char * ptr = line; 
    char * getReg = (char*)malloc(sizeof(char)*5); // the middle man

    if ( strstr(line, "load") != NULL && strstr(line, "I") == NULL ){   //this is a load operation 
        //printf("Load\n");
        instr * newInstr = (instr*)malloc(sizeof(instr)); 
        newInstr->opcode = LOAD;  // load operation

        ptr = strstr(ptr, "d");  // the character d marks the end of the load operation
        ptr++; 

        ptr = strstr(ptr, "r");  // seek for the first register
        sscanf(ptr, "%4s", getReg);


        if( !isdigit(getReg[3]) )
            sscanf(getReg, "%3s", getReg);            

        if( !isdigit(getReg[2]) )
            sscanf(getReg, "%2s", getReg);      // get the first register 

        newInstr->First = (char*)malloc(sizeof(char)*5); 
        sscanf(getReg,"%s", newInstr->First);
        ptr++;

        bzero(getReg, strlen(getReg));  

        ptr = strstr(ptr,"r");    // seek for the second register
        sscanf(ptr,"%4s", getReg); 

        if( !isdigit(getReg[3]) )
            sscanf(getReg, "%3s", getReg);        

        if( !isdigit(getReg[2]) )
            sscanf(getReg, "%2s", getReg); 

        newInstr->Target = (char*)malloc(sizeof(char)*5); 
        sscanf(getReg, "%s", newInstr->Target);
        free(getReg); 
        InsertToInstructionList(InstrList, newInstr);  // insert it to the instruction list  

    }

    else if ( strstr(line, "loadI") != NULL ) {   // this is a load immediate operation
       // printf("LoadI\n");
        instr *newInstr = (instr*)malloc(sizeof(instr)); 
        newInstr->opcode = LOADI; 

        ptr = strstr(ptr, "I"); 
        ptr++; 

        while(isspace(*ptr))
            ptr++;             // ignore all the spaces; 

        int immediate; 

        sscanf(ptr, "%d", &immediate);  // get the immediate value

        newInstr->immediate = immediate; 

        ptr = strstr(ptr, "r");     // get to the target register
        sscanf(ptr, "%4s", getReg); 

        if( !isdigit(getReg[3]) )
            sscanf(getReg, "%3s", getReg);         

        if( !isdigit(getReg[2]) )
            sscanf(getReg, "%2s", getReg); 

        newInstr->Target = (char*)malloc(sizeof(char)*5); 
        sscanf( getReg, "%s", newInstr->Target); 
        free(getReg); 
        InsertToInstructionList(InstrList, newInstr); 

    }


    else if ( strstr(line, "add") != NULL ){   // this is adding two registers and putting the result in a target register
       // printf("add\n");
        instr * newInstr =  (instr*)malloc(sizeof(instr)); 
        newInstr->First  =  (char*)malloc(sizeof(char)*5); 
        newInstr->Second =  (char*)malloc(sizeof(char)*5); 
        newInstr->Target =  (char*)malloc(sizeof(char)*5);   
        newInstr->opcode = ADD; 

        ptr = strstr(ptr, "d"); 
        ptr++;

        char * seek; 
        int i = 1; 

        while ( (seek = strstr(ptr, "r")) !=NULL ) {

            bzero(getReg,strlen(getReg)); 
            ptr = seek; 

            sscanf(ptr, "%4s", getReg); 

            if ( !isdigit(getReg[3]) ){
                sscanf(getReg,"%3s", getReg); 
            }            

            if ( !isdigit(getReg[2]) ){
                sscanf(getReg,"%2s", getReg); 
            }

            if ( i == 1){
                sscanf(getReg, "%s", newInstr->First); 
                ptr++; 
                i++; 
            }

            else if (i == 2){
                sscanf(getReg, "%s", newInstr->Second); 
                ptr++; 
                i++; 
            }

            else {
                sscanf(getReg, "%s" , newInstr->Target); 
                break; 
            }
        }

        free(getReg); 
        InsertToInstructionList(InstrList, newInstr); 
    }


    else if ( strstr(line, "sub") != NULL ){   // this is subtracting two registers and putting the result in a target register
       // printf("sub\n");
        instr * newInstr =  (instr*)malloc(sizeof(instr)); 
        newInstr->First  =  (char*)malloc(sizeof(char)*5); 
        newInstr->Second =  (char*)malloc(sizeof(char)*5); 
        newInstr->Target =  (char*)malloc(sizeof(char)*5);   
        newInstr->opcode = SUB; 

        ptr = strstr(ptr, "b"); 
        ptr++;

        char * seek; 
        int i = 1; 

        while ( (seek = strstr(ptr, "r")) !=NULL ){
            bzero(getReg,strlen(getReg)); 
            ptr = seek; 

            sscanf(ptr, "%4s", getReg); 


            if( !isdigit(getReg[3]) )
                sscanf(getReg,"%3s", getReg); 


            if( !isdigit(getReg[2]) )
                sscanf(getReg,"%2s", getReg); 

            if ( i == 1){
                sscanf(getReg, "%s", newInstr->First);
                ptr++;  
                i++; 
            }

            else if (i == 2){
                sscanf(getReg, "%s", newInstr->Second); 
                ptr++; 
                i++; 
            }

            else {
                sscanf(getReg, "%s" , newInstr->Target); 
                break; 
            }
        }

        free(getReg); 
        InsertToInstructionList(InstrList, newInstr); 
    }

    else if ( strstr(line, "mult") != NULL ){   // this is subtracting two registers and putting the result in a target register
       // printf("mult\n");
        instr * newInstr =  (instr*)malloc(sizeof(instr)); 
        newInstr->First  =  (char*)malloc(sizeof(char)*5); 
        newInstr->Second =  (char*)malloc(sizeof(char)*5); 
        newInstr->Target =  (char*)malloc(sizeof(char)*5);   
        newInstr->opcode = MULT; 

        ptr = strstr(ptr, "t"); 
        ptr++;

        char * seek; 
        int i = 1; 

        while ( (seek = strstr(ptr, "r")) !=NULL ){
            bzero(getReg,strlen(getReg)); 
            ptr = seek; 

            sscanf(ptr, "%4s", getReg); 

            if( !isdigit(getReg[3]) )
                sscanf(getReg,"%3s", getReg);             

            if( !isdigit(getReg[2]) )
                sscanf(getReg,"%2s", getReg); 

            if ( i == 1){
                sscanf(getReg, "%s", newInstr->First);
                ptr++;  
                i++; 
            }

            else if (i == 2){
                sscanf(getReg, "%s", newInstr->Second); 
                ptr++; 
                i++; 
            }

            else {
                sscanf(getReg, "%s" , newInstr->Target); 
                break; 
            }
        }

        free(getReg); 
        InsertToInstructionList(InstrList, newInstr); 
    }

    else if ( strstr(line, "lshift") != NULL ){   // this is subtracting two registers and putting the result in a target register

        instr * newInstr =  (instr*)malloc(sizeof(instr)); 
        newInstr->First  =  (char*)malloc(sizeof(char)*5); 
        newInstr->Second =  (char*)malloc(sizeof(char)*5); 
        newInstr->Target =  (char*)malloc(sizeof(char)*5);   
        newInstr->opcode = LSHIFT; 

        ptr = strstr(ptr, "t"); 
        ptr++;

        char * seek; 
        int i = 1; 

        while ( (seek = strstr(ptr, "r")) !=NULL ){
            bzero(getReg,strlen(getReg)); 
            ptr = seek; 

            sscanf(ptr, "%4s", getReg); 

            if( !isdigit(getReg[3]) )
                sscanf(getReg,"%3s", getReg); 


            if( !isdigit(getReg[2]) )
                sscanf(getReg,"%2s", getReg); 

            if ( i == 1){
                sscanf(getReg, "%s", newInstr->First);
                ptr++;  
                i++; 
            }

            else if (i == 2){
                sscanf(getReg, "%s", newInstr->Second); 
                ptr++; 
                i++; 
            }

            else {
                sscanf(getReg, "%s" , newInstr->Target); 
                break; 
            }
        }

        free(getReg); 
        InsertToInstructionList(InstrList, newInstr); 
    }


    else if ( strstr(line, "rshift") != NULL ){   // this is subtracting two registers and putting the result in a target register

        instr * newInstr =  (instr*)malloc(sizeof(instr)); 
        newInstr->First  =  (char*)malloc(sizeof(char)*5); 
        newInstr->Second =  (char*)malloc(sizeof(char)*5); 
        newInstr->Target =  (char*)malloc(sizeof(char)*5);   
        newInstr->opcode = RSHIFT; 

        ptr = strstr(ptr, "t"); 
        ptr++;

        char * seek; 
        int i = 1; 

        while ( (seek = strstr(ptr, "r")) !=NULL ){
            bzero(getReg,strlen(getReg)); 
            ptr = seek; 

            sscanf(ptr, "%4s", getReg); 

            if( !isdigit(getReg[3]) )
                sscanf(getReg,"%3s", getReg); 


            if( !isdigit(getReg[2]) )
                sscanf(getReg,"%2s", getReg); 

            if ( i == 1){
                sscanf(getReg, "%s", newInstr->First);
                ptr++;  
                i++; 
            }

            else if (i == 2){
                sscanf(getReg, "%s", newInstr->Second); 
                ptr++; 
                i++; 
            }

            else {
                sscanf(getReg, "%s" , newInstr->Target); 
                break; 
            }
        }

        free(getReg); 
        InsertToInstructionList(InstrList, newInstr); 
    }


    else if ( strstr(line, "store") != NULL ){
        //printf("store\n");
        instr * newInstr =  (instr*)malloc(sizeof(instr)); 
        newInstr->First  =  (char*)malloc(sizeof(char)*5); 
        newInstr->Target =  (char*)malloc(sizeof(char)*5); 
        newInstr->opcode =  STORE; 

        ptr = strstr(ptr, "e"); 
        ptr++; 

        ptr = strstr(ptr, "r");  // seek for the first register

        sscanf(ptr, "%4s", getReg);  // get first register

        if ( !isdigit(getReg[3]) )
            sscanf(getReg, "%3s", getReg);         

        if ( !isdigit(getReg[2]) )
            sscanf(getReg, "%2s", getReg); 

        sscanf(getReg, "%s", newInstr->First); 

        bzero(getReg,strlen(getReg)); 

        ptr++; 

        ptr = strstr(ptr, "r"); // seek for the second register

        sscanf(ptr, "%4s", getReg);  // get target register

        if ( !isdigit(getReg[3]) )
            sscanf(getReg, "%3s", getReg);         

        if ( !isdigit(getReg[2]) )
            sscanf(getReg, "%2s", getReg); 

        sscanf(getReg, "%s", newInstr->Target);  // get target register
        free(getReg); 

        InsertToInstructionList(InstrList, newInstr); 
    }

    else if (strstr(line, "output") != NULL){
      //  printf("output\n");
        instr * newInstr = (instr*)malloc(sizeof(instr)); 
        newInstr->opcode = OUTPUT; 

        int immediate; 

        sscanf(ptr, "%*[^0123556789]%d" , &immediate);

        newInstr->immediate = immediate; 
        free(getReg); 

        InsertToInstructionList(InstrList, newInstr); 
    }


    else{

        printf("Illegal start of instruction in input file\n");
        exit(1); 
    }


}


myInstrList * LoadInstructions ( char * file, char * buffer ){

    FILE * f = fopen(file, "r"); 

    if (!f){

        printf("Error opening input file\n");

        exit(1); 
    }

    myInstrList * myInstr = (myInstrList*)malloc(sizeof(myInstrList)); 

    if (!myInstr){
        printf("error allocating memory for instruction list\n");
        exit(1); 
    }


    while ( fgets(buffer, 100, f) != NULL ){

        if( strstr(buffer, "//") != NULL ){

            continue;    // invalid line
        }     

        char newBuffer[100]; 

       int trim = trimwhitespace(newBuffer, strlen(buffer), buffer); 

       if (trim == 1){  // all spaces
        continue; 
       }
        
        popInstr(myInstr, newBuffer);  // populate the list with the registers in that line
    }    

   fclose(f); 

   return myInstr; 

}


void printInstruction (instr * instruction){

    switch (instruction->opcode){

        case LOADI:
                printf("loadI   %d      =>   %s\n", instruction->immediate, instruction->Target);
                fflush(stdout); 
                break; 

        case STORE:
                printf("store   %s      =>   %s\n", instruction->First, instruction->Target );
                fflush(stdout);
                break; 

        case LOAD:
                printf("load    %s      =>   %s\n", instruction->First, instruction->Target );   
                fflush(stdout);
                break; 

        case ADD:
                printf("add     %s, %s  =>   %s\n", instruction->First, instruction->Second, instruction->Target );  
                fflush(stdout);   
                break; 

        case MULT:
                printf("mult    %s, %s  =>   %s\n", instruction->First, instruction->Second, instruction->Target );
                fflush(stdout); 
                break; 

        case SUB:
                printf("sub     %s, %s  =>   %s\n", instruction->First, instruction->Second, instruction->Target );
                fflush(stdout);
                break; 

        case LSHIFT:
                printf("lshift  %s, %s  =>   %s\n", instruction->First, instruction->Second, instruction->Target ); 
                fflush(stdout);
                break;

        case RSHIFT:
                printf("rshift  %s, %s  =>   %s\n", instruction->First, instruction->Second, instruction->Target ); 
                fflush(stdout);
                break;  

        case LOADAI: 
                printf("loadAI  %s, %d  =>   %s\n", instruction->First, instruction->immediate, instruction->Target);
                fflush(stdout);
                break; 

        case STOREAI: 
                printf("storeAI %s      =>   %s, %d\n", instruction->First, instruction->Target, instruction->immediate);
                fflush(stdout);
                break;

        case OUTPUT:
                printf("output   %d\n", instruction->immediate );
                fflush(stdout);
                break; 

        default:
                printf("Illegal instruction\n");
                //printf("%s\n", instruction->Target);
                exit(1); 
    }
}


void printInstructionList (myInstrList * instructions){
  
  instr * ptr =  instructions->head; 

  if( ptr->opcode != LOADI )
    ptr = ptr->next;  

  while ( ptr != NULL ){

    printInstruction(ptr); 

    ptr = ptr->next; 
  }

}


// simple top down register allocator. Spills based on occurrences
// no consideration of maxlive



// this gives a direct mapping of virtual registers to physical registers. 

void map (){

    int i = 1; 

    myRegisters * ptr = physical->head; 

    while ( ptr != NULL ){

        ptr->physicalReg = (char*)malloc(sizeof(char)*5); 
        sprintf(ptr->physicalReg, "%s%d", "r", i); 
        ptr = ptr->next; 
        i++; 
    }

}




// simple top down allocator

 void SimpleTopDown (myInstrList *instructions){

    instr * ptr = instructions->head->next; 

    if ( strcmp(ptr->Target, "r0") == 0 ){

        ptr = ptr->next;                        // ignore the first register if it is a load to r0 
    }

    //traverse the whole list and insert instructions
    while ( ptr != NULL ){

        if ( ptr->opcode == LOADI ){

            if ( isPhysicalReg(ptr->Target)  ){   // check if it is a physical register

                char * newReg = getPhysicalReg(ptr->Target); 
                bzero(ptr->Target, strlen(ptr->Target)); 
                sscanf(newReg, "%s", ptr->Target ); 
                ptr = ptr->next; 
                continue; 
            }
                // else it is a virtual register (put in a feasible register and insert spill code)
            else{

                int offset = setOffset(ptr->Target);  // spill to this offset
                bzero(ptr->Target, strlen(ptr->Target)); 
                sscanf(f1, "%s", ptr->Target); 

                // generate spill code

                instr * newInstr = (instr*)malloc(sizeof(instr)); 
                newInstr->opcode = STOREAI; 
                newInstr->immediate = offset; 
                newInstr->First = (char*)malloc(sizeof(char)*5);
                newInstr->Target = (char*)malloc(sizeof(char)*5);  
                sscanf(f1, "%s", newInstr->First); // set first register
                sscanf("r0", "%s", newInstr->Target); 

                // insert spill code into the list

                newInstr->next = ptr->next; 
                ptr->next->previous = newInstr; 
                ptr->next = newInstr; 
                newInstr->previous = ptr; 

                // advance ptr two pointers forward

                ptr = ptr->next->next; 
                continue; 

            }

        }


        else if ( ptr->opcode == STORE ){

            if( isPhysicalReg(ptr->First) && isPhysicalReg(ptr->Target) ){  // are both mapped to physical registers

                char * newFirst = getPhysicalReg(ptr->First); 
                char * newTarget = getPhysicalReg(ptr->Target); 
                bzero(ptr->Target, strlen(ptr->Target)); 
                bzero(ptr->First, strlen(ptr->First)); 
                sscanf(newFirst, "%s", ptr->First); 
                sscanf(newTarget, "%s", ptr->Target); 

                ptr = ptr->next; 
                continue; 
            }


            else if (isPhysicalReg(ptr->First) && !isPhysicalReg(ptr->Target)){ // target register is the virtual register

                    int offset = getOffset(ptr->Target);  // get offset

                    // insert a load instruction

                    instr * newInstr = (instr*)malloc(sizeof(instr)); 
                    newInstr->opcode = LOADAI; 
                    newInstr->First  = (char*)malloc(sizeof(char)*5); 
                    newInstr->Target = (char*)malloc(sizeof(char)*5); 
                    newInstr->immediate = offset; 

                    // set registers
                    sscanf("r0", "%s", newInstr->First); 
                    sscanf(f2, "%s", newInstr->Target); 

                    // insert it before ptr

                    instr * temp1 = ptr->previous; 
                    newInstr->next = ptr; 
                    temp1->next = newInstr; 
                    ptr->previous = newInstr; 
                    newInstr->previous = temp1; 

                    // update instructions of ptr

                    // get physical register of first and update
                    char * first = getPhysicalReg(ptr->First); 
                    bzero(ptr->First, strlen(ptr->First)); 
                    bzero(ptr->Target, strlen(ptr->Target)); 
                    sscanf(first, "%s", ptr->First); 
                    sscanf(f2,"%s", ptr->Target);
                    ptr = ptr->next;             //go on to the next instruction 
                    continue;  
            }


            else if ( !isPhysicalReg(ptr->First) && isPhysicalReg(ptr->Target)  ){  // the target register is a physical register

                // generate load instruction for virtual register and insert before ptr
                // get offset of the register
                int offset = getOffset(ptr->First); 

                //insert instruction

                instr * newInstr = (instr*)malloc(sizeof(instr)); 
                newInstr->opcode = LOADAI; 
                newInstr->First  = (char*)malloc(sizeof(char)*5); 
                newInstr->Target = (char*)malloc(sizeof(char)*5); 
                newInstr->immediate = offset; 

                //set registers

                sscanf("r0", "%s", newInstr->First); 
                sscanf(f2, "%s", newInstr->Target); 

                // insert it before ptr

                instr * temp1 = ptr->previous;
                newInstr->next = ptr; 
                temp1->next = newInstr;
                ptr->previous =  newInstr; 
                newInstr->previous = temp1; 

                // update instructions of ptr
                // get physical reg of target and update
                char * target = getPhysicalReg(ptr->Target); 
                bzero(ptr->First, strlen(ptr->First));
                bzero(ptr->Target, strlen(ptr->Target));
                sscanf(target, "%s", ptr->Target); 
                sscanf(f2, "%s", ptr->First); 
                ptr = ptr->next; 
                continue;   
            }


            else{  // both registers are virtual registers

                // generate two load instructions and insert them sequentially

                // get offset of the first register
                int offset1 = getOffset(ptr->First);

                // generate first loadAI instruction

                instr * newInstr1 = (instr*)malloc(sizeof(instr)); 
                newInstr1->opcode = LOADAI; 
                newInstr1->First  = (char*)malloc(sizeof(char)*5); 
                newInstr1->Target = (char*)malloc(sizeof(char)*5); 
                newInstr1->immediate = offset1; 

                // set registers for this instruction
                sscanf("r0", "%s", newInstr1->First); 
                sscanf(f1, "%s", newInstr1->Target); 

                // insert it before ptr

                instr * temp1 = ptr->previous; 
                newInstr1->next = ptr; 
                temp1->next = newInstr1; 
                ptr->previous = newInstr1;
                newInstr1->previous = temp1; 


                // get offset of target register

                int offset2 = getOffset(ptr->Target);

                //generate second laodAI

                instr * newInstr2 = (instr*)malloc(sizeof(instr)); 
                newInstr2->opcode = LOADAI;
                newInstr2->First  = (char*)malloc(sizeof(char)*5); 
                newInstr2->Target = (char*)malloc(sizeof(char)*5); 
                newInstr2->immediate = offset2; 

                //set registers for this instruction
                sscanf("r0", "%s", newInstr2->First); 
                sscanf(f2, "%s", newInstr2->Target); 

                // insert it before ptr

                instr * temp2 = ptr->previous; 
                newInstr2->next = ptr; 
                temp2->next = newInstr2; 
                ptr->previous = newInstr2; 
                newInstr2->previous = temp2; 

                // update instructions for ptr
                bzero(ptr->First, strlen(ptr->First)); 
                bzero(ptr->Target, strlen(ptr->Target)); 
                sscanf(f1, "%s", ptr->First); 
                sscanf(f2, "%s", ptr->Target); 

                ptr = ptr->next; // move on to the next instruction and continue 
                continue;   
            }           


        }


        else if ( ptr->opcode == LOAD ){

            if ( isPhysicalReg(ptr->First) && isPhysicalReg(ptr->Target) ){ // they are both physical registers
                // simply just replace the registers with their physical mapping
                char * first = getPhysicalReg(ptr->First);   //physical register for First
                char * target = getPhysicalReg(ptr->Target); //physical register for Target
                bzero(ptr->First, strlen(ptr->First)); 
                bzero(ptr->Target, strlen(ptr->Target)); 
                sscanf(first, "%s", ptr->First); 
                sscanf(target, "%s", ptr->Target); 

                ptr = ptr->next; 
                continue; 
            }


            else if ( ( strcmp("r0", ptr->First) == 0 ) && isPhysicalReg(ptr->Target) ){

                // simply just replace the virtual target register with its physical counterPart
                char * target = getPhysicalReg(ptr->Target); 
                bzero(ptr->Target, strlen(ptr->Target)); 
                sscanf(target,"%s", ptr->Target); 

                ptr = ptr->next; 
                continue; 
            }


            else if ( (strcmp("r0", ptr->First) == 0) && !isPhysicalReg(ptr->Target) ){

                // spill the target register
                int offset = setOffset(ptr->Target); 
                bzero(ptr->Target, strlen(ptr->Target)); 
                sscanf(f1, "%s", ptr->Target); 

                // create and insert a store instruction

                instr * newInstr = (instr*)malloc(sizeof(instr)); 
                newInstr->opcode = STOREAI; 
                newInstr->First  = (char*)malloc(sizeof(char)*5); 
                newInstr->Target = (char*)malloc(sizeof(char)*5); 
                newInstr->immediate = offset; 

                // set registers
                sscanf(f1, "%s", newInstr->First); 
                sscanf("r0", "%s", newInstr->Target); 

                //insert instruction right after ptr

                newInstr->next = ptr->next; 
                ptr->next->previous = newInstr; 
                ptr->next = newInstr; 
                newInstr->previous = ptr; 

                // advance pointer past new inserted instruction and continue
                ptr = ptr->next->next; 
                continue; 
            }

            // if first one is physical and Target is virtual, set first , spill Target
            else if ( isPhysicalReg(ptr->First) && !isPhysicalReg(ptr->Target) ){

                int offset = setOffset(ptr->Target); 
                char * first = getPhysicalReg(ptr->First); 
                bzero(ptr->First, strlen(ptr->First)); 
                bzero(ptr->Target, strlen(ptr->Target)); 
                sscanf(first, "%s", ptr->First);  // set first register 
                sscanf(f1, "%s", ptr->Target);    // set target register

                //generate spill code (instruction)

                instr *newInstr  = (instr*)malloc(sizeof(instr)); 
                newInstr->opcode = STOREAI; 
                newInstr->First  = (char*)malloc(sizeof(char)*5); 
                newInstr->Target = (char*)malloc(sizeof(char)*5); 
                newInstr->immediate = offset; 

                sscanf(f1, "%s", newInstr->First); 
                sscanf("r0", "%s", newInstr->Target); 


                //insert instruction after ptr 

                newInstr->next = ptr->next; 
                ptr->next->previous = newInstr; 
                ptr->next = newInstr; 
                newInstr->previous = ptr; 

                ptr = ptr->next->next;  // move pass the newly inserted instruction unto the next
                continue;  


            }



            // if first is virtual and second is physical, load first, set second

            else if ( !isPhysicalReg(ptr->First) && isPhysicalReg(ptr->Target) ){

                int offset = getOffset(ptr->First); 

                //create load instruction 

                instr * newInstr = (instr*)malloc(sizeof(instr)); 
                newInstr->opcode = LOADAI; 
                newInstr->First  = (char*)malloc(sizeof(char)*5); 
                newInstr->Target = (char*)malloc(sizeof(char)*5); 
                newInstr->immediate = offset; 

                // set registers

                sscanf("r0", "%s", newInstr->First); 
                sscanf(f1, "%s", newInstr->Target); 


                // insert new instruction before ptr

                instr * temp = ptr->previous; 
                newInstr->next = ptr; 
                ptr->previous = newInstr; 
                temp->next = newInstr; 
                newInstr->previous = temp; 

                //modify registers of ptr

                char * target = getPhysicalReg(ptr->Target);
                bzero(ptr->First,strlen(ptr->First)); 
                bzero(ptr->Target,strlen(ptr->Target)); 
                sscanf(f1, "%s", ptr->First); 
                sscanf(target, "%s", ptr->Target); 

                // continue to next instruction 

                ptr = ptr->next; 

            }

            // if both are virtual, load first and spill target
            else{

                int offset1 = getOffset(ptr->First); 

                // create loadAI instruction

                instr * newInstr1 = (instr*)malloc(sizeof(instr)); 
                newInstr1->opcode = LOADAI; 
                newInstr1->First  = (char*)malloc(sizeof(char)*5); 
                newInstr1->Target = (char*)malloc(sizeof(char)*5); 
                newInstr1->immediate = offset1;

                //set registers

                sscanf("r0", "%s", newInstr1->First); 
                sscanf(f1, "%s", newInstr1->Target); 

                // insert instruction before ptr

                instr * temp1 = ptr->previous; 
                newInstr1->next = ptr; 
                ptr->previous = newInstr1; 
                temp1->next = newInstr1; 
                newInstr1->previous = temp1;  

                // create storeAI instruction

                int offset2 = setOffset(ptr->Target); 

                instr * newInstr2 = (instr*)malloc(sizeof(instr)); 
                newInstr2->opcode = STOREAI; 
                newInstr2->First  = (char*)malloc(sizeof(char)*5); 
                newInstr2->Target = (char*)malloc(sizeof(char)*5); 
                newInstr2->immediate = offset2; 

                // set registers

                sscanf(f2, "%s", newInstr2->First); 
                sscanf("r0", "%s", newInstr2->Target); 

                // insert after ptr

                newInstr2->next = ptr->next; 
                ptr->next->previous = newInstr2; 
                ptr->next = newInstr2; 
                newInstr2->previous = ptr; 

                // set the registers of ptr

                bzero(ptr->First, strlen(ptr->First)); 
                bzero(ptr->Target, strlen(ptr->Target)); 
                sscanf(f1, "%s", ptr->First); 
                sscanf(f2, "%s", ptr->Target); 

                // skip past the newly inserted instruction and continue

                ptr = ptr->next->next; 
                continue; 
            }


        }


        else if (ptr->opcode == ADD || ptr->opcode == SUB || ptr->opcode == MULT || ptr->opcode == LSHIFT || ptr->opcode == RSHIFT){
                    // if all are physical, set all and move on 
            if ( isPhysicalReg(ptr->First) && isPhysicalReg(ptr->Second) && isPhysicalReg(ptr->Target)){
                //get the physical registers
                char * first  = getPhysicalReg(ptr->First); 
                char * second = getPhysicalReg(ptr->Second); 
                char * target = getPhysicalReg(ptr->Target); 
                // remove the virtual registers
                bzero(ptr->First,  strlen(ptr->First)); 
                bzero(ptr->Target, strlen(ptr->Target)); 
                bzero(ptr->Second, strlen(ptr->Second));
                // replace the registers
                sscanf(first, "%s",  ptr->First); 
                sscanf(second, "%s", ptr->Second); 
                sscanf(target, "%s", ptr->Target);

                ptr = ptr->next;  //advance to the next instruction and continue; 
                continue;  
            }

            // only target register is virtual. set the first two registers and spill the tagrget register to memory
            else if ( isPhysicalReg(ptr->First) && isPhysicalReg(ptr->Second) && !isPhysicalReg(ptr->Target) ){

                // get the physical registers for the first and second registers
                char * first  = getPhysicalReg(ptr->First); 
                char * second = getPhysicalReg(ptr->Second); 
                //remove virtual registers
                bzero(ptr->First, strlen(ptr->First)); 
                bzero(ptr->Second, strlen(ptr->Second)); 
                // replace virtual registers
                sscanf(first, "%s", ptr->First); 
                sscanf(second, "%s", ptr->Second); 

                // now spill the target register (create STOREAI instr)

                int offset = setOffset(ptr->Target); 
                instr * newInstr = (instr*)malloc(sizeof(instr)); 
                newInstr->opcode = STOREAI; 
                newInstr->First  = (char*)malloc(sizeof(char)*5); 
                newInstr->Target = (char*)malloc(sizeof(char)*5); 
                newInstr->immediate = offset; 

                // modify registers for new instruction 

                sscanf(f1, "%s", newInstr->First); 
                sscanf("r0", "%s", newInstr->Target); 

                // insert the new instruction after ptr 

                newInstr->next = ptr->next; 
                ptr->next->previous = newInstr; 
                ptr->next = newInstr; 
                newInstr->previous = ptr; 

                // modify the target registers of ptr 

                bzero(ptr->Target, strlen(ptr->Target)); 
                sscanf(f1, "%s", ptr->Target); 

                // move ptr past the newly inserted instruction to the next instruction and continue

                ptr = ptr->next->next; 
                continue; 

            }

            // second register is a virtual register, set the first and target register and load the second
            else if ( isPhysicalReg(ptr->First) && !isPhysicalReg(ptr->Second) && isPhysicalReg(ptr->Target) ){
                // get physical registers of the First and Target registers
                char * first  = getPhysicalReg(ptr->First); 
                char * target = getPhysicalReg(ptr->Target); 
                //remove virtual registers
                bzero(ptr->First, strlen(ptr->First)); 
                bzero(ptr->Target, strlen(ptr->Target)); 
                // replace registers
                sscanf(first,"%s", ptr->First); 
                sscanf(target, "%s", ptr->Target); 

                // generate a LOADAI instruction for the second register

                int offset = getOffset(ptr->Second); 

                instr * newInstr = (instr*)malloc(sizeof(instr)); 
                newInstr->opcode = LOADAI; 
                newInstr->immediate = offset; 
                newInstr->First  = (char*)malloc(sizeof(char)*5);
                newInstr->Target = (char*)malloc(sizeof(char)*5); 

                // modify registers
                sscanf("r0", "%s", newInstr->First); 
                sscanf(f1, "%s", newInstr->Target); 

                // insert new instruction before ptr

                instr * temp = ptr->previous; 
                newInstr->next = ptr; 
                ptr->previous = newInstr; 
                temp->next = newInstr;
                newInstr->previous = temp; 

                // modify second register of ptr 
                bzero(ptr->Second, strlen(ptr->Second)); 
                sscanf(f1, "%s", ptr->Second); 

                // advance ptr and continue 
                ptr = ptr->next; 
                continue;  
            }

            // Second register and Target registers are physcial, the first is virtual (set Second/Target, load first)
            else if ( !isPhysicalReg(ptr->First) && isPhysicalReg(ptr->Second) && isPhysicalReg(ptr->Target) ){
                // get physcial registers of the second and target registers
                char * second = getPhysicalReg(ptr->Second); 
                char * target = getPhysicalReg(ptr->Target); 
                // remove the virtual registers
                bzero(ptr->Second, strlen(ptr->Second)); 
                bzero(ptr->Target, strlen(ptr->Target)); 
                // set second and target registers
                sscanf(second, "%s", ptr->Second); 
                sscanf(target, "%s", ptr->Target); 

                // generate a LOADAI instruction for the first register

                int offset = getOffset(ptr->First); 
                instr *newInstr = (instr*)malloc(sizeof(instr)); 
                newInstr->opcode = LOADAI; 
                newInstr->immediate = offset; 
                newInstr->First = (char*)malloc(sizeof(char)*5); 
                newInstr->Target = (char*)malloc(sizeof(char)*5); 

                // modify registers
                sscanf("r0", "%s", newInstr->First); 
                sscanf(f1, "%s", newInstr->Target); 

                // insert before ptr

                instr * temp = ptr->previous; 
                newInstr->next = ptr; 
                ptr->previous = newInstr; 
                temp->next = newInstr; 
                newInstr->previous = temp; 

                // modfiy first register of ptr
                bzero(ptr->First, strlen(ptr->First)); 
                sscanf(f1, "%s", ptr->First); 

                //advance ptr and continue
                ptr = ptr->next; 
                continue; 

            }

            // First and second Registers are Virtual and Target register is phsyical, Set the target and laod Register 1 and 2
            else if ( !isPhysicalReg(ptr->First) && !isPhysicalReg(ptr->Second) && isPhysicalReg(ptr->Target) ){

                // get the target register
                char * target = getPhysicalReg(ptr->Target); 
                // remove virtual 
                bzero(ptr->Target, strlen(ptr->Target)); 
                //replace
                sscanf(target, "%s", ptr->Target); 

                // Generate the first LOADAI(for register 1) instruction and Insert
                int offset1 = getOffset(ptr->First); 

                instr * newInstr1 = (instr*)malloc(sizeof(instr)); 
                newInstr1->opcode = LOADAI; 
                newInstr1->immediate = offset1; 
                newInstr1->First = (char*)malloc(sizeof(char)*5); 
                newInstr1->Target = (char*)malloc(sizeof(char)*5); 

                // modify registers

                sscanf("r0", "%s", newInstr1->First); 
                sscanf(f1, "%s", newInstr1->Target); 

                // insert before ptr

                instr * temp = ptr->previous; 
                newInstr1->next = ptr; 
                ptr->previous = newInstr1; 
                temp->next = newInstr1; 
                newInstr1->previous = temp; 


                //Generate second LOADAI (for register 2) instruction and insert
                int offset2 = getOffset(ptr->Second); 

                instr *newInstr2 = (instr*)malloc(sizeof(instr)); 
                newInstr2->opcode = LOADAI; 
                newInstr2->immediate = offset2; 
                newInstr2->First  = (char*)malloc(sizeof(char)*5); 
                newInstr2->Target = (char*)malloc(sizeof(char)*5); 

                // modify registers
                sscanf("r0","%s", newInstr2->First); 
                sscanf(f2, "%s", newInstr2->Target); 

                // insert before ptr

                instr * temp2 = ptr->previous; 
                newInstr2->next = ptr; 
                ptr->previous = newInstr2; 
                temp2->next = newInstr2; 
                newInstr2->previous = temp2; 

                // modify first and second registers of ptr 

                bzero(ptr->First, strlen(ptr->First)); 
                bzero(ptr->Second, strlen(ptr->Second)); 
                sscanf(f1, "%s", ptr->First); 
                sscanf(f2, "%s", ptr->Second); 

                // advance ptr and continue

                ptr = ptr->next;
                continue; 

            }
            //comeBack
            // First and Target Registers are virtual, set the second, load the first, spill the target
            else if ( !isPhysicalReg(ptr->First) && isPhysicalReg(ptr->Second) && !isPhysicalReg(ptr->Target) ){

                // get the second register
                char * second = getPhysicalReg(ptr->Second); 
                //remove virtual register
                bzero(ptr->Second, strlen(ptr->Second)); 
                // replace register
                sscanf(second,"%s", ptr->Second);

                // generate LOADAI instruction for the first register

                int offset1 = getOffset(ptr->First); 
                instr *newInstr1  = (instr*)malloc(sizeof(instr)); 
                newInstr1->opcode = LOADAI;
                newInstr1->immediate = offset1;  
                newInstr1->First  = (char*)malloc(sizeof(char)*5); 
                newInstr1->Target = (char*)malloc(sizeof(char)*5); 

                // modify registers

                sscanf("r0", "%s", newInstr1->First); 
                sscanf(f1, "%s", newInstr1->Target); 

                // insert before ptr

                instr * temp1 = ptr->previous; 
                newInstr1->next = ptr; 
                ptr->previous = newInstr1; 
                temp1->next = newInstr1; 
                newInstr1->previous = temp1; 


                // generate STOREAI instruction for spilling target register

                int offset2 = setOffset(ptr->Target); 
                instr *newInstr2 = (instr*)malloc(sizeof(instr)); 
                newInstr2->opcode = STOREAI; 
                newInstr2->immediate = offset2; 
                newInstr2->First  = (char*)malloc(sizeof(char)*5);
                newInstr2->Target = (char*)malloc(sizeof(char)*5); 

                //modify registers

                sscanf(f2, "%s", newInstr2->First);
                sscanf("r0", "%s", newInstr2->Target); 


                //insert after ptr

                newInstr2->next = ptr->next; 
                ptr->next->previous = newInstr2; 
                ptr->next = newInstr2; 
                newInstr2->previous = ptr; 

                // modify first and target registers in ptr

                bzero(ptr->First, strlen(ptr->First)); 
                bzero(ptr->Target, strlen(ptr->Target)); 
                sscanf(f1, "%s", ptr->First); 
                sscanf(f2, "%s", ptr->Target);  

                // adavance ptr past the newly inserted instruction and continue

                ptr = ptr->next->next; 
                continue; 
            }
            // Second and Target Registers are virtual, First is physical. Set First, Load second and Spill Target
            else if ( isPhysicalReg(ptr->First) && !isPhysicalReg(ptr->Second) && !isPhysicalReg(ptr->Target) ){

                // get the first Physical register
                char * first = getPhysicalReg(ptr->First); 
                //remove virtual register
                bzero(ptr->First, strlen(ptr->First)); 
                // replace register
                sscanf(first, "%s", ptr->First); 

                // generate LOADAI instruction for the Second register
                int offset1 = getOffset(ptr->Second); 
                instr *newInstr1  = (instr*)malloc(sizeof(instr)); 
                newInstr1->opcode = LOADAI;
                newInstr1->immediate = offset1;  
                newInstr1->First  = (char*)malloc(sizeof(char)*5); 
                newInstr1->Target = (char*)malloc(sizeof(char)*5);     

                // modify registers
                sscanf("r0", "%s", newInstr1->First); 
                sscanf(f1, "%s", newInstr1->Target); 

                // insert before ptr
                instr * temp1 = ptr->previous; 
                newInstr1->next = ptr; 
                ptr->previous = newInstr1; 
                temp1->next = newInstr1; 
                newInstr1->previous = temp1; 

                // generate STOREAI instruction for spilling target register
                int offset2 = setOffset(ptr->Target); 
                instr *newInstr2 = (instr*)malloc(sizeof(instr)); 
                newInstr2->opcode = STOREAI; 
                newInstr2->immediate = offset2; 
                newInstr2->First  = (char*)malloc(sizeof(char)*5);
                newInstr2->Target = (char*)malloc(sizeof(char)*5); 

                //modify registers

                sscanf(f2, "%s", newInstr2->First);
                sscanf("r0", "%s", newInstr2->Target); 


                //insert after ptr

                newInstr2->next = ptr->next; 
                ptr->next->previous = newInstr2; 
                ptr->next = newInstr2; 
                newInstr2->previous = ptr; 


                //modify the second registers and target registers of ptr

                bzero(ptr->Second, strlen(ptr->Second)); 
                bzero(ptr->Target, strlen(ptr->Target)); 
                sscanf(f1, "%s", ptr->Second); 
                sscanf(f2, "%s", ptr->Target); 

                // advance ptr past the newly inserted instruction and continue

                ptr = ptr->next->next; 
                continue; 

            }

            // all are vritual registers. Load First and Second Register, Spill Target Regisster

            else {

                // generate LOADAI instruction for the first register
                int offset1 = getOffset(ptr->First); 

                instr * newInstr1 = (instr*)malloc(sizeof(instr)); 
                newInstr1->opcode = LOADAI; 
                newInstr1->immediate = offset1; 
                newInstr1->First  = (char*)malloc(sizeof(char)*5); 
                newInstr1->Target = (char*)malloc(sizeof(char)*5);  

                // set registers

                sscanf("r0","%s", newInstr1->First); 
                sscanf(f1, "%s", newInstr1->Target); 

                //insert instruction before ptr

                instr * temp1 = ptr->previous; 
                newInstr1->next = ptr; 
                ptr->previous = newInstr1; 
                temp1->next = newInstr1; 
                newInstr1->previous = temp1; 


                // generate LOADAI instruction for the second register

                int offset2 = getOffset(ptr->Second); 

                instr * newInstr2 = (instr*)malloc(sizeof(instr)); 
                newInstr2->opcode = LOADAI; 
                newInstr2->immediate = offset2; 
                newInstr2->First  = (char*)malloc(sizeof(char)*5); 
                newInstr2->Target = (char*)malloc(sizeof(char)*5); 

                //set registers

                sscanf("r0", "%s", newInstr2->First); 
                sscanf(f2, "%s", newInstr2->Target); 

                // insert instruction before ptr

                instr * temp2 = ptr->previous; 
                newInstr2->next = ptr; 
                ptr->previous = newInstr2; 
                temp2->next = newInstr2; 
                newInstr2->previous = temp2;  

                // generate StoreAI instruction for the target register and insert it after ptr

                int offset3 = setOffset(ptr->Target); 

                instr * newInstr3 = (instr*)malloc(sizeof(instr)); 
                newInstr3->opcode = STOREAI; 
                newInstr3->immediate = offset3; 
                newInstr3->First  = (char*)malloc(sizeof(char)*5); 
                newInstr3->Target = (char*)malloc(sizeof(char)*5); 

                // set registers

                sscanf(f1, "%s", newInstr3->First); 
                sscanf("r0", "%s", newInstr3->Target);

                // insert instruction after ptr

                newInstr3->next = ptr->next; 
                ptr->next->previous = newInstr3; 
                ptr->next = newInstr3; 
                newInstr3->previous = ptr; 


                // modify registers of ptr 
                bzero(ptr->First, strlen(ptr->First)); 
                bzero(ptr->Second, strlen(ptr->Second)); 
                bzero(ptr->Target, strlen(ptr->Target)); 
                sscanf(f1, "%s", ptr->First);
                sscanf(f2, "%s", ptr->Second); 
                sscanf(f1, "%s", ptr->Target); 

                //advance ptr to move past the newly inserted instruction and continue

                ptr = ptr->next->next; 
                continue;

            }

        }


        else{

            ptr = ptr->next; 
        }




    } // end main while loop 



} // end Simple top down allocator






// begin functions for Live Range Top down allocator

// checks whether a register is contained in a line
int isContained ( instr * instruction, char * reg ){

    int i = 0, j = 0, k = 0; 


    if ( instruction->First != NULL){
        if (strcmp(reg, instruction->First) == 0)
            return 1;  
    }

    if ( instruction->Second != NULL )
        if(strcmp(reg, instruction->Second) == 0)
            return 1; 

    if ( instruction->Target != NULL ){
        if(strcmp(reg,instruction->Target) == 0)
            return 1;   
    }

    return 0; 

}


// returns the length of the instruction set

int GetInstructionLength () {

    int i = 0; 

    instr * ptr = InstructionList->head; 

    if (ptr == NULL)
        ptr = ptr->next; 

    while ( ptr != NULL ){

        ptr = ptr->next; 
        i++;
    }
   // printf("This is the length of the instruction: %d\n", i); 
    return i; 
}


// given  a register, this calculates its liverange
void getLiveRange (myRegisters * reg){

    int start = 1; 

    instr * ptr = InstructionList->head; 

    if ( ptr->opcode != LOADI)
        ptr = ptr->next; 

    while ( ptr != NULL ){

        if ( isContained(ptr, reg->regType) ){
            break; 
        }

        start++;
        ptr = ptr->next;
    }


    int end = instructionLength;

    instr * ptr2 = InstructionList->tail; 

    while ( ptr2 != NULL ){

        if ( isContained(ptr2, reg->regType) ){
            break; 
        }

        end--; 
        ptr2 = ptr2->previous; 
    }

    reg->endLine = end;      // live on exit, get the last line where this instruction occurs

    if ( (end - start) == 0 ){

        reg->liveRange = 1; 
        return; 
    }

    reg->liveRange =  (end - start); 
}


// computes the live ranges of all the registers

void computeLiveRanges (){

    myRegisters * ptr = myList->head;

    while( ptr != NULL ){

        if (strcmp(ptr->regType, "r0") == 0){
            ptr = ptr->next; 
            continue; 
        }

        getLiveRange(ptr); 
        ptr = ptr->next; 
    }

}

// goes through list of registers and returns a specific register

myRegisters * findRegister (char * reg){

    myRegisters * ptr = myList->head; 

    while ( ptr != NULL ){

        if( strcmp(ptr->regType, reg) == 0  ){
            ptr->mark++; 
            return ptr; 
        }

        ptr = ptr->next; 
    }

    return NULL; 

}

// deletes the given register from the list

int delete ( regList * list, char * reg ){

    if( list->head == NULL )
        return 0; 

   myRegisters * prev = NULL; 
   myRegisters * ptr = list->head; 

   if ( (strcmp(ptr->regType, reg) == 0 ) ){
        list->head = list->head->next; 
        free(ptr); 
        return 1; 
   }
   prev = ptr; 
   ptr = ptr->next; 

   while ( ptr != NULL ){

        if( strcmp(ptr->regType, reg) == 0 ){
            break; 
        }

        prev = ptr; 
        ptr = ptr->next; 
   }

   if ( ptr == NULL )   // not in the list 
        return 0; 
    // last elemen
    if ( ptr->next == NULL ){
        list->tail = prev; 
        list->tail->next = NULL; 
        free(ptr); 
        return 1; 
    }

   myRegisters * temp = ptr; 
   prev->next = ptr->next; 
   free(ptr); 
   return 1; 

}

// remove all instances of a register from all the maxLives in the instruction list 
void RemoveAllInstances ( instr * line, char * reg ){

    instr * ptr = line;
    if( ptr->maxLive > numPhysicalReg ){ 

        if(delete(ptr->maxList, reg)){
            ptr->maxLive--;        
        }
    }

    ptr = ptr->next;

    while ( ptr != NULL ){

        if( ptr->opcode == OUTPUT )
            break; 

        if( ptr->maxLive > numPhysicalReg ){

           if(delete(ptr->maxList, reg)){  

            ptr->maxLive--; 

            }
        }

        ptr = ptr->next; 
    } 

}


// this function takes in a list of register, a list (previous) of the subset of registers from the previous line
// takes in a line to see what registers to ommit (registers that end at this line )
// takes in the name of the register to be appended (usuallythe target register of that line)
int buildMaxList ( regList * List , instr * previous, int curr_line, char * append){

    int length = 0; 
    // if the previous list has registers contained in its list of maxLives
    if ( previous !=NULL && previous->maxList != NULL ){

        myRegisters * ptr = previous->maxList->head;

        // in case the first element of the previous list happend to be live on exit 
        if ( ptr->endLine == curr_line || ptr->endLine == (curr_line  - 1) )
            ptr = ptr->next; 

        myRegisters * newReg = (myRegisters*)malloc(sizeof(myRegisters));
        memcpy(newReg, ptr, sizeof(myRegisters));

        List->head = newReg; 
        List->tail = newReg;
        List->tail->next = NULL;   
        length++; 
        ptr = ptr->next; 

        while( ptr != NULL ){
            // ignore things that are live on exit
            if( ptr->endLine == curr_line || ptr->endLine == (curr_line-1) || strcmp(append, ptr->regType) == 0){
                ptr = ptr->next; 
                continue; 
            }

            myRegisters * newReg = (myRegisters*)malloc(sizeof(myRegisters)); 
            memcpy(newReg, ptr, sizeof(myRegisters)); 
            List->tail->next = newReg; 
            List->tail = newReg; 
            List->tail->next = NULL; 
            length++; 
            ptr = ptr->next; 
        }

        myRegisters * insert = findRegister(append); 

        myRegisters * newReg2 = (myRegisters*)malloc(sizeof(myRegisters)); 
        memcpy(newReg2, insert, sizeof(myRegisters));
        List->tail->next = newReg2; 
        List->tail = newReg2; 
        List->tail->next = NULL;
        length++;  
        return length;  
    }

    // if the MaxList of the previous line is empty, simply just append the target register to maxList of this line

    myRegisters * insert = findRegister(append); 

    myRegisters * newReg = (myRegisters*)malloc(sizeof(myRegisters)); 
    memcpy(newReg, insert, sizeof(myRegisters)); 
    List->head = newReg; 
    List->tail = newReg; 
    List->tail->next = NULL;
    length++; 
    return length; 
}


int removeDuplicates (regList * list) {

    int numDups = 0; 

    myRegisters * ptr = list->head; 
    myRegisters * ptr2; 
    myRegisters * duplcate; 

    while ( ptr != NULL && ptr->next != NULL ){

        ptr2 = ptr; 

        while ( ptr2->next != NULL ){

            if( strcmp(ptr->regType, ptr2->next->regType) == 0 ){
                numDups++;
                duplcate = ptr2->next; 
                ptr2->next = ptr2->next->next;

                if( duplcate == list->tail ){
                    list->tail = ptr2; 
                }


                free(duplcate); 
            }

            else
                ptr2 = ptr2->next; 

        }

        ptr = ptr->next; 

    }

    return numDups; 

}



// builds a list of maxlives for every single line

void buildMaxLive (){

    int line = 2;

    instr * ptr = InstructionList->head;

    // for empty nodes in front of our list 
    if( ptr->opcode != LOADI)
        ptr = ptr->next; 
    
    // we do not care about r0 
    if( strcmp(ptr->Target, "r0") == 0 )
        ptr = ptr->next; 

    ptr->maxList = (regList*)malloc(sizeof(regList)); 
    ptr->maxList->head = NULL; 
    ptr->maxList->tail = NULL; 
    ptr->maxLive = buildMaxList( ptr->maxList, NULL, line, ptr->Target );
    int dups = removeDuplicates(ptr->maxList); 
    ptr->maxLive -= dups; 
    ptr->maxList->head = CascadeSorting(ptr->maxList->head); 
    ptr = ptr->next; 
    line++; 

    while ( ptr != NULL ){

        if ( ptr->opcode == OUTPUT ){
            break; 
        }

        ptr->maxList = (regList*)malloc(sizeof(regList)); 
        ptr->maxList->head = NULL; 
        ptr->maxList->tail = NULL; 
        ptr->maxLive = buildMaxList( ptr->maxList, ptr->previous, line, ptr->Target);
        int dups = removeDuplicates(ptr->maxList); 
        ptr->maxLive -= dups; 
        ptr->maxList->head = CascadeSorting(ptr->maxList->head); 
        line++; 
        ptr = ptr->next; 
    } 

}


// creates a list of all the registers to be spilled 


/*void InsertToSpilledSet ( myRegisters * reg ){

    if ( spilledSet->head == NULL ){

        spilledSet->head = reg; 
        spilledSet->tail = reg; 
        spilledSet->tail->next = NULL; 
        return; 
    }

    spilledSet->tail->next = reg;
    spilledSet->tail = reg;  
    spilledSet->tail->next = NULL;
    return;  
}*/


// given a MaxList, Maps the first K registers of that List to the spilled List 

void mapSpilledReg (instr * instruction, int k ){

    if ( instruction->maxList->head == NULL)
        return; 

    myRegisters * ptr = instruction->maxList->head;

    if ( spilledSet->head == NULL ){

        myRegisters * newReg = (myRegisters*)malloc(sizeof(myRegisters)); 
        memcpy(newReg, ptr, sizeof(myRegisters));
        spilledSet->head = newReg; 
        spilledSet->tail = newReg; 
        spilledSet->tail->next = NULL; 
        RemoveAllInstances(instruction, ptr->regType); 
        ptr = ptr->next; 
        k--; 
     //printf("Spilling: %s\n", newReg->regType);

        while ( ptr != NULL && k>0 ){

            myRegisters * newReg = (myRegisters*)malloc(sizeof(myRegisters)); 
            memcpy(newReg, ptr, sizeof(myRegisters));
            spilledSet->tail->next = newReg; 
            spilledSet->tail = newReg; 
            spilledSet->tail->next = NULL; 
            //printf("Spilling: %s\n", newReg->regType);
            RemoveAllInstances(instruction, newReg->regType);    // remove from all maxLives
            k--; 
             ptr = ptr->next; 

        }

        return; 

    }



    while ( ptr != NULL && k>0 ){

        myRegisters * newReg = (myRegisters*)malloc(sizeof(myRegisters)); 
        memcpy(newReg, ptr, sizeof(myRegisters));
        spilledSet->tail->next = newReg; 
        spilledSet->tail = newReg; 
        spilledSet->tail->next = NULL;  
           // printf("Spilling: %s\n", newReg->regType);
        RemoveAllInstances(instruction, newReg->regType);    // remove from all maxLives
        k--; 
        ptr = ptr->next;     

    }

    return; 

}


// builds the actual set 

void buildSpilledSet (){

    instr * ptr = InstructionList->head; 

    if ( ptr->opcode != LOADI )
        ptr = ptr->next; 

   // ptr = ptr->next; 

    if ( strcmp("r0", ptr->Target) == 0 )
        ptr = ptr->next; 

    while ( ptr != NULL){

        if ( ptr->opcode == OUTPUT )
            break; 

        if ( ptr->maxLive < numPhysicalReg ){  // if the MaxLive <= K-f , we are goof
            ptr = ptr->next; 
            continue; 
        }

        int k = ptr->maxLive - numPhysicalReg; 
        mapSpilledReg(ptr, k );
        ptr = ptr->next; 
    }

}


int Spilled ( char * reg ){

    myRegisters * ptr = spilledSet->head; 

    while ( ptr != NULL ){

        if( strcmp(ptr->regType, reg) == 0  ){
            return 1; 
        }

        ptr = ptr->next; 
    }

    return 0; 
}







createRegQ ( int physical){

    MyRegQ = (regQueue*)malloc(sizeof(regQueue)); 
    MyRegQ->head = NULL;  
    MyRegQ->tail = NULL; 

    int i = 1; 

    regNode *newRegNode = (regNode*)malloc(sizeof(regNode)); 
    newRegNode->reg = (char*)malloc(sizeof(char)*5); 
    sprintf(newRegNode->reg, "%s%d", "r", i); 
    i++; 

    MyRegQ->head = newRegNode; 
    MyRegQ->tail = newRegNode; 
    MyRegQ->tail->next = NULL; 

    while( physical > 1 ){
    // create new register node
    regNode *newRegNode = (regNode*)malloc(sizeof(regNode)); 
    newRegNode->reg = (char*)malloc(sizeof(char)*5); 
    sprintf(newRegNode->reg, "%s%d", "r", i);
    // insert to register Queue
    MyRegQ->tail->next = newRegNode; 
    MyRegQ->tail = newRegNode; 
    MyRegQ->tail->next = NULL; 

    i++; 
    physical--; 

    }
}

// returns a physical register to me
char * Dequeue(){

    if ( MyRegQ->head == NULL ){

        return NULL; 
    }

    char * str = MyRegQ->head->reg; 
    regNode * ptr = MyRegQ->head; 

    if ( ptr->next == NULL ){

        MyRegQ->head = NULL; 
        MyRegQ->tail = NULL; 
        free(ptr); 
        return str; 
    }


    MyRegQ->head = MyRegQ->head->next;
    free(ptr); 

    return str; 

}


// inserts a physical register into my list of registers
void Enqueue( char * reg ){

    if ( reg == NULL)
        return; 

    regNode * newRegNode = (regNode*)malloc(sizeof(regNode)); 
    newRegNode->reg = (char*)malloc(sizeof(char)*5); 

    sscanf(reg, "%s", newRegNode->reg); 

    if( MyRegQ->head == NULL ){

        MyRegQ->head = newRegNode; 
        MyRegQ->tail = newRegNode; 

        MyRegQ->tail->next = NULL; 
        //free(reg); 
        return; 
    }


    MyRegQ->tail->next = newRegNode; 
    MyRegQ->tail = newRegNode; 
    MyRegQ->tail->next = NULL; 
    //free(reg); 
    return;
}


int PrintQueue (){

    regNode * ptr = MyRegQ->head; 

    while ( ptr != NULL ){

        printf("%s -> " , ptr->reg); 

        ptr = ptr->next; 
    }

    printf("/\n");
}



// dest is the virtual register getting a physical register

//src is the actual register

void setPhysicalReg ( char * dest, char * src ){

    myRegisters * ptr = myList->head; 

    while ( ptr != NULL ){

        if( strcmp(dest, ptr->regType) == 0 ){

            ptr->physicalReg = (char*)malloc(sizeof(char)*5); 
            sscanf(src, "%s", ptr->physicalReg); 
            ptr->isMapped = 1; 
            return; 
        }

        ptr = ptr->next; 
    }


}

// tells you whether a virtual Register is mapped to a physical Register
int isMapped ( char * reg ){

    myRegisters * ptr = myList->head; 

    while ( ptr != NULL ){

        if ( strcmp( reg, ptr->regType ) == 0 ){

            return ptr->isMapped; 
        }

        ptr = ptr->next; 
    }

    return 0; 
}

// returns a physical register mapped to a given virtual register
char * getMappedReg( char * reg ){

    myRegisters * ptr = myList->head; 

    while ( ptr != NULL ){

        if( strcmp( ptr->regType, reg ) == 0 ){

            return ptr->physicalReg; 
        }

        ptr = ptr->next; 
    }

    return NULL; 
}

// given a virtual register, it enqueues its physical register back into the queue
void reset ( char * reg ){

    myRegisters * ptr = myList->head; 

    while ( ptr != NULL ){

        if( strcmp( ptr->regType, reg ) == 0 ){

            Enqueue(ptr->physicalReg);
            ptr->isMapped = 0; 
            free(ptr->physicalReg); 
            return; 
        }

        ptr = ptr->next; 
    }

}

// returns the endLine of a given register

int GetEndLine ( char * reg ){

    myRegisters * ptr = myList->head;

    while ( ptr != NULL ){

        if( strcmp( ptr->regType, reg ) == 0 ){

            return ptr->endLine; 
        }

        ptr = ptr->next; 
    }

    return -1; 
}



void printMaxLives ( instr * instructions ){

    if( instructions->maxList == NULL )
        return; 

    myRegisters * ptr = instructions->maxList->head; 

    printf("MaxLive: %d  : ", instructions->maxLive );

    while ( ptr != NULL ){

        printf(" R: %s ", ptr->regType);
        ptr = ptr->next; 
    }

}



// begin the LiveRange Top down allocator
void LiveRangeTopDown(myInstrList * instructions){

    instr * ptr = instructions->head->next; 
    int line = 2; 
    if ( strcmp(ptr->Target, "r0") == 0 ){

        ptr = ptr->next;                        // ignore the first register if it is a load to r0 
    }


    while ( ptr != NULL ){

        if ( ptr->opcode == LOADI ){
            // if it is a physical register
            if( !Spilled(ptr->Target) ){
                // get a physical register from queue
                char * reg = Dequeue(); 
                // map the registers
                setPhysicalReg(ptr->Target, reg); 

                // get the endLine

                int endLine = GetEndLine(ptr->Target);

                // remove current register and replace
                bzero(ptr->Target, strlen(ptr->Target)); 
                sscanf(reg, "%s", ptr->Target); 

                // check if the register is live on exit on this line

                if( line == endLine ){

                    Enqueue(reg); 
                }

                line++; 
                ptr = ptr->next; 
                continue; 

            }


            else{

                // this register is to be loaded into memory 

                int offset = setOffset(ptr->Target); 
                // replace register
                bzero(ptr->Target,strlen(ptr->Target)); 
                sscanf(f1, "%s", ptr->Target); 

                // generate a STOREAI instruction

                instr * newInstr = (instr*)malloc(sizeof(instr));
                newInstr->opcode = STOREAI;  
                newInstr->immediate = offset; 
                newInstr->Target = (char*)malloc(sizeof(char)*5); 
                newInstr->First = (char*)malloc(sizeof(char)*5); 

                //set registers

                sscanf(f1, "%s", newInstr->First); 
                sscanf("r0", "%s", newInstr->Target); 

                //insert instruction after ptr

                newInstr->next = ptr->next; 
                ptr->next->previous = newInstr; 
                ptr->next = newInstr; 
                newInstr->previous = ptr; 

                // move pass the newly inserted instruction and continue

                ptr = ptr->next->next; 
                line++; 
                continue; 
            }


        }


        else if ( ptr->opcode == STORE ){
            // both are not physical
            if ( !Spilled(ptr->First) && !Spilled(ptr->Target) ){
                // get their physical registers
                char * first  = getMappedReg(ptr->First);

                char * target = getMappedReg(ptr->Target); 

                // get their endLine
                int endLineF = GetEndLine(ptr->First); 
                int endLineT = GetEndLine(ptr->Target); 
                // set regiseters
                bzero(ptr->First, strlen(ptr->First));
                bzero(ptr->Target, strlen(ptr->Target)); 
                sscanf(first, "%s", ptr->First);
                sscanf(target, "%s", ptr->Target); 

                // check for their endLines for Live on Exit

                if( endLineF == line )
                    Enqueue(first); 

                if( endLineT == line )
                    Enqueue(target); 

                ptr = ptr->next; 
                line++; 
                continue; 
            } // end if 

            // First Virtual, Second Physical
            else if ( Spilled(ptr->First) && !Spilled(ptr->Target) ){

                // get offset of the first register
                int offset = getOffset(ptr->First); 

                //generate a LOADAI instruction 
                instr* newInstr = (instr*)malloc(sizeof(instr)); 
                newInstr->opcode = LOADAI; 
                newInstr->immediate = offset; 
                newInstr->First = (char*)malloc(sizeof(char)*5); 
                newInstr->Target = (char*)malloc(sizeof(char)*5); 

                // set registers

                sscanf("r0", "%s", newInstr->First); 
                sscanf(f1, "%s", newInstr->Target); 

                // insert instruction before ptr

                instr * temp = ptr->previous; 
                newInstr->next = ptr; 
                temp->next = newInstr; 
                newInstr->previous = temp; 

                // modify the first register of ptr
                bzero(ptr->First, strlen(ptr->First)); 
                sscanf(f1, "%s", ptr->First); 

                // now deal with the physical register

                char * target = getMappedReg(ptr->Target); 
                int endLineT = GetEndLine(ptr->Target); 

                //replace register
                bzero(ptr->Target, strlen(ptr->Target)); 
                sscanf(target, "%s", ptr->Target); 

                // check for endLines and give back register
                if ( endLineT == line )
                    Enqueue(target); 

                ptr = ptr->next; 
                line++; 
                continue; 
            }
            // if the target is a virtual register
            else if ( !Spilled(ptr->First) && Spilled(ptr->Target) ){

                // get offset of target register
                int offset = getOffset(ptr->Target); 

                // generate LOADAI instruction
                instr* newInstr = (instr*)malloc(sizeof(instr)); 
                newInstr->opcode = LOADAI; 
                newInstr->immediate = offset; 
                newInstr->First = (char*)malloc(sizeof(char)*5); 
                newInstr->Target = (char*)malloc(sizeof(char)*5); 

                // set registers
                sscanf("r0", "%s", newInstr->First); 
                sscanf(f1, "%s", newInstr->Target); 

                // insert instruction before ptr
                instr * temp = ptr->previous; 
                newInstr->next = ptr; 
                temp->next = newInstr; 
                newInstr->previous = temp;   

                // modify target register of ptr
                bzero(ptr->Target, strlen(ptr->Target)); 
                sscanf(f1, "%s", ptr->Target); 


                // now deal with physical register 
                char * first = getMappedReg(ptr->First); 
                int endLineF = GetEndLine(ptr->First);

                //replace register
                bzero(ptr->First, strlen(ptr->First));
                sscanf(first, "%s", ptr->First); 

                //check for endLine and Give back Register

                if( endLineF == line )
                    Enqueue(first); 



                ptr = ptr->next; 
                line++; 
                continue; 
            }

            // both are virtual 
            else {

                int offset1 = getOffset(ptr->First); 
                int offset2 = getOffset(ptr->Target); 

                //generate LOADAI instruction and insert before ptr 
                instr * newInstr1 = (instr*)malloc(sizeof(instr)); 
                newInstr1->opcode = LOADAI; 
                newInstr1->immediate = offset1; 
                newInstr1->First  = (char*)malloc(sizeof(char)*5); 
                newInstr1->Target = (char*)malloc(sizeof(char)*5); 

                //set registers
                sscanf("r0", "%s", newInstr1->First); 
                sscanf(f1, "%s", newInstr1->Target); 

                // insert before ptr
                instr * temp1 = ptr->previous; 
                newInstr1->next = ptr; 
                ptr->previous = newInstr1; 
                temp1->next = newInstr1; 
                newInstr1->previous = temp1; 


                // generate LOADAI instruction for target register and insert before ptr
                instr * newInstr2 = (instr*)malloc(sizeof(instr)); 
                newInstr2->opcode = LOADAI; 
                newInstr2->immediate = offset2; 
                newInstr2->First  = (char*)malloc(sizeof(char)*5); 
                newInstr2->Target = (char*)malloc(sizeof(char)*5); 

                //set registers
                sscanf("r0", "%s", newInstr2->First);
                sscanf(f2, "%s", newInstr2->Target); 

                //insert before ptr  
                instr * temp2 = ptr->previous; 
                newInstr2->next = ptr;
                ptr->previous = newInstr2; 
                temp2->next = newInstr2; 
                newInstr2->previous = temp2;

                // modify both registers in ptr

                bzero(ptr->Target, strlen(ptr->Target)); 
                bzero(ptr->First, strlen(ptr->First)); 
                sscanf(f1, "%s", ptr->First); 
                sscanf(f2, "%s", ptr->Target); 

                //increment ptr/line and continue
                ptr = ptr->next; 
                line++; 
                continue; 
            }

        }

        // load instruction 
        else if (ptr->opcode == LOAD){
            // both are physical
            if ( !Spilled(ptr->First) && !Spilled(ptr->Target) ){
                // get endlines of both registers
                int endLineF = GetEndLine(ptr->First);
                // get the mapped register of the first
                char * first = getMappedReg(ptr->First);

                if (strcmp(ptr->First, "r0") == 0 ){
                    bzero(ptr->First, strlen(ptr->First)); 
                    sscanf("r0","%s", ptr->First);                    
                }
                
                else{
                    bzero(ptr->First, strlen(ptr->First)); 
                    sscanf(first,"%s", ptr->First); 

                // give up physical register if live on exit
                    if ( line == endLineF )
                     Enqueue(first); 
                }


                    //get a register and map
                    char * target = Dequeue(); 
                    setPhysicalReg(ptr->Target, target); 
                    int endLineT = GetEndLine(ptr->Target); 
                    // replace
                    bzero(ptr->Target, strlen(ptr->Target)); 
                    sscanf(target, "%s", ptr->Target); 
                    // check if live on exit and give up physical register
                    if ( line == endLineT )
                        Enqueue(target); 
            

                ptr = ptr->next; 
                line++; 
                continue; 

            }

            // first is virtual, second is physical, load first , set second
            else if ( Spilled(ptr->First) && !Spilled(ptr->Target) ){

                // generate LOADAI instruction for first
                int offset = getOffset(ptr->First); 

                instr* newInstr = (instr*)malloc(sizeof(instr)); 
                newInstr->opcode = LOADAI; 
                newInstr->immediate = offset; 
                newInstr->First = (char*)malloc(sizeof(char)*5); 
                newInstr->Target = (char*)malloc(sizeof(char)*5);

                // set register
                sscanf("r0", "%s", newInstr->First); 
                sscanf(f1, "%s", newInstr->Target); 

                //insert instruction before ptr; 
                instr * temp = ptr->previous; 
                newInstr->next = ptr; 
                ptr->previous = newInstr; 
                newInstr->previous = temp; 

                //modify first register of ptr
                bzero(ptr->First, strlen(ptr->First)); 
                sscanf(f1, "%s", ptr->First); 

                // deal with physical Target register
                int endLineT = GetEndLine(ptr->Target); 
                // grab a physical register
                char * target = Dequeue(); 
                setPhysicalReg(ptr->Target, target); 

                //replace register
                bzero(ptr->Target, strlen(ptr->Target));
                sscanf(target, "%s", ptr->Target); 

                // check if live on exit
                if ( endLineT == line )
                    Enqueue(target); 

                ptr = ptr->next; 
                line++; 
                continue; 

            }

            //replace first, spill second
            else if ( !Spilled(ptr->First) && Spilled(ptr->Target) ){

                int endLineF = GetEndLine(ptr->First);
                // get the mapped register of the first
                char * first = getMappedReg(ptr->First);

                if (strcmp(ptr->First, "r0") == 0 ){
                    bzero(ptr->First, strlen(ptr->First)); 
                    sscanf("r0","%s", ptr->First);                    
                }
                
                else{
                    bzero(ptr->First, strlen(ptr->First)); 
                    sscanf(first,"%s", ptr->First); 

                // give up physical register if live on exit
                    if ( line == endLineF )
                     Enqueue(first); 
                }                
       


                // deal with spilled  target register, generate store instruction
                int offset = setOffset(ptr->Target); 

                instr * newInstr = (instr*)malloc(sizeof(instr)); 
                newInstr->opcode = STOREAI; 
                newInstr->immediate = offset; 
                newInstr->First  = (char*)malloc(sizeof(char)*5); 
                newInstr->Target = (char*)malloc(sizeof(char)*5);

                //set registers
                sscanf(f1, "%s", newInstr->First); 
                sscanf("r0", "%s", newInstr->Target); 

                //insert instruction after ptr
                newInstr->next = ptr->next; 
                ptr->next->previous = newInstr; 
                ptr->next = newInstr; 
                newInstr->previous = ptr; 

                // replace target register of ptr
                bzero(ptr->Target, strlen(ptr->Target)); 
                sscanf(f1,"%s", ptr->Target); 

                ptr = ptr->next->next; 
                line++; 
                continue;  

            }

            // both virtual, load first, spill second
            else{

                int offset1 = getOffset(ptr->First); 

                // create loadAI instruction

                instr * newInstr1 = (instr*)malloc(sizeof(instr)); 
                newInstr1->opcode = LOADAI; 
                newInstr1->First  = (char*)malloc(sizeof(char)*5); 
                newInstr1->Target = (char*)malloc(sizeof(char)*5); 
                newInstr1->immediate = offset1;

                //set registers

                sscanf("r0", "%s", newInstr1->First); 
                sscanf(f1, "%s", newInstr1->Target); 

                // insert instruction before ptr

                instr * temp1 = ptr->previous; 
                newInstr1->next = ptr; 
                ptr->previous = newInstr1; 
                temp1->next = newInstr1; 
                newInstr1->previous = temp1;  

                // create storeAI instruction

                int offset2 = setOffset(ptr->Target); 

                instr * newInstr2 = (instr*)malloc(sizeof(instr)); 
                newInstr2->opcode = STOREAI; 
                newInstr2->First  = (char*)malloc(sizeof(char)*5); 
                newInstr2->Target = (char*)malloc(sizeof(char)*5); 
                newInstr2->immediate = offset2; 

                // set registers

                sscanf(f2, "%s", newInstr2->First); 
                sscanf("r0", "%s", newInstr2->Target); 

                // insert after ptr

                newInstr2->next = ptr->next; 
                ptr->next->previous = newInstr2; 
                ptr->next = newInstr2; 
                newInstr2->previous = ptr; 

                // set the registers of ptr

                bzero(ptr->First, strlen(ptr->First)); 
                bzero(ptr->Target, strlen(ptr->Target)); 
                sscanf(f1, "%s", ptr->First); 
                sscanf(f2, "%s", ptr->Target); 

                // skip past the newly inserted instruction and continue

                ptr = ptr->next->next; 
                line++; 
                continue;

            }

        }



        else if ( ptr->opcode == ADD || ptr->opcode == SUB || ptr->opcode == MULT || ptr->opcode == LSHIFT || ptr->opcode == RSHIFT ){
            //get the physical register of the left operands

            if ( !Spilled(ptr->First) && !Spilled(ptr->Second) && !Spilled(ptr->Target) ){
                char * first = getMappedReg(ptr->First); 
                char * second = getMappedReg(ptr->Second); 
                int endLineF = GetEndLine(ptr->First); 
                int endLineS = GetEndLine(ptr->Second); 

                //replace registers
                bzero(ptr->First,strlen(ptr->First)); 
                bzero(ptr->Second, strlen(ptr->Second)); 
                sscanf(first, "%s", ptr->First); 
                sscanf(second, "%s", ptr->Second); 

                //check for live on exit

                if( line == endLineF )
                    Enqueue(first); 

                if( line == endLineS )
                    Enqueue(second); 

                //deal with target 

                char * target = Dequeue(); 
                setPhysicalReg(ptr->Target, target); 
                int endLineT = GetEndLine(ptr->Target); 

                //replace register
                bzero(ptr->Target, strlen(ptr->Target)); 
                sscanf(target, "%s", ptr->Target); 

                if ( endLineT == line )
                    Enqueue(target); 

                ptr = ptr->next; 
                line++; 
                continue; 
            }

            //Left operands are physical and target is virtual, set operands and spill target
            else if ( !Spilled(ptr->First) && !Spilled(ptr->Second) && Spilled(ptr->Target) ){

                char * first = getMappedReg(ptr->First); 
                char * second = getMappedReg(ptr->Second); 
                int endLineF = GetEndLine(ptr->First); 
                int endLineS = GetEndLine(ptr->Second); 

                //replace registers
                bzero(ptr->First,strlen(ptr->First)); 
                bzero(ptr->Second, strlen(ptr->Second)); 
                sscanf(first, "%s", ptr->First); 
                sscanf(second, "%s", ptr->Second); 

                //check for live on exit

                if( line == endLineF )
                    Enqueue(first); 

                if( line == endLineS )
                    Enqueue(second); 

                // deal with spilled  target register, generate store instruction
                int offset = setOffset(ptr->Target); 

                instr * newInstr = (instr*)malloc(sizeof(instr)); 
                newInstr->opcode = STOREAI; 
                newInstr->immediate = offset; 
                newInstr->First  = (char*)malloc(sizeof(char)*5); 
                newInstr->Target = (char*)malloc(sizeof(char)*5);

                //set registers
                sscanf(f1, "%s", newInstr->First); 
                sscanf("r0", "%s", newInstr->Target); 

                //insert instruction after ptr
                newInstr->next = ptr->next; 
                ptr->next->previous = newInstr; 
                ptr->next = newInstr; 
                newInstr->previous = ptr; 

                // replace target register of ptr
                bzero(ptr->Target, strlen(ptr->Target)); 
                sscanf(f1,"%s", ptr->Target); 

                ptr = ptr->next->next; 
                line++; 
                continue;  
            }

            // First physical, second Virtual, target physical. Deal with left operands first, LoadAI instr for second
            else if ( !Spilled(ptr->First) && Spilled(ptr->Second) && !Spilled(ptr->Target) ){

                // lets get the virtual register(second operand) out of the way
                int offset = getOffset(ptr->Second); 
                // generate a LOADAI instruction 
                instr * newInstr = (instr*)malloc(sizeof(instr)); 
                newInstr->opcode = LOADAI; 
                newInstr->immediate = offset; 
                newInstr->First  = (char*)malloc(sizeof(char)*5); 
                newInstr->Target = (char*)malloc(sizeof(char)*5); 

                //set registers
                sscanf("r0", "%s", newInstr->First); 
                sscanf(f1, "%s", newInstr->Target); 

                // modify ptr second register
                bzero(ptr->Second, strlen(ptr->Second)); 
                sscanf(f1, "%s", ptr->Second); 

                //insert before ptr
                instr * temp = ptr->previous; 
                newInstr->next = ptr;
                ptr->previous = newInstr; 
                temp->next = newInstr;
                newInstr->previous = temp; 

                // deal with physical register, start with left operand(First)
                int endLineF = GetEndLine(ptr->First); 
                char * first = getMappedReg(ptr->First); 

                bzero(ptr->First, strlen(ptr->First)); 
                sscanf(first, "%s", ptr->First); 

                if ( endLineF == line )
                    Enqueue(first); 

                // deal with target register

                int endLineT = GetEndLine(ptr->Target); 
                char * target = Dequeue(); 
                setPhysicalReg(ptr->Target, target); 

                //replace register
                bzero(ptr->Target, strlen(ptr->Target)); 
                sscanf(target, "%s", ptr->Target); 

                // check if live on exit

                if ( endLineT == line )
                    Enqueue(target); 

                ptr = ptr->next; 
                line++; 
                continue; 

            }

            // First is virtual second is physical, target is physical. Work left to right
            //handle LOADAI of first reg, set second reg, set target
            else if ( Spilled(ptr->First) && !Spilled(ptr->Second) && !Spilled(ptr->Target) ){
                //get offset of the first register
                int offset = getOffset(ptr->First); 
                // generate a LOADAI instruction 
                instr * newInstr = (instr*)malloc(sizeof(instr)); 
                newInstr->opcode = LOADAI; 
                newInstr->immediate = offset; 
                newInstr->First  = (char*)malloc(sizeof(char)*5); 
                newInstr->Target = (char*)malloc(sizeof(char)*5); 

                //set registers
                sscanf("r0", "%s", newInstr->First); 
                sscanf(f1, "%s", newInstr->Target); 

                // modify ptr second register
                bzero(ptr->First, strlen(ptr->First)); 
                sscanf(f1, "%s", ptr->First); 

                //insert before ptr
                instr * temp = ptr->previous; 
                newInstr->next = ptr;
                ptr->previous = newInstr; 
                temp->next = newInstr;
                newInstr->previous = temp; 

                //deal with physical registers(Second)
                int endLineS = GetEndLine(ptr->Second); 
                char * second = getMappedReg(ptr->Second); 

                bzero(ptr->Second, strlen(ptr->Second)); 
                sscanf(second, "%s", ptr->Second); 

                if ( endLineS == line )
                    Enqueue(second);

                // deal with the target register
                int endLineT = GetEndLine(ptr->Target); 
                char * target = Dequeue(); 
                setPhysicalReg(ptr->Target, target); 

                //replace register
                bzero(ptr->Target, strlen(ptr->Target)); 
                sscanf(target, "%s", ptr->Target); 

                // check if live on exit

                if ( endLineT == line )
                    Enqueue(target); 

                //move to next instruction
                ptr = ptr->next; 
                line++; 
                continue;
            }

            // virtual, physical  => virtual  loadAI first, set second, storeAI target
            else if ( Spilled(ptr->First) && !Spilled(ptr->Second) && Spilled(ptr->Target) ){
                // handle physical register first
                // get endLine and Register
                int endLineS  = GetEndLine(ptr->Second); 
                char * second = getMappedReg(ptr->Second);

                // modify ptr
                bzero(ptr->Second, strlen(ptr->Second)); 
                sscanf(second, "%s", ptr->Second);

                // check if live on exit
                if ( endLineS == line ) 
                    Enqueue(second); 

                // deal with Virtual Registers
                // generate LOADAI instruction for the first register

                int offset1 = getOffset(ptr->First); 
                instr *newInstr1  = (instr*)malloc(sizeof(instr)); 
                newInstr1->opcode = LOADAI;
                newInstr1->immediate = offset1;  
                newInstr1->First  = (char*)malloc(sizeof(char)*5); 
                newInstr1->Target = (char*)malloc(sizeof(char)*5); 

                // modify registers

                sscanf("r0", "%s", newInstr1->First); 
                sscanf(f1, "%s", newInstr1->Target); 

                // insert before ptr

                instr * temp1 = ptr->previous; 
                newInstr1->next = ptr; 
                ptr->previous = newInstr1; 
                temp1->next = newInstr1; 
                newInstr1->previous = temp1; 


                // generate STOREAI instruction for spilling target register

                int offset2 = setOffset(ptr->Target); 
                instr *newInstr2 = (instr*)malloc(sizeof(instr)); 
                newInstr2->opcode = STOREAI; 
                newInstr2->immediate = offset2; 
                newInstr2->First  = (char*)malloc(sizeof(char)*5);
                newInstr2->Target = (char*)malloc(sizeof(char)*5); 

                //modify registers

                sscanf(f2, "%s", newInstr2->First);
                sscanf("r0", "%s", newInstr2->Target); 


                //insert after ptr

                newInstr2->next = ptr->next; 
                ptr->next->previous = newInstr2; 
                ptr->next = newInstr2; 
                newInstr2->previous = ptr; 

                // modify first and target registers in ptr

                bzero(ptr->First, strlen(ptr->First)); 
                bzero(ptr->Target, strlen(ptr->Target)); 
                sscanf(f1, "%s", ptr->First); 
                sscanf(f2, "%s", ptr->Target);  

                // adavance ptr past the newly inserted instruction and continue

                ptr = ptr->next->next; 
                line++; 
                continue; 

            }

            // physical, virtual => virtual  set first, loadAI second, storeAI Target
            else if ( !Spilled(ptr->First) && Spilled(ptr->Second) && Spilled(ptr->Target) ){
                // deal with the physical register first
                // get endLine and mapped register
                int endLineF = GetEndLine(ptr->First); 
                char * first = getMappedReg(ptr->First); 

                //replace
                bzero(ptr->First, strlen(ptr->First)); 
                sscanf(first, "%s", ptr->First); 

                // check if live on exit and replace
                if ( endLineF == line )
                    Enqueue(first); 


                // deal with virtual registers 

                // generate LOADAI instruction for the Second register
                int offset1 = getOffset(ptr->Second); 
                instr *newInstr1  = (instr*)malloc(sizeof(instr)); 
                newInstr1->opcode = LOADAI;
                newInstr1->immediate = offset1;  
                newInstr1->First  = (char*)malloc(sizeof(char)*5); 
                newInstr1->Target = (char*)malloc(sizeof(char)*5);     

                // modify registers
                sscanf("r0", "%s", newInstr1->First); 
                sscanf(f1, "%s", newInstr1->Target); 

                // insert before ptr
                instr * temp1 = ptr->previous; 
                newInstr1->next = ptr; 
                ptr->previous = newInstr1; 
                temp1->next = newInstr1; 
                newInstr1->previous = temp1; 

                // generate STOREAI instruction for spilling target register
                int offset2 = setOffset(ptr->Target); 
                instr *newInstr2 = (instr*)malloc(sizeof(instr)); 
                newInstr2->opcode = STOREAI; 
                newInstr2->immediate = offset2; 
                newInstr2->First  = (char*)malloc(sizeof(char)*5);
                newInstr2->Target = (char*)malloc(sizeof(char)*5); 

                //modify registers

                sscanf(f2, "%s", newInstr2->First);
                sscanf("r0", "%s", newInstr2->Target); 


                //insert after ptr

                newInstr2->next = ptr->next; 
                ptr->next->previous = newInstr2; 
                ptr->next = newInstr2; 
                newInstr2->previous = ptr; 


                //modify the second registers and target registers of ptr

                bzero(ptr->Second, strlen(ptr->Second)); 
                bzero(ptr->Target, strlen(ptr->Target)); 
                sscanf(f1, "%s", ptr->Second); 
                sscanf(f2, "%s", ptr->Target); 

                // advance ptr past the newly inserted instruction and continue

                ptr = ptr->next->next; 
                line++; 
                continue;             
            }


            else{
                // generate LOADAI instruction for the first register
                int offset1 = getOffset(ptr->First); 

                instr * newInstr1 = (instr*)malloc(sizeof(instr)); 
                newInstr1->opcode = LOADAI; 
                newInstr1->immediate = offset1; 
                newInstr1->First  = (char*)malloc(sizeof(char)*5); 
                newInstr1->Target = (char*)malloc(sizeof(char)*5);  

                // set registers

                sscanf("r0","%s", newInstr1->First); 
                sscanf(f1, "%s", newInstr1->Target); 

                //insert instruction before ptr

                instr * temp1 = ptr->previous; 
                newInstr1->next = ptr; 
                ptr->previous = newInstr1; 
                temp1->next = newInstr1; 
                newInstr1->previous = temp1; 


                // generate LOADAI instruction for the second register

                int offset2 = getOffset(ptr->Second); 

                instr * newInstr2 = (instr*)malloc(sizeof(instr)); 
                newInstr2->opcode = LOADAI; 
                newInstr2->immediate = offset2; 
                newInstr2->First  = (char*)malloc(sizeof(char)*5); 
                newInstr2->Target = (char*)malloc(sizeof(char)*5); 

                //set registers

                sscanf("r0", "%s", newInstr2->First); 
                sscanf(f2, "%s", newInstr2->Target); 

                // insert instruction before ptr

                instr * temp2 = ptr->previous; 
                newInstr2->next = ptr; 
                ptr->previous = newInstr2; 
                temp2->next = newInstr2; 
                newInstr2->previous = temp2;  

                // generate StoreAI instruction for the target register and insert it after ptr

                int offset3 = setOffset(ptr->Target); 

                instr * newInstr3 = (instr*)malloc(sizeof(instr)); 
                newInstr3->opcode = STOREAI; 
                newInstr3->immediate = offset3; 
                newInstr3->First  = (char*)malloc(sizeof(char)*5); 
                newInstr3->Target = (char*)malloc(sizeof(char)*5); 

                // set registers

                sscanf(f1, "%s", newInstr3->First); 
                sscanf("r0", "%s", newInstr3->Target);

                // insert instruction after ptr

                newInstr3->next = ptr->next; 
                ptr->next->previous = newInstr3; 
                ptr->next = newInstr3; 
                newInstr3->previous = ptr; 


                // modify registers of ptr 
                bzero(ptr->First, strlen(ptr->First)); 
                bzero(ptr->Second, strlen(ptr->Second)); 
                bzero(ptr->Target, strlen(ptr->Target)); 
                sscanf(f1, "%s", ptr->First);
                sscanf(f2, "%s", ptr->Second); 
                sscanf(f1, "%s", ptr->Target); 

                //advance ptr to move past the newly inserted instruction and continue

                ptr = ptr->next->next; 
                line++; 
                continue;

            }



        }


        else {

            ptr = ptr->next;
            line++; 
            continue; 
        }


    } // end main while loop

}



// begin functions for bottom up allocator 

// function to computing the next farthest use ,  returns the register with the next farthest use, to be spilled

char * getMax (){
    myRegisters * max = currentlyMapped->head; 
    myRegisters * ptr = currentlyMapped->head;

    while ( ptr != NULL ){

        if( ptr->NextUse > max->NextUse ){

            max = ptr; 
        }

        ptr = ptr->next; 
    } 

    return max->regType; 
}

void getNextUse( myRegisters* reg, instr * curr ){

    instr * ptr = curr; 
    int nextUse = 0; 

    while ( ptr != NULL ){

        if( isContained(ptr, reg->regType) )
            break; 

        ptr = ptr->next; 
        nextUse++; 
    }

    if ( ptr == NULL ){
        reg->NextUse = 1000000;
        return;  
    }

    reg->NextUse = nextUse; 
    return; 
}

char * ComputeNextUse(instr * curr ){

    myRegisters * ptr = currentlyMapped->head; 

    while ( ptr != NULL ){

        getNextUse(ptr, curr); 

        ptr = ptr->next; 
    }

    // return the register with the max
    // we will spill it and remove it from the pool of registers mapped, i need the spill val sooo

    return getMax();  //get the farthest use in the future

}


int noMore ( char * reg ){  // tells me that a register is dead, no need to spill

    myRegisters * ptr = currentlyMapped->head; 

    while ( ptr != NULL ){

        if ( strcmp(reg, ptr->regType) == 0 )
            break; 

        ptr = ptr->next; 
    }

    if ( ptr->NextUse == 1000000 )
        return 1; 

    return 0; 

}


void removeFromList ( char * reg ){   // removes a virtual register from list of registers currently mapped, adds its physical register back to the pool of physical registers

    if ( currentlyMapped->head == NULL )
        return; 

    if( strcmp(currentlyMapped->head->regType, reg) == 0 && currentlyMapped->head->next == NULL ){

        myRegisters * delete = currentlyMapped->head;
        char * insert = delete->physicalReg; 
        Enqueue(insert);           // add its physical register back to the pool of registers to be used 
        currentlyMapped->head = NULL; 
        currentlyMapped->tail = NULL; 
        free(delete); 
        return; 
    }

    myRegisters * prev = NULL; 
    myRegisters * ptr = currentlyMapped->head; 

    while ( ptr != NULL ){

        if ( strcmp(ptr->regType, reg) == 0 )
            break; 

        prev = ptr; 
        ptr = ptr->next; 
    }

    if ( ptr == NULL )
        return; 

    if ( ptr->next == NULL ){   // last element
        char * insert = ptr->physicalReg; 
        Enqueue(insert);           // add its physical back to the pool of physical registers
        currentlyMapped->tail = prev;  // modify tail
        currentlyMapped->tail->next = NULL; 
        free(ptr); 
        return; 
    }

    if ( prev == NULL ){

        char * insert = ptr->physicalReg; 
        Enqueue(insert); 
        currentlyMapped->head = currentlyMapped->head->next; 
        free(ptr); 
        return;  
    }

    char * insert = ptr->physicalReg; 
    prev->next = ptr->next; 
    Enqueue(insert);   //add back to the pool of physical registers 
    free(ptr); 
    return;

}



// after we dequeue its physical register
void addToList ( char * reg, char * physicalReg ){     // adds virtual register to list of registers currently mapped
   // printf("%s\n", physicalReg);
    myRegisters * ptr = myList->head; 

    while ( ptr != NULL ){

        if( strcmp(ptr->regType, reg) == 0 ){
            break; 
        }

        ptr = ptr->next; 
    }

    myRegisters * newReg = (myRegisters*)malloc(sizeof(myRegisters)); 
    memcpy(newReg, ptr, sizeof(myRegisters)); 
    newReg->physicalReg = (char*)malloc(sizeof(char)*5); 
    sscanf(physicalReg, "%s", newReg->physicalReg); 

   // printf("This is the new Register:  %s\n", newReg->physicalReg );

    if( currentlyMapped->head == NULL ){
        currentlyMapped->head = newReg;
        currentlyMapped->tail = newReg; 
        currentlyMapped->tail->next = NULL; 
        return; 
    }

    currentlyMapped->tail->next = newReg; 
    currentlyMapped->tail = newReg; 
    currentlyMapped->tail->next = NULL; 
    return; 
}


char * Mapped ( char * reg ){  // checks if a register currently has a mapping

    if( currentlyMapped->head == NULL )
        return 0; 

    myRegisters * ptr = currentlyMapped->head;

    while ( ptr != NULL ){

        if ( strcmp( ptr->regType, reg ) == 0 ){
            return ptr->physicalReg; 
        }

        ptr = ptr->next; 
    } 

    return 0; 
}



void BottomUpAllocator ( myInstrList * list ) {

    instr * ptr = list->head->next; 

    if ( strcmp(ptr->Target, "r0") == 0 ){

        ptr = ptr->next;                        // ignore the first register if it is a load to r0 
    }


    while ( ptr != NULL ){
        // loading an immidiate to a register
        if ( ptr->opcode == LOADI ){

            char * map1 = Dequeue(); 

            if ( map1 ){   // there is a free register

                addToList(ptr->Target, map1);     // add to list of mapped registers, modify instr and continue
                bzero(ptr->Target, strlen(ptr->Target)); 
                sscanf(map1, "%s", ptr->Target);
                ptr = ptr->next; 
                continue; 
            }

            else{   // no available registers
                //compute next use
                char * evicted = ComputeNextUse(ptr);
                // spill resulting register
                int offset = setOffset(evicted); 
                // remove from list off mapped registers
                removeFromList(evicted); 
                // pop a physical register from queue
                char * map2 = Dequeue(); 
                addToList(ptr->Target, map2); 
                //replace instruction
                bzero(ptr->Target, strlen(ptr->Target)); 
                sscanf(map2, "%s", ptr->Target); 
                // insert a store instruction before ptr
                instr * newInstr = (instr*)malloc(sizeof(instr)); 
                newInstr->immediate = offset; 
                newInstr->opcode = STOREAI; 
                newInstr->First = (char*)malloc(sizeof(char)*5); 
                newInstr->Target = (char*)malloc(sizeof(char)*5); 

                //modify registers
                sscanf(map2, "%s", newInstr->First); 
                sscanf("r0", "%s", newInstr->Target); 

                //insert new instruction before ptr
                instr * temp = ptr->previous; 
                newInstr->next = ptr; 
                ptr->previous = newInstr; 
                temp->next = newInstr; 
                newInstr->previous = temp; 

                // advance ptr and continue

                ptr = ptr->next; 
                continue; 
            }

        }

        // this is a store operation 
        else if ( ptr->opcode == STORE ){

            char *p1; 
            char *p2; 
            // both registers are currently mapped, just replace and continue
            if( ( p1 = Mapped( ptr->First ) ) != NULL && ( p2 = Mapped(ptr->Target) ) != NULL ){

                bzero(ptr->First, strlen(ptr->First));
                bzero(ptr->Target, strlen(ptr->Target)); 
                sscanf(p1, "%s", ptr->First); 
                sscanf(p2, "%s", ptr->Target); 
                ptr = ptr->next; 
                continue; 
            }

            // first is mapped and second is not
            else if ( ( p1 = Mapped( ptr->First ) ) != NULL && ( p2 = Mapped(ptr->Target) ) == NULL ){

                // replace register for first right away 
                bzero(ptr->First, strlen(ptr->First));
                sscanf(p1, "%s", ptr->First); 

                // first get the offset of the register that is not currently mapped
                int offset = getOffset(ptr->Target);

                // first lets see if we have something in our queue

                p2 = Dequeue(); 

                if ( p2 ){   // there is an available physical register in our queue

                    // first add it to the list of currently mapped registers
                    addToList(ptr->Target, p2); 

                    // generate a load instruction 

                    instr* newInstr = (instr*)malloc(sizeof(instr)); 
                    newInstr->opcode = LOADAI; 
                    newInstr->immediate = offset; 
                    newInstr->First = (char*)malloc(sizeof(char)*5); 
                    newInstr->Target = (char*)malloc(sizeof(char)*5); 

                    // modify registers

                    sscanf("r0", "%s", newInstr->First); 
                    sscanf(p2, "%s", newInstr->Target); 

                    // insert instruction before ptr
                    instr * temp = ptr->previous; 
                    newInstr->next = ptr; 
                    ptr->previous = newInstr; 
                    temp->next = newInstr; 
                    newInstr->previous = temp; 

                    //modify target register of ptr
                    bzero(ptr->Target, strlen(ptr->Target)); 
                    sscanf(p2, "%s", ptr->Target); 

                    // increment ptr and move on 
                    ptr = ptr->next; 
                    continue; 
                }

                else {   // no registers in our register queue, need to kick a currently mapped one out

                    char * evicted = ComputeNextUse(ptr); 
                    removeFromList(evicted); // remove the evicted register from the list of currently mapped registers
                    p2 = Dequeue(); // remove the register that was just enqueued from queue
                    addToList(ptr->Target, p2); // add the target register to the list of registers currently mapped
                    //modify registers of ptr
                    bzero(ptr->Target, strlen(ptr->Target)); 
                    sscanf(p2, "%s", ptr->Target);
                    // if the register to be evicted is already spilled, no need to spill it again , simpy overwrite
                    if( isSpilled(evicted) ){
                        //p2 = Dequeue(); 
                        //addToList(ptr->Target, p2);

                        // generate a LOADAI instruction
                        instr * newInstr = (instr*)malloc(sizeof(instr)); 
                        newInstr->immediate = offset; 
                        newInstr->opcode = LOADAI; 
                        newInstr->First  = (char*)malloc(sizeof(char)*5); 
                        newInstr->Target = (char*)malloc(sizeof(char)*5); 

                        // set registers
                        sscanf("r0", "%s", newInstr->First); 
                        sscanf(p2, "%s", newInstr->Target); 

                        // insert before ptr
                        instr * temp = ptr->previous; 
                        newInstr->next = ptr; 
                        ptr->previous = newInstr; 
                        temp->next = newInstr; 
                        newInstr->previous = temp;

                        // move on 
                        ptr = ptr->next; 
                        continue;                          
                    }


                    else {  // if the evicted register is not spilled already, we need to spill it
                        // set the spill offset of the register to be evicted
                        int offset2 = setOffset(evicted);

                        // generate a store instruction 
                        instr * newInstr1 = (instr*)malloc(sizeof(instr)); 
                        newInstr1->immediate = offset2; 
                        newInstr1->opcode = STOREAI; 
                        newInstr1->First  = (char*)malloc(sizeof(char)*5); 
                        newInstr1->Target = (char*)malloc(sizeof(char)*5); 

                        //set registers
                        sscanf(p2, "%s", newInstr1->First); 
                        sscanf("r0", "%s", newInstr1->Target); 

                        // insert before ptr; 
                        instr * temp1 = ptr->previous;
                        newInstr1->next = ptr; 
                        ptr->previous = newInstr1;
                        temp1->next = newInstr1;
                        newInstr1->previous = temp1;

                        // generate a load instruction 

                        instr * newInstr2 = (instr*)malloc(sizeof(instr)); 
                        newInstr2->immediate = offset; 
                        newInstr2->opcode = LOADAI; 
                        newInstr2->First  = (char*)malloc(sizeof(char)*5); 
                        newInstr2->Target = (char*)malloc(sizeof(char)*5); 

                        //set registers
                        sscanf("r0", "%s", newInstr2->First); 
                        sscanf(p2, "%s", newInstr2->Target); 

                        //insert before ptr
                        instr * temp2 = ptr->previous;
                        newInstr2->next = ptr; 
                        ptr->previous = newInstr2;
                        temp2->next = newInstr2;
                        newInstr2->previous = temp2;  

                        // move on 
                        ptr = ptr->next; 
                        continue;                       
                    }


                }

            }


            // second is mapped and the fiest is not mapped
            else if ( ( p1 = Mapped( ptr->First ) ) == NULL && ( p2 = Mapped(ptr->Target) ) != NULL ) {

                // replace register for first right away 
                bzero(ptr->Target, strlen(ptr->Target));
                sscanf(p2, "%s", ptr->Target); 

                // first get the offset of the register that is not currently mapped
                int offset = getOffset(ptr->First);

                // first lets see if we have something in our queue

                p1 = Dequeue(); 

                if ( p1 ){   // there is an available physical register in our queue

                    // first add it to the list of currently mapped registers
                    addToList(ptr->First, p1); 

                    // generate a load instruction 

                    instr* newInstr = (instr*)malloc(sizeof(instr)); 
                    newInstr->opcode = LOADAI; 
                    newInstr->immediate = offset; 
                    newInstr->First = (char*)malloc(sizeof(char)*5); 
                    newInstr->Target = (char*)malloc(sizeof(char)*5); 

                    // modify registers

                    sscanf("r0", "%s", newInstr->First); 
                    sscanf(p1, "%s", newInstr->Target); 

                    // insert instruction before ptr
                    instr * temp = ptr->previous; 
                    newInstr->next = ptr; 
                    ptr->previous = newInstr; 
                    temp->next = newInstr; 
                    newInstr->previous = temp; 

                    //modify target register of ptr
                    bzero(ptr->First, strlen(ptr->First)); 
                    sscanf(p1, "%s", ptr->First); 

                    // increment ptr and move on 
                    ptr = ptr->next; 
                    continue; 
                }

                else {   // no registers in our register queue, need to kick a currently mapped one out

                    char * evicted = ComputeNextUse(ptr); 
                    removeFromList(evicted); // remove the evicted register from the list of currently mapped registers
                    p2 = Dequeue(); // remove the register that was just enqueued from queue
                    addToList(ptr->First, p2); // add the first register to the list of registers currently mapped
                    //modify registers of ptr
                    bzero(ptr->First, strlen(ptr->First)); 
                    sscanf(p2, "%s", ptr->First);
                    // if the register to be evicted is already spilled, no need to spill it again , simpy overwrite
                    if( isSpilled(evicted) ){
                        //p2 = Dequeue(); 
                        //addToList(ptr->Target, p2);

                        // generate a LOADAI instruction
                        instr * newInstr = (instr*)malloc(sizeof(instr)); 
                        newInstr->immediate = offset; 
                        newInstr->opcode = LOADAI; 
                        newInstr->First  = (char*)malloc(sizeof(char)*5); 
                        newInstr->Target = (char*)malloc(sizeof(char)*5); 

                        // set registers
                        sscanf("r0", "%s", newInstr->First); 
                        sscanf(p2, "%s", newInstr->Target); 

                        // insert before ptr
                        instr * temp = ptr->previous; 
                        newInstr->next = ptr; 
                        ptr->previous = newInstr; 
                        temp->next = newInstr; 
                        newInstr->previous = temp;

                        // move on 
                        ptr = ptr->next; 
                        continue;                          
                    }


                    else {  // if the evicted register is not spilled already, we need to spill it
                        // set the spill offset of the register to be evicted
                        int offset2 = setOffset(evicted);

                        // generate a store instruction 
                        instr * newInstr1 = (instr*)malloc(sizeof(instr)); 
                        newInstr1->immediate = offset2; 
                        newInstr1->opcode = STOREAI; 
                        newInstr1->First  = (char*)malloc(sizeof(char)*5); 
                        newInstr1->Target = (char*)malloc(sizeof(char)*5); 

                        //set registers
                        sscanf(p2, "%s", newInstr1->First); 
                        sscanf("r0", "%s", newInstr1->Target); 

                        // insert before ptr; 
                        instr * temp1 = ptr->previous;
                        newInstr1->next = ptr; 
                        ptr->previous = newInstr1;
                        temp1->next = newInstr1;
                        newInstr1->previous = temp1;

                        // generate a load instruction 

                        instr * newInstr2 = (instr*)malloc(sizeof(instr)); 
                        newInstr2->immediate = offset; 
                        newInstr2->opcode = LOADAI; 
                        newInstr2->First  = (char*)malloc(sizeof(char)*5); 
                        newInstr2->Target = (char*)malloc(sizeof(char)*5); 

                        //set registers
                        sscanf("r0", "%s", newInstr2->First); 
                        sscanf(p2, "%s", newInstr2->Target); 

                        //insert before ptr
                        instr * temp2 = ptr->previous;
                        newInstr2->next = ptr; 
                        ptr->previous = newInstr2;
                        temp2->next = newInstr2;
                        newInstr2->previous = temp2;  

                        // move on 
                        ptr = ptr->next; 
                        continue;                       
                    }


                }

            }// end else if

                        // they are both virtual, need to kick out two registers
            else {

                // lets dequeue two registers
                p1 = Dequeue(); 
                p2 = Dequeue();
                // there are two free registers
                if ( p1 && p2 ){
                    // add both registers to list of virtual registers
                    // get their offsets for LOADAI instructions
                    int offset1 = getOffset(ptr->First); 
                    int offset2 = getOffset(ptr->Target); 
                    addToList(ptr->First, p1); 
                    addToList(ptr->Second, p2); 
                    bzero(ptr->First, strlen(ptr->First)); 
                    bzero(ptr->Target, strlen(ptr->Target)); 
                    sscanf(p1, "%s", ptr->First); 
                    sscanf(p2, "%s", ptr->Target);

                    //insert first LOADAI instruction 
                    instr * newInstr1 = (instr*)malloc(sizeof(instr)); 
                    newInstr1->opcode = LOADAI; 
                    newInstr1->immediate = offset1; 
                    newInstr1->First  = (char*)malloc(sizeof(char)*5); 
                    newInstr1->Target = (char*)malloc(sizeof(char)*5); 

                    // modify registers
                    sscanf("r0", "%s", newInstr1->First); 
                    sscanf(p1, "%s", newInstr1->Target); 

                    //insert before ptr
                    instr * temp1 = ptr->previous; 
                    newInstr1->next = ptr; 
                    ptr->previous = newInstr1; 
                    temp1->next = newInstr1; 
                    newInstr1->previous = temp1; 

                    //Insert Second LOADAI instruction 
                    instr * newInstr2 = (instr*)malloc(sizeof(instr)); 
                    newInstr2->opcode = LOADAI; 
                    newInstr2->immediate = offset2; 
                    newInstr2->First  = (char*)malloc(sizeof(char)*5); 
                    newInstr2->Target = (char*)malloc(sizeof(char)*5);

                    // modify registers
                    sscanf("r0", "%s", newInstr2->First); 
                    sscanf(p2, "%s", newInstr2->Target); 

                    //insert before ptr
                    instr * temp2 = ptr->previous;
                    newInstr2->next = ptr; 
                    ptr->previous = newInstr2; 
                    temp2->next = newInstr2; 
                    newInstr2->previous = temp2;  
                    // move on 
                    ptr = ptr->next; 
                    continue;  
                }
                // only one available register 
                // give the availabe register to the first operan, kick out a register for the second operand
                else if ( p1 ){
                    //add first register to list and replace
                    int offset1 = getOffset(ptr->First); 
                    int offset2 = getOffset(ptr->Target); 
                    addToList(ptr->First, p1); 
                    bzero(ptr->First, strlen(ptr->First)); 
                    sscanf(p1, "%s", ptr->First); 

                    //insert LOADAI instruction for the first operand
                    instr * newInstr1 = (instr*)malloc(sizeof(instr)); 
                    newInstr1->opcode = LOADAI; 
                    newInstr1->immediate = offset1; 
                    newInstr1->First = (char*)malloc(sizeof(char)*5); 
                    newInstr1->Target = (char*)malloc(sizeof(char)*5); 

                    // modify registers
                    sscanf("r0", "%s", newInstr1->First); 
                    sscanf(p1, "%s", newInstr1->Target); 

                    //insert before ptr
                    instr * temp1 = ptr->previous; 
                    newInstr1->next = ptr; 
                    ptr->previous = newInstr1; 
                    temp1->next = newInstr1; 
                    newInstr1->previous = temp1; 


                    // now lets  handle kicking out a register for the second operand
                    char * evicted = ComputeNextUse(ptr);  // get the register to be evicted; 
                    removeFromList(evicted); 

                    p2 = Dequeue();  // get the register that was just enqueued
                    addToList(ptr->Target, p2); // add this register to list of registers currently mapped
                    bzero(ptr->Target, strlen(ptr->Target)); //set register for ptr
                    sscanf(p2, "%s", ptr->Target); 
                    // now lets see whether we need a STOREAI instruction for the evicted register
                    // evicted is alread spilled, just need a LOADAI instruction for target
                    if ( isSpilled(evicted) ){

                        instr * newInstr2 = (instr*)malloc(sizeof(instr)); 
                        newInstr2->opcode = LOADAI; 
                        newInstr2->immediate = offset2; 
                        newInstr2->First  = (char*)malloc(sizeof(char)*5); 
                        newInstr2->Target = (char*)malloc(sizeof(char)*5); 

                        // set registers
                        sscanf("r0", "%s", newInstr2->First); 
                        sscanf(p2, "%s", newInstr2->Target); 

                        //insert before ptr
                        instr * temp2 = ptr->previous; 
                        newInstr2->next = ptr; 
                        ptr->previous = newInstr2; 
                        temp2->next = newInstr2; 
                        newInstr2->previous = temp2; 

                        // increment ptr and continue;
                        ptr = ptr->next; 
                        continue;  
                    }
                    // the evicted register has not been spilled
                    else {

                        int offset3 = setOffset(evicted); 

                        // generate a STOREAI instruction 
                        instr * newInstr3 = (instr*)malloc(sizeof(instr)); 
                        newInstr3->opcode = STOREAI; 
                        newInstr3->immediate = offset3; 
                        newInstr3->First  = (char*)malloc(sizeof(char)*5); 
                        newInstr3->Target = (char*)malloc(sizeof(char)*5); 

                        // set registers
                        sscanf(p2, "%s", newInstr3->First);
                        sscanf("r0", "%s", newInstr3->Target); 

                        //insert before ptr
                        instr * temp3 = ptr->previous; 
                        newInstr3->next = ptr; 
                        ptr->previous = newInstr3; 
                        temp3->next = newInstr3; 
                        newInstr3->previous = temp3; 

                        // generate a LOADAI instruction and insert before ptr
                        instr * newInstr2 = (instr*)malloc(sizeof(instr)); 
                        newInstr2->opcode = LOADAI; 
                        newInstr2->immediate = offset2; 
                        newInstr2->First  = (char*)malloc(sizeof(char)*5); 
                        newInstr2->Target = (char*)malloc(sizeof(char)*5); 

                        // set registers
                        sscanf("r0", "%s", newInstr2->First); 
                        sscanf(p2, "%s", newInstr2->Target); 

                        //insert before ptr
                        instr * temp2 = ptr->previous; 
                        newInstr2->next = ptr; 
                        ptr->previous = newInstr2; 
                        temp2->next = newInstr2; 
                        newInstr2->previous = temp2; 

                        // increment ptr and continue;
                        ptr = ptr->next; 
                        continue;                          
                    }

                }

                else {   // no available registers, need to kick out two registers

                    char * evicted1 = ComputeNextUse(ptr); 
                    char * evicted2 = ComputeNextUse(ptr); 
                    removeFromList(evicted1);
                    removeFromList(evicted2); 
                    p1 = Dequeue(); 
                    p2 = Dequeue();
                    // check if the first evicted has been spilled
                    if ( !isSpilled(evicted1) ){

                        int offset = setOffset(evicted1); 

                        // generated a store instruction 

                        instr * newInstr = (instr*)malloc(sizeof(instr)); 
                        newInstr->opcode = STOREAI; 
                        newInstr->immediate = offset; 
                        newInstr->First  = (char*)malloc(sizeof(char)*5); 
                        newInstr->Target = (char*)malloc(sizeof(char)*5); 

                        //set registers
                        sscanf(p1, "%s", newInstr->First); 
                        sscanf("r0", "%s", newInstr->Target);

                        //insert before ptr
                        instr * temp = ptr->previous; 
                        newInstr->next = ptr; 
                        ptr->previous = newInstr; 
                        temp->next = newInstr; 
                        newInstr->previous = temp;  
                    }

                    if ( !isSpilled(evicted2) ){

                        int offset = setOffset(evicted2); 

                        // generated a store instruction 

                        instr * newInstr = (instr*)malloc(sizeof(instr)); 
                        newInstr->opcode = STOREAI; 
                        newInstr->immediate = offset; 
                        newInstr->First  = (char*)malloc(sizeof(char)*5); 
                        newInstr->Target = (char*)malloc(sizeof(char)*5); 

                        //set registers
                        sscanf(p2, "%s", newInstr->First); 
                        sscanf("r0", "%s", newInstr->Target);

                        //insert before ptr
                        instr * temp = ptr->previous; 
                        newInstr->next = ptr; 
                        ptr->previous = newInstr; 
                        temp->next = newInstr; 
                        newInstr->previous = temp; 

                    }

                    // replace registers at ptr and generate two LOADAI instructions
                    addToList(ptr->First, p1); 
                    addToList(ptr->Second, p2); 
                    int offset1 = getOffset(ptr->First); 
                    int offset2 = getOffset(ptr->Target); 
                    // replace registers
                    bzero(ptr->First, strlen(ptr->First)); 
                    bzero(ptr->Target, strlen(ptr->Target)); 
                    sscanf(p1, "%s", ptr->First); 
                    sscanf(p2, "%s", ptr->Target); 

                    // generate first load instruction 
                    instr * newInstr1 = (instr*)malloc(sizeof(instr)); 
                    newInstr1->opcode = LOADAI; 
                    newInstr1->immediate =offset1; 
                    newInstr1->First  = (char*)malloc(sizeof(char)*5); 
                    newInstr1->Target = (char*)malloc(sizeof(char)*5); 

                    //set registers
                    sscanf("r0", "%s", newInstr1->First); 
                    sscanf(p1, "%s", newInstr1->Target); 

                    //insert before ptr

                    instr * temp1 = ptr->previous; 
                    newInstr1->next = ptr; 
                    ptr->previous = newInstr1; 
                    temp1->next = newInstr1; 
                    newInstr1->previous = temp1;


                    //generate second load instruction 
                    instr * newInstr2 = (instr*)malloc(sizeof(instr)); 
                    newInstr2->opcode = LOADAI; 
                    newInstr2->immediate = offset2; 
                    newInstr2->First  = (char*)malloc(sizeof(char)*5); 
                    newInstr2->Target = (char*)malloc(sizeof(char)*5); 

                    //set registers
                    sscanf("r0", "%s", newInstr2->First); 
                    sscanf(p2, "%s", newInstr2->Target); 

                    //insert before ptr
                    instr *temp2 = ptr->previous; 
                    newInstr2->next = ptr; 
                    ptr->previous = newInstr2; 
                    temp2->next = newInstr2; 
                    newInstr2->previous = temp2; 

                    //increment ptr and move on 
                    ptr = ptr->next; 
                    continue;   
                }

            }

        }

        // load instruction 

        else if ( ptr->opcode == LOAD ){

            //deal with First (opcode) register first
            char * first; 
            // this register already has a mapping
            if( ( first = Mapped(ptr->First) ) != NULL ){

                // check if this register is done
                int done = noMore(ptr->First);

                if (done)
                    removeFromList(ptr->First);  // give back its register if it is done

                // replace register at ptr 
                bzero(ptr->First, strlen(ptr->First)); 
                sscanf(first, "%s", ptr->First); 

            }

            // if it is not mapped, then it has to live somewhere in memory, first check queue for a free register, then kick one out
            else{

                first = Dequeue(); 

                if ( first ){  // there is an available register
                    // add to list off currently mapped registers
                    addToList(ptr->First, first); 
                    int offset = getOffset(ptr->First);  // get its offset in memory
                    // replace register (ptr)
                    bzero(ptr->First, strlen(ptr->First)); 
                    sscanf(first, "%s", ptr->First); 

                    //generate LoadAI instruction 
                    instr * newInstr = (instr*)malloc(sizeof(instr)); 
                    newInstr->opcode = LOADAI; 
                    newInstr->immediate = offset; 
                    newInstr->First  = (char*)malloc(sizeof(char)*5); 
                    newInstr->Target = (char*)malloc(sizeof(char)*5); 

                    //set registers
                    sscanf("r0", "%s", newInstr->First); 
                    sscanf(first, "%s", newInstr->Target); 

                    // insert instruction before ptr 
                    instr * temp = ptr->previous;
                    newInstr->next = ptr; 
                    ptr->previous = newInstr; 
                    temp->next = newInstr; 
                    newInstr->previous = temp; 
                }


                else{  // there is no available register so we need to evict one

                    char * evicted = ComputeNextUse(ptr); 
                    removeFromList(evicted);
                    first = Dequeue(); 

                    if ( !isSpilled(evicted) ){

                        int offset = setOffset(evicted); 

                        // generate STOREAI instruction
                        instr * newInstr = (instr*)malloc(sizeof(instr)); 
                        newInstr->opcode = STOREAI; 
                        newInstr->immediate = offset; 
                        newInstr->First  = (char*)malloc(sizeof(char)*5); 
                        newInstr->Target = (char*)malloc(sizeof(char)*5); 

                        // set registers
                        sscanf(first, "%s", newInstr->First); 
                        sscanf("r0", "%s", newInstr->Target); 

                        // insert before ptr
                        instr * temp = ptr->previous; 
                        newInstr->next = ptr; 
                        ptr->previous = newInstr; 
                        temp->next = newInstr; 
                        newInstr->previous = temp;  
                    }

                    // add the first register to the list
                    addToList(ptr->First, first); 
                    int offset = getOffset(ptr->First); 
                    // modify first register of ptr
                    bzero(ptr->First, strlen(ptr->First)); 
                    sscanf(first, "%s", ptr->First); 

                    //insert LOADAI instruction before ptr
                    instr * newInstr = (instr*)malloc(sizeof(instr)); 
                    newInstr->opcode = LOADAI; 
                    newInstr->immediate = offset; 
                    newInstr->First  = (char*)malloc(sizeof(char)*5); 
                    newInstr->Target = (char*)malloc(sizeof(char)*5); 

                    //set registers
                    sscanf("r0", "%s", newInstr->First); 
                    sscanf(first, "%s", newInstr->Target); 

                    // insert instruction before ptr 
                    instr * temp = ptr->previous;
                    newInstr->next = ptr; 
                    ptr->previous = newInstr; 
                    temp->next = newInstr; 
                    newInstr->previous = temp;

                }

            }
            // will return here for special case (if needed)
            // deal with the left hand side
            char * p1 = Dequeue(); 

            // there is a free register
            if ( p1 ){
                // just add to list and set ptr
                addToList(ptr->Target, p1);
                bzero(ptr->Target, strlen(ptr->Target)); 
                sscanf(p1, "%s", ptr->Target);

                //move on to next instruction 
                ptr = ptr->next; 
                continue; 
            }

            // there is no available register, need to kick out one
            else{

                char * evicted = ComputeNextUse(ptr); 
                removeFromList(evicted);  // remove evicted register from list of currently mapped registers
                char * target = Dequeue(); 

                if ( !isSpilled(evicted) ){  // evicted register is not currently in memory 

                    int offset = setOffset(evicted); 

                    instr * newInstr = (instr*)malloc(sizeof(instr)); 
                    newInstr->opcode = STOREAI; 
                    newInstr->immediate = offset; 
                    newInstr->First  = (char*)malloc(sizeof(char)*5); 
                    newInstr->Target = (char*)malloc(sizeof(char)*5); 

                    //set registers
                    sscanf(target, "%s", newInstr->First); 
                    sscanf("r0", "%s", newInstr->Target); 

                    // insert before ptr
                    instr * temp = ptr->previous;
                    newInstr->next = ptr; 
                    ptr->previous = newInstr; 
                    temp->next = newInstr; 
                    newInstr->previous = temp;                    
                }

                //simply replace this instruction 
                addToList(ptr->Target, target);
                bzero(ptr->Target,strlen(ptr->Target)); 
                sscanf(target, "%s", ptr->Target);

                ptr = ptr->next; 
                continue;   
            }

        }

        //three operators
        else if ( ptr->opcode == ADD || ptr->opcode == SUB || ptr->opcode == MULT || ptr->opcode == LSHIFT || ptr->opcode == RSHIFT  ){

            // lets start with the first Operator
            // first lets see if there is already a mapping
            char * first;
            char * second;
            char * target; 
            // this register already has a mapping
            if ( (first = Mapped(ptr->First) ) != NULL ){
                first = Mapped(ptr->First); 
                bzero(ptr->First, strlen(ptr->First)); 
                sscanf(first,"%s", ptr->First); 
            }
            // this register has no mapping, meaning that it is virtual and it currently lives somewhere in memory 
            else{

                first = Dequeue(); 

                if ( first ){  // there is an available register
                    // add to list off currently mapped registers
                    addToList(ptr->First, first); 
                    int offset = getOffset(ptr->First);  // get its offset in memory
                    // replace register (ptr)
                    bzero(ptr->First, strlen(ptr->First)); 
                    sscanf(first, "%s", ptr->First); 

                    //generate LoadAI instruction 
                    instr * newInstr = (instr*)malloc(sizeof(instr)); 
                    newInstr->opcode = LOADAI; 
                    newInstr->immediate = offset; 
                    newInstr->First  = (char*)malloc(sizeof(char)*5); 
                    newInstr->Target = (char*)malloc(sizeof(char)*5); 

                    //set registers
                    sscanf("r0", "%s", newInstr->First); 
                    sscanf(first, "%s", newInstr->Target); 

                    // insert instruction before ptr 
                    instr * temp = ptr->previous;
                    newInstr->next = ptr; 
                    ptr->previous = newInstr; 
                    temp->next = newInstr; 
                    newInstr->previous = temp; 
                }


                else{  // there is no available register so we need to evict one

                    char * evicted = ComputeNextUse(ptr); 
                    removeFromList(evicted);
                    first = Dequeue(); 

                    if ( !isSpilled(evicted) ){

                        int offset = setOffset(evicted); 

                        // generate STOREAI instruction
                        instr * newInstr = (instr*)malloc(sizeof(instr)); 
                        newInstr->opcode = STOREAI; 
                        newInstr->immediate = offset; 
                        newInstr->First  = (char*)malloc(sizeof(char)*5); 
                        newInstr->Target = (char*)malloc(sizeof(char)*5); 

                        // set registers
                        sscanf(first, "%s", newInstr->First); 
                        sscanf("r0", "%s", newInstr->Target); 

                        // insert before ptr
                        instr * temp = ptr->previous; 
                        newInstr->next = ptr; 
                        ptr->previous = newInstr; 
                        temp->next = newInstr; 
                        newInstr->previous = temp;  
                    }

                    // add the first register to the list
                    addToList(ptr->First, first); 
                    int offset = getOffset(ptr->First); 
                    // modify first register of ptr
                    bzero(ptr->First, strlen(ptr->First)); 
                    sscanf(first, "%s", ptr->First); 

                    //insert LOADAI instruction before ptr
                    instr * newInstr = (instr*)malloc(sizeof(instr)); 
                    newInstr->opcode = LOADAI; 
                    newInstr->immediate = offset; 
                    newInstr->First  = (char*)malloc(sizeof(char)*5); 
                    newInstr->Target = (char*)malloc(sizeof(char)*5); 

                    //set registers
                    sscanf("r0", "%s", newInstr->First); 
                    sscanf(first, "%s", newInstr->Target); 

                    // insert instruction before ptr 
                    instr * temp = ptr->previous;
                    newInstr->next = ptr; 
                    ptr->previous = newInstr; 
                    temp->next = newInstr; 
                    newInstr->previous = temp;

                }

            } // end else


            // deal with the second operand
            // this register is a currently mapped register
            if ( ( second = Mapped(ptr->Second) ) != NULL ){

              //  int done = noMore(ptr->Second); 

               // if(done)
                 //   removeFromList(ptr->Second); 
                //printf("This is true.......\n");
                second = Mapped(ptr->Second); 
                //printf("%s.........\n",second );
                bzero(ptr->Second, strlen(ptr->Second)); 
                sscanf(second, "%s", ptr->Second); 

            }

            // this register is not currently mapped, it is a virtual register that needs to be loaded from memory 
            else {

                second = Dequeue(); 

                if ( second ){  // there is an available register
                    // add to list off currently mapped registers
                    addToList(ptr->Second, second); 
                    int offset = getOffset(ptr->Second);  // get its offset in memory
                    // replace register (ptr)
                    bzero(ptr->Second, strlen(ptr->Second)); 
                    sscanf(second, "%s", ptr->Second); 

                    //generate LoadAI instruction 
                    instr * newInstr = (instr*)malloc(sizeof(instr)); 
                    newInstr->opcode = LOADAI; 
                    newInstr->immediate = offset; 
                    newInstr->First  = (char*)malloc(sizeof(char)*5); 
                    newInstr->Target = (char*)malloc(sizeof(char)*5); 

                    //set registers
                    sscanf("r0", "%s", newInstr->First); 
                    sscanf(second, "%s", newInstr->Target); 

                    // insert instruction before ptr 
                    instr * temp = ptr->previous;
                    newInstr->next = ptr; 
                    ptr->previous = newInstr; 
                    temp->next = newInstr; 
                    newInstr->previous = temp; 
                }


                else{  // there is no available register so we need to evict one

                    char * evicted = ComputeNextUse(ptr); 
                    removeFromList(evicted);
                    second = Dequeue(); 

                    if ( !isSpilled(evicted) ){

                        int offset = setOffset(evicted); 

                        // generate STOREAI instruction
                        instr * newInstr = (instr*)malloc(sizeof(instr)); 
                        newInstr->opcode = STOREAI; 
                        newInstr->immediate = offset; 
                        newInstr->First  = (char*)malloc(sizeof(char)*5); 
                        newInstr->Target = (char*)malloc(sizeof(char)*5); 

                        // set registers
                        sscanf(second, "%s", newInstr->First); 
                        sscanf("r0", "%s", newInstr->Target); 

                        // insert before ptr
                        instr * temp = ptr->previous; 
                        newInstr->next = ptr; 
                        ptr->previous = newInstr; 
                        temp->next = newInstr; 
                        newInstr->previous = temp;  
                    }

                    // add the first register to the list
                    addToList(ptr->Second, second); 
                    int offset = getOffset(ptr->Second); 
                    // modify first register of ptr
                    bzero(ptr->Second, strlen(ptr->Second)); 
                    sscanf(second, "%s", ptr->Second); 

                    //insert LOADAI instruction before ptr
                    instr * newInstr = (instr*)malloc(sizeof(instr)); 
                    newInstr->opcode = LOADAI; 
                    newInstr->immediate = offset; 
                    newInstr->First  = (char*)malloc(sizeof(char)*5); 
                    newInstr->Target = (char*)malloc(sizeof(char)*5); 

                    //set registers
                    sscanf("r0", "%s", newInstr->First); 
                    sscanf(second, "%s", newInstr->Target); 

                    // insert instruction before ptr 
                    instr * temp = ptr->previous;
                    newInstr->next = ptr; 
                    ptr->previous = newInstr; 
                    temp->next = newInstr; 
                    newInstr->previous = temp;

                }

            } // end else 


            // deal with last operand
            target = Dequeue(); 

            // there is a free register
            if ( target ){
                // just add to list and set ptr
                addToList(ptr->Target, target);
                bzero(ptr->Target, strlen(ptr->Target)); 
                sscanf(target, "%s", ptr->Target);

                //move on to next instruction 
                ptr = ptr->next; 
                continue; 
            }

            // there is no available register, need to kick out one
            else{

                char * evicted = ComputeNextUse(ptr); 
                removeFromList(evicted);  // remove evicted register from list of currently mapped registers
                target = Dequeue(); // now we must have a free register

                if ( !isSpilled(evicted) ){  // evicted register is not currently in memory 

                    int offset = setOffset(evicted); 

                    instr * newInstr = (instr*)malloc(sizeof(instr)); 
                    newInstr->opcode = STOREAI; 
                    newInstr->immediate = offset; 
                    newInstr->First  = (char*)malloc(sizeof(char)*5); 
                    newInstr->Target = (char*)malloc(sizeof(char)*5); 

                    //set registers
                    sscanf(target, "%s", newInstr->First); 
                    sscanf("r0", "%s", newInstr->Target); 

                    // insert before ptr
                    instr * temp = ptr->previous;
                    newInstr->next = ptr; 
                    ptr->previous = newInstr; 
                    temp->next = newInstr; 
                    newInstr->previous = temp;                    
                }

                //simply replace this instruction 
                addToList(ptr->Target, target);
                bzero(ptr->Target,strlen(ptr->Target)); 
                sscanf(target, "%s", ptr->Target);

                ptr = ptr->next; 
                continue;   
            }            


        } // end



        else {

            ptr = ptr->next; 
        }


    }    // end main while loop 


}








int main(int argc, char *argv[]){

    char * file = argv[3]; 
    int regNum = atoi(argv[1]); 

    char buffer[100]; 

    myList = parsefile(file,buffer);    // get list of registers

    numPhysicalReg = regNum - 2;    // subtract two to account for the feasible registers

    myList->head = mergeSort(myList->head);   // sort the list  

    InstructionList = LoadInstructions(file, buffer); // get list of instructions

    // set up feasible registers
    f1 = (char*)malloc(sizeof(char)*5);     
    f2 = (char*)malloc(sizeof(char)*5); 
    sprintf(f1, "%s%d","r", numPhysicalReg+1); 
    sprintf(f2, "%s%d", "r", numPhysicalReg+2);  

    if ( strcmp("s", argv[2]) == 0 ){

    getVirtualToPhysical(myList, numPhysicalReg); // A list of virtual registers that have a physical mapping
    map();

    SimpleTopDown(InstructionList); 

    }

    else if ( strcmp("t", argv[2]) == 0 ){
        instructionLength = GetInstructionLength();
        spilledSet = (regList*)malloc(sizeof(regList)); 
        spilledSet->head = NULL; 
        spilledSet->tail = NULL;
        computeLiveRanges();
        buildMaxLive();
        buildSpilledSet(); 
        createRegQ(numPhysicalReg);
        LiveRangeTopDown(InstructionList);
       // printInstructionList(InstructionList);
      //  printf("------------------\n");
       // printList(spilledSet); 
       // PrintQueue(); 

       // printList(myList); 

    }

    else if ( strcmp("b", argv[2] ) == 0  ){

        createRegQ(regNum); 
        currentlyMapped = (regList*)malloc(sizeof(regList)); 
        currentlyMapped->head = NULL;   // list of currently mapped registers
        currentlyMapped->tail = NULL; 
        BottomUpAllocator(InstructionList); 
    }

    else{

        printf(" If you have entered a flag for the optinal Allocator, I didn't do it but Have a great day anyways (: \n");
    }

    printInstructionList(InstructionList);
 


    return 0;
}