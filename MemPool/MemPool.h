#ifndef _MEM_POOL_H
#define _MEM_POOL_H

class MemPool{
  public:
	int init_pool(int class_num=4, int min_chunk=64,int size=4194304 /*4M*/, float factor=1.5);
	int free_pool();

	int new_slab(int id);
	int grow_slab_list(int id);
	
	Item* item_alloc(int size);
	int item_del(Item* item);

   private:
	int slabclass_num;    //class number of slab
	int mem_size;         //total memory size 
	int mem_left;         //avilable memmory size
	
	void* mem_base;       //the start address of pool
	void* mem_cur;        //the start address of free memmory
	
	float factor;         //growing factor of slabclass

	Slabclass* slabclass;
};

#endif
