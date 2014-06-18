#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<stdio.h>
#include<sys/socket.h>
#include<signal.h>
#include<sys/stat.h>
#include"epollserver.h"


int main(int argc, char* argv[]){
	//useage:./epollserver port  worker_process_num 
	if(argc < 3){
		printf("useage:./epollserver port  worker_process_num\n");
		exit(0); 
	}
	int port = atoi(argv[1]);
	int worker_num = atoi(argv[2]);

		
	//master start work
	master_process(port, worker_num);
	

	
}
