#include "q3buffer.h"

_Task Consumer {
	BoundedBuffer<int> & buffer;
	int delay;
	int &sum;

	void main(){
		try {
			_Enable{
				for(;;){
					yield(::prng(delay));
					int removeItem = buffer.remove();
					sum += removeItem;
				}
			}
		} catch(BoundedBuffer<int>::Poison &){			
			return;
 		}
	}

  public:
	Consumer( BoundedBuffer<int> & buffer, const int Delay, int &sum ):
		buffer{ buffer }, delay {Delay}, sum{sum} {}
};
