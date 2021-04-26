/* source file for client side */
#include <stdio.h> /*printf() and fprintf()*/
#include <sys/socket.h> /*socket(), connect(), send(), and recv()*/
#include <arpa/inet.h> /*sockaddr_in and inet_addr()*/
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h> /* for random seed */
#include "Packet.h"

#define MAX_SIZE 255 /*Longest string to send*/

void crashOnError(char *errorMessage); /* fucntion in HandleError.c */
void printPacketWithNtohs(Packet *packet); /* fucntion in HandleError.c */
void printRecvMessage(Packet *packet, int *totalPkt, int *totalByte); /* fucntion in HandleError.c */

int simulateACKLoss(double ACKLossRatio);


int main(){
    printf("start client...\n");
    srand(time(NULL)); /* randmize seed */
    int sock; /*Socket descriptor*/
	struct sockaddr_in servAddr; /* Echo server address */
	struct sockaddr_in fromAddr; /* Source address of echo */
	unsigned short servPort;
	unsigned int fromSize;
	char *servIP;
	char *echoString; /* for test only  */
	char echoBuffer[MAX_SIZE+1];
	unsigned int echoStringLen;
    unsigned int packetSize;
	int respStringLen;

    Packet sendPacket;
    double ACKLossRatio;

    servIP = SERVER_IP;
    servPort = PORT;
    echoString = "test"; /* for test only  */
    echoStringLen = strlen(echoString); /* for test only  */

    printf("server IP: %s, Port: %d\n", servIP, servPort); /* check IP and Port */

    /* prompt fileName and ACK Loss Ratio*/
    printf("Please enter the file name: ");
    fgets(sendPacket.data, DATA_SIZE, stdin);  // read file name into packet
    sendPacket.data[strlen(sendPacket.data) - 1] = NULL; // handle the \n
    printf("input file name is: %s\n", sendPacket.data);
    sendPacket.count = htons(strlen(sendPacket.data));
    sendPacket.pSeqNo = htons(0);

    packetSize = sizeof(sendPacket);

    printf("Please enter a ACK Loss Ratio between 0 and 1:");
    scanf("%lf", &ACKLossRatio);
    if (ACKLossRatio > 1){
        crashOnError("ACK Loss Ratio should be between 0 and 1");
    }
    
    printf("%d\n", simulateACKLoss(ACKLossRatio)); /* test simulate function */

    /* create UDP socket */
    printf("creating socket...\n");
    if((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP))<0)
		crashOnError("failed to create socket");

    /* create the server address structure */
    memset(&servAddr, 0, sizeof(servAddr)); /* Zero out structure */
	servAddr.sin_family = AF_INET;   /* Internet addr family */
	servAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
	servAddr.sin_port = htons(servPort);  /* Server port */


    /* send the packet to the server */
    printf("sending...\n");
    if (sendto(sock, &sendPacket, packetSize, 0, (struct sockaddr *)&servAddr, sizeof(servAddr))!=packetSize)
		crashOnError("Error on send");

    /* Receive from server */
    printf("receiving...\n");
    struct Packet packetBuffer;
    fromSize = sizeof(fromAddr);
	if ((respStringLen = recvfrom(sock, &packetBuffer, sizeof(packetBuffer), 0, (struct sockaddr *) &fromAddr, &fromSize)) != packetSize)
		crashOnError("recvfrom() failed") ;
	
	if (servAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
	{
		fprintf(stderr,"Error: received a packet from unknown source.\n");
		exit(1);
	}
	
	
	/* null-terminate the received data */
	echoBuffer[respStringLen] = '\0' ;
	printf("Received: %s,%d,%d\n", packetBuffer.data, packetBuffer.count, packetBuffer.pSeqNo); /* Print the echoed arg */
	printPacketWithNtohs(&packetBuffer);
    close(sock);
	exit(0);
}


int simulateACKLoss(double ACKLossRatio){
    double n = 0;
    
    n = ((double)rand() / (double)RAND_MAX);
    printf("random: %f; input: %f\n",n, ACKLossRatio);
    if (n < ACKLossRatio){
        return 1;
    }else{
        return 0;
    }
}