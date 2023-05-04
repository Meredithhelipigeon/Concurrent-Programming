#ifndef TALLYVOTES_H
#define TALLYVOTES_H

#include <uPRNG.h>
_Monitor Printer;

#if defined( MC )
#include "BargingCheckVote.h"
class TallyVotes {

#elif defined( SEM )
#include "BargingCheckVote.h"
#include <uSemaphore.h>
class TallyVotes {

#elif defined ( BAR )
#include <uBarrier.h>

_Cormonitor TallyVotes : public uBarrier {

#endif

  public:
	_Event Failed {};
	struct Ballot { unsigned int picture, statue, giftshop; };
	enum TourKind : char { Picture = 'p', Statue = 's', GiftShop = 'g' };
	struct Tour { TourKind tourkind; unsigned int groupno; };

	TallyVotes( unsigned int voters, unsigned int group, Printer & printer );
	Tour vote( unsigned int id, Ballot ballot );
	void done( 
		#if defined( MC ) || defined( BAR )
		unsigned int id 
		#endif
	);
	unsigned int getGroupSize();	// generates size of next tour group and prints in 'TG'

   #if defined( MC )
   private:
		const int maxGroupSize;
		Printer & printer;
		uOwnerLock voterOwnerLock;
		uCondLock voterConLock;
		uCondLock bargingLock;

		int currentGroupSize;
		int formedGroupSize = 0;
		int votersNum;
		Ballot curBallot;
		Tour curTour;
		int groupno = 0; // current group number
		int waitingNum = 0; // number of voters that are waiting to be unblocked
		int bargingNum = 0; // number of bargers
		bool ifbarging = false;

		BCHECK_DECL;
	#elif defined ( BAR )
		const int maxGroupSize;
		Printer & printer;
		int votersNum;

		Ballot curBallot;
		Tour curTour;
		uBaseTask *Nth_ = nullptr;
		int groupno = 0;
		bool ifStart = false;

		protected:
			void last();
	#elif defined ( SEM )
		const int maxGroupSize;
		Printer & printer;
		
		int votersNum;
		int currentGroupSize;
		Ballot curBallot;
		Tour curTour;
		int groupno = 0;
		int formedGroupSize = 0;
		int waitingNum = 0;
		uSemaphore comLock;
		uSemaphore syncLock = uSemaphore(0);
		uSemaphore waitingLock = uSemaphore(0);
		BCHECK_DECL;
	#endif
};
#endif 