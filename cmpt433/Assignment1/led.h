/**************
 * Name: Jacky Ken Tse
 * Date: Janunary 27, 2021
 * Purpose: Led will handle all initialization and writing to the files
 * ****************/
#include <stdbool.h>

#ifndef LED_H_
#define LED_H_

const char* LED_getFileLocation(bool isTrigger, int LED_NUM);

void LED_init(int LED_NUM);
void LED_flip(int LED_NUM);
void LED_flipAll(void);
void LED_reset(int LED_NUM);

#endif
