#include<unistd.h>
#include<errno.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<signal.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/epoll.h>
#include<sys/sendfile.h>

#define MAX_CONNECTIONS 10000
#define MAX_EVENT 100
struct server_t{
	int listenfd;
	struct sockaddr_in addr;
};

server_t server;

int conncount;

int set_nonblock(int fd);

int init_server(int port, int backlog){
	printf("port:%d backlog:%d\n",port,backlog);
	if( (server.listenfd = socket(AF_INET, SOCK_STREAM, 0))<0){
		perror("create socket");
		exit(1);
	}

	bzero(&server.addr, sizeof(server.addr));
	server.addr.sin_family = AF_INET;
	server.addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server.addr.sin_port = htons(port);
	
	
	int flag = 1;
	if(setsockopt(server.listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag))<0){
		perror("setsockopt");
		exit(1);
	} 
	if(bind(server.listenfd, (struct sockaddr*)&server.addr, sizeof(server.addr))<0){
		perror("bind socket");
		exit(1);
	}
	if(listen(server.listenfd, backlog)<0){
		perror("listen on socket");
		exit(1);
	}
	printf("listen on port %d\n",port);
	return 0;
}
//file server using sendfile, zero copy
int run_file_server(){
	conncount = 0;	
	int fd = open("test.txt",O_RDONLY);
	if(fd<0){
		perror("open test.txt");
		exit(0);
	}
	struct stat stat_buf;
	fstat(fd, &stat_buf);
	printf("file server started for accepting connections....\n");
	while(1){
		int connfd = accept(server.listenfd,(struct sockaddr*)NULL, NULL);
		sendfile(connfd, fd, NULL, stat_buf.st_size);
		 conncount++;
		 printf("accept a connection\n");
		close(connfd);
	}
	close(fd);
}
//iterate server
int run_server1(){
	conncount = 0;	
	printf("server started for accepting connections....\n");
	while(1){
		int connfd = accept(server.listenfd,(struct sockaddr*)NULL, NULL);
		 conncount++;
		 printf("accept a connection\n");
	}
}

//single thread, asynchronous server, using select
int run_server2(){
	printf("server using select I/O ....\n");
	int fd[MAX_CONNECTIONS];
	fd_set rfds, wfds;
	struct timeval tv;
	int ret;
	
	FD_ZERO(&rfds);
	FD_SET(STDIN_FILENO,&rfds);
	FD_SET(server.listenfd, &rfds);
	
	FD_ZERO(&wfds);

	memset(&fd, -1, MAX_CONNECTIONS);
	
	fd[0] = 0;
	fd[1] = 1;
	fd[2] = server.listenfd;
	
	int max_idx = 3;

	tv.tv_sec = 5;
	tv.tv_usec = 0;
	
	int maxfd = server.listenfd;
	while(1){
	//must re-initilize fd_set everytime.
		FD_ZERO(&rfds);
		FD_SET(STDIN_FILENO,&rfds);
		FD_SET(server.listenfd, &rfds);
		FD_ZERO(&wfds);

		// add conncted sockets
		printf("current connection:%d max_idx:%d\n",conncount,max_idx);
		for(int i = 3; i < max_idx; i++){
//			printf("i:%d conn fd:%d\n",i,fd[i]);
			if(fd[i] >0){
				FD_SET(fd[i], &rfds);
			}
		}
	
		tv.tv_sec = 5;
		tv.tv_usec = 0;
	
		ret = select(maxfd+1, &rfds, &wfds, NULL,&tv);
		if(ret < 0){
			perror("select");
			exit(1);
		}
		if(ret == 0){
			printf("time out\n");
			continue;
		}
		//a new connection
		if(FD_ISSET(server.listenfd,&rfds)){
			int newfd = accept(server.listenfd,(struct sockaddr*)NULL, NULL);
			if(newfd < 0){
				perror("accept");
				continue;
			}
			
			printf("accept %d connections\n",conncount++);
			//conncount++;
			// add the connection to fd queue
			if(max_idx < MAX_CONNECTIONS){
				fd[max_idx++] = newfd;
			}
			else{
				int i = 3;
				for(; i<max_idx; i++){
					if(fd[i] == -1){
						fd[i] = newfd;
						break;
					}
				}
				if(i == max_idx){
					send(newfd, "busy",sizeof("busy"),0);
					close(newfd);
					continue;
				}
			}
			
			char *msg = "wherecome to server...";
			send(newfd, msg, strlen(msg),0);
			maxfd = newfd > maxfd?newfd:maxfd;
			continue;
		}
		
		//handle each connection
		for(int i=0; i < max_idx;i++){
//			printf("read event...\n");
			if(fd[i]<0)continue;
			if(FD_ISSET(fd[i],&rfds)){    //recv data in block mode

				char buf[1000000];
				if(i==0){             //stdin
					fgets(buf, 1024, stdin);
					fputs(buf,stdout);
					continue;
				}
				ret = recv(fd[i],buf,1000000,0);
				printf("recv fd=%d,length=%d\n",fd[i],ret);  
				if(ret == 0){
					close(fd[i]);
					printf("close socket:%d\n",fd[i]);
					fd[i] = -1;
					continue;
				} 
			//	buf[ret] = '\0';
			//	printf("recv socket:%d %s\n",fd[i],buf);
			}
		//	else{      
		//	}
		}		
	
	}
}
//single thread, asynchronous server, using select, non-block I/O
int run_server3(){
	printf("server using select NON-Block I/O ....\n");
	int fd[MAX_CONNECTIONS];
	fd_set rfds, wfds;
	struct timeval tv;
	int ret;
	
        	
	memset(&fd, -1, MAX_CONNECTIONS);
	
	fd[0] = 0;
	fd[1] = 1;
	fd[2] = server.listenfd;
	
	int max_idx = 3;
	int maxfd = server.listenfd;
	while(1){
	//must re-initilize fd_set everytime.
		FD_ZERO(&rfds);
		FD_SET(STDIN_FILENO,&rfds);
		FD_SET(server.listenfd, &rfds);
		FD_ZERO(&wfds);

		// add conncted sockets
		printf("current connection:%d max_idx:%d\n",conncount,max_idx);
		for(int i = 3; i < max_idx; i++){
//			printf("i:%d conn fd:%d\n",i,fd[i]);
			if(fd[i] >0){
				FD_SET(fd[i], &rfds);
			}
		}
	
		tv.tv_sec = 5;
		tv.tv_usec = 0;
	
		ret = select(maxfd+1, &rfds, &wfds, NULL,&tv);
		if(ret < 0){
			perror("select");
			exit(1);
		}
		if(ret == 0){
			printf("time out\n");
			continue;
		}
		//a new connection
		if(FD_ISSET(server.listenfd,&rfds)){
			int newfd = accept(server.listenfd,(struct sockaddr*)NULL, NULL);
			if(newfd < 0){
				perror("accept");
				continue;
			}
			
			printf("accept %d connections\n",conncount++);
			//conncount++;
			// add the connection to fd queue

			int i = 3;
			if(max_idx < MAX_CONNECTIONS){
				fd[max_idx++] = newfd;
			}
			else{
				for(; i<max_idx; i++){
					if(fd[i] == -1){
						fd[i] = newfd;
						break;
					}
				}
				if(i == max_idx){
					send(newfd, "busy",sizeof("busy"),0);
					close(newfd);
					continue;
				}
			}
			// set non-block socket
			set_nonblock(newfd);		
			char *msg = "wherecome to server...";
			int nLeft = strlen(msg);
			while(nLeft > 0){
				int nSend = send(newfd, msg, nLeft,0);
				if(nSend < 0){
					if(errno != EAGAIN){
						perror("send");
						close(newfd);
						printf("error read, close %d\n",newfd);
						fd[i] = -1;
						continue;
					}
				}
				else{
					nLeft -= nSend;
				}
			}
			maxfd = newfd > maxfd?newfd:maxfd;
			continue;
		}
		
		//handle each connection
		for(int i=0; i < max_idx;i++){
//			printf("read event...\n");
			if(fd[i]<0)continue;
			if(FD_ISSET(fd[i],&rfds)){    //recv data in block mode

				char buf[1000000];
				if(i==0){             //stdin
					fgets(buf, 1024, stdin);
					fputs(buf,stdout);
					continue;
				}
				ret = recv(fd[i],buf,1000000,0);  
				printf("recv fd=%d,length=%d:\n",fd[i],ret);
				if(ret == 0){
					close(fd[i]);
					printf("close socket:%d\n",fd[i]);
					fd[i] = -1;
					continue;
				} 
				else if(ret < 0){
					if(errno != EAGAIN){
						perror("recv");
						close(fd[i]);
						printf("recv error, close %d\n",fd[i]);
						fd[i] = -1;
						continue;
					}
				}
			//	buf[ret] = '\0';
			//	printf("recv socket:%d %s\n",fd[i],buf);
			}
		//	else{      
		//	}
		}		
	
	}
}

// a server use epoll 
int run_server4(){
	printf("server using epoll NON-Block I/O ....\n");
	struct epoll_event ev, events[MAX_EVENT];
	int epollfd = epoll_create(MAX_EVENT);
	if(epollfd < 0){
		perror("epoll create");
		exit(1);
	}

	ev.events = EPOLLIN;
	ev.data.fd = server.listenfd;
	if(epoll_ctl(epollfd, EPOLL_CTL_ADD, server.listenfd, &ev)<0){
		perror("epoll_ctl");
		exit(1);
	}

	int nfds;
	while(1){
		nfds = epoll_wait(epollfd, events, MAX_EVENT,3000);
		if(nfds < 0){
			perror("epoll_wait");
			exit(1);
		}
		for(int i = 0; i<nfds;i++){
			if(events[i].events != EPOLLIN)continue;
			if(events[i].data.fd == server.listenfd){
				int newfd = accept(server.listenfd,NULL,NULL);
				if(newfd < 0){
					perror("accept");
					continue;
				}
				set_nonblock(newfd);
				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = newfd;
				if(epoll_ctl(epollfd,EPOLL_CTL_ADD, newfd, &ev)<0){
					perror("epoll_ctl in loop");
					close(newfd);
					continue;
				}
				char *msg = "welcome to server";
				send(newfd, msg,strlen(msg),0);
			}
			else{
				char buf[1024];
				int n;
				while((n = recv(events[i].data.fd, buf, 1024,0))!=0){
				printf("recv:fd=%d, length=%d\n",events[i].data.fd,n);				}
				if(n == 0){
					printf("close socket %d\n",events[i].data.fd);
					close(events[i].data.fd);
					continue;
				}
				
		
			}
		}
	}
	
}

int set_nonblock(int fd){
	int flags = fcntl(fd, F_GETFL, 0);
	if(flags < 0){
		perror("fcntl nonblock");
		return -1;
	}
	flags |= O_NONBLOCK;
	if(fcntl(fd, F_SETFL,flags)<0){
		perror("setblock");
		return -1;
	}
	return 0;
}
int start_server(int port, int backlog){
	init_server(port, backlog);
//	run_server4();
	run_file_server();
}

void close_server(int signo){
	close(server.listenfd);
	printf("%d connections\n",conncount);
	printf("server exit...\n");
	exit(0);
}

int main(int argc, char **argv){
	if(argc < 3){
		printf("usage:server [port] [backlog]\n");
		exit(0);
	}
	signal(SIGINT, close_server);
	start_server(atoi(argv[1]), atoi(argv[2]));
}
