#include <stdlib.h>
#include <stdio.h>

#include "thread.h"
#include "config-pin.h"


int main( int argc, const char* argv[] ){

    configI2CPin();
    Thread_initialize();
    
}