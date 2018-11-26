/**************
Name: Jacky Ken Tse
Purpose: A simple chat program that sends and recieves messages through a specified port
Date Jan 28 2017
************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "list.h"

//All the structures used in this code
struct PCB {
    char    buffer[40];                         //A buffer to store messages
    int     state;                              //1 means running, 2 means ready, 3 means blocked
    int     priority;                           //1 means high, 2 means medium, 3 means low
    int     processId;                          //An id to identify the process
    int     sendId;
    int     recvId;
} pcb;

struct Semaphore{
    int value;
    LIST pcbWaiting;
}semaphore;

LIST * allPCB;                                //All new process will be stored there
LIST * highPriorityWait;
LIST * mediumPriorityWait;
LIST * lowPriorityWait;
LIST * blockedSend;                           //A list for processes waiting for reply
LIST * blockedRecieve;
LIST * runningPCB;

int comparator(struct PCB* cmp1, struct PCB* cmp2){
    printf("The first id is: %d, The second id is: %d \n", cmp1->processId, cmp2->processId);

    if(cmp1->processId == cmp2->processId){
        return(1);
    }
    else{
        return(0);
    }
}

int comparatorSend(struct PCB* cmp1, struct PCB* cmp2){
    printf("The send is: %d, The second id is: %d \n", cmp1->sendId, cmp2->processId);
    if(cmp1->sendId == cmp2->processId){
        return(1);
    }
    else{
        return(0);
    }
}

void checkBlockSend(){
    int processID;
    struct PCB *tempPCB;//A pcb to be got blocked recieve
    NODE* tempNode = blockedRecieve->head;
    for(int i = 0; i < ListCount(blockedRecieve); i ++){
        tempPCB = tempNode->data;
        if(ListCount(blockedSend) == 0){
            return;
        }
        ListFirst(blockedSend);
        if(ListSearch(blockedSend, comparatorSend, tempPCB) != NULL){
            ListFirst(blockedSend);
            struct PCB *removeWait = ListSearch(blockedSend, comparatorSend, tempPCB);
            removeWait->state = 2;
            memcpy(tempPCB->buffer, removeWait->buffer, sizeof(tempPCB->buffer));
                if(tempPCB->priority == 1){
                    if(ListCount(highPriorityWait) != 0)
                    {
                        ListLast(highPriorityWait);
                    }
                    ListAdd(highPriorityWait, tempPCB);
                    ListFirst(highPriorityWait);
                }
                else if(tempPCB->priority == 2){
                    if(ListCount(mediumPriorityWait) != 0)
                    {
                        ListLast(mediumPriorityWait);
                    }
                    ListAdd(mediumPriorityWait, tempPCB);
                    ListFirst(mediumPriorityWait);
                }
                else if(tempPCB->priority == 3){
                    if(ListCount(lowPriorityWait) != 0)
                    {
                        ListLast(lowPriorityWait);
                    }
                    ListAdd(lowPriorityWait, tempPCB);
                    ListFirst(lowPriorityWait);
                    }
            ListRemove(blockedRecieve);
        }
        else{
            tempNode = tempNode->next;
        }

    }


}

//Helper functions to make code more modular
int createNewPCB(int id){
    int priority;
    printf("Please enter a priority (1 for high, 2 for medium, 3 for low): ");
    scanf("%d", &priority);
    printf("The value we have is %d \n", priority);
    while(priority != 1 && priority != 2 && priority != 3){
            priority = 0;
            printf("Invalid input remember (1 for high, 2 for medium, 3 for low)\n");
            printf("Please enter priority for the pbc: ");
            scanf("%d", &priority);
    }
    struct PCB *newPCB = malloc(sizeof(struct PCB));
    newPCB->state = 2;
    newPCB->priority = priority;
    newPCB->processId = id;
    newPCB->sendId = 999;
    memset(newPCB->buffer, '\0', 40);

    ListLast(allPCB);
    if( ListAdd(allPCB, newPCB) == -1)
    {
        return(0);
    }

    if(priority == 1){
        if(ListCount(highPriorityWait) != 0)
        {
            ListLast(highPriorityWait);
        }
        ListAdd(highPriorityWait, newPCB);
        ListFirst(highPriorityWait);
    }
    else if(priority == 2){
        if(ListCount(mediumPriorityWait) != 0)
        {
            ListLast(mediumPriorityWait);
        }
        ListAdd(mediumPriorityWait, newPCB);
        ListFirst(mediumPriorityWait);
    }
    else if(priority == 3){
        if(ListCount(lowPriorityWait) != 0)
        {
            ListLast(lowPriorityWait);
        }
        ListAdd(lowPriorityWait, newPCB);
        ListFirst(lowPriorityWait);
    }
    return(1);
}

void forkPCB(struct PCB *forkProcess, int id){
    if(forkProcess->processId == 0){
        printf("This is the init process we cannot fork\n");
    }
    else{
        struct PCB *newPCB = malloc(sizeof(struct PCB));
        memcpy(newPCB, forkProcess, sizeof(struct PCB));

        int priority = newPCB->priority;
        newPCB->processId = id;

        if(priority == 1){
            if(ListCount(highPriorityWait) != 0)
            {
                ListLast(highPriorityWait);
            }
            ListAdd(highPriorityWait, newPCB);
        }
        else if(priority == 2){
            if(ListCount(mediumPriorityWait) != 0)
            {
                ListLast(mediumPriorityWait);
            }
            ListAdd(mediumPriorityWait, newPCB);
        }
        else if(priority == 3){
            if(ListCount(lowPriorityWait) != 0)
            {
                ListLast(lowPriorityWait);
            }
            ListAdd(lowPriorityWait, newPCB);
        }
        printf("Testing newPcb state: %d, priority: %d, processID: %d\n", newPCB->state, newPCB->priority, newPCB->processId);
        printf("Fork was sucessful.\n");
    }

}

int killPCB(int killId){
    printf("The process id is: %d\n", killId);
    if(killId == 0 && ListCount(allPCB) == 1){
        printf("Getting Ready to terminate OS simulation as Init is the remaining process.\n");
    }
    else if(killId == 0){
        printf("There are remaining process, task failed.\n");
        return(0);
    }

    struct PCB *checkPCB = malloc(sizeof(struct PCB));
    checkPCB->processId = killId;

    struct PCB *tempPCB = ListSearch(allPCB, comparator, checkPCB);
    struct PCB *startNewProcess;

    ListRemove(allPCB);
    printf("Temp Pcb state is: %d", tempPCB->state);
    if(tempPCB->state == 1)
    {
        ListFirst(runningPCB);
        ListSearch(runningPCB, comparator, checkPCB);
        ListRemove(runningPCB);
        if(ListCount(allPCB) != 0){
            printf("The process is currently running thus a new one will replace it.\n");
            if(ListCount(highPriorityWait) != 0){
                startNewProcess = ListFirst(highPriorityWait);
                ListAdd(runningPCB,startNewProcess);
                startNewProcess->state = 1;
                ListRemove(highPriorityWait);
            }
            else if(ListCount(mediumPriorityWait) != 0){
                startNewProcess = ListFirst(mediumPriorityWait);
                ListAdd(runningPCB,startNewProcess);
                startNewProcess->state = 1;
                ListRemove(mediumPriorityWait);
            }
            else if(ListCount(lowPriorityWait) != 0){
                startNewProcess = ListFirst(lowPriorityWait);
                ListAdd(runningPCB,startNewProcess);
                startNewProcess->state = 1;
                ListRemove(lowPriorityWait);
            }
        }
    }
    else if (tempPCB->state == 2){
        if(tempPCB->priority == 1){
            ListFirst(highPriorityWait);
            ListSearch(highPriorityWait, comparator, checkPCB);
            ListRemove(highPriorityWait);
        }
        else if(tempPCB->priority == 2){
            ListFirst(mediumPriorityWait);
            ListSearch(mediumPriorityWait, comparator, checkPCB);
            ListRemove(mediumPriorityWait);
        }
        else if(tempPCB->priority == 3){
            ListFirst(lowPriorityWait);
            ListSearch(lowPriorityWait, comparator, checkPCB);
            ListRemove(lowPriorityWait);
        }
    }
    else if (tempPCB->state == 3){
        if(ListSearch(blockedRecieve, comparator, checkPCB) != NULL){
            ListRemove(blockedRecieve);
        }
        else if(ListSearch(blockedSend, comparator, checkPCB) != NULL){
            ListRemove(blockedSend);
        }
    }

    free(checkPCB);
    //free(tempPCB);
    return(1);
}

int switchQuantum(){
    if(ListCount(allPCB) == 1){
        printf("Only one item left we are not switching to a new process");
        return(1);
    }

    struct PCB *tempProcess  = ListFirst(runningPCB);
    tempProcess->state = 2;
    struct PCB *startNewProcess;

    if(ListCount(highPriorityWait) != 0){
        startNewProcess = ListFirst(highPriorityWait);
        ListAdd(runningPCB,startNewProcess);
        startNewProcess->state = 1;
        ListRemove(highPriorityWait);
    }
    else if(ListCount(mediumPriorityWait) != 0){
        startNewProcess = ListFirst(mediumPriorityWait);
        ListAdd(runningPCB,startNewProcess);
        startNewProcess->state = 1;
        ListRemove(mediumPriorityWait);
    }
    else if(ListCount(lowPriorityWait) != 0){
        startNewProcess = ListFirst(lowPriorityWait);
        ListAdd(runningPCB,startNewProcess);
        startNewProcess->state = 1;
        ListRemove(lowPriorityWait);
    }

    if(tempProcess->priority == 1){
        if(ListCount(highPriorityWait) != 0)
        {
            ListLast(highPriorityWait);
        }
        ListAdd(highPriorityWait, tempProcess);
        ListFirst(highPriorityWait);
    }
    else if(tempProcess->priority == 2){
        if(ListCount(mediumPriorityWait) != 0)
        {
            ListLast(mediumPriorityWait);
        }
        ListAdd(mediumPriorityWait, tempProcess);
        ListFirst(mediumPriorityWait);
    }
    else if(tempProcess->priority == 3){
        if(ListCount(lowPriorityWait) != 0)
        {
            ListLast(lowPriorityWait);
        }
        ListAdd(lowPriorityWait, tempProcess);
        ListFirst(lowPriorityWait);
    }

    ListFirst(runningPCB);
    ListRemove(runningPCB);

    return(1);
}

int sendMessage(){
    int sendId;
    char buffer[40];
    struct PCB *currentProcess = ListFirst(runningPCB);

    printf("Please enter the id of the process you want to send to: ");
    scanf("%d", &sendId);
    printf("Please enter a message to send (40 char max): \n");
    read(0, buffer, 40);

    currentProcess->sendId = sendId;
    memcpy(currentProcess->buffer, buffer, sizeof(currentProcess->buffer));

    ListAdd(blockedSend, currentProcess);
    currentProcess->state = 3;

    struct PCB *startNewProcess;

    if(ListCount(highPriorityWait) != 0){
        startNewProcess = ListFirst(highPriorityWait);
        ListAdd(runningPCB,startNewProcess);
        startNewProcess->state = 1;
        ListRemove(highPriorityWait);
    }
    else if(ListCount(mediumPriorityWait) != 0){
        startNewProcess = ListFirst(mediumPriorityWait);
        ListAdd(runningPCB,startNewProcess);
        startNewProcess->state = 1;
        ListRemove(mediumPriorityWait);
    }
    else if(ListCount(lowPriorityWait) != 0){
        startNewProcess = ListFirst(lowPriorityWait);
        ListAdd(runningPCB,startNewProcess);
        startNewProcess->state = 1;
        ListRemove(lowPriorityWait);
    }

    ListFirst(runningPCB);
    ListRemove(runningPCB);
}

int recieveMessage(){

    struct PCB *currentProcess = ListFirst(runningPCB);
    ListAdd(blockedRecieve, currentProcess);
    currentProcess->state = 3;

    struct PCB *startNewProcess;

    if(ListCount(highPriorityWait) != 0){
        startNewProcess = ListFirst(highPriorityWait);
        ListAdd(runningPCB,startNewProcess);
        startNewProcess->state = 1;
        ListRemove(highPriorityWait);
    }
    else if(ListCount(mediumPriorityWait) != 0){
        startNewProcess = ListFirst(mediumPriorityWait);
        ListAdd(runningPCB,startNewProcess);
        startNewProcess->state = 1;
        ListRemove(mediumPriorityWait);
    }
    else if(ListCount(lowPriorityWait) != 0){
        startNewProcess = ListFirst(lowPriorityWait);
        ListAdd(runningPCB,startNewProcess);
        startNewProcess->state = 1;
        ListRemove(lowPriorityWait);
    }

    ListFirst(runningPCB);
    ListRemove(runningPCB);
    checkBlockSend();
}

int replyMessage(){
    int replyId;
    char buffer [40];

    printf("Please enter the process id you want to reply to: ");
    scanf("%d", &replyId);

    printf("Please enter a message to send (40 char max): \n");
    read(0, buffer, 40);

    struct PCB *currentProcess = ListFirst(runningPCB);
    struct PCB *checkPCB = malloc(sizeof(struct PCB));
    checkPCB->processId = replyId;


    if(ListCount(blockedSend) == 0){
        printf("Empty Blocked Send\n");
        return(0);
    }

    if(ListSearch(blockedSend, comparator, checkPCB) == NULL){
        printf("Target not waiting for reply.\n");
        return(0);
    }
    else{
        struct PCB *tempPCB = ListSearch(blockedSend, comparator, checkPCB);

        if(tempPCB->sendId != currentProcess->processId){
            printf("Ids do not match!\n");
            return(0);
        }
        else{
            tempPCB->state = 2;
            memcpy(tempPCB->buffer, buffer, sizeof(tempPCB->buffer));
                if(tempPCB->priority == 1){
                    if(ListCount(highPriorityWait) != 0)
                    {
                        ListLast(highPriorityWait);
                    }
                    ListAdd(highPriorityWait, tempPCB);
                    ListFirst(highPriorityWait);
                }
                else if(tempPCB->priority == 2){
                    if(ListCount(mediumPriorityWait) != 0)
                    {
                        ListLast(mediumPriorityWait);
                    }
                    ListAdd(mediumPriorityWait, tempPCB);
                    ListFirst(mediumPriorityWait);
                }
                else if(tempPCB->priority == 3){
                    if(ListCount(lowPriorityWait) != 0)
                    {
                        ListLast(lowPriorityWait);
                    }
                    ListAdd(lowPriorityWait, tempPCB);
                    ListFirst(lowPriorityWait);
                    }
            ListSearch(blockedSend, comparator, checkPCB);
            ListRemove(blockedSend);
        }
    }
    free(checkPCB);

}

void procInfo(){
    int procId;
    printf("Please enter the process id whoes information you want: ");
    scanf("%d", &procId);

    struct PCB *processLookUp = malloc(sizeof(struct PCB));
    processLookUp->processId = procId;

    struct PCB *tempPCB;

    if(ListCount(allPCB) == 0)
    {
        printf("NO PCB created!?!?!");
        return;
    }
    if( ListSearch(allPCB, comparator, processLookUp) == NULL){
        printf("Target NOT found");
        tempPCB = ListSearch(allPCB, comparator, processLookUp);
        printf("Its state: %d, priority: %d, processID: %d\n", tempPCB->state, tempPCB->priority, tempPCB->processId);
        if(tempPCB->state == 3){
            if(ListSearch(blockedRecieve, comparator, processLookUp) != NULL){
                printf("PCB is also located at the blocked Recieve list");
            }
            if(ListSearch(blockedSend, comparator, processLookUp) != NULL){
                printf("PCB is also located at the blocked Send list");
            }
        }
    }
}

void displayAll(){
    NODE* tempNode = runningPCB->head;
    struct PCB *tempPCB;

    printf("Checking Running QUEUE\n");
    for(int i = 0; i < ListCount(runningPCB); i ++){
        tempPCB = tempNode->data;
        printf("PCB: %d    Its state: %d, priority: %d, processID: %d\n", i, tempPCB->state, tempPCB->priority, tempPCB->processId);
		tempNode = tempNode->next;
    }

    printf("Checking highPriorityQueue\n");
    tempNode = highPriorityWait->head;
    for(int i = 0; i < ListCount(highPriorityWait); i ++){
        tempPCB = tempNode->data;
        printf("PCB: %d     Its state: %d, priority: %d, processID: %d\n", i, tempPCB->state, tempPCB->priority, tempPCB->processId);
		tempNode = tempNode->next;
    }

    printf("Checking mediumPriorityQueue\n");
    tempNode = mediumPriorityWait->head;
    for(int i = 0; i < ListCount(mediumPriorityWait); i ++){
        tempPCB = tempNode->data;
        printf("PCB: %d     Its state: %d, priority: %d, processID: %d\n", i, tempPCB->state, tempPCB->priority, tempPCB->processId);
		tempNode = tempNode->next;
    }

    printf("Checking lowPriorityQueue\n");
    tempNode = lowPriorityWait->head;
    for(int i = 0; i < ListCount(lowPriorityWait); i ++){
        tempPCB = tempNode->data;
        printf("PCB: %d     Its state: %d, priority: %d, processID: %d\n", i, tempPCB->state, tempPCB->priority, tempPCB->processId);
		tempNode = tempNode->next;
    }

    printf("Checking Blocked Recieve\n");
    tempNode = blockedRecieve->head;
    for(int i = 0; i < ListCount(blockedRecieve); i ++){
        tempPCB = tempNode->data;
        printf("PCB: %d     Its state: %d, priority: %d, processID: %d\n", i, tempPCB->state, tempPCB->priority, tempPCB->processId);
		tempNode = tempNode->next;
    }

    printf("Checking Blocked Send\n");
    tempNode = blockedSend->head;
    for(int i = 0; i < ListCount(blockedSend); i ++){
        tempPCB = tempNode->data;
        printf("PCB: %d     Its state: %d, priority: %d, processID: %d, sendId: %d\n", i, tempPCB->state, tempPCB->priority, tempPCB->processId, tempPCB->sendId);
		tempNode = tempNode->next;
    }

    printf("Checking all PCB\n");
    tempNode = allPCB->head;
    for(int i = 0; i < ListCount(allPCB); i ++){
        tempPCB = tempNode->data;
        printf("PCB: %d     Its state: %d, priority: %d, processID: %d, sendId: %d\n", i, tempPCB->state, tempPCB->priority, tempPCB->processId, tempPCB->sendId);
		tempNode = tempNode->next;
    }
}

int main(){

    allPCB = ListCreate();

    //Creating ready/wait list
    highPriorityWait    = ListCreate();         //Create the waiting list
    mediumPriorityWait  = ListCreate();
    lowPriorityWait     = ListCreate();
    //Create a list for process that can be blocked
    blockedSend         = ListCreate();
    blockedRecieve      = ListCreate();
    //Create a running vairable to store the PCB that is being ran
    runningPCB = ListCreate();

    struct PCB *initialPcb = malloc(sizeof(struct PCB));
    initialPcb->state = 1;
    initialPcb->priority = 1;
    initialPcb->processId = 0;
    initialPcb->sendId = 999;
    memset(initialPcb->buffer, '\0', 40);

    printf("Testing initialPcb state: %d, priority: %d, processID: %d\n", initialPcb->state, initialPcb->priority, initialPcb->processId);
    ListAdd(allPCB, initialPcb);
    ListAdd(runningPCB, initialPcb);

    printf("Welcome to OS simulation, please read the README before continuing.\n");

    int pcbId =1;
    char command[5];

    while(ListCount(allPCB) != 0){
        ListFirst(allPCB);
        struct PCB *currentProcess = ListFirst(runningPCB);
        if(strlen(currentProcess->buffer) != 0){
            printf("Recieved Message: %s", currentProcess->buffer);
            memset(currentProcess->buffer, '\0', 40);
        }

        printf("Please enter a command: ");
        scanf("%s", command);

        while(strlen(command) != 1){
            memset(command, '\0', 1);
            printf("Invalid input please read the README to understand what are valid instructions. \n");
            printf("Please enter a command: ");
            scanf("%s", command);
        }

        if(command[0] == 'C' || command[0] == 'c'){
            printf("You entered the command to create a new process.\n");
            if(createNewPCB(pcbId) == 1){
                printf("Creation of new process successful\n");
            }
            else{
                printf("Error in creating new Process\n");
            }
            pcbId++;
        }
        else if (command[0] == 'F' ||command[0] == 'f'){
            printf("You entered the command to Fork the current process.\n");
            forkPCB(ListFirst(runningPCB), pcbId);
            pcbId++;
        }
        else if (command[0] == 'K' ||command[0] == 'k'){
            printf("You entered the command to kill a selected process.\n");

            int killId;
            int callback = 0;

            printf("Please input the id of the process you want to kill: ");
            scanf("%d", &killId);

            struct PCB *checkPCB = malloc(sizeof(struct PCB));
            checkPCB->processId = killId;

            ListFirst(allPCB);
            if(ListSearch(allPCB, comparator, checkPCB) == NULL){
                printf("Target not found exiting kill\n");
            }
            else{
                callback = killPCB(killId);
            }

            if(callback == 1){
                printf("Target PCB was found and killed.. IE was remomved from Lists and Freed.\n");
            }
            else{
                printf("Kill Failed\n");
            }
            free(checkPCB);
        }
        else if(command[0] == 'E' ||command[0] == 'e'){
            printf("You entered the command to kill the current running process.\n");
            struct PCB *tempPCB = ListFirst(runningPCB);

            int callback = killPCB(tempPCB->processId);
            if(callback == 1){
                printf("Target PCB was found and killed.. IE was remomved from Lists and Freed.\n");
            }
            else{
                printf("Kill Failed\n");
            }
        }
        else if(command[0] == 'Q' ||command[0] == 'q'){
            printf("End of current time quantum...\n");
            switchQuantum();
            printf("Time Quantum switch sucessful.\n");
        }
        else if(command[0] == 'S' ||command[0] == 's'){
            printf("Preparing to send message..\n");
            sendMessage();
        }
        else if(command[0] == 'R' ||command[0] == 'r'){
            printf("Preparing to wait for a message..\n");
            recieveMessage();
        }
        else if(command[0] == 'Y' ||command[0] == 'y'){
            printf("Preparing to reply to a process\n");
            replyMessage();
        }
        else if(command[0] == 'I' ||command[0] == 'i'){
            procInfo();
        }
        else if(command[0] == 'T' ||command[0] == 't'){
            displayAll();
        }










        //checkBlockSend();
        memset(command, '\0', 2);
    }




    return 0;
}
