#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <string.h>

#include "sorter.h"
#include "display.h"

static int arraySize = 100;
static int nextArraySize = 100;
static long long arraySorted = 0;
static int *arrayPtr = NULL;
static bool keepGoing = true;
static pthread_t sortingThread;
// a2d needs to modify the array size, however we cannot have it modifying when we are bubble sotring or creating a new array
static pthread_mutex_t arraySizeMutex = PTHREAD_MUTEX_INITIALIZER;
// we need to get the array mid sorting, we cannot access the arrays mid sort.
static pthread_mutex_t arrayMutex = PTHREAD_MUTEX_INITIALIZER;
// Just incase we do not update and read at the same time.
static pthread_mutex_t nextArraySizeMutex = PTHREAD_MUTEX_INITIALIZER;

// This randomization algorthim is the Fisher-Yates Algorithm
// I learnt this in MATH 343, and it is based off the algorith my professor has provided
// Fisher-Yates algotirhm generate a permuation n with probability of 1/n!
static void Sorter_randomizeArray(void){
    // Set seed to current time to randomize.
    time_t t;
    srand((unsigned) time(&t));

    int j = 0;
    int i = 0;
    int temp = 0;
    // printf("Starting Random\n");
    for(i = arraySize - 1; i > 0; i--){
        // Take a random j from 0 to i
        j = (rand() % i);
        // Swap the contents of i and j
        temp = arrayPtr[i];
        arrayPtr[i] = arrayPtr[j];
        arrayPtr[j] = temp;
    }
}

static void Sorter_fillArray(void){
    for(int i = 1; i <= arraySize; i ++){
        arrayPtr[i - 1] = i;
    }
    // printf("Filled\n");
    Sorter_randomizeArray();
}

static void Sorter_createArray(void){

    arrayPtr = (int *)malloc(sizeof(int)*arraySize); // Allocate arraySize number of int
    Sorter_fillArray();
}

static void *Sorter_bubbleSort(void *args){

    while(keepGoing){
        pthread_mutex_lock(&arraySizeMutex);

        pthread_mutex_lock(&nextArraySizeMutex);
        Sorter_setArraySize(nextArraySize);
        pthread_mutex_unlock(&nextArraySizeMutex);

        pthread_mutex_lock(&arrayMutex);
        Sorter_createArray();
        pthread_mutex_unlock(&arrayMutex);
        
        bool notSorted = true;
        int temp = 0;

        while (notSorted)
        {
            pthread_mutex_lock(&arrayMutex);
            notSorted = false;
            for(int i = 0; i < arraySize - 1; i++){
                //If the adjacent element is larger we swap.
                if(arrayPtr[i] < arrayPtr[i+1]){
                    if(!notSorted){
                        notSorted = true;
                    }
                    // Swap with the adjacent value
                    temp = arrayPtr[i];
                    arrayPtr[i] = arrayPtr[i+1];
                    arrayPtr[i+1] = temp;
                }
            }
            pthread_mutex_unlock(&arrayMutex);
        }
      
        free(arrayPtr);
        arrayPtr = NULL;
        arraySorted ++;
        pthread_mutex_unlock(&arraySizeMutex);
    }
    pthread_mutex_destroy(&arraySizeMutex);
    pthread_mutex_destroy(&nextArraySizeMutex);
    pthread_mutex_destroy(&arrayMutex);
    pthread_exit(NULL);
    return NULL;
}

void Sorter_startSorting(void){
    pthread_create(&sortingThread, NULL, &Sorter_bubbleSort, NULL);
}

void Sorter_stopSorting(void){
    keepGoing = false;
}

int* Sorter_getArrayData(int *length){
    int *buffer;
    buffer = malloc(sizeof(int)* (*length));
    pthread_mutex_lock(&arrayMutex);
    memcpy(buffer, arrayPtr, sizeof(int)* (*length));
    pthread_mutex_unlock(&arrayMutex);
    return buffer;
}

void Sorter_setNextArraySize(int newSize){
    pthread_mutex_lock(&nextArraySizeMutex);
    nextArraySize = newSize;
    pthread_mutex_unlock(&nextArraySizeMutex);
}

void Sorter_setArraySize(int newSize){
    arraySize = newSize;
}

int Sorter_getArrayLength(void){
    return arraySize;
}

int Sorter_getIndexedElement(int index){
    int retVal;
    pthread_mutex_lock(&arrayMutex);
    retVal = arrayPtr[index - 1];
    pthread_mutex_unlock(&arrayMutex);
    return retVal;
}

long long Sorter_getNumberArraySorted(void){
    return arraySorted;
}

long unsigned int Sorter_getThread(){
    return sortingThread;
}

pthread_mutex_t Sorter_getMutex(){
    return arraySizeMutex;
}