#include"MemPool.h"
#include"log.h"

#define CHUNK_ALIGN_SIZE 8
#define SLAB_SIZE 1024

int MemPool::init_pool(int class_num,int min_chunk, int size, float _factor){
	slabclass_num = class_num;
	mem_size = mem_left = size;
	factor = _factor;
       
	slabclass = new Slabclass[slabclass_num];
	int chunk_size = min_chunk;
	for(int i = 0; i < slabclass_num; i++){
		//align the memory with 8 bytes
		if(chunk_size%CHUNK_ALIGN_SIZE !=0){
			chunk_size += CHUNK_ALIGN_SIZE- chunk_size%CHUNK_ALIGN_SIZE;
		}
		slabclass[i].chunk_size = chunk_size;
		slabclass[i].chunk_num = SLAB_SIZE/chunk_size;

		chunk_size *= factor;
	}

	mem_base = malloc(mem_size);
	if(mem_base == NULL){
		log_error("allocate memmory failed");
	}

	//alocate a new slab for each slabclass
	for(int i = 0; i < slabclass_num; i++){
		new_slab(i);
	}
}

int MemPool::free_pool(){

}

//alocate a new slab for slabclass[id]
int MemPool::new_slab(int id){
	slabclass* p = &slabclass[id];
	
	if(mem_left < SLAB_SIZE){
		return 1;   //no avalible memmory in pool
	}

	if(grow_slab_list(id)<0){
		log_error("grow slab list faild.");
	}
	
	char* ptr;
	if((ptr = mem_allocate(SLAB_SIZE)) == NULL){
		log_error("mem_allocate");
	}

	p->slab_list[p->slabs++] = ptr;
	p->ptr = ptr;

	mem_left -= SLAB_SIZE;
}

//allocate memory for void** slab_list as the number of slab is growing
int MemPool::grow_slab_list(int id){
	
}

Item* MemPool::item_alloc(int size){

}

int MemPool::item_del(Item* item){

}
