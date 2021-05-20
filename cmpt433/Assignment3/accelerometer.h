#ifndef ACCEL_H_
#define ACCEL_H_

// Prototypes
int Accel_initI2cBus(char* bus, int address);
unsigned char* Accel_readI2CReg(int i2cFileDesc, unsigned char regAddr);

void * Accel_readData(void * args);

void Accel_initThread();
void Accel_closeThread();

unsigned long Accel_getThread();


#endif