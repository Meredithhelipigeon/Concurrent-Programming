_Coroutine Player {
	// YOU MAY ADD PRIVATE MEMBERS, INCLUDING STATICS
  public:
	enum { DEATH_DECK_DIVISOR = 7 };
	static void players( unsigned int num );
	Player( PRNG & prng, Printer &printer, unsigned int id );
	void start( Player &lp, Player &rp );
	void play( unsigned int deck );
	void drink();
};

class Printer {
	// YOU MAY ADD PRIVATE MEMBERS
  public:
	Printer( const unsigned int NoOfPlayers, const unsigned int NoOfCards );
	void prt( unsigned int id, int took, unsigned int RemainingPlayers );  // card play
	void prt( unsigned int id );  // drink (Schmilblick)
};
