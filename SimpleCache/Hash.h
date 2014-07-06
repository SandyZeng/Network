#ifndef _HASH_H
#define _HASH_H

#include<string>
using namespace std;

class Hash{
   public:
	virtual unsigned int hash(const char* key);
};

#endif
