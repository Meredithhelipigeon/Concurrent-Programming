#include "q3tallyVotes.h"
#include "q3printer.h"

TallyVotes::TallyVotes( unsigned int voters, unsigned int group, Printer & printer ):
uBarrier(prng(1,group)), maxGroupSize(group), printer{printer} , votersNum(voters) {
    printer.print(Voter::States::NextTourSize, uBarrier::total());
}

void TallyVotes::last(){
    // calculate the final results
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
    Nth_ = &uThisTask();
}

unsigned int TallyVotes::getGroupSize(){
    int s = prng(1,maxGroupSize);
    return s;
}

void TallyVotes::done( unsigned int id ){

    if (votersNum-1 < maxGroupSize){
        // when waiters is more than 0, it means we have to unblock them
        // in done
        if (waiters() > 0){
            uBarrier::block();
            printer.print(id, Voter::Done);
        }
    }

    votersNum -= 1;
}

TallyVotes::Tour TallyVotes::vote( unsigned int id, Ballot ballot ){
    if (votersNum<maxGroupSize) { _Throw Failed(); }
    
    // voters vote
    curBallot.picture += ballot.picture;
    curBallot.statue += ballot.statue;
    curBallot.giftshop += ballot.giftshop;
    printer.print(id, Voter::Vote, ballot);

    if (uBarrier::waiters() + 1 == uBarrier::total()) {
        uBarrier::block(); // the last one of the group, will unblock others threads
        printer.print(id, Voter::Complete, curTour);
    } else {
        printer.print(id, Voter::Block, uBarrier::waiters() + 1);
        uBarrier::block();
        printer.print(id, Voter::Unblock, uBarrier::waiters());
    }

    if (votersNum<maxGroupSize) { _Throw Failed(); }

    // when there is no one waiting reset the group
    if (uBarrier::waiters()==0){
        int nextGroupSize = prng(1,maxGroupSize);
        reset(nextGroupSize);
        printer.print(Voter::States::NextTourSize, nextGroupSize);
    }

    return curTour;
}