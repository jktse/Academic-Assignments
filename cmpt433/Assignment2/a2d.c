#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include "sorter.h"
#include "timing.h"
#include "a2d.h"

#define A2D_FILE_VOLTAGE0  "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"
#define PIECEWISE_NUM_POINTS 10
const int PIECEWISE_READING[PIECEWISE_NUM_POINTS] = {0, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4100};
const int PIECEWISE_SIZE[PIECEWISE_NUM_POINTS] = {1, 20, 60, 120, 250, 300, 500, 800, 1200, 2100};

static pthread_t a2dReadThread;
static bool keepGoing = true;

// Code is identical to the guide for a2c
int a2d_getReading(void)
{
	FILE *A2D_FILE_READ = fopen(A2D_FILE_VOLTAGE0, "r");
	if (A2D_FILE_READ == NULL) {
        printf("ERROR OPENING %s.\n", A2D_FILE_VOLTAGE0);
        exit(-1);
    }

	// Get reading
	int a2dReading = 0;
	int itemsRead = fscanf(A2D_FILE_READ, "%d", &a2dReading);
	if (itemsRead <= 0) {
		printf("ERROR: Unable to read values from voltage input file.\n");
		exit(-1);
	}

	// Close file
	fclose(A2D_FILE_READ);

	return a2dReading;
}

void* a2d_updateArraySize(){
	static int a2dValue;
	float piecewiseEQ1;
	int piecewiseEQ2;
	int piecewiseSize;
	int indexPieceWise;
	while(keepGoing){
		a2dValue = a2d_getReading();
		for(int i = 0; i < PIECEWISE_NUM_POINTS; i ++){
			if(PIECEWISE_READING[i] > a2dValue){
				indexPieceWise = i;
				i = PIECEWISE_NUM_POINTS;
			}
		}
		piecewiseEQ1 = (float)((a2dValue - PIECEWISE_READING[indexPieceWise-1]))/
						(float)(PIECEWISE_READING[indexPieceWise]-PIECEWISE_READING[indexPieceWise-1]);
		piecewiseEQ2 = (PIECEWISE_SIZE[indexPieceWise] - PIECEWISE_SIZE[indexPieceWise - 1]);
		piecewiseSize = (int)(piecewiseEQ1 * piecewiseEQ2) + PIECEWISE_SIZE[indexPieceWise - 1];

		Sorter_setNextArraySize(piecewiseSize);
		printf("New array size calculated: %d \n", piecewiseSize);
		Timing_delay(1,0);
	}
	pthread_exit(NULL);
    return NULL;
}

void a2d_startReading(void){
    pthread_create(&a2dReadThread, NULL , &a2d_updateArraySize, NULL);
}

void a2d_stopReading(void){
    keepGoing = false;
}

long unsigned int a2d_getThread(){
	return a2dReadThread;
}