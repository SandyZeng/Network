#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<stdio.h>
#include<sys/socket.h>
#include<signal.h>
#include<sys/stat.h>
#include"epollserver.h"
#include<sys/epoll.h>
#include<string.h>
#include<arpa/inet.h>
#include"ProcessPool.h"

void handle_connection(int connfd){
	char buf[256];
	char *msg = "hello, welcom to server,process:";
	sprintf(buf, "%s%d",msg,getpid());
	send(connfd,buf, strlen(buf),0);
	close(connfd);
}
void work_exit(int statut){
	exit(0);
}
void master_exit(int staut){
	//when master exit, handl the chiled process 
	//(1) exit instantly
	//(2) wait the child process finish the current connection
/*	for(int i = 0; i < worker_num; i++){
		int info;
		int pid = waitpid(worker[i],&info,WNOHANG);
		printf("child process %d exited.\n",pid);
	}
*/
}

void work_process(int listenfd, int pipefd[2]){
	printf("worker process:%d\n",getpid());
	// work process do the logic work with client, per process per work process
	close(pipefd[0]);
	//epoll
	struct epoll_event ev1, events[MAX_EVENT];
	int epollfd = epoll_create(MAX_EVENT);
	if(epollfd < 0){
		perror("epoll_create");
		work_exit(1);
	} 
	
	ev1.events = EPOLLIN;
	ev1.data.fd = pipefd[1];
	if(epoll_ctl(epollfd, EPOLL_CTL_ADD, pipefd[1], &ev1)<0){
		perror("epoll_ctl");
		work_exit(1);
	}

	int nfds;
	while(1){
		nfds = epoll_wait(epollfd, events, MAX_EVENT,3000);
		for(int i = 0; i < nfds; i++){
			if(events[i].data.fd == pipefd[1]){  //a new connection
				//get the cmd
				char buf[16];
				int n =	read(pipefd[1], buf, 16);
				if(strcmp(buf,"run")==0){
					//handle connection
					int connfd = accept(listenfd,(struct sockaddr*)NULL,NULL);
					handle_connection(connfd);
				}
				else if(strcmp(buf,"stop")==0){
					//stop the work process
					work_exit(2);
				}
			}
		}
	}	
	exit(0);	
}

void master_process(int port, int num){
	// master process is listen on the listen port for connection
//	printf("master listen on port:%d \n",port);
	int listenfd;
	struct sockaddr_in addr;
	if( (listenfd = socket(AF_INET, SOCK_STREAM, 0))<0){
		perror("create socket");
		master_exit(1);
	}

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);
	
	int flag = 1;
	if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag))<0){
		perror("setsockopt");
		master_exit(1);
	} 
	if(bind(listenfd, (struct sockaddr*)&addr, sizeof(addr))<0){
		perror("bind socket");
		master_exit(1);
	}
	int backlog = 5;
	if(listen(listenfd, backlog)<0){
		perror("listen on socket");
		master_exit(1);
	}
	printf("master listen on port %d\n",port);


	//create work process
	//(1) first create pipe to exchange information between process
	//(2) create process pool
	//(3) register listenfd and pipefd event
	


	ProcessPool* pool = ProcessPool::getInstance();
	pool->init_pool(num,listenfd);


	struct epoll_event ev,ev1, events[MAX_EVENT];
	int epollfd = epoll_create(MAX_EVENT);
	if(epollfd < 0){
		perror("epoll_create");
		master_exit(1);
	} 

	ev.events = EPOLLIN;
	ev.data.fd = listenfd;
	if(epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev)<0){
		perror("epoll_ctl");
		master_exit(1);
	}
	
/*	ev1.events = EPOLLIN;
	ev1.data.fd = pipefd[0];
	if(epoll_ctl(epollfd, EPOLL_CTL_ADD, pipefd[0], &ev1)<0){
		perror("epoll_ctl");
		master_exit(1);
	}
*/
	int nfds;
	while(1){
		nfds = epoll_wait(epollfd, events, MAX_EVENT,3000);
		for(int i = 0; i < nfds; i++){
			if(events[i].data.fd == listenfd){  //a new connection
				//send the fd to work process
				Process *work = pool->get_process();
				work->run();  //send the fd to work,
			}
		}
		
	}
}
