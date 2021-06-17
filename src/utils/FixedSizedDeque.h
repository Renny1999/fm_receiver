#ifndef FIXED_SIZED_DEQUE_H
#define FIXED_SIZED_DEQUE_H

#include <deque>

template <class T> class FixedSizedDeque{
	std::deque<T> data;	
	int capacity;

	public:
	FixedSizedDeque<T>(int cap);

	T get(int index);
	T operator[](int index);
	void push_back(T element);
	void push_front(T element);
	void pop_back();
	void pop_front();
};

template <class T> FixedSizedDeque<T>::FixedSizedDeque(int cap){
	this->capacity = cap;
	for(int i = 0; i < cap; i++){
		this->data.push_back(T());
	}
}

template <class T> void FixedSizedDeque<T>::push_back(T e){
	this->data.pop_front();
	this->data.push_back(e);
}
template <class T> void FixedSizedDeque<T>::push_front(T e){
	this->data.pop_back();
	this->data.push_front(e);
}

template <class T> void FixedSizedDeque<T>::pop_back(){
	this->data.pop_back();
}

template <class T> void FixedSizedDeque<T>::pop_front(){
	this->data.pop_front();
}

template <class T> T FixedSizedDeque<T>::get(int index){
	return this->data[index];
}

template <class T> T FixedSizedDeque<T>::operator[](int index){
	return this->data[index];
}

#endif