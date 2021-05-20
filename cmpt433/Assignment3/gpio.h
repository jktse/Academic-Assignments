#ifndef GPIO_H_
#define GPIO_H_

enum GPIO_state{up = 26, right = 47, down = 46, left = 65, push = 27, none = 0};

const char* GPIO_getFileLocation(char location, int GPIO_NUM);

void GPIO_init(int GPIO_PIN);
int GPIO_pressed(void);

void * GPIO_joystickThread(void * args);
void GPIO_startThread();
long unsigned int GPIO_getThread();

#endif