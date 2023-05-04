using namespace std;

struct printerInfo{
	int took;
	int remaining;
	bool ifPrint;
	bool ifDrunk;
	bool ifDead;
	bool ifLastPlyer;
};

class Printer {
	// Private Members
    static unsigned int noOfCards;
	printerInfo * buffer;
	int noOfPlayers;

	// helper function for the printer
	void printNotPrintedBuffer();
	void printHeader(const unsigned int NoOfPlayers, const unsigned int NoOfCards);
	
  public:
	Printer( const unsigned int NoOfPlayers, const unsigned int NoOfCards );
	void prt( unsigned int id, int took, unsigned int RemainingPlayers );  // card play
	void prt( unsigned int id );  // drink (Schmilblick)
	~Printer() {
		delete []buffer;
	}
};
