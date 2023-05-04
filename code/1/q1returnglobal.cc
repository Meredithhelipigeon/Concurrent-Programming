#include <iostream>
#include <cstdlib>										// access: rand, srand
#include <cstring>										// access: strcmp
using namespace std;
#include <unistd.h>										// access: getpid

int rtn_status = 0;
short int rtn1_code = 0;
int rtn2_code = 0;
long int rtn3_code = 0;

void cleanupGlobalVariables(){
	rtn_status = 0;
	rtn1_code = 0;
	rtn2_code = 0;
	rtn3_code = 0;
}

intmax_t eperiod = 10000;								// exception period
int randcnt = 0;
int Rand() { randcnt += 1; return rand(); }

double rtn1( double i ) {
	if ( Rand() % eperiod == 0 ) { 
	  rtn_status = 1;
	  rtn1_code =  (short int) Rand();
	  return 0.0;
    }

	return i + Rand();
}
double rtn2( double i ) {
	if ( Rand() % eperiod == 0 ) { 
	  rtn_status = 2;
	  rtn2_code =  Rand();
	  return 0.0;
	}
	double rtn1Val = rtn1( i );
	if (rtn_status == 0) return rtn1Val + Rand();
	else return 0.0;
}
double rtn3( double i ) {
	if ( Rand() % eperiod == 0 ) { 
	  rtn_status = 3;
	  rtn3_code =  Rand();
	  return 0.0;
	}
	double rtn2Val = rtn2( i );
	if (rtn_status == 0) return rtn2Val + Rand();
	else return 0.0;
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

int main( int argc, char * argv[] ) {
	intmax_t times = 100000000, seed = getpid();		// default values
	try {
		switch ( argc ) {
		  case 4: if ( strcmp( argv[3], "d" ) != 0 ) {	// default ?
			seed = convert( argv[3] ); if ( seed <= 0 ) throw 1; }
		  case 3: if ( strcmp( argv[2], "d" ) != 0 ) {	// default ?
			eperiod = convert( argv[2] ); if ( eperiod <= 0 ) throw 1; }
		  case 2: if ( strcmp( argv[1], "d" ) != 0 ) {	// default ?
			times = convert( argv[1] ); if ( times <= 0 ) throw 1; }
		  case 1: break;								// use all defaults
		  default: throw 1;
		} // switch
	} catch( ... ) {
		cerr << "Usage: " << argv[0] << " [ times > 0 | d [ eperiod > 0 | d [ seed > 0 | d ] ] ]" << endl;
		exit( EXIT_FAILURE );
	} // try
	srand( seed );

	double rv = 0.0;
	int ev1 = 0, ev2 = 0, ev3 = 0;
	int rc = 0, ec1 = 0, ec2 = 0, ec3 = 0;

	for ( int i = 0; i < times; i += 1 ) {
		double tempValue = rtn3( i );

		if (rtn_status == 1) { ev1 += rtn1_code; ec1 += 1; 
		rtn1_code = 0;}
		else if (rtn_status == 2) { ev2 += rtn2_code; ec2 += 1; 
		rtn2_code = 0;}
		else if (rtn_status == 3) { ev3 += rtn3_code; ec3 += 1;
		rtn3_code = 0;}
		else { rv += tempValue; rc += 1;}
		rtn_status = 0;
	} // for
	cout << "randcnt " << randcnt << endl;
	cout << "normal result " << rv << " exception results " << ev1 << ' ' << ev2 << ' ' << ev3 << endl;
	cout << "calls "  << rc << " exceptions " << ec1 << ' ' << ec2 << ' ' << ec3 << endl;
}

