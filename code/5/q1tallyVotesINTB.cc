#include "q1tallyVotes.h"
#include "q1printer.h"

TallyVotes::TallyVotes( unsigned int voters, unsigned int group, Printer & printer ):
maxGroupSize{group}, printer{printer} , votersNum{voters} {
    currentGroupSize = getGroupSize();
    printer.print(Voter::States::NextTourSize, currentGroupSize);
    validVoters = currentGroupSize;
}

TallyVotes::Tour TallyVotes::vote(unsigned int id, Ballot ballot){
    VOTER_ENTER;
    // in quorum failure, new calls to vote immediately raise exception Failed
    if (votersNum<maxGroupSize) {
        _Throw Failed();
    }

    // We get a ticket for every voter
    // If voter is valid now, it could be processed;
    // We use while loop here to make sure as long as it is not their term, even if it is waken up,
    // it will still wait again.
    ticket += 1;
    int curTicket = ticket;
    while (curTicket > validVoters) {
        bargingNum += 1;
        printer.print(id, Voter::Barging, bargingNum, curTour.groupno);
        wait();
        bargingNum -= 1;
        if (votersNum<maxGroupSize) {
            _Throw Failed();
        }
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
        // tourists have to wait here until the group is formed
        wait();
        waitingNum -= 1;
        printer.print(id, Voter::Unblock, waitingNum);
        if (votersNum<maxGroupSize) {
            VOTER_LEAVE;
            _Throw Failed();
        }
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
        // wake up all the members(the tourists that have voted will exit, barger will still be in the loop)
        signalAll();
    }

    if (waitingNum==0){
        validVoters += currentGroupSize;
        // wake up everyone if all the member of previous group exit
        signalAll();
    }

    Tour retTour = curTour;
    VOTER_LEAVE;
    return retTour;
}

void TallyVotes::done(){
    votersNum -= 1;
    // wake up everyone if quorem failure
    if (votersNum<maxGroupSize) {
        signalAll();
    }
}

unsigned int TallyVotes::getGroupSize(){
    int newGroupSize = prng(1,maxGroupSize);
    NEW_GROUP( newGroupSize );
    return newGroupSize;
}

void TallyVotes::wait() { 
    bench.wait(); // wait until signalled
    while ( rand() % 2 == 0 ) { // multiple bargers allowed 
        try {
                _Accept( vote || done ) { // accept barging callers
                }
                _Else {  // do not wait if no callers       
                } // _Accept
        } catch( uMutexFailure::RendezvousFailure & ) {} 
    } // while
}

void TallyVotes::signalAll() { // also useful
    while ( ! bench.empty() ) bench.signal(); // drain the condition
}
