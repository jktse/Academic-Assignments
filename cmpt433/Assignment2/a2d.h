/**************
 * Name: Jacky Ken Tse, Kenneth Liang
 * Date: Febuary 16, 2021
 * Purpose: Read data from potentiometer and update the size of the array that is being sorted
 * ****************/

#ifndef a2d_H_
#define a2d_H_

int a2d_getReading(void);
void a2d_startReading(void);
void a2d_stopReading(void);
long unsigned int a2d_getThread();

#endif