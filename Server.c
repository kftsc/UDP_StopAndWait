/* source file for server side */
#include <stdio.h> /*printf() and fprintf()*/
#include <sys/socket.h> /*socket(), connect(), send(), and recv()*/
#include <arpa/inet.h> /*sockaddr_in and inet_addr()*/
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h> /* for random seed */
#include <math.h>
#include "Packet.h"

#define MAX_SIZE 255   /* Longest string to echo */

void crashOnError(char *errorMessage); /* fucntion in HandleError.c */
void handleClient(int clientSock); /* function to handle client connection */
void readFileToPacket(char *fileName); /* function to read file in server side */
int simulateLoss(double packetLossRatio);
void printPackets(); /* test function to print packets array */
void printSendMessage(Packet *packet); /* fucntion in HandleError.c */
void printPacketWithNtohs(Packet *packet); /* fucntion in HandleError.c */

struct Packet *packets; // a sequence of Packet

int packetsLen; // number of Packet in the sequence

int main(){
    printf("start server...\n");
    int sock; /* Socket */
	struct sockaddr_in servAddr; /* Local address */
	struct sockaddr_in clntAddr; /* Client address */
	unsigned int cliAddrLen; /* Length of incoming message */
	char echoBuffer[MAX_SIZE]; /* Buffer for echo string */
	unsigned short servPort; /* Server port */
	int recvMsgSize; /* Size of received message */

    int timeOutExpon;
    double timeout; /* time out value in microsecond */
    double packetLossRatio;

    
    servPort = PORT;
    printf("running on: %d\n", servPort);

    /*prompt Timeout and Packet Loss Ratio*/
    printf("Please enter timeout exponent between 1 and 10: ");
    scanf("%d", &timeOutExpon);
    if (timeOutExpon < 1 || timeOutExpon > 10){
        crashOnError("timeout exponent should be between 1 and 10");
    }
    timeout = pow(10, timeOutExpon);
    printf("timeout: %lf\n", timeout);

    printf("Please enter a Packet Loss Ratio between 0 and 1:");
    scanf("%lf", &packetLossRatio);
    if (packetLossRatio > 1){
        crashOnError("Packet Loss Ratio should be between 0 and 1");
    }
    
    printf("%d\n", simulateLoss(packetLossRatio));
    readFileToPacket("input.txt");
    printPackets();

    /* Create socket for connections */
    printf("creating socket...\n");
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		crashOnError( "failed to create socket") ;

    /* create local address structure */
    memset(&servAddr, 0, sizeof(servAddr)); /* Zero out structure */
	servAddr.sin_family = AF_INET; /* Internet address family */
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
	servAddr.sin_port = htons(servPort); /* Local port */

    /* Bind to the local address */
    printf("binding...\n");
	if (bind(sock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
		crashOnError ( "failed to bind");

    /* waiting for connection */
    printf("running...\n");
    for (;;){
		/* Set the size of the in-out parameter */
		cliAddrLen = sizeof(clntAddr);
        struct Packet packetBuffer;
		
		/* Block until receive message from a client */
		if ((recvMsgSize = recvfrom(sock, &packetBuffer, sizeof(packetBuffer), 0, (struct sockaddr *) &clntAddr, &cliAddrLen)) < 0)
			crashOnError("recvfrom() failed") ;
		
		
		printf("Handling client %s\n", inet_ntoa(clntAddr.sin_addr));
		
		/* Send received datagram back to the client */
		if (sendto(sock, &packetBuffer, recvMsgSize, 0, (struct sockaddr *) &clntAddr, sizeof(clntAddr)) != recvMsgSize)
			crashOnError("sendto() sent a different number of bytes than expected");
    }

}


void handleClient(int clientSock){

}

void readFileToPacket(char* fileName){
    printf("open the file: %s\n", fileName);
    FILE *fp = fopen(fileName, "r");
    if (fp == NULL){
        crashOnError("fail to read file");
    }

    char chunk[DATA_SIZE];
    packetsLen = 0;
    while(fgets(chunk, DATA_SIZE, fp) != NULL){
        packetsLen ++ ;
    }
    //printf("number of packet: %i\n", packetsLen);
    fclose(fp);

    packets = malloc(packetsLen * sizeof(Packet)); // accloate space for the packet array

    // open file again  to read each line
    fp = fopen(fileName, "r");
    if (fp == NULL){
        crashOnError("fail to read file");
    }

    for(int i = 0; fgets(packets[i].data, sizeof(packets[i].data), fp) != NULL; i++){
        packets[i].pSeqNo = htons(i);
        packets[i].count = htons(strlen(packets[i].data));
    }
    fclose(fp);
}

int simulateLoss(double packetLossRatio){
    double n = 0;
    
    n = ((double)rand() / (double)RAND_MAX);
    printf("random: %f; input: %f\n",n, packetLossRatio);
    if (n < packetLossRatio){
        return 1;
    }else{
        return 0;
    }
}

void printPackets(){
    printf("print the packet seqence:\n");
    for (int i = 0; i < packetsLen; i++){
        printf("count: %d, sequenceNumber: %d, data: %s", ntohs(packets[i].count), ntohs(packets[i].pSeqNo), packets[i].data);
    }
    printf("\n");
}