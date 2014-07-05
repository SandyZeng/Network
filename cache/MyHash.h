#ifndef _MY_HASH_H
#define _MY_HASH_H

#include"Hash.h"

class MyHash : public Hash{
	public:
	unsigned int hash(const char* key);
};

#endif
