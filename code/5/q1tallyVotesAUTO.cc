#include "q1tallyVotes.h"
#include "q1printer.h"

TallyVotes::TallyVotes( unsigned int voters, unsigned int group, Printer & printer ):
maxGroupSize{group}, printer{printer} , votersNum{voters} {
    currentGroupSize = getGroupSize();
    printer.print(Voter::States::NextTourSize, currentGroupSize);
}

TallyVotes::Tour TallyVotes::vote(unsigned int id, Ballot ballot){
    // in quorum failure, new calls to vote immediately raise exception Failed
    if (votersNum<maxGroupSize) _Throw Failed();

    // voters vote
    curBallot.picture += ballot.picture;
    curBallot.statue += ballot.statue;
    curBallot.giftshop += ballot.giftshop;
    formedGroupSize += 1;
    printer.print(id, Voter::Vote, ballot);

    // get the ticket number for the group that is going to form
    int newCurNum = groupno+1;

    if (formedGroupSize<currentGroupSize){
        // have to wait until the group is formed or quorem failure
        WAITUNTIL(((votersNum<maxGroupSize)||(newCurNum==groupno)),
        waitingNum += 1;
        printer.print(id, Voter::Block, waitingNum);,
        waitingNum -= 1;
        printer.print(id, Voter::Unblock, waitingNum);
        )
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
    }

    // detect quorum failure after waking up
    if (votersNum<maxGroupSize) {
        EXIT();
        _Throw Failed();
    }

    Tour retTour = curTour;
    EXIT();
    return retTour;
}

void TallyVotes::done(){
    votersNum -= 1;
    EXIT();
}

unsigned int TallyVotes::getGroupSize(){
    int newGroupSize = prng(1,maxGroupSize);
    return newGroupSize;
}
