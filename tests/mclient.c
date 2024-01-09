#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MYPORT "3490"  // the port users will be connecting to
#define MAXDATASIZE 100

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
	int status;
	struct addrinfo hints, *res;
	int sockfd, numbytes;
	char buf [MAXDATASIZE];
	
	if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

	if ((status = getaddrinfo(argv[1], MYPORT, &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		exit(1);
	}

	// servinfo now points to a linked list of 1 or more struct addrinfos

	// ... do everything until you don't need servinfo anymore ....

	if((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1){
			perror("socket");
			exit(1);
	}
	//if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) != 0) {
		//fprintf(stderr, "bind error: %s\n", gai_strerror(status));
		//exit(1);
	//}
	
	if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
            perror("client: connect");
            exit(1);
    }
    
    char s[INET6_ADDRSTRLEN];
    inet_ntop(res->ai_family, get_in_addr((struct sockaddr *)res->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

	freeaddrinfo(res); // free the linked-list
	
	//printf("input text for sending: \n");
	//char input[MAXDATASIZE];
	//int len, bytes_sent;
	//scanf("%s", input);
	
	//len = strlen(input);
	//if ((bytes_sent = send(sockfd, input, len, 0)) == -1) {
			//perror("send");
			//exit(1);
	//}
	
	
	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
			perror("recv");
			exit(1);
		}
	buf[numbytes] = '\0';
    
	printf("received from server: '%s'\n",buf);
	
    

	
	return 0;
}

