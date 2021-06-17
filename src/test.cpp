#include <iostream>
#include <complex>
#include "./utils/FixedSizedDeque.h"

using namespace std;

int main(){
	int cap = 3;

	FixedSizedDeque<complex<double>> dq(cap);
	dq.push_front(complex<double>(1,1));
	dq.push_back(complex<double>(2,2));
	dq.push_back(complex<double>(3,3));
	for(int i = 0; i < cap; i++){
		cout<<dq[i]<<endl;
	}



	return 0;
}