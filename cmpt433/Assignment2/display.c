#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <time.h>
#include <pthread.h>

#include "sorter.h"
#include "timing.h"

#define I2CDRV_LINUX_BUS0 "/dev/i2c-0"
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2CDRV_LINUX_BUS2 "/dev/i2c-2"

#define I2C_DEVICE_ADDRESS 0x20

#define REG_DIRA 0x00	// lower register
#define REG_DIRB 0x01	// uppser register
#define REG_OUTA 0x14	// lower Segs
#define REG_OUTB 0x15	// upper Segs

#define LEFT_DIGIT_VALUE_DIR "/sys/class/gpio/gpio61/value"
#define RIGHT_DIGIT_VALUE_DIR "/sys/class/gpio/gpio44/value"


static pthread_t displayingThread; 
static bool keepGoing;


static int initI2cBus(char* bus, int address)
{
	int i2cFileDesc = open(bus, O_RDWR);
	if (i2cFileDesc < 0) {
		printf("I2C DRV: Unable to open bus for read/write (%s)\n", bus);
		perror("Error is:");
		exit(-1);
	}

	int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
	if (result < 0) {
		perror("Unable to set I2C device to slave address.");
		exit(-1);
	}
	return i2cFileDesc;
}


static void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value)
{
	unsigned char buff[2];
	buff[0] = regAddr;
	buff[1] = value;
	int res = write(i2cFileDesc, buff, 2);
	if (res != 2) {
		perror("Unable to write i2c register");
		exit(-1);
	}
}

static void displaySameNumberOnBothDigit(int number) {

	int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);

	switch (number)
	{
	case 0:
		writeI2cReg(i2cFileDesc, REG_OUTA, 0xa1);
		writeI2cReg(i2cFileDesc, REG_OUTB, 0x86);
		close(i2cFileDesc);
		break;
	case 1:
		writeI2cReg(i2cFileDesc, REG_OUTA, 0x80);
		writeI2cReg(i2cFileDesc, REG_OUTB, 0x12);
		close(i2cFileDesc);
		break;
	case 2:
		writeI2cReg(i2cFileDesc, REG_OUTA, 0x31);
		writeI2cReg(i2cFileDesc, REG_OUTB, 0x0e);
		close(i2cFileDesc);
		break;
	case 3:
		writeI2cReg(i2cFileDesc, REG_OUTA, 0xb0);
		writeI2cReg(i2cFileDesc, REG_OUTB, 0x0f);
		close(i2cFileDesc);
		break;
	case 4:
		writeI2cReg(i2cFileDesc, REG_OUTA, 0x90);
		writeI2cReg(i2cFileDesc, REG_OUTB, 0x8a);
		close(i2cFileDesc);
		break;
	case 5:
		writeI2cReg(i2cFileDesc, REG_OUTA, 0xb0);
		writeI2cReg(i2cFileDesc, REG_OUTB, 0x8c);
		close(i2cFileDesc);
		break;
	case 6:
		writeI2cReg(i2cFileDesc, REG_OUTA, 0xb1);
		writeI2cReg(i2cFileDesc, REG_OUTB, 0x8c);
		close(i2cFileDesc);
		break;
	case 7:
		writeI2cReg(i2cFileDesc, REG_OUTA, 0x04);
		writeI2cReg(i2cFileDesc, REG_OUTB, 0x14);
		close(i2cFileDesc);
		break;
	case 8:
		writeI2cReg(i2cFileDesc, REG_OUTA, 0xb1);
		writeI2cReg(i2cFileDesc, REG_OUTB, 0x8e);
		close(i2cFileDesc);
		break;
	case 9:
		writeI2cReg(i2cFileDesc, REG_OUTA, 0xb0);
		writeI2cReg(i2cFileDesc, REG_OUTB, 0x8f);
		close(i2cFileDesc);
		break;
	default:
		close(i2cFileDesc);
		perror("Nummber out of range. Please enter a number in range of [0,9]\n");
		break;
	}

}

static void turnLeftDigitOnOff(int value){

	FILE* leftDigitDirFile = fopen(LEFT_DIGIT_VALUE_DIR, "w");

	if(leftDigitDirFile == NULL){
		printf("ERROR OPENING %s\n", LEFT_DIGIT_VALUE_DIR);
        exit(1);
	}

	int charWritten = 0;

	if(value == 0){
		charWritten = fprintf(leftDigitDirFile, "0");
	}
	else if(value == 1){
		charWritten = fprintf(leftDigitDirFile, "1");
	}
	else{
		perror("Enter 0 for OFF and 1 for ON");
	}

    if (charWritten <= 0) {
        printf("ERROR WRITING DATA");
        exit(1);
    }

    fclose(leftDigitDirFile);
}


static void turnRightDigitOnOff(int value){

	FILE* rightDigitDirFile = fopen(RIGHT_DIGIT_VALUE_DIR, "w");

	if(rightDigitDirFile == NULL){
		printf("ERROR OPENING %s\n", RIGHT_DIGIT_VALUE_DIR);
        exit(1);
	}

	int charWritten = 0;


	if(value == 0){
		charWritten = fprintf(rightDigitDirFile, "0");
	}
	else if(value == 1){
		charWritten = fprintf(rightDigitDirFile, "1");
	}
	else{
		perror("Enter 0 for OFF and 1 for ON");
	}

    if (charWritten <= 0) {
        printf("ERROR WRITING DATA");
        exit(1);
    }

    fclose(rightDigitDirFile);
}

static void *Display_displayNumber(){


	time_t start = clock();
    long long int numArraySortedLastSecond = 0;
    long long int numArraySortedSinceLastSecond = 0;
	long long int number = 0;

	while(keepGoing){
		if(number > 99){
			number = 99;
		}

		int rightDigitNum = number % 10;

		int leftDigitNum = (number - rightDigitNum) / 10;


		turnLeftDigitOnOff(0);
		turnRightDigitOnOff(0);

		displaySameNumberOnBothDigit(leftDigitNum);
		
        turnLeftDigitOnOff(1);
		
        Timing_delay(0,5000000);
		
        turnLeftDigitOnOff(0);
		turnRightDigitOnOff(0);

		displaySameNumberOnBothDigit(rightDigitNum);

		turnRightDigitOnOff(1);
		
        Timing_delay(0,5000000);

		if((clock()-start)/CLOCKS_PER_SEC == 1){
            numArraySortedSinceLastSecond = Sorter_getNumberArraySorted() - numArraySortedLastSecond;
            number = numArraySortedSinceLastSecond;
            numArraySortedLastSecond = numArraySortedSinceLastSecond + numArraySortedLastSecond;
            start = clock();
        }

	}

	return NULL;

}


void Display_startDisplaying(){
	keepGoing = true;
	pthread_create(&displayingThread, NULL, &Display_displayNumber, NULL);
}

long unsigned int Display_getThread(){
	return displayingThread;
}

void Display_stopDisplaying(){
	keepGoing = false;
}


// 00: 0x14-0xa1; 0x15-0x86
// 11: 0x14-0x80; 0x15-0x12
// 22: 0x14-0x31; 0x15-0x0e
// 33: 0x14-0xb0; 0x15-0x0f
// 44: 0x14-0x90; 0x15-0x8a
// 55: 0x14-0xb0; 0x15-0x8c
// 66: 0x14-0xb1; 0x15-0x8c
// 77: 0x14-0x04; 0x15-0x14
// 88: 0x14-0xb1; 0x15-0x8e
// 99: 0x14-0xb0; 0x15-0x8f