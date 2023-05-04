#include "q3buffer.h"

_Task Producer {
	BoundedBuffer<int> & buffer;
	int produce;
	int delay;
	
	void main(){
		for(int i = 0; i < produce; i++){
			yield(::prng(delay));
			buffer.insert(i+1);
		}
	}
  public:
	Producer( BoundedBuffer<int> & buffer, const int Produce, const int Delay ):
		buffer{ buffer }, produce {Produce}, delay {Delay} {}
};