

myRegisters * mergeOccurrence (myRegisters * L1, myRegisters * L2){


    if ( L1 == NULL )   return  L2; 
    if ( L2 == NULL )   return  L1; 

    if( L1->occurrences < L2->occurrences ){

        L1->next = mergeOccurrence( L1->next, L2 ); 
        return L1; 
    }

    L2->next = mergeOccurrence( L1, L2->next ); 
    return L2;      

}

myRegisters * mergeLiveRange (myRegisters * L1, myRegisters * L2){


    if ( L1 == NULL )   return  L2; 
    if ( L2 == NULL )   return  L1; 

    if( L1->liveRange < L2->liveRange ){

        L1->next = mergeLiveRange( L1->next, L2 ); 
        return L1; 
    }

    L2->next = mergeLiveRange( L1, L2->next ); 
    return L2;      

}


myRegisters * CascadeSplit(myRegisters * head){

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


myRegisters * SortLiveRange (myRegisters * head){

    if(head->next == NULL)    return  head; 

    myRegisters * secondHalf = CascadeSplit(head); 

    myRegisters * firstSort = SortLiveRange(head); 
    myRegisters * secondSort = SortLiveRange(secondHalf); 

    return mergeLiveRange(firstSort, secondSort); 

}

myRegisters * SortOccurrence (myRegisters *head){

    if(head->next == NULL)    return  head; 

    myRegisters * secondHalf = CascadeSplit(head); 

    myRegisters * firstSort = SortOccurrence(head); 
    myRegisters * secondSort = SortOccurrence(secondHalf); 

    return mergeOccurrence(firstSort, secondSort); 

}

myRegisters * CascadeSorting (myRegisters * head){

    head = SortLiveRange(head); 

    return SortOccurrence(head);

}
