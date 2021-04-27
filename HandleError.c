/* This file contain functions to handle error and print message */
#include <stdio.h> 
#include <stdlib.h>
#include <arpa/inet.h> /* for ntohs */

#include "Packet.h"


// print error and exit
void crashOnError(char *errorMessage)
{
	
	perror(errorMessage);
	exit(1);
}

// print the packet
void printPacketWithNtohs(Packet *packet){
	printf("pSeqNo: %d, count: %d, data: %s\n", ntohs(packet->pSeqNo), ntohs(packet->count), packet->data);
}

// print the ACKPacket
void printACKPacketWithNtohs(ACKPacket *ack){
	printf("ACK (%d) received\n", ntohs(ack->index));
}

// print message required by the project instruction
void printSendMessage(Packet *packet){
	if (packet->count == 0 && packet->data[0] == 0){
		printf("End of Transmission Packet with sequence number %d transmitted with %d data bytes\n", ntohs(packet->pSeqNo), ntohs(packet->count));

	}else{
		printf("Packet %d transmitted with %d data bytes\n", ntohs(packet->pSeqNo), ntohs(packet->count));
	}
}

// print message required by the project instruction
void printRecvMessage(Packet *packet, int *totalPkt, int *totalByte){
	if (packet->count == 0 && packet->data[0] == 0){
		printf("End of Transmission Packet with sequence number %d received with %d data bytes\n", ntohs(packet->pSeqNo), ntohs(packet->count));

	}else{
		printf("Packet %d received with %d data bytes\n", ntohs(packet->pSeqNo), ntohs(packet->count));
		*totalPkt += 1;
		*totalByte += ntohs(packet->count);
	}
}