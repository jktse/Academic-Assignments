#ifndef BEATS_H_
#define BEATS_H_

void Beats_init();
void Beats_deinit();

void Beats_superPosition();

void * playBeats();
void Beats_playBase();
void Beats_playSnare();
void Beats_playHiHat();
long unsigned int Beats_getThread();

void Beats_setStates(int newState);
void Beats_setNextState();
int Beats_getStates();

void Beats_startThread();
void Beats_stopThread();
#endif