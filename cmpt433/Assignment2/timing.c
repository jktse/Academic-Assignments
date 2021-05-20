/*******************
 * Name: Jacky Ken Tse
 * Date: Feb 19, 2021
 * Purpose: A function to allow us to run nanosleep
 * *****************/

#include "timing.h"
#include <time.h>

void Timing_delay(long sec, long nanoSec){
    struct timespec reqDelay ={sec, nanoSec};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}