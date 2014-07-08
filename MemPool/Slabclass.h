#ifndef _SLABCLASS_H
#define _SLABCLASS_H

class Item{
   public:
	Item* pre;      //double linked list
	Item* next;

	Item* hash_next; //pointer to the next item in the same hash bucket

	int nbytes;      //size of data;
	char data[]; 
};

class Slabclass{
   public:
	Slabclass():slabs_size(0),list_size(0),slots_num(0),head(NULL),tail(NULL),slots(NULL){}
	void LRU();

	void** slab_list;         //list of slab in the slabclass
	void* slots; 	  //free list of items
	
	Item* head;	          //head of the LRU linked list
	Item* tail;               //tail of the LRU linked list

	int slots_num;       // number of free items
//	int free_chunks;          //number of free chunks

	int slabs_size;           //number of allocated slabs;
	int list_size;            //size of slab_list 

//	void* ptr;                //cur address to allocate a chunk for item
	int chunk_size; 
	int chunk_num;

};

#endif
