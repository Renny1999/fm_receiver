#include <iostream>
#include <complex>
#include "./utils/FixedSizedDeque.h"
#include "./utils/Deque.h"

using namespace std;

int main(){
	int cap = 3;

	Deque<complex<double>> dq(cap);
	dq.push_front(complex<double>(1,1));
	dq.push_front(complex<double>(2,2));
	dq.push_front(complex<double>(4,4));
	dq.push_back(complex<double>(3,3));


	Node<complex<double>>* temp = dq.head;
	for(int i = 0; i < cap; i++){
		cout<<temp->data<<endl;;
		temp = temp->next;
	}



	return 0;
}