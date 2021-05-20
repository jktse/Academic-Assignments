#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "timing.h"
#include "accelerometer.h"
#include "beats.h"

#define I2CDRV_LINUX_BUS0 "/dev/i2c-0"
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2CDRV_LINUX_BUS2 "/dev/i2c-2"

// I2C device information
#define I2C_DEVICE_ADDRESS 0x1C
#define REG_WHO_AM_I 0x0D
#define CTRL_REG1 0x2A
#define START_READ 0x00

static int i2cFileDesc;
static pthread_t accelThreadID;
static bool keepGoing;


// From i2c guide
int Accel_initI2cBus(char* bus, int address){

    int i2cFileDesc = open(bus, O_RDWR);
    if(i2cFileDesc < 0){
        printf("I2C DRV: Unable to open bus for read.write (%s) \n", bus);
        perror("Error is: ");
        exit(EXIT_FAILURE);
    }

    int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
    if(result < 0){
        perror("T2C: Unable to set I2C device to slave address.");
        exit(1);
    }
    return i2cFileDesc;
}

// Code also from i2c guide
unsigned char * Accel_readI2CReg(int i2cFileDesc, unsigned char regAddr){
    // To read a register, must first write the address
    int res = write(i2cFileDesc, &regAddr, sizeof(regAddr)*7);
    if (res != sizeof(regAddr)*7){
        perror("I2C: Unable to write to i2c register.");
        exit(1);
    }

    // Now read the value and return it
    unsigned char *value;
    value = malloc(sizeof(unsigned char) * 7);

    res = read(i2cFileDesc, value, sizeof(unsigned char)*7);

    if(res != sizeof(unsigned char)*7){
        perror("I2C: Unable to read from i2c register");
        exit(1);
    }
    return value;
}

static void Accel_writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value)
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

void * Accel_readData(void * args){
    unsigned char *data;
    float Gx;
    float Gy;
    float Gz;
    int16_t parseData;
    while(keepGoing){
        data = Accel_readI2CReg(i2cFileDesc, START_READ);
        // From piazza @144
        parseData = (data[1] << 8) | (data[2]);
        parseData >>= 4;
        Gx = (float)parseData;

        parseData = (data[3] << 8) | (data[4]);
        parseData >>= 4;
        Gy = (float)parseData;

        parseData = (data[5] << 8) | (data[6]);
        parseData >>= 4;
        Gz = (float)parseData;

        Gx = Gx/1024.0;
        Gy = Gy/1024.0;
        Gz = Gz/1024.0;

        Gx = fabs(Gx);
        Gy = fabs(Gy);
        Gz = fabs(Gz);

        if(Gx > 1.0){
            Beats_playBase();
        }
        if(Gy > 1.0){
            Beats_playSnare();
        }
        if(Gz > 1.3){
            Beats_playHiHat();
        }

        Timing_delay(0, 250000000);
        free(data);
        data = NULL;
    }
    return(NULL);
}

void Accel_initThread(){
    keepGoing = true;
    i2cFileDesc = Accel_initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);

    Accel_writeI2cReg(i2cFileDesc, CTRL_REG1, 0X01);
    // Read the register:

    pthread_create(&accelThreadID, NULL, &Accel_readData, NULL);
}

void Accel_closeThread(){
    Accel_writeI2cReg(i2cFileDesc, CTRL_REG1, 0X00);
    keepGoing = false;
    close(i2cFileDesc);
}

unsigned long Accel_getThread(){
    return(accelThreadID);
}