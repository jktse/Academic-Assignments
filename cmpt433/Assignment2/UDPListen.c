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
#include "UDPListen.h"
#include "sorter.h"
#include "display.h"
#include "a2d.h"

#define PORT 12345
#define MSG_MAX_LEN 1024
#define HELP_MSG_LEN 2048


static char HELP_MSG[HELP_MSG_LEN];
static bool keepGoing = true;
static pthread_t listeningThread;  

// Code from https://stackoverflow.com/questions/14422775/how-to-check-a-given-string-contains-only-number-or-not-in-c
static int digitsOnly(const char *s){
	while (*s){
		if(isdigit(*s++) == 0) return 0;
	}
	return 1;
}
// Code from StackOverflow ends here

static void sendMessage(char* message, int socketDescriptor, struct sockaddr_in *sinRemote){
	unsigned int sin_len = sizeof(*sinRemote);
	sendto( socketDescriptor,
			message, strlen(message),
			0,
			(struct sockaddr *) sinRemote, sin_len);
}


 // Code from http://www.strudel.org.uk/itoa/
static char* itoa(int value, char* result, int base) {
	// check that the base if valid
	if (base < 2 || base > 36) { *result = '\0'; return result; }

	char* ptr = result, *ptr1 = result, tmp_char;
	int tmp_value;

	do {
		tmp_value = value;
		value /= base;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
	} while ( value );

	// Apply negative sign
	if (tmp_value < 0) *ptr++ = '-';
	*ptr-- = '\0';
	while(ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr--= *ptr1;
		*ptr1++ = tmp_char;
	}
	return result;
}
// reference code ends here

static int countDigits(int num){
	if(num < 0){
		return countDigits((num == INT_MIN) ? INT_MAX: -num);
	}
	if(num < 10){
		return 1;
	}
	return 1 + countDigits(num/10);

}


static void acceptCommands(char *command, int socketDescriptor,
							unsigned int sin_len, struct sockaddr_in *sinRemote){
	
	if(strcmp(command, "help") == 0){
		sendto( socketDescriptor,
			HELP_MSG, strlen(HELP_MSG),
			0,
			(struct sockaddr *) sinRemote, sin_len);
	}
	else if(strcmp(command, "count") == 0){
		char messageTx[MSG_MAX_LEN];
		sprintf(messageTx, "Number of arrays sorted = %lld\n", Sorter_getNumberArraySorted());
		sendMessage(messageTx, socketDescriptor, sinRemote);
	}
	else if(strcmp(command, "get length") == 0){
		char messageTx[MSG_MAX_LEN];
		sprintf(messageTx, "Current array length = %d\n", Sorter_getArrayLength());
		sendMessage(messageTx, socketDescriptor, sinRemote);
	}
	else if(strcmp(command, "get array") == 0){

		int length = Sorter_getArrayLength();
		
		int *arrayPtr = Sorter_getArrayData(&length);
	
		int integersSent = 0;
		int index = 0;
		char *buffer = malloc(MSG_MAX_LEN + 1);
		while (integersSent != length)
		{
			memset(buffer, 0, MSG_MAX_LEN);
			int numberOfIntegersToSend = 0;
			int numberOfBytesFree = MSG_MAX_LEN;
			
			while (numberOfBytesFree > 0 && index <= length -1){
				// accounts for the '\n' in the char array
				if((index+1) % 10 == 0){
					numberOfBytesFree--;
				}
				numberOfBytesFree = numberOfBytesFree - countDigits(arrayPtr[index]); 
				numberOfBytesFree -= 2; // for commas and space
				if(numberOfBytesFree < 0){
					break;
				}
				else{
					numberOfIntegersToSend++;
					index++;
				}
			}
			
			for(int i = integersSent; i < (integersSent + numberOfIntegersToSend); i++){

				if((i+1) % 10 == 0){
					itoa(arrayPtr[i], buffer + strlen(buffer), 10);
					strcat(buffer + strlen(buffer), ", \n");
				}
				else if (i == length - 1){
					itoa(arrayPtr[i], buffer + strlen(buffer), 10);
					strcat(buffer + strlen(buffer), "\n");

				}
				else{
					itoa(arrayPtr[i], buffer + strlen(buffer), 10);
					strcat(buffer + strlen(buffer), ", ");

				}

			}

			// join(arrayPtr + (integersSent), numberOfIntegersToSend, ", ", buffer);
			sendMessage(buffer, socketDescriptor, sinRemote);
			integersSent += numberOfIntegersToSend;
			
		}
		free(buffer);
		buffer = NULL;
		free(arrayPtr);
		arrayPtr = NULL;
	}
	else if(strncmp(command, "get ", 4) == 0){

		char * splitCommand = strtok(command, " ");
		splitCommand = strtok(NULL, " ");
		if(digitsOnly(splitCommand)){
			int index = atoi(splitCommand);
			if(index <= Sorter_getArrayLength() && index >= 1){
				char messageTx[MSG_MAX_LEN];
				sprintf(messageTx, "Value %d = %d\n", index, Sorter_getIndexedElement(index));
				sendMessage(messageTx, socketDescriptor, sinRemote);

			}else{
				char messageTx[MSG_MAX_LEN];
				sprintf(messageTx, "Invalid argument. Must be between 1 and %d.\n", Sorter_getArrayLength());
				sin_len = sizeof(*sinRemote);
				sendMessage(messageTx, socketDescriptor, sinRemote);
			}
		}
		else{
			char messageTx[MSG_MAX_LEN];
			sprintf(messageTx, "Invalid number. Please enter digits only.\n");
			sendMessage(messageTx, socketDescriptor, sinRemote);

			
		}
	}
	else if(strcmp(command, "stop") == 0){

		char messageTx[MSG_MAX_LEN];
		sprintf(messageTx, "Program Terminating\n");

		sendMessage(messageTx, socketDescriptor, sinRemote);

		keepGoing = false;
		Sorter_stopSorting();
		a2d_stopReading();
		Display_stopDisplaying();
	}
	else{
		char messageTx[MSG_MAX_LEN];
		sprintf(messageTx, "Not a valid command. Please use 'help' to see avaible commands.\n");
		sendMessage(messageTx, socketDescriptor, sinRemote);
		
	}
}

static void *UDP_Listen(void *args){

    printf("Listening on UDP port %d:\n", PORT);
	printf("Connect using: \n");
	printf("    netcat -u 127.0.0.1 %d\n", PORT);

	// Address
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;                   // Connection may be from network
	sin.sin_addr.s_addr = htonl(INADDR_ANY);    // Host to Network long
	sin.sin_port = htons(PORT);                 // Host to Network short
	
	// Create the socket for UDP
	int socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);

	// Bind the socket to the port (PORT) that we specify
	bind (socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));
	
	while (keepGoing) {
		// Get the data (blocking)
		// Will change sin (the address) to be the address of the client.
		// Note: sin passes information in and out of call!
		struct sockaddr_in sinRemote;
		unsigned int sin_len = sizeof(sinRemote);
		char messageRx[MSG_MAX_LEN];
		int bytesRx = recvfrom(socketDescriptor,
			messageRx, MSG_MAX_LEN, 0,
			(struct sockaddr *) &sinRemote, &sin_len);

		// Make it null terminated (so string functions work):
		int terminateIdx = (bytesRx < MSG_MAX_LEN) ? bytesRx : MSG_MAX_LEN - 1;
		messageRx[terminateIdx-1] = 0;
		
		acceptCommands(messageRx, socketDescriptor, sin_len, &sinRemote);
		
	}

	// Close
	close(socketDescriptor);
	return NULL;
}


void UDPListen_initializeHelpMsg(){

	sprintf(HELP_MSG, "%s%-12s%s%-12s%s%-12s%s%-12s%s%-12s%s", 
	"Accepted command examples:\n", 
	"count", "-- display number arrays sorted.\n",
	"get length", "-- display the length of array currently being sorted.\n",
	"get array", "-- display the full array being sorted.\n",
	"get 10", "-- display the tenth element of array currently being sorted.\n",
	"stop", "-- cause the server program to end.\n\0");

}


void UDPListen_startListening(){
	UDPListen_initializeHelpMsg();
	pthread_create(&listeningThread, NULL, &UDP_Listen, NULL);
}

long unsigned int UDPListen_getThread(){
	return listeningThread;
}
