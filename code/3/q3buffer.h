#ifndef BOUNDEDBUFFER
#define BOUNDEDBUFFER

#include <uPRNG.h>
#ifdef BUSY
template<typename T> class BoundedBuffer {
	uOwnerLock mlk; 
	uCondLock consumerLock;
	uCondLock producerLock;
	bool done = false;
	unsigned int bufferNum = 0;
	T *buffers;
	const unsigned int size;

  public:
	_Event Poison {};
	BoundedBuffer( const unsigned int size = 10 ): buffers {new T[size]{}}, size{size} {}
	~BoundedBuffer(){ delete [] buffers;}
	unsigned long int blocks(){
		mlk.acquire();
		int ret = bufferNum;
		mlk.release();
		return ret;
	}

	void poison() {
		mlk.acquire();
		done = true;
		// unblock all the tasks, move tasks to ready queue again
		consumerLock.broadcast(); 
		mlk.release();
	}

	void insert( T elem ){
		mlk.acquire();
		while (bufferNum==size){
			producerLock.wait(mlk);
		}
		buffers[bufferNum] = elem;
		bufferNum += 1;
		consumerLock.signal();
		mlk.release();
	}
	T remove() __attribute__(( warn_unused_result )){
		mlk.acquire();
		while (bufferNum==0 && !done){
			consumerLock.wait(mlk);
		}
		T removeItem = 0;
		if (!done) {
			bufferNum -= 1;
			removeItem = buffers[bufferNum];
			producerLock.signal();
		}
		mlk.release();
		return removeItem;
	}
};
#endif // BUSY

#ifdef NOBUSY							// no busy waiting implementation
#include "BargingCheck.h"
template<typename T> class BoundedBuffer {
	uOwnerLock mlk; 
	uCondLock consumerLock;
	uCondLock producerLock;
	bool done = false;
	unsigned int bufferNum = 0;
	bool consumerBlock = false;
	bool producerBlock = false;
	bool ifBarging = false;

	T *buffers;
	const unsigned int size;
	BCHECK_DECL;
  public:

	_Event Poison {};
	BoundedBuffer( const unsigned int size = 10 ): buffers {new T[size]{}}, size{size} {}
	~BoundedBuffer(){ delete [] buffers;} 

	unsigned long int blocks(){
		mlk.acquire();
		int ret = bufferNum;
		mlk.release();
		return ret;
	}
	void poison() {
		mlk.acquire();
		done = true;
		// unblock all the tasks, move tasks to ready queue again
		consumerLock.broadcast(); 
		mlk.release();
	}

	void insert( T elem ) {
		mlk.acquire();
		PROD_ENTER;
		// if there is no extra task currently being unblocked
		if ((!producerBlock)&&(!ifBarging)){
			if (bufferNum==size){
				producerLock.wait(mlk);
			}
		// if there is one task waiting for unblock
		} else {
			producerLock.wait(mlk);
		}
		ifBarging = false;
		buffers[bufferNum] = elem;
		bufferNum += 1;
		INSERT_DONE;
		CONS_SIGNAL( consumerLock );
		// first check if there are any consumers needed to be unblocked
		bool ifConsumerBlock = consumerLock.signal();
		if (!ifConsumerBlock) {
			consumerBlock = false;
			if (bufferNum < size){
				PROD_SIGNAL( producerLock );
				// secondly check if there are any producers needed to be unblocked
				bool ifProduceBlock = producerLock.signal();
				if (!ifProduceBlock) {
				producerBlock = false;
				} else {
				producerBlock = true;
				ifBarging = true;
				}
			}
		} else{
			consumerBlock = true;
      		ifBarging = true;
		}
		mlk.release();
	}
	T remove() __attribute__(( warn_unused_result )) {
		mlk.acquire();
		CONS_ENTER;
		T removeItem = 0;
		if (!done){
			if ((!consumerBlock)&&(!ifBarging)){
				if (bufferNum==0){
					consumerLock.wait(mlk);
				}
			} else {
				consumerLock.wait(mlk);
			}
			ifBarging = false;
			if ((!done) && (bufferNum>0) ){
				bufferNum -= 1;
				removeItem = buffers[bufferNum];
				REMOVE_DONE;
				// first check if there are any producers needed to be unblocked
				PROD_SIGNAL( producerLock );
				bool ifProduceBlock = producerLock.signal();
				if (!ifProduceBlock) {
					producerBlock = false;
					if (bufferNum>0){
						// secondly check if there are any consumers needed to be unblocked
						CONS_SIGNAL( consumerLock );
						bool ifConsumerBlock = consumerLock.signal();
						if (!ifConsumerBlock) consumerBlock = false;
						else{
							consumerBlock = true;
							ifBarging = true;
						}
					}
				} else {
					producerBlock = true;
          			ifBarging = true;
				}
			} 
		}
		mlk.release();
		return removeItem;
	}
};
#endif // NOBUSY

#endif
