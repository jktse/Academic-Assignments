#include <pthread.h>
#include "UDPListen.h"
#include "sorter.h"
#include "a2d.h"
#include "display.h"

void Thread_initialize(){
    UDPListen_startListening();
    Sorter_startSorting();
    a2d_startReading();
    Display_startDisplaying();
    
    pthread_join(Sorter_getThread(), NULL);
    pthread_join(a2d_getThread(), NULL);
    pthread_join(UDPListen_getThread(), NULL);
    pthread_join(Display_getThread(), NULL);

}