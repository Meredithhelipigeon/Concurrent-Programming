#include <iostream>
using namespace std;

static volatile long int shared = -6;					// volatile to prevent dead-code removal
static intmax_t iterations = 500000000;

_Task increment {
	void main() {
		for ( int i = 1; i <= iterations; i += 1 ) {
			shared += 1;								// two increments to increase pipeline size
			shared += 2;
		} // for
	} // increment::main
}; // increment

static intmax_t convert( const char * str ) {			// convert C string to integer
	char * endptr;
	errno = 0;											// reset
	intmax_t val = strtoll( str, &endptr, 10 );			// attempt conversion
	if ( errno == ERANGE ) throw std::out_of_range("");
	if ( endptr == str ||								// conversion failed, no characters generated
		 *endptr != '\0' ) throw std::invalid_argument(""); // not at end of str ?
	return val;
} // convert

int main( int argc, char * argv[] ) {
	intmax_t processors = 2;
	try {												// process command-line arguments
		switch ( argc ) {
		  case 3: processors = convert( argv[2] ); if ( processors <= 0 ) throw 1;
		  case 2: iterations = convert( argv[1] ); if ( iterations <= 0 ) throw 1;
		  case 1: break;								// use defaults
		  default: throw 1;
		} // switch
	} catch( ... ) {
		cout << "Usage: " << argv[0] << " [ iterations (> 0) [ processors (> 0) ] ]" << endl;
		exit( 1 );
	} // try
	uProcessor p[processors - 1];						// create additional kernel threads
	{
		increment t[2];
	} // wait for tasks to finish
	cout << "shared:" << shared << endl;
} // main

// Local Variables: //
// tab-width: 4 //
// End: //
