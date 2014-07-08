#include<iostream>
using namespace std;
#include"MemPool.h"
#include<cstdlib>

int main(){
	MemPool pool;
	pool.init_pool();
	pool.stats();
	
	//allocation space for items
	for(int i = 0; i < 20; i++){
		int size = rand()%200;
		cout<<"item "<<i+1<<" size:"<<size<<endl;
		pool.item_alloc(size);
	}

	pool.stats();
//	pool.free_pool();
}
