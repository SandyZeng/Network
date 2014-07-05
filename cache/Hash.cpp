#include"Hash.h"

unsigned int Hash::hash(const char* key){
	//default hash 		
	unsigned int hash = 0;
	while(key){
		hash += hash<<5 + (*key++);
	}
	return (hash & 0x7fffffff);
}
