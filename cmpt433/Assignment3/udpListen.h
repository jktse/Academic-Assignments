/*******************
 * Name: Jacky Ken Tse, Kenneth Liang
 * Date: Feb 19, 2021
 * Purpose: UDP code to initialize the socket and start a thread. This thread will listen at port 12345 and await commands.
 * If the command is invalid we display a message otherwise it will display the reuqested command.
 * *****************/

#ifndef UDPLISTEN_H_
#define UDPLISTEN_H_


void UDPListen_startListening();
long unsigned int UDPListen_getThread();
#endif