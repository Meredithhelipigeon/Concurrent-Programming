void TallyVotes::wait() {
	bench.wait();							// wait until signalled
	while ( rand() % 2 == 0 ) {				// multiple bargers allowed
		try {
			_Accept( vote || done ) {		// accept barging callers
			} _Else {						// do not wait if no callers
			} // _Accept
		} catch( uMutexFailure::RendezvousFailure & ) {}
	} // while
}

void TallyVotes::signalAll() {				// also useful
	while ( ! bench.empty() ) bench.signal(); // drain the condition
}

#if defined( EXT )					// external scheduling monitor solution
// includes for this kind of vote-tallier
_Monitor TallyVotes {
	// private declarations for this kind of vote-tallier
#elif defined( INT )				// internal scheduling monitor solution
// includes for this kind of vote-tallier
_Monitor TallyVotes {
	// private declarations for this kind of vote-tallier
#elif defined( INTB )				// internal scheduling monitor solution with barging
// includes for this kind of vote-tallier
_Monitor TallyVotes {
	// private declarations for this kind of vote-tallier
	uCondition bench;				// only one condition variable (variable name may be changed)
	void wait();					// barging version of wait
	void signalAll();				// unblock all waiting tasks
#elif defined( AUTO )				// automatic-signal monitor solution
// includes for this kind of vote-tallier
_Monitor TallyVotes {
	// private declarations for this kind of vote-tallier
#elif defined( TASK )				// internal/external scheduling task solution
_Task TallyVotes {
	// private declarations for this kind of vote-tallier
#else
	#error unsupported voter type
#endif
	// common declarations
  public:							// common interface
	_Event Failed {};
	struct Ballot { unsigned int picture, statue, giftshop; };
	enum TourKind : char { Picture = 'p', Statue = 's', GiftShop = 'g' };
	struct Tour { TourKind tourkind; unsigned int groupno; };

	TallyVotes( unsigned int voters, unsigned int group, Printer & printer );
	Tour vote( unsigned int id, Ballot ballot );
	void done(
		#if defined( TASK )			// task solution
		unsigned int id
		#endif
	);
	unsigned int getGroupSize();	// generates size of next tour group and prints in 'TG'
};

#include "BargingCheck.h"
_Monitor TallyVotes {
	...									// regular declarations
	BCHECK_DECL;
  public:
	...									// regular declarations
	Tour vote( unsigned int id __attribute__(( unused )), Ballot ballot ) {
		VOTER_ENTER;
		...								// voting code
		VOTER_LEAVE;
		// return majority vote
	}
	unsigned int getGroupSize() {
		// generate group size
		NEW_GROUP( next_group_size );
		// return group size
	}
};

#include <uPRNG.h>
_Task Voter {
	TallyVotes::Ballot cast() __attribute__(( warn_unused_result )) {  // cast 3-way vote
		// O(1) random selection of 3 items without replacement using divide and conquer.
	    static const unsigned int voting[3][2][2] = { { {2,1}, {1,2} }, { {0,2}, {2,0} }, { {0,1}, {1,0} } };
	    unsigned int picture = prng( 3 ), statue = prng( 2 );
	    return (TallyVotes::Ballot){ picture, voting[picture][statue][0], voting[picture][statue][1] };
	}
  public:
	enum States : char { Start = 'S', Vote = 'V', Block = 'B', Unblock = 'U', Barging = 'b',
		NextTourSize = 'N', Done = 'D', Complete = 'C', Going = 'G', Failed = 'X', Terminated = 'T' };
	Voter( unsigned int id, unsigned int nvotes, TallyVotes & voteTallier, Printer & printer );
};

_Monitor / _Cormonitor Printer {	// chose one of the two kinds of type constructor
  public:
	Printer( unsigned int voters );
	void print( Voter::States state, unsigned int nextGroupSize );
	void print( unsigned int id, Voter::States state );
	void print( unsigned int id, Voter::States state, TallyVotes::Tour tour );
	void print( unsigned int id, Voter::States state, TallyVotes::Ballot vote );
	void print( unsigned int id, Voter::States state, unsigned int numBlocked );
	void print( unsigned int id, Voter::States state, unsigned int numBlocked, unsigned int group );
};
