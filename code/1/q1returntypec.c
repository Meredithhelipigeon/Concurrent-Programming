#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  
#include <errno.h>
#include <stdint.h>

typedef enum {NORMAL, NONPOSITIVE, EXCEED_RANGE, INVALID_ARGS} CmdStatus ;
typedef enum { SUCCEED, EX1, EX2, EX3 } RtnStatus ;

intmax_t eperiod = 10000;	// exception period
int randcnt = 0;
int Rand() { randcnt += 1; return rand(); }
// set command line status as normal at first
CmdStatus cmdStatus = NORMAL;
typedef struct {RtnStatus rtnStatus; 
union {
	double rtnVal;
    short int ex1code; 
    int ex2code; 
    long int ex3code; 
  };
} RtnResult;

RtnResult rtn1( double i ) {
	if ( Rand() % eperiod == 0 ) { 
		return (RtnResult) {.rtnStatus = EX1, .ex1code = (short int) Rand() };
  	}
  	return (RtnResult) {.rtnStatus = SUCCEED, .rtnVal = i + Rand()};
}

RtnResult rtn2( double i ) {
	if ( Rand() % eperiod == 0 ) { 
	  return (RtnResult) {.rtnStatus = EX2, .ex2code = Rand() };
	}
	RtnResult rtn1Result = rtn1( i );
	if (rtn1Result.rtnStatus == SUCCEED) return (RtnResult){.rtnStatus = SUCCEED, .rtnVal = rtn1Result.rtnVal + Rand() };
	else return rtn1Result;
}
RtnResult rtn3( double i ) {
	if ( Rand() % eperiod == 0 ) { 
	  return (RtnResult){.rtnStatus = EX3, .ex3code = Rand() };
	}
	RtnResult rtn2Result = rtn2( i );
	if (rtn2Result.rtnStatus == SUCCEED) return (RtnResult){.rtnStatus = SUCCEED, .rtnVal = rtn2Result.rtnVal + Rand() };
	else return rtn2Result;
}

static intmax_t convert( const char * str ) {			// convert C string to integer
	char * endptr;
	errno = 0;											// reset
	intmax_t val = strtoll( str, &endptr, 10 );			// attempt conversion
	if ( errno == ERANGE ) cmdStatus = EXCEED_RANGE;
	if ( endptr == str ||								// conversion failed, no characters generated
		 *endptr != '\0' ) cmdStatus = INVALID_ARGS; // not at end of str ?
	return val;
} // convert

int main( int argc, char * argv[] ) {
	intmax_t times = 100000000, seed = getpid();		// default values
    switch ( argc ) {
        case 4: if ( strcmp( argv[3], "d" ) != 0 ) {
                    seed = convert( argv[3] ); 
                    if ( seed <= 0 ) cmdStatus = NONPOSITIVE;
                    if (cmdStatus != NORMAL) break;
                }
		case 3: if ( strcmp( argv[2], "d" ) != 0 ) {
                    eperiod = convert( argv[2] ); 
                    if ( eperiod <= 0 ) cmdStatus = NONPOSITIVE;
                    if (cmdStatus != NORMAL) break;
                }
		case 2: if ( strcmp( argv[1], "d" ) != 0 ) {
                    times = convert( argv[1] ); 
                    if ( times <= 0) cmdStatus = NONPOSITIVE;
                    if (cmdStatus != NORMAL) break;
                }
		case 1: break;								// use all defaults
		default: 
            cmdStatus = NONPOSITIVE;
            break;
    }

    if (cmdStatus != NORMAL){
        if (cmdStatus == NONPOSITIVE) printf("Usage: %s [times > 0 | d [eperiod > 0 | d [ seed > 0 ] ] ]\n", argv[0]);
        else if (cmdStatus == EXCEED_RANGE) printf("Usage: %s exceeds maximum range\n", argv[0]);
        else if (cmdStatus == INVALID_ARGS) printf("Usage: %s exceeds maximum range\n", argv[0]);
        exit( EXIT_FAILURE );
    }
	srand( seed );

	double rv = 0.0;
	int ev1 = 0, ev2 = 0, ev3 = 0;
	int rc = 0, ec1 = 0, ec2 = 0, ec3 = 0;

	for ( int i = 0; i < times; i += 1 ) {
		RtnResult rtn3Result = rtn3( i );
		switch (rtn3Result.rtnStatus)
		{
		case EX1:
			ev1 += rtn3Result.ex1code; ec1 += 1;
			break;
		case EX2:
			ev2 += rtn3Result.ex2code; ec2 += 1;
			break;
		case EX3:
			ev3 += rtn3Result.ex3code; ec3 += 1;
			break;
		case SUCCEED:
			rv += rtn3Result.rtnVal; rc += 1;
			break;
		default:
			break;
		}

	}
	printf("randcnt %d\n", randcnt);
	printf("normal result %g exception results %d %d %d\n", rv, ev1, ev2, ev3);
    printf("calls %d exceptions %d %d %d\n", rc, ec1, ec2, ec3);
}
