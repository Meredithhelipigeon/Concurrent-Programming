template<typename T> class BoundedBuffer {
  public:
	_Event Poison {};
	BoundedBuffer( const unsigned int size = 10 );
	unsigned long int blocks();
	void poison();
	void insert( T elem );
	T remove() __attribute__(( warn_unused_result ));
};

#ifdef BUSY							// busy waiting implementation
// implementation
#endif // BUSY

#ifdef NOBUSY						// no busy waiting implementation
// implementation
#endif // NOBUSY

#ifdef NOBUSY							// no busy waiting implementation
#include "BargingCheck.h"
template<typename T> class BoundedBuffer {
	...									// regular declarations
	BCHECK_DECL;
  public:
	void insert( T elem ) {
		// acquire mutual exclusion
		PROD_ENTER;
		...
		// buffer insert
		INSERT_DONE;
		...
		CONS_SIGNAL( cond-lock );
		cond-lock.signal();				// signal consumer task
		...
		PROD_SIGNAL( cond-lock );		// if necessary
		cond-lock.signal();				// signal producer task
		...
	}
	T remove() __attribute__(( warn_unused_result )) {
		// acquire mutual exclusion
		CONS_ENTER;
		...
		// buffer remove
		REMOVE_DONE;
		...
		PROD_SIGNAL( cond-lock );
		cond-lock.signal();				// signal producer task
		...
		CONS_SIGNAL( cond-lock );		// if necessary
		cond-lock.signal();				// signal consumer task
		...
	}
};
#endif // NOBUSY

_Task Producer {
	void main();
  public:
	Producer( BoundedBuffer<int> & buffer, const int Produce, const int Delay );
};

_Task Consumer {
	void main();
  public:
	Consumer( BoundedBuffer<int> & buffer, const int Delay, int &sum );
};
