#include"ProcessPool.h"
#include"epollserver.h"
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>


ProcessPool* ProcessPool::pool = NULL;

//send msg to work process
void Process::run(){
	char *buf = "run";
	write(pipefd[0], buf, sizeof(buf)); //send msg to workprocess

}


void Process::stop(){
	char *buf = "stop";
	write(pipefd[0], buf, sizeof(buf)); //send stop cmd to workprocess
}

ProcessPool* ProcessPool::getInstance(){
	if(pool == NULL){
		pool =	new ProcessPool();
	}
	return pool;
}

int ProcessPool::init_pool(int num, int listenfd){
	for(int i = 0; i < num; i++){
		add_process(listenfd);
	}
	return 0;
}
int ProcessPool::add_process(int listenfd){
	Process *process = new Process();
	if(process == NULL)return -1;
	
	if(socketpair(AF_UNIX, SOCK_STREAM, 0, process->pipefd)<0){
		perror("socketpair");
		return -1;
	}	

	pid_t pid = fork();
	if(pid < 0){
			perror("fork");
			return -1;
	}
	else if(pid == 0){
			sleep(1);
			work_process(listenfd, process->pipefd);
	}
	else{

		close(process->pipefd[1]);
		process->pid = pid;
		printf("create process, pid=%d\n",process->pid);
		workers.push_back(process);
	}

	return 0;
}

Process* ProcessPool::get_process(){
	//using round-robin to choose a process from pool
	static int idx;
	struct Process* process = workers[(idx++)%workers.size()];
	return process;
}

ProcessPool::~ProcessPool(){
	int n = workers.size();
	for(int i = 0; i < n; i++){
		workers[i]->stop();
	}
}



