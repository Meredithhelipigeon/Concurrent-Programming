#include "q3tallyVotes.h"
#include "q3printer.h"

TallyVotes::TallyVotes( unsigned int voters, unsigned int group, Printer & printer ):
maxGroupSize{group}, printer{printer} , votersNum{voters} {
    currentGroupSize = getGroupSize();
    printer.print(Voter::States::NextTourSize, currentGroupSize);
}

TallyVotes::Tour TallyVotes::vote(unsigned int id, Ballot ballot){

    comLock.P();
    VOTER_ENTER;
    // in quorum failure, new calls to vote immediately raise exception Failed
    if (votersNum<maxGroupSize) {
        comLock.V();
        _Throw Failed();
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
        waitingLock.P(comLock);
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
        curBallot = (Ballot) {0,0,0};
        printer.print(id, Voter::Complete, curTour);
        // set new group
        formedGroupSize = 0;
        currentGroupSize = getGroupSize();
        printer.print(Voter::States::NextTourSize, currentGroupSize);
    }

    if (votersNum<maxGroupSize) {
        comLock.V();
        _Throw Failed();
    }

    if (!waitingLock.empty()) {
        waitingLock.V();
        // not releasing the lock comLock for barging prevention
        VOTER_LEAVE;
        return curTour;
    }
    
    Tour retTour = curTour;
    VOTER_LEAVE;
    comLock.V();
    return retTour;
}

void TallyVotes::done(){
    comLock.P();
    votersNum -= 1;
    if (votersNum<maxGroupSize) {
        if (!waitingLock.empty()) {
            waitingLock.V();
            return;
        }
    }
    comLock.V();
}


unsigned int TallyVotes::getGroupSize(){
    int s = prng(1,maxGroupSize);
    NEW_GROUP( s );
    return s;
}
