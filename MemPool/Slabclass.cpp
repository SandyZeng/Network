#include"Slabclass.h"
#include<string>

void Slabclass::LRU(){
	//perform LRU in the double-linked list, drop the item in tail
	if(tail){
		tail->next = (Item*)slots;
		if(tail->next)tail->next->pre = tail;
		slots = tail;
		
		if(tail == head)head = tail = NULL;
		else{
			tail = tail->pre;
			tail->next = NULL;
		}
		slots_num++;
	}
}
