#include "q1printer.h"

static intmax_t convert( const char * str ) {			// convert C string to integer
	char * endptr;
	errno = 0;											// reset
	intmax_t val = strtoll( str, &endptr, 10 );			// attempt conversion
	if ( errno == ERANGE ) throw std::out_of_range("");
	if ( endptr == str ||								// conversion failed, no characters generated
		 *endptr != '\0' ) throw std::invalid_argument(""); // not at end of str ?
	return val;
} // convert

int main( int argc, char * argv[] ){
    int voters = 6;
    int group = 3;
    int votes = 1;
    long int seed = 1342;
    int processors = 1;

    try { // process command-line arguments
    switch ( argc ) {
        case 7: throw 1;
        case 6:
            if ( strcmp( argv[5], "d" ) != 0 ) {processors = convert( argv[5] ); if ( processors <= 0 ) throw 1;}
        case 5: 
            if ( strcmp( argv[4], "d" ) != 0 ) {seed = convert( argv[4] ); if ( seed <= 0 ) throw 1;}
        case 4: 
            if ( strcmp( argv[3], "d" ) != 0 ) {votes = convert( argv[3] ); if ( votes <= 0 ) throw 1;}
        case 3: 
            if ( strcmp( argv[2], "d" ) != 0 ) {group = convert( argv[2] ); if ( group <= 0 ) throw 1;}
        case 2: 
            if ( strcmp( argv[1], "d" ) != 0 ) {voters = convert( argv[1] ); if ( voters <= 0 ) throw 1; }}
    } catch( ... ) {
        cout << "Usage:" << argv[0] 
        << "vote [voters |'d'[ group |'d'[ votes |'d'[ seed |'d'[ processors |'d'] ] ] ] ]" << endl; 
        exit( 1 );
    } // try
    uProcessor p[processors - 1];

    set_seed(seed);
    Voter * voterTasks[voters];
    Printer printer(voters);
    TallyVotes tallyVotes(voters, group, printer);

    for(int i = 0; i < voters; i++){
        voterTasks[i] = new Voter(i, votes, tallyVotes, printer);
    }

    // use magic delete for waiting for tasks to be completed
    for(int i = 0; i < voters; i++){
        delete voterTasks[i];
    }
}