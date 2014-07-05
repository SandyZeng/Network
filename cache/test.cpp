#include<iostream>
using namespace std;
#include"LRUCache.h"
#include"Cache.h"
#include"MyHash.h"

int main(){
	LRUCache<int>* cache = new LRUCache<int>(new MyHash());
	cache->set("hello", 1);
	cache->get("hello");
}
