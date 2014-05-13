#include<unistd.h>
#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<signal.h>
#include<string.h>
#include<arpa/inet.h>
#include<pthread.h>

#define THREAD_NUM 1

struct sockaddr_in servaddr;
static int threadcount;
void sig_int(int signo){

	printf("create thread:%d\n",threadcount);
	exit(1);
}

void *thread_conn(void *data){
//	printf("thread start..\n");
	int sockfd = socket(AF_INET, SOCK_STREAM,0);
	if(sockfd < 0){
		perror("socket");
	//	pthread_exit(NULL);
	}
	if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))<0){
		perror("connect");
	}
//	printf("connected\n");
	char buf[1024];
	int nRead = recv(sockfd,buf, 1024,0);
	if(nRead <0){
		perror("recv");
		close(sockfd);
		pthread_exit(NULL);
	} 
	if(nRead == 0){
		close(sockfd);
	}
	else{
		printf("recv:%s\n",buf);
		char msg[1000000];
		for(int i = 0; i< 1000000; i++){
			msg[i] = '0'+i%26;
		}
		printf("send msg %s\n",msg);
//		sprintf(msg, "hello, pid:%d threadID:%lu",getpid(),(unsigned long)pthread_self());
		int n = send(sockfd, msg, strlen(msg),0);
		if( n < 0){
			perror("send");
		}
//		sleep(3);
		close(sockfd);
	}
//	sleep(10);
	pthread_detach(pthread_self());
}

int main(int argc, char **argv){
	if(argc < 3){
		printf("usage:client [host] [port]\n");
	}
//	sleep(1);
//	int sockfd = socket(AF_INET, SOCK_STREAM,0);
//	if(sockfd < 0){
//		perror("socket");
	//	pthread_exit(NULL);
//	}

	//struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));
	
	printf("host:%s, port:%d\n",argv[1],atoi(argv[2]));
	if(inet_pton(AF_INET, argv[1],&servaddr.sin_addr)<0){
		perror("inet_pton");
		exit(1);
	}

	signal(SIGINT, sig_int);
	//pthread_t pt[THREAD_NUM];
	pthread_attr_t attr;
	if(pthread_attr_init(&attr)<0){
		perror("pthread_attr_init");
	}
	if(pthread_attr_setstacksize(&attr, 1024)<0){
		perror("set stack size");
	}
	for(int i = 0; i<THREAD_NUM; i++){
//	while(1){
		pthread_t pt;
		if(pthread_create(&pt, &attr, thread_conn, NULL)<0){
			perror("create thread");
		}
		threadcount++;
		printf("create thread:%d\n",threadcount);
//	}
//		pthread_detach(pt[i]);
	}
	//for(int i = 0; i<THREAD_NUM; i++){
	//	pthread_join(pt[i], NULL);
	//}	
	pause();
}
