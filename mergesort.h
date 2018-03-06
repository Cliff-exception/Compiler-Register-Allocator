

myRegisters * merge (myRegisters * L1, myRegisters * L2){


    if ( L1 == NULL )   return  L2; 
    if ( L2 == NULL )   return  L1; 

    if( L1->occurrences > L2->occurrences ){

        L1->next = merge( L1->next, L2 ); 
        return L1; 
    }

    L2->next = merge( L1, L2->next ); 
    return L2;      

}


myRegisters * splitList(myRegisters * head){

    int size = 0; 

    myRegisters * ptr = head; 

    while( ptr!=NULL ){
        size++; 
        ptr = ptr->next; 
    }

    int mid = (size+1)/2; 

    ptr = head; 

    while ( mid > 1 ){

        ptr = ptr->next; 

        --mid; 
    }

    myRegisters * second = ptr->next; 
    ptr->next = NULL; 

    return second; 
}



myRegisters * mergeSort (myRegisters *head){

    if(head->next == NULL)    return  head; 

    myRegisters * secondHalf = splitList(head); 

    myRegisters * firstSort = mergeSort(head); 
    myRegisters * secondSort = mergeSort(secondHalf); 

    return merge(firstSort, secondSort); 

}

