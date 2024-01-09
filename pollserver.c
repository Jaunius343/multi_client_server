/*
** pollserver.c -- a cheezy multiperson chat server
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>

#include <unistd.h>  //Header file for sleep()

#define PORT "9034"   // Port we're listening on

// Get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Return a listening socket
int get_listener_socket(void)
{
    int listener;     // Listening socket descriptor
    int yes=1;        // For setsockopt() SO_REUSEADDR, below
    int rv;

    struct addrinfo hints, *ai, *p;

    // Get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }
    
    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) { 
            continue;
        }
        
        // Lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    // If we got here, it means we didn't get bound
    if (p == NULL) {
        return -1;
    }

    freeaddrinfo(ai); // All done with this

    // Listen
    if (listen(listener, 10) == -1) {
        return -1;
    }

    return listener;
}

// Add a new file descriptor to the set
void add_to_pfds(struct pollfd *pfds[], int newfd, int *fd_count, int *fd_size)
{
    // If we don't have room, add more space in the pfds array
    if (*fd_count == *fd_size) {
        *fd_size *= 2; // Double it

        *pfds = realloc(*pfds, sizeof(**pfds) * (*fd_size));
    }

    (*pfds)[*fd_count].fd = newfd;
    (*pfds)[*fd_count].events = POLLIN; // Check ready-to-read

    (*fd_count)++;
}

// Remove an index from the set
void del_from_pfds(struct pollfd pfds[], int i, int *fd_count)
{
    // Copy the one from the end over this one
    pfds[i] = pfds[*fd_count-1];

    (*fd_count)--;
}

// Main
int main(void)
{
    int listener;     // Listening socket descriptor
	
	int bot; 		  // Bot descriptor
	
    int newfd;        // Newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // Client address
    socklen_t addrlen;

    char buf[256];    // Buffer for client data
    //printf("test 0\n");
    int ncount = 50,
		 nlen = 256;
    char names[ncount][nlen];
	//strcpy(names[2], "test");
	//printf("test 1\n");
	for(int i = 0; i < ncount; ++i){
		memset(names[i], '\0', nlen);
		//for(int j = 0; j < nlen; ++j){
			//printf("%c ", names[i][j]);
			//names[i][j] = '\0';
		//}
		//printf("\n");
	}
	//for(int i = 0; i < 15; ++i){
		//for(int j = 0; j < 50; ++j){
			//printf("%c ", names[i][j]);
			////names[i][j] = '\0';
		//}
		//printf("\n");
	//}
	//printf("test 2\n");	

    char remoteIP[INET6_ADDRSTRLEN];

    // Start off with room for 5 connections
    // (We'll realloc as necessary)
    int fd_count = 0;
    int fd_size = 5;
    struct pollfd *pfds = malloc(sizeof *pfds * fd_size);

    // Set up and get a listening socket
    listener = get_listener_socket();

    if (listener == -1) {
        fprintf(stderr, "error getting listening socket\n");
        exit(1);
    }
    
    printf("Waiting for connections...\n");

    // Add the listener to set
    pfds[0].fd = listener;
    pfds[0].events = POLLIN; // Report ready to read on incoming connection

    fd_count = 1; // For the listener

    // Main loop
    for(;;) {
        int poll_count = poll(pfds, fd_count, -1);

        if (poll_count == -1) {
            perror("poll");
            exit(1);
        }

        // Run through the existing connections looking for data to read
        for(int i = 0; i < fd_count; i++) {
			//printf("fd_count: %d, current %d\n", fd_count, i);

            // Check if someone's ready to read
            if (pfds[i].revents & POLLIN) { // We got one!!

                if (pfds[i].fd == listener) {
                    // If listener is ready to read, handle new connection

                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener,
                        (struct sockaddr *)&remoteaddr,
                        &addrlen);
                        
                      //printf("Step %d\n", 1);
                    //printf("Listener: %d\n", listener);
                    //printf("newfd: %d\n", newfd);

                    if (newfd == -1) {
                        perror("accept");
                    } else {
						
						//printf("Step %d\n", 2);
						
                        //add_to_pfds(&pfds, newfd, &fd_count, &fd_size);

                        printf("pollserver: new connection from %s on "
                            "socket %d\n",
                            inet_ntop(remoteaddr.ss_family,
                                get_in_addr((struct sockaddr*)&remoteaddr),
                                remoteIP, INET6_ADDRSTRLEN),
                            newfd);
                        //printf("Step %d\n", 3);
                        
                        //--------------------------------------------------------
                        char msg[256] = "ATSIUSKVARDA\n";
                        char msg2[256] = "VARDASOK\n";
                        char msg3[256] = "ENTER PASSWORD\n";
                        char msg4[256] = "PASSWORD OK\n";
                        int len = strlen(msg);
                        int len2 = strlen(msg2);
                        int len3 = strlen(msg3);
                        int len4 = strlen(msg4);
                        int numbytes;
						if ((numbytes = send(newfd, msg, len, 0)) == -1) {
								perror("send");
							}
						//printf("sent bytes: %d\n", numbytes);
						
						
						//printf("Step %d\n", 4);
						
						numbytes = recv(newfd, buf, sizeof buf, 0);
						
						buf[numbytes] = '\0';
	
						printf("client --> server: '%s'\n", buf);
						
						//sscanf(buf, "%[^\n]", names[newfd]);
						
						//printf("offset: %d", strcspn(buf, "\n\r"));
						strncpy(names[newfd], buf, strcspn(buf, "\n\r"));		
						//printf("postprocessed name: '%s'\n", names[newfd]);
						
						if (strcmp(names[newfd], "#BOTAS") == 0){
							bot = newfd;
							if ((numbytes = send(newfd, msg3, len3, 0)) == -1) {
								perror("send");
							}
							
							numbytes = recv(newfd, buf, sizeof buf, 0);
						
							buf[numbytes] = '\0';
	
							printf("client --> server: '%s'\n", buf);
							
							if (strcmp(buf, "botpassword") == 0){
								if ((numbytes = send(newfd, msg4, len4, 0)) == -1) {
									perror("send");
								}
							} else {
								close(newfd);
							}
						}
						
						//strcpy(names[newfd], buf);
						
						//memset(msg, '\0', sizeof(msg));
						//strcpy(msg, "VARDASOK\n");
						
						if ((numbytes = send(newfd, msg2, len2, 0)) == -1) {
								perror("send");
							}
						
						add_to_pfds(&pfds, newfd, &fd_count, &fd_size);
						
						
                    }
                } /*else if(pfds[i].fd == bot){
					numbytes = recv(bot, buf, sizeof buf, 0);
						
					buf[numbytes] = '\0';
	
					printf("bot --> server: '%s'\n", buf);
					
				} */else {
                    // If not the listener, we're just a regular client
                    int nbytes = recv(pfds[i].fd, buf, sizeof buf, 0);
                    buf[nbytes] = '\0';
                    //printf("msg from recv: '%s'\n", buf);

                    int sender_fd = pfds[i].fd;
                    //printf("regular client, pfds[i].fd = %d, i = %d\n", pfds[i].fd, i);

                    if (nbytes <= 0) {
                        // Got error or connection closed by client
                        if (nbytes == 0) {
                            // Connection closed
                            printf("pollserver: socket %d hung up\n", sender_fd);
                        } else {
                            perror("recv");
                        }

                        close(pfds[i].fd); // Bye!
                        
                        //if(pfds[i].fd == bot){
							//bot = NULL;
						//}

                        del_from_pfds(pfds, i, &fd_count);
                        
                        memset(names[i], '\0', nlen);

                    } else {
                        // We got some good data from a client
                        
                        if(sender_fd == bot){
							printf("bot -> server: %s\n", buf);
							
							char temp[256];
							memset(temp, '\0', 256);
							
							if(strncmp(buf, "##BAN ", 6) == 0){
								
								char *p = buf + 6;
								char name[256];
								memset(name, '\0', 256);
								strncpy(name, p, strcspn(p, "\0"));
								//printf("name: %s\n", name);
								
								for(int k = 0; k < ncount; ++k){
									if(strcmp(name, names[k]) == 0){
										close(k);
										
										int cl_index;
										for(int i = 0; i < fd_count; ++i){
											if (pfds[i].fd == k){
												cl_index = i;
											}
										}
										del_from_pfds(pfds, cl_index, &fd_count); 
																		   //pacheckinti i reiksme
																		   //kol kas ne ta delete'inu
																		   //cia i yra boto, reik
																		   //kliento surast
										memset(names[k], '\0', nlen);
									}
								}
								
								strcat(temp, name);
								strcat(temp, " banned\n");
								if (send(sender_fd, temp, strlen(temp), 0) == -1) {
									perror("send");
								}
							}
							
						} else {
							printf("msg from recv: '%s'\n", buf);
							
							for(int j = 0; j < fd_count; j++) {
                            // Send to everyone!
                            int dest_fd = pfds[j].fd;
                            
                            char msg[256], temp[256];
                            //printf("size of char: %d\n", sizeof(char));
                            //printf("size of msg: %d\n", sizeof(msg));
							memset(temp, '\0', sizeof(char) * 256);
							memset(msg, '\0', sizeof(char) * 256);
							
							strcat(msg, "PRANESIMAS ");
							//printf("the name: '%s'\n\n", names[sender_fd]);
							
							
							//int idx = 0;
							//char *e = names[sender_fd];
							//while (*e++ != '\n' && idx < 256) idx++;
							//printf("the offset %d\n", idx);
							//strncpy(temp, names[sender_fd], idx-1);
							//sscanf(names[sender_fd], "%[^\n]", temp);
							//printf("the temp: '%s'\n\n", temp);
							
							//sscanf(names[sender_fd], "%[^\\0]", temp); //be sito all good telnet, tik vardo ner dabar
							strcat(msg, names[sender_fd]);
							//strcat(msg, temp);  		//is telnet ateina be \n, todel visa
														//string uzmeta ant virsaus
														
														//nesuprantu kodel neveikia, offset'as
														//rodo, kad \n yra, bet neveikia
														
														
														//tos pacios zinutes '\n' offsetas
														//skiriasi tarp java ir telnet
														
														//carriage byte'as iš telneto ateina...
														//FIXED
							strcat(msg, ": ");
							//printf("msg from init: '%s'\n\n", msg);
							
							strcat(msg, buf);
							//printf("msg from concat-1: '%s'\n\n", msg);
							
							memset(temp, '\0', sizeof(char) * 256);
							sscanf(msg, "%[^\n]", temp);
							strcat(temp, "\n");
							//printf("msg from concat-2: '%s'\n\n", temp);
							//printf("'%s'", msg);

                            // Except the listener and ourselves
                            //if (dest_fd != listener && dest_fd != sender_fd) {
							if (dest_fd != listener) {
								

								//sscanf(msg, "%[^\n]", temp);
								//printf("msg from fgets: '%s'\n", temp);
                                if (send(dest_fd, temp, strlen(temp), 0) == -1) {
                                    perror("send");
                                }
                                printf("Message sent to %d '%s'\n\n", dest_fd, msg);
                            }
						}

                        
                        }
                    }
                } // END handle data from client
            } // END got ready-to-read from poll()
        } // END looping through file descriptors
    } // END for(;;)--and you thought it would never end!
    
    return 0;
}
