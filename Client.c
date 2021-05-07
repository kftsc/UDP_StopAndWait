/* source file for client side */
#include <stdio.h> /*printf() and fprintf()*/
#include <sys/socket.h> /*socket(), connect(), send(), and recv()*/
#include <arpa/inet.h> /*sockaddr_in and inet_addr()*/
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h> /* for random seed */
#include "Packet.h"


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
    unsigned int fromSize;
	unsigned short servPort;
	char *servIP;
    unsigned int packetSize;
    int recvDataSize;

    int totalPktRecv = 0; /* Total number of data packets received successfully */
    int totalDupPkt = 0;  /* Number of duplicate data packets received */
    int pktRecv = 0; /* Number of data packets received successfully, not including duplicates */
    int totalByteRecv = 0; /* Total number of data bytes received which are delivered to user (this should be the sum of the count fields of allreceived packets not including duplicates) */
    int ackSended = 0; /* Number of ACKs transmitted without loss */
    int ackDropped = 0; /* Number of ACKs generated but dropped due to loss */
    int totalAckGener = 0; /* Total number of ACKs generated (with and without loss) */

    Packet sendPacket;
    Packet recvPacket;
    int lastArrive = -1;
    double ACKLossRatio;



    servIP = SERVER_IP;
    servPort = PORT;

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


    /* send the file name packet to the server */
    printf("sending...\n");
    if (sendto(sock, &sendPacket, packetSize, 0, (struct sockaddr *)&servAddr, sizeof(servAddr))!=packetSize)
		crashOnError("Error on send");

    /* Receive from server */
    printf("receiving...\n");
    FILE *output = fopen("out.txt", "w");
    if (output == NULL){
        crashOnError("failed to open output file\n");
    }
    do{
        fromSize = sizeof(fromAddr);fromSize = sizeof(fromAddr);
        /* receive packet from server */
        if ((recvDataSize = recvfrom(sock, &recvPacket, sizeof(Packet), 0, (struct sockaddr *) &fromAddr, &fromSize)) != sizeof(Packet))
            crashOnError("failed to receive packet header");
        
        if (recvPacket.count == 0 && recvPacket.data[0] == 0){
            /* receive eof */
            printf("End of Transmission Packet with sequence number %d received with %d data bytes\n", ntohs(recvPacket.pSeqNo), ntohs(recvPacket.count));
            break;
        }
        printf("Packet %d received with %d data bytes\n", ntohs(recvPacket.pSeqNo), ntohs(recvPacket.count));
        totalPktRecv++;
        if (lastArrive != recvPacket.pSeqNo){
            /* first time recv pkt, output to file */
            fputs(recvPacket.data, output);

            lastArrive = recvPacket.pSeqNo;

            printf("Packet %d delivered to user\n", ntohs(recvPacket.pSeqNo));
            pktRecv++;
            totalByteRecv += ntohs(recvPacket.count);
        }else{
            /* receive duplicate */
            printf("Duplicate packet %d received with %d data bytes\n", ntohs(recvPacket.pSeqNo), ntohs(recvPacket.count));

            totalDupPkt++;
        }

        /* send ack to server for non-EOF packet*/
        if (recvPacket.count != 0 && recvPacket.data != NULL){
            totalAckGener++;
            printf("ACK %d generated for transmission\n", ntohs(recvPacket.pSeqNo));
            if (simulateACKLoss(ACKLossRatio) == 0){ /* send ack successfully */
                ACKPacket ack;
                ack.index = recvPacket.pSeqNo;
                if (sendto(sock, &ack, sizeof(ack), 0, (struct sockaddr *)&servAddr, sizeof(servAddr))!=sizeof(ACKPacket))
                    crashOnError("Error on send ack to server");
                
                ackSended++;
                printf("ACK %d successfully transmitted\n", ntohs(ack.index));
            }else{
                /* ack loss */
                ackDropped++;
                printf("ACK %d lost\n", ntohs(recvPacket.pSeqNo));
            }
        }

    }while(recvPacket.count != 0 && recvPacket.data != NULL);
    fclose(output);

    printf("----------------------------------------------------------------------------------------------\n");
    printf("%d data packets received successfully\n", totalPktRecv);
    printf("%d duplicate data packets received)\n", totalDupPkt);
    printf("%d data packets received successfully, not including duplicates\n", pktRecv);
    printf("%d data bytes received which are delivered to user\n", totalByteRecv);
    printf("%d ACKs transmitted without loss\n", ackSended);
    printf("%d ACKs generated but dropped due to loss\n", ackDropped);
    printf("%d ACKs generated (with and without loss)\n", totalAckGener);
    close(sock);
	exit(0);
}


int simulateACKLoss(double ACKLossRatio){
    double n = 0;
    
    n = ((double)rand() / (double)RAND_MAX);
    //printf("random: %f; input: %f\n",n, ACKLossRatio);
    if (n < ACKLossRatio){
        return 1;
    }else{
        return 0;
    }
}