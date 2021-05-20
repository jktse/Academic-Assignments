#include "led.h"
#include <stdio.h>
#include <stdbool.h>


//Define the led files for LED_0 to LED_3
#define tLED_0 "/sys/class/leds/beaglebone:green:usr0/trigger"
#define tLED_1 "/sys/class/leds/beaglebone:green:usr1/trigger"
#define tLED_2 "/sys/class/leds/beaglebone:green:usr2/trigger"
#define tLED_3 "/sys/class/leds/beaglebone:green:usr3/trigger"

//Define the brightness files for LED_0 to LED_3
#define bLED_0 "/sys/class/leds/beaglebone:green:usr0/brightness"
#define bLED_1 "/sys/class/leds/beaglebone:green:usr1/brightness"
#define bLED_2 "/sys/class/leds/beaglebone:green:usr2/brightness"
#define bLED_3 "/sys/class/leds/beaglebone:green:usr3/brightness"

const char* LED_getFileLocation(bool isTrigger, int LED_NUM)
{
	if (isTrigger) {
		if (LED_NUM == 0) {
			return tLED_0;
		} else if (LED_NUM == 1) {
			return tLED_1;
		} else if (LED_NUM == 2) {
			return tLED_2;
		} else if (LED_NUM == 3) {
			return tLED_3;
		}
	} else {
		if (LED_NUM == 0) {
			return bLED_0;
		} else if (LED_NUM == 1) {
			return bLED_1;
		} else if (LED_NUM == 2) {
			return bLED_2;
		} else if (LED_NUM == 3) {
			return bLED_3;
		}
	}
	return NULL;
}

void LED_init(int LED_NUM)
{
	// Change trigger to NONE
	const char* fileName = LED_getFileLocation(true, LED_NUM);
	FILE *pLedTriggerFile = fopen(fileName, "w");
	
	if (pLedTriggerFile == NULL) {
		printf("ERROR OPENING %s.\n", fileName);
		return;
	}
	fprintf(pLedTriggerFile, "none");
	fclose(pLedTriggerFile);

	// Ensure that the LED is off
	fileName = LED_getFileLocation(false, LED_NUM);
	FILE *pLedBightnessFile = fopen(fileName, "w");
	
	if (pLedBightnessFile == NULL) {
		printf("ERROR OPENING %s.\n", fileName);
		return;
	}
	fprintf(pLedBightnessFile, "0");
	fclose(pLedBightnessFile);
}

void LED_flip(int LED_NUM)
{
	const char* fileName = LED_getFileLocation(false, LED_NUM);
	FILE *pLedBightnessFile = fopen(fileName, "r+");
	
	if (pLedBightnessFile == NULL) {
		printf("ERROR OPENING %s.\n", fileName);
		return;
	}

	int state = fgetc(pLedBightnessFile);
	// printf("State currently is %d \n", state);
	if(state == 49){
		fprintf(pLedBightnessFile, "0");
	}else
	{
		fprintf(pLedBightnessFile, "1");
	}

	fclose(pLedBightnessFile);
}

void LED_flipAll(void){
	for (int i = 0; i < 4; i++)
	{
		LED_flip(i);
	}
}

// To ensure the LEDs are set back to what it was doing prior to running the application
void LED_reset(int LED_NUM)
{
	// Ensure that the LED is off
	const char* fileName = LED_getFileLocation(false, LED_NUM);
	FILE *pLedBightnessFile = fopen(fileName, "w");
	
	if (pLedBightnessFile == NULL) {
		printf("ERROR OPENING %s.\n", fileName);
		return;
	}
	fprintf(pLedBightnessFile, "0");
	fclose(pLedBightnessFile);

	// Update trigger file after all LEDs are off
	fileName = LED_getFileLocation(true, LED_NUM);
	FILE *pLedTriggerFile = fopen(fileName, "w");
	
	if (pLedTriggerFile == NULL) {
		printf("ERROR OPENING %s.\n", fileName);
		return;
	}

	if (LED_NUM == 0) {
			fprintf(pLedTriggerFile, "heartbeat");
		} else if (LED_NUM == 1) {
			fprintf(pLedTriggerFile, "mmc0");
		} else if (LED_NUM == 2) {
			fprintf(pLedTriggerFile, "cpu0");
		} else if (LED_NUM == 3) {
			fprintf(pLedTriggerFile, "mmc1");
		}
	fclose(pLedTriggerFile);
}