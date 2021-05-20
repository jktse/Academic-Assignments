#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "timing.h"
#include "gpio.h"

#define GPIO_EXPORT "/sys/class/gpio/export"

#define dGPIO_26 "/sys/class/gpio/gpio26/direction"
#define dGPIO_47 "/sys/class/gpio/gpio47/direction"
#define dGPIO_46 "/sys/class/gpio/gpio46/direction"
#define dGPIO_65 "/sys/class/gpio/gpio65/direction"

#define vGPIO_26 "/sys/class/gpio/gpio26/value"
#define vGPIO_47 "/sys/class/gpio/gpio47/value"
#define vGPIO_46 "/sys/class/gpio/gpio46/value"
#define vGPIO_65 "/sys/class/gpio/gpio65/value"

#define aGPIO_26 "/sys/class/gpio/gpio26/active_low"
#define aGPIO_47 "/sys/class/gpio/gpio47/active_low"
#define aGPIO_46 "/sys/class/gpio/gpio46/active_low"
#define aGPIO_65 "/sys/class/gpio/gpio65/active_low"

const char* GPIO_getFileLocation(char location, int GPIO_NUM)
{
	if (location == 'v') {
		if (GPIO_NUM == 26) {
			return vGPIO_26;
		} else if (GPIO_NUM == 47) {
			return vGPIO_47;
		} else if (GPIO_NUM == 46) {
			return vGPIO_46;
		} else if (GPIO_NUM == 65) {
			return vGPIO_65;
		}
	} else if (location == 'd'){
		if (GPIO_NUM == 26) {
			return dGPIO_26;
		} else if (GPIO_NUM == 47) {
			return dGPIO_47;
		} else if (GPIO_NUM == 46) {
			return dGPIO_46;
		} else if (GPIO_NUM == 65) {
			return dGPIO_65;
		}
	} else if (location == 'a'){
		if (GPIO_NUM == 26) {
			return aGPIO_26;
		} else if (GPIO_NUM == 47) {
			return aGPIO_47;
		} else if (GPIO_NUM == 46) {
			return aGPIO_46;
		} else if (GPIO_NUM == 65) {
			return aGPIO_65;
		}
	} 
	return NULL;
}

// Ensure all GPIO pins have been exported and the direction of the pin is set for input...
void GPIO_init(int GPIO_PIN){
    // Export the PIN
    FILE *pGpioExportFile = fopen(GPIO_EXPORT, "w");

    if (pGpioExportFile == NULL) {
        printf("ERROR OPENING %s.\n", GPIO_EXPORT);
        exit(-1);
    }

    printf("Exporting Linux GPIO Number: %d\n", GPIO_PIN);
    fprintf(pGpioExportFile, "%d", GPIO_PIN);
    fclose(pGpioExportFile);

    Timing_delay(0, 500000000);

    // After a delay we ensure that the direction is set to input
    const char* fileName = GPIO_getFileLocation('d', GPIO_PIN);
    FILE *pGpioDirectionFile = fopen(fileName, "w");

    if (pGpioDirectionFile == NULL) {
        printf("ERROR OPENING %s.\n", fileName);
        exit(-1);
    }
    //printf("Updating direction for Linux GPIO Number: %d\n", GPIO_PIN);
    fprintf(pGpioDirectionFile, "in");
    fclose(pGpioDirectionFile);

    // We also want to ensure the active_low is 0 just in case it was not setup properly
    fileName = GPIO_getFileLocation('a', GPIO_PIN);
    FILE *pGpioActiveLowFile = fopen(fileName, "w");

    if (pGpioActiveLowFile == NULL) {
        printf("ERROR OPENING %s.\n", fileName);
        exit(-1);
    }
    //printf("Updating Active Low for Linux GPIO Number: %d\n", GPIO_PIN);
    fprintf(pGpioActiveLowFile, "%d", 0);
    fclose(pGpioActiveLowFile);
}

// Return the value located at the GPIO_PIN
static char* GPIO_pinRead(int GPIO_PIN){
    const char* fileName = GPIO_getFileLocation('v', GPIO_PIN);
    FILE *pGpioPinFile = fopen(fileName, "r");
	
        if (pGpioPinFile == NULL) {
            printf("ERROR OPENING %s.\n", fileName);
            exit(-1);
        }

    // Read string (line) We only want to read the one value.
    const int MAX_LENGTH = 1024;
    char * buffer;
    buffer = malloc(sizeof(char)*MAX_LENGTH);
    fgets(buffer, MAX_LENGTH, pGpioPinFile);

    fclose(pGpioPinFile);

    return buffer;
}

int GPIO_pressed(void){
    int none = 0;
    char * state = GPIO_pinRead(up);
    if(*state == '0'){
        return up;
    }
    free(state);
    state = GPIO_pinRead(down);
    if(*state == '0'){
        return down;
    }
    free(state);
    state = GPIO_pinRead(left);
    if(*state == '0'){
        return left;
    }
    free(state);
    state = GPIO_pinRead(right);
    if(*state == '0'){
        return right;
    }
    free(state);

    return none;
}
