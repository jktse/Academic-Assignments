/**************
 * Name: Jacky Ken Tse
 * Date: Janunary 27, 2021
 * Purpose: GPIO will handle all initialization and handling regarding the gpio (joystick in our case)
 * ****************/

#ifndef GPIO_H_
#define GPIO_H_

enum GPIO_state{up = 26, right = 47, down = 46, left = 65, none = 0};

const char* GPIO_getFileLocation(char location, int GPIO_NUM);

void GPIO_init(int GPIO_PIN);
int GPIO_pressed(void);

#endif
