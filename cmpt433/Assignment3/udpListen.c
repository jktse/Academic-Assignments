#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>			
#include <pthread.h>
#include <unistd.h>	
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <limits.h>
#include "udpListen.h"
#include "beats.h"
#include "audioMixer.h"

#define PORT 12345
#define MSG_MAX_LEN 1024

#define UPTIME_FILE "/proc/uptime"


static bool keepGoing = true;
static pthread_t listeningThread;  
static time_t uptime = 0;
static int upHours = 0;
static int upMinutes = 0;
static int upSeconds = 0;
static int socketDescriptor;
static struct sockaddr_in sinRemote;

static void sendMessage(char* message, int socketDescriptor, struct sockaddr_in *sinRemote){
	unsigned int sin_len = sizeof(*sinRemote);
	sendto( socketDescriptor,
			message, strlen(message),
			0,
			(struct sockaddr *) sinRemote, sin_len);
}


static void acceptCommands(char *command, int socketDescriptor,
							unsigned int sin_len, struct sockaddr_in *sinRemote){
				
	char messageTx[MSG_MAX_LEN];
	if(strcmp(command, "update uptime") == 0){
		FILE *file;
		file = fopen(UPTIME_FILE, "r");

		if(file == NULL){
			perror("error opening %s\n" UPTIME_FILE);
			return;
		}
		char buffer[MSG_MAX_LEN];

		char *b = fgets(buffer, MSG_MAX_LEN, file);
		
		if(b == buffer){
			char *end_ptr;
			double upsecs = strtod(buffer, &end_ptr);
			if(buffer != end_ptr){
				uptime = (0 <= upsecs && upsecs < LONG_MAX ? upsecs : -1);
			}

		}

		fclose(file);
		// printf("Read: %s'\n", buffer);
		// printf("uptime is %ld\n", uptime);
		upHours = floor(uptime / 3600);
		upMinutes = floor((uptime/ 60) % 60 );
		upSeconds = floor(uptime % 60);
		// printf("upHour is %d, UpMin is %d, UpSec is %d\n", upHours, upMinutes, upSeconds);
		sprintf(messageTx, "%d: %d: %d(H:M:S)\n", upHours, upMinutes, upSeconds);
	}
	else if(strcmp(command, "mode none") == 0){
		Beats_setStates(0);
		sprintf(messageTx, "Not playing any beats\n");
	}
	else if(strcmp(command, "mode rock 1") == 0){
		Beats_setStates(1);
		sprintf(messageTx, "Playing Rock #1\n");
	}
	else if(strcmp(command, "mode rock 2") == 0){
		Beats_setStates(2);
		sprintf(messageTx, "Playing Rock #2\n");
	}
	else if(strcmp(command, "mode Kenenth beat") == 0){
		Beats_setStates(3);
		sprintf(messageTx, "Playing Kenneth's beat\n");
	}
	else if(strcmp(command, "mode Jacky beat") == 0){
		Beats_setStates(4);
		sprintf(messageTx, "Playing Jacky's beat\n");
	}
	else if(strcmp(command, "volume down") == 0){
		int volume = AudioMixer_getVolume();
		AudioMixer_setVolume(volume - 5);
		sprintf(messageTx, "Decrease volume to: %d\n", AudioMixer_getVolume());		
	}
	else if(strcmp(command, "volume up") == 0){
		
		int volume = AudioMixer_getVolume();
		AudioMixer_setVolume(volume + 5);
		sprintf(messageTx, "Increased volume to: %d\n", AudioMixer_getVolume());
	}
	else if(strcmp(command, "tempo down") == 0){
		int tempo = AudioMixer_getBPM();

		AudioMixer_setBPM(tempo - 5);
		sprintf(messageTx, "Decrease tempo to: %d\n", AudioMixer_getBPM());
		
	}
	else if(strcmp(command, "tempo up") == 0){
		
		int tempo = AudioMixer_getBPM();
		AudioMixer_setBPM(tempo + 5);
		sprintf(messageTx, "Increase tempo to: %d\n", AudioMixer_getBPM());
	}
	else if(strcmp(command, "play hitHat") == 0){
	
		Beats_playHiHat();
		sprintf(messageTx, "Hit-Hat played\n");
	}
	else if(strcmp(command, "play snare") == 0){

		Beats_playSnare();
		sprintf(messageTx, "Snare played\n");
	}
	else if(strcmp(command, "play base") == 0){

		Beats_playBase();
		sprintf(messageTx, "base played\n");
	}
	else if(strcmp(command, "update volume") == 0){
		sprintf(messageTx, "volume: %d\n", AudioMixer_getVolume());
	}
	else if(strcmp(command, "update tempo") == 0){
		// printf("updating tempo...\n");
		sprintf(messageTx, "tempo: %d\n", AudioMixer_getBPM());
	}
	else if(strcmp(command, "update mode") == 0){
		int state = Beats_getStates();
		switch (state)
		{
		case 0:
			sprintf(messageTx, "mode None\n");
			break;
		case 1:
			sprintf(messageTx, "mode Rock 1\n");
			break;
		case 2:
			sprintf(messageTx, "mode Rock 2\n");
			break;
		case 3:
			sprintf(messageTx, "mode Kenneth's Beat\n");
			break;
		case 4:
			sprintf(messageTx, "mode Jacky's Beat\n");
			break;
		default:
			break;
		}
	}
	else{
		sprintf(messageTx, "Unreconized command\n");
	}
	
	sendMessage(messageTx,socketDescriptor, sinRemote);


}


static void *UDP_Listen(void *args){

    printf("Listening on UDP port %d:\n", PORT);

	// Address
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;                   // Connection may be from network
	sin.sin_addr.s_addr = htonl(INADDR_ANY);    // Host to Network long
	sin.sin_port = htons(PORT);                 // Host to Network short
	
	// Create the socket for UDP
	socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);
	
	// Bind the socket to the port (PORT) that we specify
	bind (socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));
	

	while (keepGoing) {
		// Get the data (blocking)
		// Will change sin (the address) to be the address of the client.
		// Note: sin passes information in and out of call!
		
		unsigned int sin_len = sizeof(sinRemote);
		char messageRx[MSG_MAX_LEN];
		int bytesRx = recvfrom(socketDescriptor,
			messageRx, MSG_MAX_LEN, 0,
			(struct sockaddr *) &sinRemote, &sin_len);

		// Make it null terminated (so string functions work):
		int terminateIdx = (bytesRx < MSG_MAX_LEN) ? bytesRx : MSG_MAX_LEN - 1;
		messageRx[terminateIdx] = 0;
		
		acceptCommands(messageRx, socketDescriptor, sin_len, &sinRemote);
		
	}

	// Close
	close(socketDescriptor);
	return NULL;
}


void UDPListen_startListening(){
	pthread_create(&listeningThread, NULL, &UDP_Listen, NULL);
}

long unsigned int UDPListen_getThread(){
	return listeningThread;
}
