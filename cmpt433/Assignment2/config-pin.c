#include <stdlib.h>
#include <stdio.h>

#include "config-pin.h"
#include "timing.h"

#define I2CDRV_LINUX_BUS0 "/dev/i2c-0"
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2CDRV_LINUX_BUS2 "/dev/i2c-2"

#define GPIO_EXPORT_DIR "/sys/class/gpio/export"
#define GPIO61_DIRECTION_DIR "/sys/class/gpio/gpio61/direction"
#define GPIO44_DIRECTION_DIR "/sys/class/gpio/gpio44/direction"
#define GPIO61_VALUE_DIR "/sys/class/gpio/gpio61/value"
#define GPIO44_VALUE_DIR "/sys/class/gpio/gpio44/value"

static void showZeros(){
    FILE *topZero = popen("i2cset -y 1 0x20 0x15 0x86", "w");
    
    if(topZero == NULL){
        perror("Failed command\n");
    }
    int exitCode = WEXITSTATUS(pclose(topZero));
    if(exitCode != 0){
        printf("Program failed: %d \n", exitCode);
    }

    FILE *bottomZero = popen("i2cset -y 1 0x20 0x14 0xa1", "w");
    
    if(bottomZero == NULL){
        perror("Failed command\n");
    }

    exitCode = WEXITSTATUS(pclose(bottomZero));
    if(exitCode != 0){
        printf("Program failed: %d \n", exitCode);
    }

}



const char* GPIO_getFileLocation(char location, int GPIO_NUM){
	if (location == 'v') {
		if (GPIO_NUM == 61) {
			return GPIO61_VALUE_DIR;
		} else if (GPIO_NUM == 44) {
			return GPIO44_VALUE_DIR;
		}
	} else if (location == 'd'){
		if (GPIO_NUM == 61) {
			return GPIO61_DIRECTION_DIR;
		} else if (GPIO_NUM == 44) {
			return GPIO44_DIRECTION_DIR;
		}
	}
	return NULL;
}

void GPIO_init(int GPIO_PIN){
    // Export the PIN
    FILE *GpioExportFile = fopen(GPIO_EXPORT_DIR, "w");

    if (GpioExportFile == NULL) {
        printf("ERROR OPENING %s.\n", GPIO_EXPORT_DIR);
        exit(-1);
    }

    printf("Exporting Linux GPIO Number: %d\n", GPIO_PIN);
    fprintf(GpioExportFile, "%d", GPIO_PIN);
    fclose(GpioExportFile);

    Timing_delay(0, 500000000);

    // After a delay we ensure that the direction is set to input
    const char* fileName = GPIO_getFileLocation('d', GPIO_PIN);
    FILE *GpioDirectionFile = fopen(fileName, "w");

    if (GpioDirectionFile == NULL) {
        printf("ERROR OPENING %s.\n", fileName);
        exit(-1);
    }
    //printf("Updating direction for Linux GPIO Number: %d\n", GPIO_PIN);
    fprintf(GpioDirectionFile, "out");
    fclose(GpioDirectionFile);

    // Set value to be 1
    fileName = GPIO_getFileLocation('v', GPIO_PIN);
    FILE *GpioValueFile = fopen(fileName, "w");

    if (GpioValueFile == NULL) {
        printf("ERROR OPENING %s.\n", fileName);
        exit(-1);
    }
    //printf("Updating direction for Linux GPIO Number: %d\n", GPIO_PIN);
    fprintf(GpioValueFile, "1");
    fclose(GpioValueFile);
}


void configI2CPin(){

    //  Enable Linux I2C support for Linux I2C bus #1 (HW bus 1)
    FILE *pipeP9_18 = popen("config-pin P9_18 i2c", "w");
    
    if(pipeP9_18 == NULL){
        perror("Failed command\n");
    }
    int exitCode = WEXITSTATUS(pclose(pipeP9_18));
    if(exitCode != 0){
        printf("Program failed: %d \n", exitCode);
    }

    FILE *pipeP9_17 = popen("config-pin P9_17 i2c", "w");
    
    if(pipeP9_17 == NULL){
        perror("Failed command\n");
    }

    exitCode = WEXITSTATUS(pclose(pipeP9_17));
    if(exitCode != 0){
        printf("Program failed: %d \n", exitCode);
    }


    // Set direction of both ports on I2C GPIO extender 

    FILE *pipePort0 = popen("i2cset -y 1 0x20 0x00 0x00", "w");
    
    if(pipePort0 == NULL){
        perror("Failed command\n");
    }
    exitCode = WEXITSTATUS(pclose(pipePort0));
    if(exitCode != 0){
        printf("Program failed: %d \n", exitCode);
    }

    FILE *pipePort1 = popen("i2cset -y 1 0x20 0x01 0x00", "w");
    
    if(pipePort1 == NULL){
        perror("Failed command\n");
    }

    exitCode = WEXITSTATUS(pclose(pipePort1));
    if(exitCode != 0){
        printf("Program failed: %d \n", exitCode);
    }

    //  Turn on both digits
    GPIO_init(P8_26);
    GPIO_init(P8_27);

    // makes a "00"
    showZeros();

}