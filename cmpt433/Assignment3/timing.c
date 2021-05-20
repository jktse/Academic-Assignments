#include "timing.h"
#include <time.h>

void Timing_delay(long sec, long nanoSec){
    struct timespec reqDelay ={sec, nanoSec};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}