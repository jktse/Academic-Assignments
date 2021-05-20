/**************
 * Name: Jacky Ken Tse, Kenneth Liang
 * Date: Febuary 16, 2021
 * Purpose: Code to initialize the threaded bubble sort of arrays.
 * ****************/

#ifndef SORTER_H_
#define SORTER_H_

void Sorter_startSorting(void);
void Sorter_stopSorting(void);

void Sorter_setArraySize(int newSize);
void Sorter_setNextArraySize(int newSize);
int Sorter_getArrayLength(void);

int* Sorter_getArrayData();
int Sorter_getIndexedElement(int index);

long long Sorter_getNumberArraySorted(void);

long unsigned int Sorter_getThread();
pthread_mutex_t Sorter_getMutex();

#endif