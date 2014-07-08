#include<iostream>
using namespace std;
#include"MemPool.h"

int main(){
	MemPool pool;
	pool.init_pool();
	pool.stats();
	pool.free_pool();
}
