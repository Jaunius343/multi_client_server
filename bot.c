#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MYPORT "9034"  // the port users will be connecting to
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
	
	int ncount_max = 50, ncount = 0,
		 nlen = 256;
    char names[ncount_max][nlen];
	for(int i = 0; i < ncount_max; ++i){
		memset(names[i], '\0', nlen);
	}
	int msg_count[ncount_max];
	for(int i = 0; i < ncount_max; ++i){
		msg_count[i] = 1;
	}
	
	if (argc != 3) {
        fprintf(stderr,"usage: client hostname port\n");
        exit(1);
    }

	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

	if ((status = getaddrinfo(argv[1], argv[2], &hints, &res)) != 0) {
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
	
	
	
	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
		perror("recv");
		exit(1);
	}
	buf[numbytes] = '\0';
    
	printf("serveris -> klientas: %s\n",buf);
	
	char name[] = "#BOTAS";
	char pass[] = "botpassword";
	char msg1[] = "no bans\n";
	int len = strlen(name),
		len2 = strlen(pass),
		len3 = strlen(msg1),
		bytes_sent;
	
	
	//printf("input text for sending: \n");
	//char input[MAXDATASIZE];
	//scanf("%s", input);
	
	//send name
	if ((bytes_sent = send(sockfd, name, len, 0)) == -1) {
			perror("send");
			exit(1);
	}
	
	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
			perror("recv");
			exit(1);
		}
	buf[numbytes] = '\0';
    
	printf("serveris -> klientas: '%s'\n",buf);
	
	//send password
	if ((bytes_sent = send(sockfd, pass, len2, 0)) == -1) {
			perror("send");
			exit(1);
	}
	
	//receive password confirmation
	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
			perror("recv");
			exit(1);
		}
	buf[numbytes] = '\0';
    
	printf("serveris -> klientas: '%s'\n",buf);
	
	//receive name confirmation
	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
			perror("recv");
			exit(1);
		}
	buf[numbytes] = '\0';
    
	printf("serveris -> klientas: '%s'\n",buf);
	
	//formatas
	//PRANESIMAS [name]: [message]
    while(1){
		int flag2 = 0;
		for(int i = 0; i < ncount; ++i){
			if (msg_count[i] > 10){
				char ban[256];
				memset(ban, '\0', sizeof(char) * 256);
				strcat(ban, "##BAN ");
				strcat(ban, names[i]);
				if ((bytes_sent = send(sockfd, ban, strlen(ban), 0)) == -1) {
					perror("send");
					exit(1);
				}
				
				//add receive 
				if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
					perror("recv");
					exit(1);
				}
				
				buf[numbytes] = '\0';
				printf("serveris -> klientas: '%s'\n",buf);
				
				flag2 = 1;
				
			} 
		}
		if (flag2 == 0){
			if ((bytes_sent = send(sockfd, msg1, len3, 0)) == -1) {
					perror("send");
					exit(1);
			}
		}
		
		if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
			perror("recv");
			exit(1);
		}
		
		buf[numbytes] = '\0';
		printf("serveris -> klientas: '%s'\n",buf);
		
		char *p = buf+11;
		printf("p: %s\n", p);
		//printf("p: %s\n", *p);
		//printf("p: %s\n", &p);
		
		char temp[MAXDATASIZE];
		memset(temp, '\0', MAXDATASIZE);
		strncpy(temp, p, strcspn(p, ":"));
		printf("name: '%s'\n", temp);
		
		int flag = 0;
		for(int i = 0; i < ncount; ++i){
			if(strcmp(temp, names[i]) == 0){
				++msg_count[i];
				flag = 1;
			}
		}
		if(flag == 0){
			strcpy(names[ncount], temp);
			++ncount;
		}
		
		//system("clear");
		printf("\033[2J");
		for(int i = 0; i < ncount; ++i){
			printf("'%s' messages sent: %d\n", names[i], msg_count[i]);
		}
		
	}

	
	return 0;
}

