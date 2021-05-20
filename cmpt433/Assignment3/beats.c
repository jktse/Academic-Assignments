#include <alsa/asoundlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <limits.h>
#include <alloca.h> // needed for mixer
#include "audioMixer.h"
#include "timing.h"


#define BASE_DRUM "wave-files/100051__menegass__gui-drum-bd-hard.wav"
#define HI_HAT "wave-files/100053__menegass__gui-drum-cc.wav"
#define SNARE_DRUM "wave-files/100059__menegass__gui-drum-snare-soft.wav"
#define HI_TOM "wave-files/100063__menegass__gui-drum-tom-hi-soft.wav"
#define MID_TOM "wave-files/100067__menegass__gui-drum-tom-mid-soft.wav"
#define SNARE_HARD "wave-files/100058__menegass__gui-drum-snare-hard.wav"
#define SAMPLE_SIZE (sizeof(short)) 			// bytes per sample

#define NUMBER_STATES 5

wavedata_t baseDrumFile;
wavedata_t hiHatFile;
wavedata_t snareDrumFile;

wavedata_t endingCrashFile;
wavedata_t splashFile;
wavedata_t hiTomFile;
wavedata_t midTomFile;
wavedata_t snareHardFile;


static bool play = true;
static pthread_t beatsThread;

static int states = 0;

// Load the drum beat data into data structure.
void Beats_init(){
    AudioMixer_readWaveFileIntoMemory(BASE_DRUM, &baseDrumFile);
    AudioMixer_readWaveFileIntoMemory(HI_HAT, &hiHatFile);
    AudioMixer_readWaveFileIntoMemory(SNARE_DRUM, &snareDrumFile);
    AudioMixer_readWaveFileIntoMemory(HI_TOM, &hiTomFile);
    AudioMixer_readWaveFileIntoMemory(MID_TOM, &midTomFile);
    AudioMixer_readWaveFileIntoMemory(SNARE_HARD, &snareHardFile);
    
}

void Beats_deinit(){
    AudioMixer_freeWaveFileData(&baseDrumFile);
    AudioMixer_freeWaveFileData(&hiHatFile);
    AudioMixer_freeWaveFileData(&snareDrumFile);
    AudioMixer_freeWaveFileData(&hiTomFile);
    AudioMixer_freeWaveFileData(&midTomFile);
    AudioMixer_freeWaveFileData(&snareHardFile);
}

void Beats_playBase(){
    AudioMixer_queueSound(&baseDrumFile);
}

void Beats_playSnare(){
    AudioMixer_queueSound(&snareDrumFile);
}

void Beats_playHiHat(){
    AudioMixer_queueSound(&hiHatFile);
}

static void Beats_rock(){
    AudioMixer_queueSound(&hiHatFile);
    AudioMixer_queueSound(&baseDrumFile);
    Timing_delay(0, AudioMixer_getHalfBeatNanosec());
    if(states != 1) return;

    AudioMixer_queueSound(&hiHatFile);
    Timing_delay(0, AudioMixer_getHalfBeatNanosec());
    if(states != 1) return;

    AudioMixer_queueSound(&hiHatFile);
    AudioMixer_queueSound(&snareDrumFile);
    Timing_delay(0, AudioMixer_getHalfBeatNanosec());
    if(states != 1) return;

    AudioMixer_queueSound(&hiHatFile);
    Timing_delay(0, AudioMixer_getHalfBeatNanosec());
}

static void Custom_Rock(){
    AudioMixer_queueSound(&hiHatFile);
    AudioMixer_queueSound(&baseDrumFile);
    Timing_delay(0, AudioMixer_getHalfBeatNanosec());
    if(states != 2) return;

    AudioMixer_queueSound(&hiHatFile);
    Timing_delay(0, AudioMixer_getHalfBeatNanosec());
    if(states != 2) return;

    AudioMixer_queueSound(&hiHatFile);
    AudioMixer_queueSound(&snareDrumFile);
    Timing_delay(0, AudioMixer_getHalfBeatNanosec());
    if(states != 2) return;

    AudioMixer_queueSound(&hiHatFile);
    Timing_delay(0, AudioMixer_getHalfBeatNanosec());
    if(states != 2) return;

    AudioMixer_queueSound(&hiHatFile);
    AudioMixer_queueSound(&baseDrumFile);
    Timing_delay(0, AudioMixer_getHalfBeatNanosec());
    if(states != 2) return;

    AudioMixer_queueSound(&hiHatFile);
    AudioMixer_queueSound(&baseDrumFile);
    Timing_delay(0, AudioMixer_getHalfBeatNanosec());
    if(states != 2) return;

    AudioMixer_queueSound(&hiHatFile);
    AudioMixer_queueSound(&snareDrumFile);
    Timing_delay(0, AudioMixer_getHalfBeatNanosec());
    if(states != 2) return;

    AudioMixer_queueSound(&hiHatFile);
    Timing_delay(0, AudioMixer_getHalfBeatNanosec());
}

void kennethBeats(){
    AudioMixer_queueSound(&hiHatFile);
    Timing_delay(0, AudioMixer_getHalfBeatNanosec());
    if(states != 3) return;
    AudioMixer_queueSound(&hiHatFile);
    Timing_delay(0, AudioMixer_getHalfBeatNanosec());
    if(states != 3) return;
    AudioMixer_queueSound(&snareHardFile);
    Timing_delay(0, AudioMixer_getHalfBeatNanosec());
    if(states != 3) return;
    return;
}

// Only 180 BPM above XD
void jackyBeats(){
    AudioMixer_queueSound(&baseDrumFile);
    AudioMixer_queueSound(&midTomFile);   
    Timing_delay(0, AudioMixer_getHalfBeatNanosec());
    if(states != 4) return;
    
    Timing_delay(0, AudioMixer_getHalfBeatNanosec());
    if(states != 4) return;

    AudioMixer_queueSound(&baseDrumFile);
    AudioMixer_queueSound(&midTomFile);   
    Timing_delay(0, AudioMixer_getHalfBeatNanosec());
    if(states != 4) return;

    AudioMixer_queueSound(&baseDrumFile);
    Timing_delay(0, AudioMixer_getHalfBeatNanosec());
    if(states != 4) return;
    
    Timing_delay(0, AudioMixer_getHalfBeatNanosec());
    if(states != 4) return;

    AudioMixer_queueSound(&baseDrumFile);
    AudioMixer_queueSound(&midTomFile);   
    Timing_delay(0, AudioMixer_getHalfBeatNanosec());
    if(states != 4) return;

    AudioMixer_queueSound(&hiTomFile);
    Timing_delay(0, AudioMixer_getHalfBeatNanosec());
    if(states != 4) return;
}

void none(){
    return;
}

void * playBeats(){
    while(play){
        if(states == 0){
            none();
        }else if(states == 1){
            Beats_rock();
        }else if(states == 2){
            Custom_Rock();
        }else if(states == 3){
            kennethBeats();
        }else if(states == 4){
            jackyBeats();
        }
    }
    return NULL;
}

int Beats_getStates(){
    return states;
}

void Beats_setStates(int newState){

    if(newState >= 0 && newState < NUMBER_STATES){
        states = newState;
    }
    else{
        states = states % NUMBER_STATES;
    }
    printf("State is now : %d \n", states);
}

void Beats_setNextState(){
    states ++;
    if(states % NUMBER_STATES == 0){
        states = 0;
    }
    printf("State is now : %d \n", states);
}

void Beats_startThread(){
    pthread_create(&beatsThread, NULL, &playBeats, NULL);
}

long unsigned int Beats_getThread(){
    return beatsThread;
}

void Beats_stopThread(){
    play = false;
}