#include"MyHash.h"

unsigned int MyHash::hash(const char* key){
	//implements the BKDR hash function
	unsigned int seed = 131; //31 131 1313 ...etc
	unsigned int hash = 0;
	
	while(*key){
		hash = hash * seed + (*key++);
	}
	
	return (hash & 0x7fffffff);
}
