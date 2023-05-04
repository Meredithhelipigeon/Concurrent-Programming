#include "q3tallyVotes.h"
#include "q3printer.h"

TallyVotes::TallyVotes( unsigned int voters, unsigned int group, Printer & printer ):
maxGroupSize{group}, printer{printer} , votersNum{voters} {
    currentGroupSize = getGroupSize();
    printer.print(Voter::States::NextTourSize, currentGroupSize);
}

TallyVotes::Tour TallyVotes::vote(unsigned int id, Ballot ballot){
    voterOwnerLock.acquire();
    VOTER_ENTER;

    // in quorum failure, new calls to vote immediately raise exception Failed
    if (votersNum<maxGroupSize) {
        voterOwnerLock.release();
        _Throw Failed();
    }

    if (ifbarging){
        bargingNum += 1;
        printer.print(id, Voter::Barging, bargingNum, curTour.groupno);
        bargingLock.wait( voterOwnerLock );
        bargingNum -= 1;
    }

    // voters vote
    curBallot.picture += ballot.picture;
    curBallot.statue += ballot.statue;
    curBallot.giftshop += ballot.giftshop;
    formedGroupSize += 1;
    printer.print(id, Voter::Vote, ballot);

    if (formedGroupSize<currentGroupSize){
        waitingNum += 1;
        printer.print(id, Voter::Block, waitingNum);
        ifbarging = bargingLock.signal();
        // have to wait until the group is formed
        voterConLock.wait( voterOwnerLock );
        waitingNum -= 1;
        printer.print(id, Voter::Unblock, waitingNum);
    } else {
        // calculate the results
        TourKind curTourKind = Statue;
        if ((curBallot.giftshop>=curBallot.statue) &&
            (curBallot.giftshop>=curBallot.picture)) {
                curTourKind = GiftShop;
        } else if (curBallot.picture>=curBallot.statue){
            curTourKind = Picture;
        }

        // set curTour
        groupno+=1;
        curTour.tourkind = curTourKind;
        curTour.groupno = groupno;
        printer.print(id, Voter::Complete, curTour);
        // set new group
        formedGroupSize = 0;
        currentGroupSize = getGroupSize();
        printer.print(Voter::States::NextTourSize, currentGroupSize);
        // reset ballot
        curBallot = (Ballot) {0,0,0};

        if (!voterConLock.empty()) ifbarging = voterConLock.signal();
        else ifbarging = bargingLock.signal();

        Tour retTour = curTour;

        VOTER_LEAVE;
        voterOwnerLock.release();
        return retTour;
    }

    if (votersNum<maxGroupSize) {
        VOTER_LEAVE;
        voterOwnerLock.release();
        _Throw Failed();
    }

    // unblock other threads
    if (!voterConLock.empty()) ifbarging = voterConLock.signal();
    else ifbarging = bargingLock.signal();

    Tour retTour = curTour;
    VOTER_LEAVE;
    voterOwnerLock.release();
    return retTour;
}

void TallyVotes::done(unsigned int id){
    voterOwnerLock.acquire();
    // in the function of done, although the voter is not terminated formally
    // it is considered as terminated when considering quorem failure
    // so we use votersNum-1 to compare
    if (votersNum-1<maxGroupSize){
        // if there are bargers, let them go first (pretend to be the barger)
        if (ifbarging) {
            bargingLock.wait( voterOwnerLock );
            // if the barging lock wake up, all the bargers(not bargers in done) finished
            // we could set ifbarging as false now
            ifbarging=false;
            printer.print(id, Voter::Done);
        }
        // always wake up the other tasks
        if (!voterConLock.empty())voterConLock.signal();
        else bargingLock.signal();
    }
    votersNum -= 1;
    voterOwnerLock.release();
}

unsigned int TallyVotes::getGroupSize(){
    int newGroupSize = prng(1,maxGroupSize);
    NEW_GROUP( newGroupSize );
    return newGroupSize;
}
