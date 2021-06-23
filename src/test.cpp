#include <iostream>
#include <complex>
#include "./utils/FixedSizedDeque.h"
#include "./utils/Deque.h"

using namespace std;

int main(){
	float d = 1.0;

	char* buffer = static_cast<char*>(static_cast<void*>(&d));

	for(int i = 0; i < 8; i++){
		cout<<buffer[i]<<endl;
	}

	cout<<sizeof(d)<<endl;

	return 0;
}