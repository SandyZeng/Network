#ifndef _LOG_H
#define _LOG_H

#include<string>
#include<iostream>
using namespace std;
#include<stdlib.h>
#include<fcntl.h>

void log(const string &str){
	cout<<str<<endl;	
}

void log_error(const string& str){
	cout<<str<<endl;
	exit(1);
}

#endif
