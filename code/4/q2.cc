void CriticalSection() {
	static volatile uBaseTask * curr;					// shared
	curr = &uThisTask();
	for ( volatile unsigned int i = 0; i < 100; i += 1 ) { // work
		if ( curr != &uThisTask() ) { abort( "interference" ); }
	} // for
}

volatile bool intent[2] = { false, false };				// shared between tasks
volatile int turn = 0;

_Task Hyman {
	int me, you;
	void main() {
		for ( unsigned int i = 0; i < 10000000; i += 1 ) {
			intent[me] = true;							// entry protocol
			while ( turn != me ) {
				while ( intent[you] ) {}
				turn = me;
			} // while
			CriticalSection();
			intent[me] = false;							// exit protocol
		} // for
	}
  public:
	Hyman( int me, int you ) : me(me), you(you) {}
};

int main() {
	uProcessor p;										// processor per task
	Hyman h0( 0, 1 ), h1( 1, 0 );
}
