/*
** talker.c -- a datagram "client" demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <time.h>

#define SERVERPORT "10012"	// the port users will be connecting to

#define MAXBUFLEN 100

using namespace std;

typedef struct{
    int8_t TML;
    int8_t requestID;
    int8_t opCode;
    int8_t numOperands;
    int16_t operand1;
    int16_t operand2;
}Packets;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	char buf[MAXBUFLEN];
    char s[INET6_ADDRSTRLEN];

	if (argc != 3) {
		fprintf(stderr,"usage: talker server portnumber \n");
		exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and make a socket
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("talker: socket");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "talker: failed to create socket\n");
		return 2;
	}
    int requestId = 1;
    int opCode;
    bool moreOps = true;
    while(moreOps == true) {
	Packets* newPacket = new Packets();
	newPacket->numOperands = 1;
	newPacket->requestID = requestId;
	string option;
    cout << "Enter an Opcode: (0 = +, 1 = -, 2 = |, 3 = &, 4 = >>, 5 = << : ";
    cin >> opCode;
    while (opCode < 0 || opCode > 5) {
        cout << "Enter an Opcode: (0 = +, 1 = -, 2 = |, 3 = &, 4 = >>, 5 = << : ";
        cin >> opCode;
    }
    newPacket->opCode = opCode;
    cout << "Please enter Operand 1: ";
    cin >> newPacket->operand1;
    newPacket->operand1 = htons(newPacket->operand1);
    cout << "Do you need a second operand? yes or no? ";
    cin >> option;
    if (option.compare("yes") == 0) {
    cout << "Please enter Operand 2: ";
    cin >> newPacket->operand2;
    newPacket->operand2 = htons(newPacket->operand2);
    newPacket->numOperands = 2;
    }
    else if (option.compare("yes") != 0 || option.compare("no") != 0) {
        exit(1);
    }

    newPacket->TML = sizeof(newPacket->TML) + sizeof(newPacket->requestID) + sizeof(newPacket->numOperands)
                    + sizeof(newPacket->opCode) + sizeof(newPacket->operand1) + sizeof(newPacket->operand2);
    newPacket->requestID = requestId;
    struct timespec start, stop;
		clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	if ((numbytes = sendto(sockfd, newPacket, newPacket->TML, 0,
			 p->ai_addr, p->ai_addrlen)) == -1) {
		perror("talker: sendto");
		exit(1);
	}

	printf("talker: sent %u bytes to %s\n", numbytes, argv[1]);
    printf("I am the listener now: waiting to recvfrom...\n");

	struct sockaddr_storage their_addr;
	socklen_t addr_len = sizeof their_addr;

	if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
		(struct sockaddr *)&their_addr, &addr_len)) == -1) {
		perror("recvfrom");
		exit(1);
	}
	clock_gettime(CLOCK_MONOTONIC_RAW, &stop);

	int32_t results = 0;
	results = ((unsigned int)(unsigned char)buf[3] << 24 | (unsigned int)(unsigned char)buf[4] << 16 | (unsigned int)(unsigned char)buf[5] << 8 | (unsigned int)(unsigned char)buf[6]);
	printf("\nThe round trip time for the request was %.5f seconds.\n",((double) stop.tv_sec + 1.0e-9*stop.tv_nsec) - ((double)start.tv_sec + 1.0e-9*start.tv_nsec));
	cout << "The result is: " << results << endl;
    printf("listener: got packet from %s\n",
		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s));
	printf("listener: packet is %d bytes long\n", numbytes);
	buf[numbytes] = '\0';
	printf("listener: packet contains \"%s\"\n", buf);
    string exit;
	cout << endl << "Are we quitting now? yes or no? ";
	cin >> exit;
	if(exit.compare("yes") == 0) {
        moreOps = false;
	}
	else {
        moreOps == true;
        requestId++;
	}
    }

	freeaddrinfo(servinfo);
	close(sockfd);
	return 0;
}
