#include <iostream>
#include <cstdlib>										// access: rand, srand
#include <cstring>										// access: strcmp
using namespace std;
#include <unistd.h>										// access: getpid
#include <csetjmp>
#ifdef NOOUTPUT
#define PRINT( stmt )
#else
#define PRINT( stmt ) stmt
#endif // NOOUTPUT
struct E {};											// exception type
intmax_t eperiod = 100, excepts = 0, calls = 0, dtors = 0, depth = 0; // counters
PRINT( struct T { ~T() { dtors += 1; } }; )
int count = 0;

// an array type suitable for storing information to restore a calling environment
std::jmp_buf L;

long int Ackermann( long int m, long int n, long int depth ) {
	jmp_buf prevLabel; 
	int labelSize = sizeof(L);
	memcpy(prevLabel, L, labelSize);
	calls += 1;
	if ( m == 0 ) {
		if ( rand() % eperiod <= 2 ) { PRINT( T t; ) excepts += 1; 
		longjmp(L, 1); }
		return n + 1;
	} else if ( n == 0 ) {
        if (setjmp(L)==0){
			long int result = Ackermann( m - 1, 1, depth + 1 );
			memcpy(L, prevLabel, labelSize);
            return result;
        } else {
			memcpy(L, prevLabel, labelSize); 
            PRINT( cout << " depth " << depth << " E1 " << m << " " << n << " |" );
            if ( rand() % eperiod <= 3 ) { PRINT( T t; ) excepts += 1; 
			longjmp(L, 1); }
        }
		PRINT( cout << " E1X " << m << " " << n << endl );
	} else {
		if (setjmp(L)==0){
			long int result = Ackermann( m - 1, Ackermann( m, n - 1, depth + 1 ), depth + 1 );
			memcpy(L, prevLabel, labelSize);
			return result;
		} else {
			memcpy(L, prevLabel, labelSize); 
			PRINT( cout << " depth " << depth << " E2 " << m << " " << n << " |" );
			if ( rand() % eperiod == 0 ) { PRINT( T t; ) excepts += 1;
			longjmp(L, 1);
			}
		}
		PRINT( cout << " E2X " << m << " " << n << endl );
	} // if
	return 0;											// recover by returning 0
}
static intmax_t convert( const char * str ); // copy from https://student.cs.uwaterloo.ca/~cs343/examples/uIO.cc
int main( int argc, char * argv[] ) {
	volatile intmax_t m = 4, n = 6, seed = getpid();	// default values (volatile needed for longjmp)
	try {												// process command-line arguments
		switch ( argc ) {
		  case 5: if ( strcmp( argv[4], "d" ) != 0 ) {	// default ?
			eperiod = convert( argv[4] ); if ( eperiod <= 0 ) throw 1; }
		  case 4: if ( strcmp( argv[3], "d" ) != 0 ) {	// default ?
			seed = convert( argv[3] ); if ( seed <= 0 ) throw 1; }
		  case 3: if ( strcmp( argv[2], "d" ) != 0 ) {	// default ?
			n = convert( argv[2] ); if ( n < 0 ) throw 1; }
		  case 2: if ( strcmp( argv[1], "d" ) != 0 ) {	// default ?
			m = convert( argv[1] ); if ( m < 0 ) throw 1; }
		  case 1: break;								// use all defaults
		  default: throw 1;
		} // switch
	} catch( ... ) {
		cerr << "Usage: " << argv[0] << " [ m (>= 0) | d [ n (>= 0) | d"
			" [ seed (> 0) | d [ eperiod (> 0) | d ] ] ] ]" << endl;
		exit( EXIT_FAILURE );
	} // try
	srand( seed );										// seed random number

    if (setjmp(L)==0) {
        PRINT( cout << "Arguments " << m << " " << n << " " << seed << " " << eperiod << endl );
        long int val = Ackermann( m, n, 0 );
        PRINT( cout << "Ackermann " << val << endl );
    } else {
        PRINT( cout << "E3" << endl );
    }
	cout << "calls " << calls << " exceptions " << excepts << " destructors " << dtors << endl;
}

static intmax_t convert( const char * str ) {			// convert C string to integer
	char * endptr;
	errno = 0;											// reset
	intmax_t val = strtoll( str, &endptr, 10 );			// attempt conversion
	if ( errno == ERANGE ) throw std::out_of_range("");
	if ( endptr == str ||								// conversion failed, no characters generated
		 *endptr != '\0' ) throw std::invalid_argument(""); // not at end of str ?
	return val;
} // convert

