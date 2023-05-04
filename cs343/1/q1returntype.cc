#include <iostream>
#include <variant>
#include <cstdlib>										// access: rand, srand
#include <cstring>										// access: strcmp
using namespace std;
#include <unistd.h>										// access: getpid

typedef enum { EX1, EX2, EX3 } RtnEx ;

struct Ex{RtnEx rtnEx; short int ex1Code; int ex2Code; long int ex3Code;};
struct Ex1: Ex { Ex1(short int code) { ex1Code = code; rtnEx = EX1; }};
struct Ex2: Ex { Ex2(int code) { ex2Code = code; rtnEx = EX2; }};
struct Ex3: Ex { Ex3(long int code) {ex3Code = code; rtnEx = EX3; }};

intmax_t eperiod = 10000;								// exception period
int randcnt = 0;
int Rand() { randcnt += 1; return rand(); }

variant<double, Ex> rtn1( double i ) {
    if ( Rand() % eperiod == 0 ) { return Ex1{ (short int)Rand() }; } 
	return i + Rand();
}
variant<double, Ex>  rtn2( double i ) {
    if ( Rand() % eperiod == 0 ) { return Ex2{ Rand() }; }
    
    variant<double, Ex> rtn1Val = rtn1( i );
    if (holds_alternative<double>(rtn1Val)) return get<double>(rtn1Val) + Rand();
    else return get<Ex>(rtn1Val);
}
variant<double, Ex>  rtn3( double i ) {
    if ( Rand() % eperiod == 0 ) { return Ex3{ Rand() }; }

    variant<double, Ex> rtn2Val = rtn2( i );
    if (holds_alternative<double>(rtn2Val)) return get<double>(rtn2Val) + Rand();
    else return get<Ex>(rtn2Val);
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
        variant<double, Ex> rtn3Val = rtn3( i );
        if (holds_alternative<Ex>(rtn3Val)){
            Ex exeception = get<Ex>(rtn3Val);
            switch (exeception.rtnEx)
			{
			case EX1:
				ev1 += exeception.ex1Code; ec1 += 1;
				break;
			case EX2:
				ev2 += exeception.ex2Code; ec2 += 1;
				break;
			case EX3:
				ev3 += exeception.ex3Code; ec3 += 1;
				break;
			default:
				break;
			}
        } else {
            rv += get<double>(rtn3Val); rc += 1;
        }
	} // for
	cout << "randcnt " << randcnt << endl;
	cout << "normal result " << rv << " exception results " << ev1 << ' ' << ev2 << ' ' << ev3 << endl;
	cout << "calls "  << rc << " exceptions " << ec1 << ' ' << ec2 << ' ' << ec3 << endl;
}
