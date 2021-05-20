#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "timing.h"
#include "led.h"
#include "gpio.h"
#include "logic.h"


int main( int argc, const char* argv[] ){
	// Initialize Variables
	bool isPlaying = true;
	bool newRound = true;

	int selectPin = 0;
	int pin = 0;
	int randomVal = 0;
	// First part "Hello World"
	printf("Hello embedded world, from Jacky Tse!\n\n\n");

	// Second part "Use LEDs & Read Joystick"

	// First we initialize all 4 LEDs and the 4 joysticks direction in the GPIO
	for(int i = 0; i < 4; i++){
		LED_init(i);

		if(i == 0){
			pin = up;
		}else if (i == 1){
			pin = right;
		}else if (i == 2){
			pin = down;
		}else if (i == 3){
			pin = left;
		}
		GPIO_init(pin);
	}
	printf("\n\n");
	// Print the startup message.
	printf("Press the Zen cape's Joystick in the direction of the LED.\n");
	printf("	Up for LED 0 (top)\n");
	printf("	Down for LED 3 (bottom)\n");
	printf("	Left/Right to exit app.\n");

	pin = 0;
	while(isPlaying){
		if(newRound && (pin == none)){
			if(pin == none){
				//printf("I see none \n");
			}
			newRound = false;
			// Setup which pin to choose, the lights and display message.
			randomVal = rand() % (100 + 1);
			if(randomVal < 50){
				selectPin = up;
				LED_flip(0);
			}else{
				selectPin = down;
				LED_flip(3);
			}
			printf("Press joystick; Current score (%d / %d) \n", LOGIC_getUserScore(), LOGIC_getTotalScore());
		}else{
			pin = GPIO_pressed();
			// Only check if joystick was pushed
			if((pin != none) && (newRound == false)){
				if((pin == left) || (pin == right)){
					// User has quit the application
					isPlaying = false;
					printf("Your final score was (%d / %d) \n", LOGIC_getUserScore(), LOGIC_getTotalScore());
					printf("Thank you for playing! \n");
				}else{
					newRound = true;
					// User has choosen up or down
					if(pin == selectPin){
						// User has choosen correct
						if (selectPin == up){
							LED_flip(0);
						}else{
							LED_flip(3);
						}
						printf("Correct! :P \n");
						LOGIC_incrementTotalScore();
						LOGIC_incrementUserScore();
						LED_flipAll();
						// Light stays on for 0.1 sec
						Timing_delay(0, 100000000);
						LED_flipAll();
					}else{
						// User did not choose correctly
						if (selectPin == up){
							LED_flip(0);
						}else{
							LED_flip(3);
						}
						printf("Incorrect! :( \n");
						LOGIC_incrementTotalScore();
						for(int i = 0; i < 5; i ++){
							LED_flipAll();
							// Light stays on for 0.1 sec
							Timing_delay(0, 100000000);
							LED_flipAll();
							// Light stays off for 0.1 sec
							Timing_delay(0, 100000000);
						}
					}
				}
			}
		}
		// Only check for GPIO input every 25 ms rather than every cycle.
		Timing_delay(0, 25000000);
	}

	printf("Now resetting the LEDS\n");
	for(int i = 0; i < 4; i++){
		LED_reset(i);
	}
	return 0;
}
