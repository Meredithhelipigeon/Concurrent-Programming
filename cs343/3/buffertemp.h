#ifndef BOUNDEDBUFFER
#define BOUNDEDBUFFER
#include <iostream>
using namespace std;

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
		return bufferNum;
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
		T removeItem;
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
	// unsigned int bufferNum = 0;
	// bool consumerBlock = false;
	// bool producerBlock = false;
	// bool ifBarging = false;

	T *buffers;
	const unsigned int size;
	BCHECK_DECL;
  public:
	int produceCounter = 0;
	int removeCounter = 0;
	int removeWaitCounter = 0;
	int removeWakeUpCounter = 0;

  	unsigned int bufferNum = 0;
  	bool consumerBlock = false;
	bool producerBlock = false;
	bool ifBarging = false;
	_Event Poison {};
	BoundedBuffer( const unsigned int size = 10 ): buffers {new T[size]{}}, size{size} {}
	~BoundedBuffer(){ delete [] buffers;} 

	unsigned long int blocks(){
		return bufferNum;
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
		// if there is no extra tasking waiting for unblock
		if ((!producerBlock)&&(!ifBarging)){
			if (bufferNum==size){
				producerLock.wait(mlk);
			}
		} else {
			producerLock.wait(mlk);
		}
		produceCounter += 1;
		ifBarging = false;
		if(producerLock.empty()) producerBlock=false;
		// if(bufferNum==size-1) producerBlock = false;
		cout << "producing "<< elem << endl;
		buffers[bufferNum] = elem;
		bufferNum += 1;
		INSERT_DONE;
		CONS_SIGNAL( consumerLock );
		bool ifConsumerBlock = consumerLock.signal();
		if (!ifConsumerBlock) consumerBlock = false;
		else{
			cout << "producer informs consumerBlock=true" << " with bufferNum " << bufferNum 
			<< " with produce counter" << produceCounter
			<< endl;
			consumerBlock = true;
			ifBarging = true;
			removeWakeUpCounter += 1;
		}
		mlk.release();
	}
	T remove() __attribute__(( warn_unused_result )) {
		mlk.acquire();
		CONS_ENTER;
		T removeItem;
		if (!done){
			if ((!consumerBlock)&&(!ifBarging)){
				if (bufferNum==0){
					removeWaitCounter += 1;
					consumerLock.wait(mlk);
				}
			} else {
				removeWaitCounter += 1;
				consumerLock.wait(mlk);
			}
			ifBarging = false;
			if(consumerLock.empty()) consumerBlock=false;
			if (!done){
				removeCounter += 1;
				bufferNum -= 1;
				removeItem = buffers[bufferNum];
				REMOVE_DONE;
				cout << "consuming "<< removeItem << " with consumerBlock " << consumerBlock 
				<< " with removeCounter: " << removeCounter
				<< endl;
				PROD_SIGNAL( producerLock );
				bool ifProduceBlock = producerLock.signal();
				if (!ifProduceBlock) producerBlock = false;
				else {
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
