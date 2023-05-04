#ifndef TALLYVOTES_H
#define TALLYVOTES_H

#include <uPRNG.h>
_Monitor Printer;

#if defined( EXT )					// external scheduling monitor solution
_Monitor TallyVotes {

#elif defined( INT )				// internal scheduling monitor solution
_Monitor TallyVotes {

#elif defined( INTB )				// internal scheduling monitor solution with barging
#include "BargingCheckVote.h"
_Monitor TallyVotes {
	uCondition bench;				// only one condition variable (variable name may be changed)
	void wait();					// barging version of wait
	void signalAll();				// unblock all waiting tasks

#elif defined( AUTO )				// automatic-signal monitor solution
#include "AutomaticSignal.h"
_Monitor TallyVotes {

#elif defined( TASK )				// internal/external scheduling task solution
_Task TallyVotes {

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
  #if defined( EXT )
	private:
		const int maxGroupSize;
		Printer & printer;

		int currentGroupSize;
		int formedGroupSize = 0;
		int votersNum;
		Ballot curBallot;
		Tour curTour;
		int groupno = 0; // current group number
		int waitingNum = 0; // number of voters that are waiting to be unblocked
  #elif defined ( INT )
	private:
		const int maxGroupSize;
		Printer & printer;

		int currentGroupSize;
		int formedGroupSize = 0;
		int votersNum;
		Ballot curBallot;
		Tour curTour;
		int groupno = 0; // current group number
		int waitingNum = 0; // number of voters that are waiting to be unblocked
		uCondition group;
  #elif defined ( INTB )
	private:
		const int maxGroupSize;
		Printer & printer;

		int currentGroupSize;
		int formedGroupSize = 0;
		int votersNum;
		Ballot curBallot;
		Tour curTour;
		int groupno = 0; // current group number
		int waitingNum = 0; // number of voters that are waiting to be unblocked

		int validVoters;
		int ticket = 0;
		int bargingNum = 0;
		BCHECK_DECL;
  #elif defined (AUTO)
	private:
		const int maxGroupSize;
		Printer & printer;

		int currentGroupSize;
		int formedGroupSize = 0;
		int votersNum;
		Ballot curBallot;
		Tour curTour;
		int groupno = 0; // current group number
		int waitingNum = 0; // number of voters that are waiting to be unblocked
		AUTOMATIC_SIGNAL;
  #elif defined ( TASK )
	private:
		private:
		const int maxGroupSize;
		Printer & printer;

		int currentGroupSize;
		int formedGroupSize = 0;
		int votersNum;
		Ballot curBallot;
		int voterId;
		int doneId;
		Ballot voterBallot;
		Tour curTour;
		int groupno = 0; // current group number
		int waitingNum = 0; // number of voters that are waiting to be unblocked
		uCondition group;
		void main();
  #endif
};
#endif 