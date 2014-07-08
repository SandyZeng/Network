#include"Slabclass.h"

void Slabclass::LRU(){
	//perform LRU in the double-linked list, drop the item in tail
	if(tail){
		tail->next = slots;
		if(slots)slots->pre = tail;
		slots = tail;
		
		if(tail == head)head = tail = NULL;
		else{
			tail = tail->pre;
			tail->next = NULL;
		}
		slots_num++;
	}
}
