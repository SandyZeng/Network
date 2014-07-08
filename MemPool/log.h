#ifndef _LOG_H
#define _LOG_H

#include<string>
#include<iostream>
using namespace std;
#include<stdlib.h>
#include<fcntl.h>

void log(const char* str){
	cout<<str<<endl;	
}

void log_error(const char* str){
	cout<<str<<endl;
	exit(1);
}

#endif
