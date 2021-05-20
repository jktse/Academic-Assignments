// Note: Some function/implementations are identical to the ones Dr.Fraser has provided in the template.
// New Functions: 
// Note: Generates low latency audio on BeagleBone Black; higher latency found on host.
#include <alsa/asoundlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <limits.h>
#include <alloca.h> // needed for mixer
#include "audioMixer.h"
#include "timing.h"
#include "udpListen.h"

typedef struct {
	// A pointer to a previously allocated sound bite (wavedata_t struct).
	// Note that many different sound-bite slots could share the same pointer
	// (overlapping cymbal crashes, for example)
	wavedata_t *pSound;

	// The offset into the pData of pSound. Indicates how much of the
	// sound has already been played (and hence where to start playing next).
	int location;
} playbackSound_t;

#define DEFAULT_VOLUME 80

#define MIN_BPM 40
#define DEFAULT_BPM 120
#define MAX_BPM 300

#define SAMPLE_RATE 44100
#define NUM_CHANNELS 1
#define SAMPLE_SIZE (sizeof(short)) 			// bytes per sample

// Currently active (waiting to be played) sound bites
#define MAX_SOUND_BITES 30

static snd_pcm_t *handle;

// Sample size note: This works for mono files because each sample ("frame') is 1 value.
// If using stereo files then a frame would be two samples.
static unsigned long playbackBufferSize = 100;
static short *playbackBuffer = NULL;

static playbackSound_t soundBites[MAX_SOUND_BITES];


// Playback threading
void* playbackThread(void* arg);
static _Bool stopping = false;

static pthread_t playbackThreadId;
static pthread_mutex_t audioMutex = PTHREAD_MUTEX_INITIALIZER;

static int volume = 0;
static int BPM = 300;
static long halfBeatNanosec = 0;

long unsigned int AudioMixer_getThread(){
	return playbackThreadId;
}

static void insertSoundBite(wavedata_t *soundData){
	// Check if there is space, if there is we insert.
	for(int i = 0; i < MAX_SOUND_BITES; i ++){
		if(soundBites[i].pSound == NULL){
			soundBites[i].pSound = soundData;
			soundBites[i].location = 0;
			break;
		}
	}
}



int AudioMixer_getBPM(){
	return BPM;
}

long AudioMixer_getHalfBeatNanosec(){
	return halfBeatNanosec;
}

// For a full beat we need to double the value.
static void AudioMixer_setHalfBeatNanosec(int newBPM){
	halfBeatNanosec = (long)((float)((float)(60.0/(float)newBPM)/2.0) * 1000000000);
}

// Everytime we set the BPM we need to update the timing for half beats in nanosec
void AudioMixer_setBPM(int newBPM){
    if(newBPM < MIN_BPM || newBPM > MAX_BPM){
        printf("BPM: %d \n", BPM);
        return;
    }
	printf("BPM: %d \n", newBPM);
	BPM = newBPM;
	AudioMixer_setHalfBeatNanosec(BPM);
}

// Calling this is the same as starting the thread for audioplayback.
void AudioMixer_init(void)
{
	AudioMixer_setVolume(DEFAULT_VOLUME);
	AudioMixer_setBPM(DEFAULT_BPM);

    // Note: This for loop is the only addition to the code Dr.Fraser Provided
    // This loops initializes the soundbites array to all NULL and 0.
	for(int i = 0; i < MAX_SOUND_BITES; i++){
		soundBites[i].pSound = NULL;
		soundBites[i].location = 0;
	}


	// Open the PCM output
	int err = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
	if (err < 0) {
		printf("Playback open error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}

	// Configure parameters of PCM output
	err = snd_pcm_set_params(handle,
			SND_PCM_FORMAT_S16_LE,
			SND_PCM_ACCESS_RW_INTERLEAVED,
			NUM_CHANNELS,
			SAMPLE_RATE,
			1,			// Allow software resampling
			50000);		// 0.05 seconds per buffer
	if (err < 0) {
		printf("Playback open error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}

	// Allocate this software's playback buffer to be the same size as the
	// the hardware's playback buffers for efficient data transfers.
	// ..get info on the hardware buffers:
 	unsigned long unusedBufferSize = 0;
	snd_pcm_get_params(handle, &unusedBufferSize, &playbackBufferSize);
	// ..allocate playback buffer:
	playbackBuffer = malloc(playbackBufferSize * sizeof(*playbackBuffer));

	// Launch playback thread:
	pthread_create(&playbackThreadId, NULL, playbackThread, NULL);
}

// No changes has been made to the next five functions.
// Client code must call AudioMixer_freeWaveFileData to free dynamically allocated data.
void AudioMixer_readWaveFileIntoMemory(char *fileName, wavedata_t *pSound)
{
	assert(pSound);

	// The PCM data in a wave file starts after the header:
	const int PCM_DATA_OFFSET = 44;

	// Open the wave file
	FILE *file = fopen(fileName, "r");
	if (file == NULL) {
		fprintf(stderr, "ERROR: Unable to open file %s.\n", fileName);
		exit(EXIT_FAILURE);
	}

	// Get file size
	fseek(file, 0, SEEK_END);
	int sizeInBytes = ftell(file) - PCM_DATA_OFFSET;
	pSound->numSamples = sizeInBytes / SAMPLE_SIZE;

	// Search to the start of the data in the file
	fseek(file, PCM_DATA_OFFSET, SEEK_SET);

	// Allocate space to hold all PCM data
	pSound->pData = malloc(sizeInBytes);
	if (pSound->pData == 0) {
		fprintf(stderr, "ERROR: Unable to allocate %d bytes for file %s.\n",
				sizeInBytes, fileName);
		exit(EXIT_FAILURE);
	}

	// Read PCM data from wave file into memory
	int samplesRead = fread(pSound->pData, SAMPLE_SIZE, pSound->numSamples, file);
	if (samplesRead != pSound->numSamples) {
		fprintf(stderr, "ERROR: Unable to read %d samples from file %s (read %d).\n",
				pSound->numSamples, fileName, samplesRead);
		exit(EXIT_FAILURE);
	}
	fclose(file);
}

void AudioMixer_freeWaveFileData(wavedata_t *pSound)
{
	pSound->numSamples = 0;
	free(pSound->pData);
	pSound->pData = NULL;
}

void AudioMixer_cleanup(void)
{
	printf("Stopping audio...\n");

	// Stop the PCM generation thread
	stopping = true;
	pthread_join(playbackThreadId, NULL);

	// Shutdown the PCM output, allowing any pending sound to play out (drain)

	snd_pcm_drain(handle);
	snd_pcm_close(handle);

	// Free playback buffer
	// (note that any wave files read into wavedata_t records must be freed
	//  in addition to this by calling AudioMixer_freeWaveFileData() on that struct.)
	
	free(playbackBuffer);
	playbackBuffer = NULL;

	fflush(stdout);
}

int AudioMixer_getVolume()
{
	// Return the cached volume; good enough unless someone is changing
	// the volume through other means and the cached value is out of date.
	return volume;
}

// Function copied from:
// http://stackoverflow.com/questions/6787318/set-alsa-master-volume-from-c-code
// Written by user "trenki".
void AudioMixer_setVolume(int newVolume)
{
	// Ensure volume is reasonable; If so, cache it for later getVolume() calls.
	if (newVolume < 0 || newVolume > AUDIOMIXER_MAX_VOLUME) {
		printf("Volume: %d \n", volume);
		return;
	}
	printf("Volume: %d \n", newVolume);
	volume = newVolume;

    long min, max;
    snd_mixer_t *handle;
    snd_mixer_selem_id_t *sid;
    const char *card = "default";
    const char *selem_name = "PCM";

    snd_mixer_open(&handle, 0);
    snd_mixer_attach(handle, card);
    snd_mixer_selem_register(handle, NULL, NULL);
    snd_mixer_load(handle);

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, selem_name);
    snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);

    snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
    snd_mixer_selem_set_playback_volume_all(elem, volume * max / 100);

    snd_mixer_close(handle);
}

void AudioMixer_queueSound(wavedata_t *pSound)
{
	// Ensure we are only being asked to play "good" sounds:
	assert(pSound->numSamples > 0);
	assert(pSound->pData);

    // Created a function dedicated to queueing sounds.
	pthread_mutex_lock(&audioMutex);
	insertSoundBite(pSound);
	pthread_mutex_unlock(&audioMutex);

}

// Fill the playbackBuffer array with new PCM values to output.
//    playbackBuffer: buffer to fill with new PCM data from sound bites.
//    size: the number of values to store into playbackBuffer
static void fillPlaybackBuffer(short *playbackBuffer, int size)
{
	int currentlocation = 0;
	int numSample = 0;
	int indexToFill = 0;
	int counter = 0;
	memset(playbackBuffer, 0, size * sizeof(short));
	pthread_mutex_lock(&audioMutex);
	for(int i = 0; i < MAX_SOUND_BITES; i++){
		indexToFill = 0;
		if(soundBites[i].pSound != NULL){
			counter ++;
			currentlocation = soundBites[i].location;
			numSample = soundBites[i].pSound->numSamples;
			while(currentlocation < numSample && indexToFill < size){

				int soundData = playbackBuffer[indexToFill] + soundBites[i].pSound->pData[currentlocation];

				if(soundData > SHRT_MAX){
					soundData = SHRT_MAX;
				}
				else if(soundData < SHRT_MIN){
					soundData = SHRT_MIN;
				}

				playbackBuffer[indexToFill] = (short)soundData;
				currentlocation++;
				indexToFill++;
			}
			// Update the soundBite location
			soundBites[i].location = currentlocation;
			if(currentlocation >= numSample){
				soundBites[i].pSound = NULL;
				soundBites[i].location = 0;
			}
		}
	}
	pthread_mutex_unlock(&audioMutex);
}

// No changes to this.
void* playbackThread(void* arg)
{

	while (!stopping) {
		// Generate next block of audio
		//printf("Before Fill Playback Buffer\n");
		fillPlaybackBuffer(playbackBuffer, playbackBufferSize);
		//printf("After Fill Playback Buffer\n");

		// Output the audio
		snd_pcm_sframes_t frames = snd_pcm_writei(handle,
				playbackBuffer, playbackBufferSize);

		// Check for (and handle) possible error conditions on output
		if (frames < 0) {
			fprintf(stderr, "AudioMixer: writei() returned %li\n", frames);
			frames = snd_pcm_recover(handle, frames, 1);
		}
		if (frames < 0) {
			fprintf(stderr, "ERROR: Failed writing audio with snd_pcm_writei(): %li\n",
					frames);
			exit(EXIT_FAILURE);
		}
		if (frames > 0 && frames < playbackBufferSize) {
			printf("Short write (expected %li, wrote %li)\n",
					playbackBufferSize, frames);
		}
	}

	return NULL;
}