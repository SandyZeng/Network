#ifndef _BUCKET_H
#define _BUCKET_H

#include<string>
using namespace std;

template<class T>
class Item{
   public:
	string key;
	T value;
	Item<T> * next;
	Item<T> *pre;
	Item():key(""),next(NULL),pre(NULL){}
	Item(string _key, const T& _value):key(_key),value(_value),pre(NULL),next(NULL){}
};

template<class T>
class Bucket{
   public:
	Bucket(int size = 5);
	T& find(string key);
	int add(string key, const T& value);
   private:
	Item<T>* head;
	Item<T> * tail;
	int maxSize;
	int curSize;
};

template<class T>
Bucket<T>::Bucket(int size){
	head = tail = NULL;
	maxSize = size;
	curSize = 0;
	head = new Item<T>();
	tail = new Item<T>();
	head->next = tail;
	tail->pre = head;
}
template<class T>
T& Bucket<T>::find(string key){
	Item<T>* p = head->next;
	while(p != tail && p->key!= key)p = p->next;
	if(p){
		//perform LRU, move the accessed item to front of LRU
		if(p == head ->next)return p->value;

		Item<T>* it1 = p->next;

		p->pre->next = head->next;
		head->next->pre = p->pre;
	
		p->next = head->next->next;
		head->next->next->pre = p;

		head->next->next = it1;
		it1->pre = head->next;

		p->pre = head;
		head->next = p;

		
		 return p->value;
 	}
	T temp = NULL;
	return temp;
}

template<class T>
int Bucket<T>::add(string key, const T& value){
	Item<T> *item = new Item<T>(key, value);
	
	if(curSize == maxSize){
	//perform LRU
	        tail->pre->pre->next = item;
		item->pre = tail->pre->pre;

		tail->pre = item;
		item->next = tail;
	}
	else{
		item->next = head->next;
		head->next->pre = item;
		
		head->next = item;
		item->pre = head;
		curSize++;
	}	
}
#endif
