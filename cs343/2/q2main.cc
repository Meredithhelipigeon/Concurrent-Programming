#include "q2player.h"
#include <iostream>
#include <ostream>
#include <unistd.h>
using namespace std;

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
    int games = 5;
    unsigned int players = 0;
    int cards = 0;
    int seed = getpid();

    try {
		switch ( argc ) {
		  case 5:
			if ( strcmp( argv[4], "d" ) != 0 ) {		// default ?
				seed = convert( argv[4] ); if ( seed < 0 ) throw 1; // invalid ?
			} // if
		  case 4:
			if ( strcmp( argv[3], "d" ) != 0 ) {		// default ?
				cards = convert( argv[3] ); if ( cards < 0 ) throw 1; // invalid ?
			} // if
			// FALL THROUGH
		  case 3:
			if ( strcmp( argv[2], "d" ) != 0 ) {		// default ?
				players = convert( argv[2] ); if ( players < 2 ) throw 1; // invalid ?
			} // if
          case 2:
			if ( strcmp( argv[1], "d" ) != 0 ) {		// default ?
				games = convert( argv[1] ); if ( games < 0 ) throw 1; // invalid ?
			} // if
		  case 1:										// defaults
			break;
		  default:										// wrong number of options
			throw 1;
		} // switch
	} catch( ... ) {
		cerr << "Usage: " << argv[0]
			 << "[ games (>=0) | 'd' (default 5) "
             << "[ players (>=2) | 'd' (random 2-10) "
             << "[ cards (>0) | 'd' (random 10-200) "
             << "[ seed (>0) | 'd' (random) ] ] ] ]" << endl;
		exit( EXIT_FAILURE );							// TERMINATE
	} // try

    PRNG prng;
    prng.set_seed(seed);

    // start the game
    PRNG mainPrng;
    mainPrng.set_seed(seed);

    bool ifPlayer = true;
    bool ifCards = true;
    if (players==0) ifPlayer = false;
    if (cards==0) ifCards = false;

    for(int j = 0; j < games; j++){
        // set up the printers and player num for Player class
        if (not ifPlayer) players = mainPrng(2,10);
        if (not ifCards) cards = mainPrng(10,200);
        Printer printer(players, cards);
        Player::players( players );
        int randomStart = mainPrng(players);
        // build the cicycle
        uNoCtor<Player> playersInGame[players];
        for(unsigned int i = 0; i < players; i++){
            playersInGame[i].ctor(prng, printer,i);
        }
        for(unsigned int i = 0; i < players; i++){
            if (i==0){
                playersInGame[i]->start(*playersInGame[players-1], *playersInGame[i+1]);
            } else if (i==players-1){
                playersInGame[i]->start(*playersInGame[i-1], *playersInGame[0]);
            } else {
                playersInGame[i]->start(*playersInGame[(i-1)%players], *playersInGame[(i+1)%players]);
            }
        }
        
        playersInGame[randomStart]->play(cards);
    }
}