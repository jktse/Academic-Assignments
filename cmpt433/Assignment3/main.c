#include <stdio.h>
#include <pthread.h>
#include "beats.h"
#include "audioMixer.h"
#include "gpio.h"
#include "accelerometer.h"
#include "udpListen.h"



int main(void)
{
    // Sets up playback and starts the playback thread    
    AudioMixer_init();
    // Loads .wav files that is being used into memory
    Beats_init();
    // Export and setup the GPIO pins and start thread.
    GPIO_startThread();
    
    // Starts the beats thread which will constantly load sound data for playback.
    Beats_startThread();
    // Starts the Accelerometer thread. (starts listening to data)
    Accel_initThread();

    UDPListen_startListening();

    pthread_join(AudioMixer_getThread(), NULL);
    pthread_join(Beats_getThread(), NULL);
    pthread_join(GPIO_getThread(), NULL);
    pthread_join(Accel_getThread(), NULL);
    pthread_join(UDPListen_getThread(), NULL);


    // Clean up
    AudioMixer_cleanup();
	return 0;
}