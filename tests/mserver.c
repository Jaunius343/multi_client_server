#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h> 
#include <unistd.h>  //Header file for sleep()
#include <pthread.h>

#define MYPORT "3490"  // the port users will be connecting to
#define BACKLOG 10     // how many pending connections queue will hold
#define HOST "127.0.0.2"
#define HOST6 "::1"
#define MAXDATASIZE 100


void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void to_upper(char *str)
{
	int i = 0;
	char chr;
	
	while (str[i]) { 
        chr = str[i]; 
        str[i] = toupper(chr);
        ++i; 
    } 
}

//void *myThread(void *vargp)
//{
	
	//while(1){
		//printf("in thread\n");
		//sleep(5);
	//}
//}

int main(int argc, char *argv[])
{
	//pthread_t thread_id;
	//pthread_create(&thread_id, NULL, myThread, NULL);
	//while(1){
		//printf("main\n");
		//sleep(3);
	//}
	
	
	int status;
	struct addrinfo hints, *res;
	int sockfd, new_fd, numbytes;
	char buf [MAXDATASIZE];
	int yes=1;
	
	int connections[BACKLOG];					//connections don't start with 0, might lose track
	for(int i = 0; i < BACKLOG; ++i){			//of connections.
		connections[i] = 0;
		//printf("%d: %d\n", i, connections[i]);
	}
	
	struct sockaddr_storage their_addr;
	socklen_t addr_size;

	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	//hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
	

	if ((status = getaddrinfo(HOST, MYPORT, &hints, &res)) != 0) {
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
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
    }

	if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1){
		perror("bind");
		exit(1);
	}
	//if ((bind(sockfd, res->ai_addr, res->ai_addrlen)) ! = 0) {
		//fprintf(stderr, "bind error: %s\n", gai_strerror(status));
		//exit(1);
	//}
	
	if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }
    
    

	freeaddrinfo(res); // free the linked-list
	
	printf("server: waiting for connections...\n");
	printf("server: sockfd %d \n", sockfd);
	
	int c_count = 0;
	
	while(1){
		addr_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
		
		if (new_fd == -1) {
            perror("accept");
            continue;
        } else {
			++c_count;
			connections[new_fd] = 1;
			printf("%d \n", new_fd);
		}
		
		char s[INET6_ADDRSTRLEN];
		inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);
        
        char msg1[] = "enter your name: ";
        
        int bytes_sent;
		if ((bytes_sent = send(new_fd, msg1, strlen(msg1), 0)) == -1) {
			perror("send");
			exit(1);
		}
		
		//if ((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1) {
			//perror("recv");
			//exit(1);
		//}g
		
		//buf[numbytes] = '\0';
    
		//printf("received from client: '%s'\n",buf);
		
		//printf("converting...\n");
		//to_upper(buf);
		
		//printf("sending: %s\n", buf);
		//int len = strlen(buf), bytes_sent;
		//if ((bytes_sent = send(new_fd, buf, len, 0)) == -1) {
			//perror("send");
			//exit(1);
		//}
		
		if (send(new_fd, "Hello, world!", 13, 0) == -1){
                perror("send");
        }

	}
	

}



//kaip zinoti, is kurio kliento imti duomenis
//ar reikia naujam thread'e tiesiog tikrinti su recv visus connection'us

