#include <uPRNG.h>
#include "q2printer.h"

_Event Stop {};
_Event Drink {};
_Coroutine Player {
    // Private 
    static unsigned int numOfPlayers;
    PRNG * prng;
    Printer * printer;
    Player * lplayer;
    Player * rplayer;
    bool ifTerminated = false;
    int deck;
    void main();
    static unsigned int numOfDrunkPeople;
    static int startId;
    int id;
  
  public:
	enum { DEATH_DECK_DIVISOR = 7, EVEN_CHECK = 2 };
  const int MAXIMUM_CHOSEN_CARDS_NUM = 8;
	static void players( unsigned int num );
	Player( PRNG & prng, Printer &printer, unsigned int id );
	void start( Player &lp, Player &rp );
	void play( unsigned int deck );
	void drink();
}; 
