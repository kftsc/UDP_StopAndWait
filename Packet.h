#include <stdio.h> 
#include <stdlib.h>

#define SERVER_IP "127.0.0.1"
#define PORT 18062
#define DATA_SIZE 81

/* struct for Header and Packet */

typedef struct PacketHeader{
    short count;
    short pSeqNo;
} PacketHeader;

typedef struct Packet{
    //PacketHeader header;
    char data[DATA_SIZE];  // the data should be 80 bits long with the \n. In C, however, there is a invisiable bit in array. So, here use 81
    short count;
    short pSeqNo;
    char padding[3];
} Packet;

Packet eof;


