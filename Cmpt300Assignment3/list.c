/*************
Name: Jacky Ken Tse
Purpose: To implement routiens for list manipulation
Date: Jan 28 2018
************/

#include <stdio.h>
#include <stdlib.h>

#include "list.h"

#define N 1000															//Change this to have more or less statically allocated memory for LIST
#define M 1000														//Change this to have more or less statically allocated memory for NODES

/**********************Global Variables**********************/


LIST listArray[N];													//Memory for List so we don't have to malloc
NODE nodeArray[M];													//Node space so we don't have to malloc

int initializeStack = 1;											//A one time thing to initialize the stacks to control which nodes are free

int listStackCount = N-1;											//An integer to count the amount of objects in the list stack
int listStack[N];													//An array that will work like a stack telling us which list space in the listArray is open to use

int nodeStackCount = M-1;											//A integer to keep count of how many nodes are in the stack
int nodeStack[M];													//An integer array that will tell us which location to use


/**********************Helper Functions**********************/

int isValid(LIST * list)											//Check if the given list is valid
{
	if(list->listValid == 0)										//The simple case, if listValid is 0 it tells us the list is not initialted and thus not valid
	{
		return(-1);
	}

	LIST * baseList = listArray; 									//An extreme case, where if the listValid variable is 1 but the list is not really initiated
	int value = list -baseList;										//Check to see if the Address differ between 0 and N, if it is in between it means the list is initiated
	if(value >= 0 && value <= N)									//Using listArray and thus in our statically allocated array
	{
		return(0);
	}

	return(-1);
}


void createStacks()													//One time use function, only when we first call listCreate
{																	//Used to initialte the stacks so we can keep track of free nodes
	int i;

	for(i = 0; i < N; i++)											//Filling up the nodes with numbers from 0 - N-1 so we can use those specific space location in out static array
	{
		listStack[N-(i+1)] = i;
		if(i < M)													//Special Case when M is less than N
		{
			nodeStack[M-(i+1)] = i;									//We stop filing up nodeStack when i is bigger than M
		}
	}

	while(i < M)													//For efficientcy reason we did N-1 in the first loop and then if M is > than N we continue on
	{
		nodeStack[M-(i+1)] = i;
		i++;
	}

	initializeStack = 0;											//Only place that modifies initializeStack therefore we only initialize stack once and only once
	return;
}

void *nodePop()
{
	int value = nodeStack[nodeStackCount];							//Popping off from nodeStack to get a value to use
	nodeStackCount--;
	NODE * ret = &nodeArray[value];									//Assigning ret some space to point in nodeArray

	return(ret);
}

void listEmpty(LIST * list, NODE * newNode)
{
	newNode->next = NULL;											//This means newNode will be the first to be put in.
	newNode->prev = NULL;

	list->head = newNode;											//Head,tail,current will have to point to it as it is the first item in the list
	list->tail = newNode;
	list->current = newNode;
	list->itemCount += 1;

	return;
}

void listBeyond(LIST * list, NODE * newNode)
{
	if(list->current == list->head)									//Current is beyond start as current is pointing to head but is out of bounds
	{
		list->head->prev = newNode;									//This means we have to put the node at the begining and update head
		newNode->next = list->head;
		newNode->prev = NULL;
		list->head = newNode;
		list->current = list->head;

	}
	else if(list->current == list->tail)							//Implies beyond end
	{
		list->tail->next = newNode;									//We have to put the node at the very end implying that this is the new tail
		newNode->prev = list->tail;
		newNode->next = NULL;
		list->tail = newNode;
		list->current = list->tail;
	}

	list->itemCount += 1;

	return;
}

/**********************List Manipulation Routines**********************/

LIST *ListCreate()													//Makes a new, empty list, and returns its reference on sucess. Returns a NULL pointer on failure
{
	if(initializeStack == 1)										//Called only once to initialize stacks
	{
		createStacks();
	}

	if(listStackCount < 0)											//If listStackCount is < 0 it means that the stack is out of values to pull from meaning we have exhausted all the space for list
	{
		printf("ListArray is full.\n");								//Sends a message to the user
		return(NULL);
	}

	int value = listStack[listStackCount];							//If there is values we can use, we will get the value on the stack, ie Popping from the stack
	listStackCount--;												//Decrement the numver of valid values

	LIST * ret = &listArray[value];									//Create a list struct pointer pointing to that specific location in the static array we got from the stack

	ret->head = NULL;												//Standard initialization, we will set all nodes to NULL
	ret->tail = NULL;
	ret->current = NULL;

	ret->currentBeyond = 0;											//Current pointer will be pointing to nothing so it is no Beyond the list (this is a grey area for me)
	ret->itemCount = 0;												//No items in list so itemCount is 0
	ret->listValid = 1;												//This section of the listarray is Valid

	return (ret);
}

int ListCount(LIST * list)											//Returns the total number of items in list
{
	return(list->itemCount);										//Done in O(1) as we are constantly counting it
}

void *ListFirst(LIST * list)										//Returns a pointer to the first item in list and makes the first item the current item.
{
	list->current = list->head;										//Current will point to the head as that is the first item
	list->currentBeyond = 0;
	return(list->head->data);
}

void *ListLast(LIST * list)											//Returns a pointer to the last item in list and makes the first item the current item.
{
	list->current = list->tail;										//Current will point to the tail as that is the last item in the list
	list->currentBeyond = 0;
	return(list->tail->data);
}

void *ListNext(LIST * list)											//Returns a poiner to the next item in the list and makes it curent. Also if it goes out of bounds return NULL.
{
	if(list->itemCount == 0)										//If the list is empty then we are unable to find the next item
	{
		printf("List is empty cannot find next item.\n");
		return(NULL);
	}
	else if(list->current->next == NULL)							//The next item is NULL meaning that we are at the tail. or it could mean we are already beyond the end
	{
		printf("You are out of bounds.\n");
		list->currentBeyond = 1;									//Current is still pointing to tail but, we change the flag telling us it is out of bounds.
		return(NULL);												//Notice it is not really out of bounds.
	}
	else if(list->currentBeyond == 1)								//Check to see if we are at the tail, if we are not at the tail but are out of bounds it means we are beyond the start.
	{
		printf("No longer out of bounds.\n");						//So going forward will cause us to be in bounds ie, the head.
		list->currentBeyond = 0;									//Change the flag so we are no longer out of bounds
	}
	else															//The general case.
	{
		list->current = list->current->next;
	}

	return(list->current->data);
}

void *ListPrev(LIST * list)											//Returns a pointer to the previous item in the list and makes it current. Also if it goes out of bounds return NULL.
{
	if(list->itemCount == 0)										//If it is an empty list there is no previous item
	{
		printf("List is empty there is no previous item.\n");		//Send message to user so they know what happend
		return(NULL);
	}
	else if(list->current->prev == NULL)							//There is no previous items meaning you are at the head, or it means you are already out of bounds
	{
		printf("You are out of bounds.\n");							//Changing the counter so the programs knows the current pointer is out of bounds
		list->currentBeyond = 1;
		return(NULL);
	}
	else if(list->currentBeyond == 1)								//If it is out of bounds and is not at the head, it means we are out of bounds at the tail
	{
		printf("No longer out of bounds.\n");						//Going back will take the current pointer to the tail.
		list->currentBeyond = 0;									//Now nolonger out of bounds
	}
	else															//General Case
	{
		list->current = list->current->prev;
	}

	return(list->current->data);
}




int ListAdd(LIST * list, void * item)									//Adds the item after current into the list, if sucessful we return 0 else return -1 //////////////////////////////////////////////////////////////////////////Void Star
{
	if(nodeStackCount < 0)											//If nodeStackCounter is < 0 it means thhere is no more valid node space that we can use.
	{
		printf("No more node spaces ListAdd failed.\n");
		return(-1);
	}

	if(isValid(list) == -1)											//If we check and the list is not valid we cannot add to the list.
	{
		printf("List is not valid\n");
		return(-1);
	}

	NODE * newNode = nodePop();										//Calling helper function to create node
	newNode->data = item;


	if(list->itemCount == 0) 										//List is empty, a corner case
	{
		listEmpty(list, newNode);									//Calling helper function for empty list
	}
	else if (list->currentBeyond == 1)								//When current is beyond the list, another corner case
	{
		listBeyond(list, newNode);									//Calling helper function for current being beyond list
	}
	else 															//If the list is not empty and current is within the list.
	{
		if(list->current->next == NULL)								//If the next node is NULL, it means we are pointing at the tail another corner case
		{
			list->tail = newNode;									//This means newNode is the new tail when we add
			newNode->next = NULL;
		}
		else														//If it is not the we have to force the next node previous point to newnode.
		{
			NODE * tempNode = list->current->next;					//Creating a temp node to adjust the order of the nodes
			tempNode->prev = newNode;								//Causing tempNode-> prev to point to newNode opposed to Current
			newNode->next = tempNode;
		}
		newNode->prev = list->current;
		list->current->next = newNode;
		list->itemCount += 1;
	}

	return(0);
}

int ListInsert(LIST * list, void * item)								//Adds the item before curent into the list, if sucessful we return 0 else return -1. //////////////////////////////////////////////////////////////////////Void Star
{
	if(nodeStackCount < 0)											//No more free node space
	{
		printf("No more node spaces ListInsert failed.\n");
		return(-1);
	}

	if(isValid(list) == -1)											//Checks to see if list is valid
	{
		printf("List is not valid\n");
		return(-1);
	}

	NODE * newNode = nodePop();										//Calling helper function to create node
	newNode->data = item;

	if(list->itemCount == 0)										//List is empty, corner cases
	{
		listEmpty(list, newNode);									//Calling helper functions for empty list
	}
	else if (list->currentBeyond == 1)								//Current is beyond list, corner case
	{
		listBeyond(list, newNode);									//Calling helper function for current being beyond list
	}
	else
	{
		if(list->current->prev == NULL)								//The prev is NULL ie we are at head, corener case
		{
			list->head = newNode;									//This means newNode is the new head when we insert
			newNode->prev = NULL;
		}
		else														//For the generic cases
		{
			NODE * tempNode = list->current->prev;					// temp->newNode and temp<-newNode
			tempNode->next = newNode;
			newNode->prev = tempNode;
		}
		newNode->next = list->current;								//newNode->current and newNode <-current
		list->current->prev = newNode;
		list->itemCount += 1;
	}
	return(0);
}

int ListAppend(LIST * list, void * item)								//Adds item to the end of the list and makes it the current one, return 0 on sucess and -1 on failure/////////////////////////////////////Void Star
{
	if(nodeStackCount < 0)											//No more node spaces
	{
		printf("No more node spaces ListAppend failed.\n");
		return(-1);
	}

	if(isValid(list) == -1)											//Check if list is Valid
	{
		printf("List is not valid\n");
		return(-1);
	}

	NODE * newNode = nodePop();										//Calling helper function to create node
	newNode->data = item;

	if(list->itemCount == 0) 										//It means the list is empty
	{
		listEmpty(list, newNode);									//Calling helper functions for empty list
	}
	else
	{
		list->tail->next = newNode;									//tail -> newNode

		newNode-> prev = list->tail;								//tail <- newNode
		newNode->next = NULL;										//newNode -> NULL

		list->tail = newNode;										//newNode is now new tail
		list->current = list->tail;									//Current points to the new tail ie newNode
		list->itemCount +=1;
	}

	list->currentBeyond = 0;										//If pointer was beyonf list it is nolonger.

	return(0);
}

int ListPrepend(LIST * list, void * item)							//Adds item to the fron of the list and akes it the current one, if sucessfull we reurn 0 else reurn -1.//////////////////////////////////////Void Star
{
	if(nodeStackCount < 0)											//Checks to see if there is free node spaces
	{
		printf("No more node spaces ListPrepend failed.\n");
		return(-1);
	}

	if(isValid(list) == -1)
	{
		printf("List is not valid\n");
		return(-1);
	}

	NODE * newNode = nodePop();										//Calling helper function to create node
	newNode->data = item;

	if(list->itemCount == 0) 										//It means the list is empty
	{
		listEmpty(list, newNode);									//Calling helper functions for empty list
	}
	else
	{
		list->head->prev = newNode;									//newNode <- head

		newNode->next = list->head;									//newNode -> head
		newNode->prev = NULL;										//NULL <- newNode

		list->head = newNode;										//newNode is now new head
		list->current = list->head;									//Update current to be head
		list->itemCount +=1;
	}

	list->currentBeyond = 0;										//If pointer was beyonf list it is nolonger.

	return(0);
}

void *ListRemove(LIST * list)										//Removes current item and makes the next item current;
{
	if(isValid(list) == -1)											//Check to see if list is valid
	{
		printf("List is not valid\n");
		return(NULL);
	}

	if(list->itemCount == 0)										//Empty list so we can't remove anything
	{
		printf("Empty List, there is nothing to remove.\n");
		return(NULL);
	}

	if(list->currentBeyond == 1)									//Current pointer is beyond list so there is nothing to remove
	{
		printf("Current pointer is beyond list.\n");
		return(NULL);
	}

	NODE * currentAddress = list->current;							//Creates 2 Nodes inorder to calculate which index value the current pointer is at.
	NODE * baseAddress = nodeArray;

	int value = (currentAddress)-(baseAddress);						//Done by subtracting current with base

	NODE * newNode = list->current;									//Created another list->current because a bug was encountered and this fixes it

	if(list->itemCount == 1)										//A coner case when we have to remove te last item making head and tail NULL
	{
		list->current = NULL;
		list-> head = NULL;
		list->tail = NULL;
	}
	else if(newNode->prev == NULL)									//If we remove head we have to update the head, corner case
	{
		NODE * nextNode = newNode->next;
		nextNode->prev = NULL;
		list->head = nextNode;
		list->current = nextNode;
	}
	else if(newNode->next == NULL)									//If we remove tail we have to update the tail, corner case
	{
		NODE * prevNode = newNode->prev;
		prevNode->next = NULL;
		list->tail = prevNode;
		list->current = prevNode;
	}
	else															//Regualar cases where the prev node and the next node has to link up
	{
		NODE * prevNode = newNode->prev;
		NODE * nextNode = newNode->next;
		prevNode->next = nextNode;									//prev-> next
		nextNode->prev = prevNode;									//next<- prev
		list->current = nextNode;									//current becomes the nextnode
	}

	list->itemCount -= 1;											//Since a node was removed we have to decrement itemCount

	nodeStackCount++;												//Pushing values to the stack to we can resuse later
	nodeStack[nodeStackCount] = value;								//Also incrementing by 1 so we know we have stuff in stack
	return(list->current);											//Retunrning the new current
}

void ListConcat(LIST * list1, LIST * list2)							//adds list2 to the end of list1, the current pointer is et to the current pointer in list1, list2 is also removed
{
	if(isValid(list1) == -1)										//Check if list1 is valid
	{
		printf("First list is not valid\n");
		return;
	}

	if(isValid(list2) == -1)										//Check if list2 is valid
	{
		printf("Second list is not valid\n");
		return;
	}

	if(list1->itemCount == 0)										//List1 is an empty list, cornercase
	{
		list1->head = list2->head;									//List1 will point to the tail and head of list2 (similar to copying)
		list1->tail = list2->tail;
		list1->current = list2->head;
		list1->itemCount = list2->itemCount;						//Copying its itemCount
		list1->currentBeyond = 1;									//Notice that since list1 was empty its curent pointer must be beyonds
	}
	else if(list2->itemCount == 0) 									//List2 is empty list
	{
		//Do nothing
	}
	else															//Generic Case here
	{
		if(list1->currentBeyond == 1)								//A very specific cornercase >_<
		{															//When list1 current is beond the list then either it gets directed to list1 head
			if(list1->current == list1->head)
			{
				list1->current ==list1->head;
			}
			else if(list1->current == list1->tail)					//Or to list2 tail rather than being at the end of list1 tail andis not longer beyond the end
			{
				list1->current = list2->tail;
			}
		}

		list1->tail->next = list2->head;							//l1tail -> l2head
		list2->head->prev = list1->tail;							//l1tail <- l2head
		list1->tail = list2->tail;									//l1tail = l2tail

		list1->itemCount = list1->itemCount + list2->itemCount;		//totals up the item count from the two lists
	}


	list2->head = NULL;
	list2->tail = NULL;
	list2->current = NULL;
	list2->itemCount = 0;
	list2->listValid = 0;

	LIST * baseList = listArray;									//Grabing the listArray base pointer address to find the lindex of list2
	int value = (list2) - (baseList);								//Value is found by subracting list2 by base

	listStack[listStackCount] = value;								//Pushing List Stack Count
	listStackCount++;

	return;
}

void ListFree(LIST * list, void (*itemFree)(NODE * node))			//Frees an entire list compltely removing it.
{
	list->current = list->head;										//Forcing current to be the list->head
	list->currentBeyond = 0;										//Forcing out of bounds to be false

	int value = list->itemCount;									//Value will countain the inteCount in the list inorder to remove the nodes

	for(int i = 0; i < value; i++)									//A loop that will call itemfree(to free the data)
	{
		itemFree(list->head);
		ListRemove(list);											//It will also call list remove to remove the current node
	}

	LIST * baseList = listArray;									//Same as ListConcat we will determin which index this list was part of and free it.

	value = (list) - (baseList);

	listStack[listStackCount] = value;								//Pushing the values back into the stack
	listStackCount++;

	list->head = NULL;												//Clearning out list in the off chance it gets used
	list->tail = NULL;
	list->current = NULL;
	list->itemCount = 0;
	list->listValid = 0;


	return;

}

void *ListTrim(LIST * list)											//Return last item and take it out of list. Makes last item current
{
	list->currentBeyond = 0;										//Making current the tail and calling listremove to remove it. Thus making the tail->prev current
	list->current = list->tail;
	return(ListRemove(list));
}

void *ListSearch(LIST * list, int (*comparator)(void* cmp1, void* cmp2), void* comparisonArg)		//Search the list startin from current, if it is found current becomes that node and is retuned oterwise current is left to were it is and return NULL. ////////////////////////////VOID STAR
{
	if(isValid(list) == -1)											//Check to see if the list is valid
	{
		printf("List is not valid.\n");
		return(NULL);
	}

	if(list->currentBeyond == 1 && list->current == list-> tail)	//If the current is beyond the end we have no next.
	{
		printf("Beyond the tail.\n");
		return(NULL);
	}

	NODE * tempNode = list->current;								//tempNode will be list->current so we don't update current untill we find the target.
	while(tempNode->next != NULL && comparator(tempNode->data, comparisonArg) == 0)
	{
		tempNode = tempNode->next;									//Either we make it to the end or we find it is when this loop terminates
	}

	if(comparator(tempNode->data, comparisonArg) == 1)				//Once the loop ends we check to see if item was found or did we reach the end.
	{
		printf("Item found\n");										//Item found then we update current to the found item which is tempNode
		list->current = tempNode;
		list->currentBeyond = 0;									//Since we run despite the starting beyond start if item was found then current is no longer beyond start.
		return(list->current->data);
	}
	else
	{
		printf("Not found\n");										//Otherwise send message to user item was not found
	}

	return(NULL);

}
