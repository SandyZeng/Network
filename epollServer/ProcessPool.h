#ifndef _PROCESS_POOL_
#define _PROCESS_POOL_
/*
process pool
*/
#include<vector>
#include<sys/types.h>
#include<unistd.h>

struct Process{
	pid_t pid;
	int state;  
	int pipefd[2];  //pipe for ipc
	Process():pid(-1),state(0){}
	void start();
	void run();       //choose process to accept the connection
	void stop();      //stop the work process
};
class ProcessPool{
	public:
		static	ProcessPool* getInstance();
		int init_pool(int num, int listenfd);          //create process pool
		Process* get_process();           //get a process from pool
		int add_process(int listenfd);    //add a process
		~ProcessPool();
	private:
		ProcessPool(){};
		
		std::vector<struct Process*>workers;
	public:
		static ProcessPool* pool;
};

#endif
