#include<iostream>
using namespace std;
#include"LRUCache.h"
#include"Cache.h"
#include"MyHash.h"

int main(){
	Cache<int>* cache = new LRUCache<int>(new MyHash());
	cache->set("hello", 1);
	cout<<"get(hello):"<<cache->get("hello")<<endl;
	cache->set("world", 2);
	cout<<"get(world):"<<cache->get("world")<<endl;

}
