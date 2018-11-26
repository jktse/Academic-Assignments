/****************
Name: Jacky Ken Tse
Purpose: Header file for list.c
************/


#include "node.h"

typedef struct list{
    NODE * head;
    NODE * tail;
    NODE * current;
    int currentBeyond;
    int listValid;
    int itemCount;
}LIST;

//Checks to see if the list is valid (its here because it will make checking to see if the list that we are going to use are valid)
int isValid(LIST * list);


//Makes a new, empty list, and returns its reference on sucess. Returns a NULL pointer on failure
LIST *ListCreate();

//Returns the total number of items in list
int ListCount(LIST * list);

//Returns a pointer to the first item in list and makes the first item the current item.
void *ListFirst(LIST * list);

//Returns a pointer to the last item in list and makes the last item the current item.
void *ListLast(LIST * list);

//Returns a poiner to the next item in the list and makes it curent. Also if it goes out of bounds return NULL.
void *ListNext(LIST * list);

//Returns a pointer to the previous item in the list and makes it current. Also if it goes out of bounds return NULL.
void *ListPrev(LIST * list);

//Adds the item after current into the list, if sucessful we return 0 else return -1
int ListAdd(LIST * list, void * item);/////////////////////////////////////////////////////////////////////////////////Void Star

//Adds the item before curent into the list, if sucessful we return 0 else return -1.
int ListInsert(LIST * list, void * item);////////////////////////////////////////////////////////////////////////////Void Star

//Adds item to the end of the list and makes it the current one, return 0 on sucess and -1 on failure.
int ListAppend(LIST * list, void * item);/////////////////////////////////////Void Star

//Adds item to the fron of the list and akes it the current one, if sucessfull we reurn 0 else reurn -1.
int ListPrepend(LIST * list, void * item);//////////////////////////////////////Void Star

//Removes current item and makes the next item current;
void *ListRemove(LIST * list);

//adds list2 to the end of list1, the current pointer is et to the current pointer in list1, list2 is also removed
void ListConcat(LIST * list1, LIST * list2);

//Frees an entire list compltely removing it.
void ListFree(LIST * list, void (*itemFree)(NODE * node));

//Return last item and take it out of list. Makes last item current/
void *ListTrim(LIST * list);

//Search the list startin from current, if it is found current becomes that node and is retuned oterwise current is left to were it is and return NULL.
void *ListSearch(LIST * list, int (*comparator)(void * cmp1, void * cmp2), void * comparisonArg);		/////////////////////////////VOID STAR
