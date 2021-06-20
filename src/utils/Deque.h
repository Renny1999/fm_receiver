#ifndef DEQUE_H
#define DEQUE_H

template<class T> struct Node{
	T data;
	Node<T>* next;
	Node<T>* prev;
};

template<class T> class Deque{
	public:
	Node<T>* head;
	Node<T>* tail;
	int capacity;
	Deque<T>(int capacity);
	void push_left(T n);
	void push_right(T n);
	void pop_left();
	void pop_right();

	void push_front(T n);
	void push_back(T n);
};

template<class T>
Deque<T>::Deque(int cap){
	this->capacity = cap;
	Node<T>* node = new Node<T>();
	node->data = T();
	node->next = nullptr;
	node->prev = nullptr;

	this->head = node;
	this->tail = node;
	Node<T>* last = node;

	for(int i = 1; i < cap; i++){
		Node<T>* temp = new Node<T>();
		temp->data = T();

		temp->next = nullptr;

		temp->prev = last;
		last->next = temp;

		last = temp;
		this->tail = temp;
	}
}

template<class T>
void Deque<T>::push_left(T n){
	Node<T>* node = new Node<T>();
	node->data = n;

	node->next = this->head;
	this->head->prev = node;
	this->head = node;
}

template<class T>
void Deque<T>::push_right(T n){
	Node<T>* node = new Node<T>();
	node->data = n;

	node->prev = this->tail;
	this->tail->next = node;
	this->tail = node;
}


template<class T>
void Deque<T>::pop_left(){
	this->head = this->head->next;
	delete this->head->prev;
	this->head->prev = nullptr;
}

template<class T>
void Deque<T>::pop_right(){
	this->tail = this->tail->prev;
	delete this->tail->next;
	this->tail->next = nullptr;
}

template<class T>
void Deque<T>::push_front(T n){
	this->pop_right();
	this->push_left(n);
}

template<class T>
void Deque<T>::push_back(T n){
	this->pop_left();
	this->push_right(n);
}

#endif