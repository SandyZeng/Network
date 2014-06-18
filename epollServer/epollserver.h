#ifndef _EPOLLSERVER_H
#define _EPOLLSERVER_H

#define MAX_EVENT 10



void master_exit(int staut);

void work_process(int listenfd, int pipefd[2]);

void master_process(int port, int num);

#endif
