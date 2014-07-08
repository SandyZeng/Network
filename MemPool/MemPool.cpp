#include"MemPool.h"
#include"log.h"
#include<stdio.h>

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

	mem_cur = mem_base = malloc(mem_size);
	if(mem_base == NULL){
		log_error("allocate memmory failed");
	}

	//alocate a new slab for each slabclass
	for(int i = 0; i < slabclass_num; i++){
		new_slab(i);
	}
}

int MemPool::free_pool(){
	for(int i = 0; i < slabclass_num; i++){
		free(slabclass[i].slab_list);
	}
	free(mem_base);
}

//alocate a new slab for slabclass[id]
int MemPool::new_slab(int id){
	Slabclass* p = &slabclass[id];
	
	if(mem_left < SLAB_SIZE){
		return -1;   //no avalible memmory in pool
	}

	if(grow_slab_list(id)<0){
		log("grow slab list faild.");
		return -1;
	}
	
	char* ptr;
	if((ptr = (char*)mem_alloc(SLAB_SIZE)) == NULL){
		log("mem_allocate");
		return -1;
	}

	p->slab_list[p->slabs_size++] = ptr;

	//put the chunks to slots list
	slab_to_slots(ptr, id);

	mem_left -= SLAB_SIZE;
	return 0;
}

//allocate memory for void** slab_list as the number of slab is growing
int MemPool::grow_slab_list(int id){
	Slabclass* p = &slabclass[id];
	if(p->slabs_size == p->list_size){ //need grow the list size
		int newsize = p->list_size==0?16:p->list_size<<1;
		void* newlist = realloc(p->slab_list, newsize*sizeof(void*));
		if(newlist==NULL)
			return -1;
		p->list_size = newsize;
		p->slab_list = (void**)newlist;
	}
	return 0;
}

//split the new slab to slots list
void MemPool::slab_to_slots(char* ptr, int id){
	Slabclass* p = &slabclass[id];
	for(int i = 0; i < p->chunk_num; i++){
	  //link the slots to double-linked list
		Item* item = (Item*)ptr;   
		item->next = (Item*)p->slots;
		if(item->next)item->next->pre = item;
		
		p->slots = item;
		item->pre = NULL;

		ptr += p->chunk_size;
	}
	p->slots_num += p->chunk_num;
}
//alocate memory from the memory pool
void* MemPool::mem_alloc(int size){
	void* ptr = mem_cur;
	
//	if(size%CHUNK_ALIGN_SIZE !=0){
//		size += CHUNK_ALIGN_SIZE - size%CHUNK_ALIGN_SIZE;
//	}
	mem_cur = (char*)mem_cur +  size;
	return ptr;
}	
Item* MemPool::item_alloc(int size){
	Item* item = NULL;
	//find proper slabclass
	int i = 0;
	while(i<slabclass_num&&slabclass[i++].chunk_size<size);
	
	//(1)find slots if not next
	//(2)new a slab for the class if not
	//(3)perform LRU to replace
	Slabclass*p = &slabclass[--i];	
	int ret;
	if(p->slots_num==0){
		//perform (2)
		ret = new_slab(i);
	}


	if(ret < 0){
		//perform (3)
		p->LRU();
	}

	if(p->slots_num != 0){
		//perform (1)
		item = (Item*)p->slots;
		p->slots = item->next;
		if(item->next)item->next->pre = NULL;
		p->slots_num--;
	}

	return item;
}

int MemPool::item_del(Item* item){

}

void MemPool::stats(){
	log("........................................");
	char buf[128];
	sprintf(buf, "total memory:%d",mem_size);
	log(buf);
	sprintf(buf,"free memory:%d",mem_left);
	log(buf);
	
	sprintf(buf,"slabclass size:%d",slabclass_num);
	log(buf);
	
	sprintf(buf, "slab size:%d",SLAB_SIZE);
	log(buf);

	log("class| chunk_size| chunk_num| slab_size|slots_num");
	for(int i = 0; i < slabclass_num; i++){
		char buffer[256];
		sprintf(buffer, "%d\t%d\t\t%d\t%d\t%d",i,slabclass[i].chunk_size,slabclass[i].chunk_num,slabclass[i].slabs_size,slabclass[i].slots_num);
		log(buffer);
	}
}
