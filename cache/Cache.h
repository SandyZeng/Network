#ifndef _CACHE_H_
#define _CACHE_H_

#include<string>
using namespace std;

template <class T>
class Cache{
    public:
	virtual T& get(string key);
	virtual void set(string key, const T& value);
};

template<class T>
T& Cache<T>::get(string key){

}

template<class T>
void Cache<T>::set(string key, const T& value){

}


#endif
