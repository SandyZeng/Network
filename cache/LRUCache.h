#ifndef _LRU_CACHE_H_
#define _LRU_CACHE_H_

#include"Cache.h"
#include<string>
#include"Hash.h"
#include"Bucket.h"

#define TABLE_SIZE_POWER 8

template <class T>
class LRUCache : public Cache<T>{
    public:
	LRUCache();
	LRUCache(Hash*);
        virtual T& get(string key);
	virtual void set(string key, const T& value);
    private:
	Hash* hashFunc;
	Bucket<T>** hashTable;
	

};

#include<iostream>
using namespace std;

//template<typename T> 
//Hash* LRUCache<T>::hashFunc = NULL;

template<class T>
LRUCache<T>::LRUCache(){
//	if(hashFunc == NULL){
	hashFunc = new Hash();
//	}
	hashTable = new Bucket<T>*[2<<TABLE_SIZE_POWER];
}

template<class T>
LRUCache<T>::LRUCache(Hash* hashfunc){
	hashFunc = hashfunc;
	hashTable = new Bucket<T>*[2<<TABLE_SIZE_POWER];
}

template<class T>
T& LRUCache<T>::get(string key){
	unsigned int id = hashFunc->hash(key.c_str());
	id = id&(2<<TABLE_SIZE_POWER-1);
	cout<<"get key:"<<key<<" hash:"<<id <<endl;
	
	return hashTable[id]->find(key);
}

template<class T>
void LRUCache<T>::set(string key, const T& value){
	unsigned int id = hashFunc->hash(key.c_str());
	cout<<"set key:"<<key<<" hash:"<<id;
	id = id % (2<<TABLE_SIZE_POWER);
	cout<<" id:"<<id<<endl;
	hashTable[id]->add(key,value);
}
	
#endif
