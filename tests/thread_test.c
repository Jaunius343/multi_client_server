#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  //Header file for sleep()
#include <pthread.h>

void *myThread(void *vargp)
{
	
	while(1){
		printf("in thread\n");
		sleep(5);
	}
}

int main(int argc, char *argv[])
{
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, myThread, NULL);
	while(1){
		printf("main\n");
		sleep(3);
	}
}



