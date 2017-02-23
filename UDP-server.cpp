/*
** listener.c -- a datagram sockets "server" demo
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

#define MYPORT "10012"	// the port users will be connecting to

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

typedef struct{
    int8_t TML;
    int8_t requestID;
    int8_t errorCode;
    int32_t result;
}Return;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	struct sockaddr_storage their_addr;
	char buf[MAXBUFLEN];
	socklen_t addr_len;
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("listener: socket");
			continue;
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("listener: bind");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "listener: failed to bind socket\n");
		return 2;
	}

	freeaddrinfo(servinfo);
while(1) {
	printf("listener: waiting to recvfrom...\n");

	addr_len = sizeof their_addr;
	if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
		(struct sockaddr *)&their_addr, &addr_len)) == -1) {
		perror("recvfrom");
		exit(1);
	}
	for(int i = 0; i < 8; i++) {
        buf[i] = int8_t(buf[i]);
        //cout << endl << int(buf[i]);
	}
    Packets* receiver = new Packets();
    receiver->TML = buf[0];
    receiver->requestID = buf[1];
    receiver->opCode = buf[2];
    receiver->numOperands = buf[3];
    receiver->operand1 = ((unsigned int)(unsigned char)buf[4] << 8 | (unsigned int)(unsigned char)buf[5]);
    receiver->operand2 = ((unsigned int)(unsigned char)buf[6] << 8 | (unsigned int)(unsigned char)buf[7]);

    Return* answer = new Return();
    if(receiver->opCode == 0) {
	    answer->result = receiver->operand1 + receiver->operand2;
        answer->errorCode = 0;
        answer->requestID = receiver->requestID;
	}
	else if (receiver->opCode == 1) {
        answer->result = receiver->operand1 - receiver->operand2;
        answer->errorCode = 0;
        answer->requestID = receiver->requestID;
	}
	else if (receiver->opCode == 2) {
            answer->result = receiver->operand1 | receiver->operand2;
            answer->errorCode = 0;
            answer->requestID = receiver->requestID;
	}
	else if (receiver->opCode == 3) {
	    answer->result = receiver->operand1 & receiver->operand2;
	    answer->errorCode = 0;
        answer->requestID = receiver->requestID;
	}
	else if (receiver->opCode == 4) {
	    answer->result = receiver->operand1 >> receiver->operand2;
	    answer->errorCode = 0;
        answer->requestID = receiver->requestID;
	}
	else if (receiver->opCode == 5) {
	    answer->result = receiver->operand1 << receiver->operand2;
	    answer->errorCode = 0;
        answer->requestID = receiver->requestID;
	}
	else {
        answer->errorCode = 127;
        answer->requestID = receiver->requestID;
	}
    answer->result = ntohs(answer->result);
	answer->TML = sizeof(answer->TML) + sizeof(answer->requestID) + sizeof(answer->errorCode) + sizeof(answer->result);

	printf("listener: got packet from %s\n",
		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s));
	printf("listener: packet is %d bytes long\n", numbytes);
	buf[numbytes] = '\0';
	printf("listener: packet contains \"%s\"\n", buf);


    if ((numbytes = sendto(sockfd, answer, answer->TML, 0,
        (struct sockaddr *)&their_addr, addr_len)) == -1) {
		perror("talker: sendto");
		exit(1);
	}
	printf("talker: echoing message now with size %d bytes %s\n", numbytes,
		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s));
}
	close(sockfd);

	return 0;
}
