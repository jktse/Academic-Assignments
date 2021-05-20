/*******************
 * Name: Jacky Ken Tse, Kenneth Liang
 * Date: Feb 19, 2021
 * Purpose: Exports and configure the I2C pins
 * *****************/
#ifndef config_pin_H_
#define config_pin_H_

enum GPIO_state{P8_26 = 61, P8_27 = 44};
const char* GPIO_getFileLocation(char location, int GPIO_NUM);

void configI2CPin();
void GPIO_init(int GPIO_PIN);

#endif