List of relvant files: README.txt input.txt Makefile Packet.h Client.c Server.c HandleError.c 

Compilation instructions: To compile, type in make or make all. Type in make clean to clean

Configuration file: Makefile

Running instruction: Client and Server are two exeutable files generate by Makefile. Run both of them at same time to send and receive packet.

Example:
For server side: ./server 
For client side: ./client

Notes: the <port> and <server IP> are built in the code. They are defined in the Packet.h as SERVER_IP and PORT.

While server side is running, please enter timeout exponent and Packet Loss Ratio as prompted.
while client side is runing, please enter file name (input.txt) and ACK loss Ratio as prompted.

The name for the output file is out.txt.