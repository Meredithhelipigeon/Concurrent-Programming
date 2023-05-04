#include "q3tallyVotes.h"
#include "q3printer.h"
#include <iostream>
using namespace std;

TallyVotes::TallyVotes( unsigned int voters, unsigned int group, Printer & printer ):
maxGroupSize{group}, printer{printer} , votersNum{voters} {
    currentGroupSize = getGroupSize();
    printer.print(Voter::States::NextTourSize, currentGroupSize);
}

TallyVotes::Tour TallyVotes::vote(unsigned int id, Ballot ballot){
    voterOwnerLock.acquire();
    // cout << id << " is acquiring lock" << " with " << i << endl;
    VOTER_ENTER;
    if (votersNum<maxGroupSize) {
        voterOwnerLock.release();
        _Throw Failed();
    }

    if (ifbarging){
        bargingNum += 1;
        printer.print(id, Voter::Barging, bargingNum, curTour.groupno);
        bargingLock.wait( voterOwnerLock );
        // cout << "barging lock " << id << " is being unblocked" << endl;
        bargingNum -= 1;
    }

    // need to update tour information
    if ( formedGroupSize==0 ){
        curBallot.picture = 0;
        curBallot.statue = 0;
        curBallot.giftshop = 0;
    }

    // update tickets
    curBallot.picture += ballot.picture;
    curBallot.statue += ballot.statue;
    curBallot.giftshop += ballot.giftshop;
    formedGroupSize += 1;
    printer.print(id, Voter::Vote, ballot);
    // cout << "id with " << id << " is voting" << endl;

    if (formedGroupSize<currentGroupSize){
        waitingNum += 1;
        printer.print(id, Voter::Block, waitingNum);
        ifbarging = bargingLock.signal();
        // cout << "id " << id << " unblocking bargingLock " << endl;
        voterConLock.wait( voterOwnerLock );
        // cout << "voterOwnerLock lock " << id << " is being unblocked" << endl;
        waitingNum -= 1;
        printer.print(id, Voter::Unblock, waitingNum);
    } else {
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

        if (!voterConLock.empty()){
            ifbarging = voterConLock.signal();
            // if (ifbarging) cout << "id " << id << " unblocking voterConLock" << endl;
        } else {
            ifbarging = bargingLock.signal();
            // if (ifbarging) cout << "id " << id << " unblocking bargingLock" << endl;
        }

        Tour retTour = curTour;
        if (waitingNum>0) ifwaiting = true;

        // cout << id << " is releasing lock with "<< j << endl;
        VOTER_LEAVE;
        voterOwnerLock.release();
        return retTour;
    }

    if (!voterConLock.empty()){
        ifbarging = voterConLock.signal();
        // if (ifbarging) cout << "id " << id << " unblocking voterConLock" << endl;
    } else {
        ifbarging = bargingLock.signal();
        // if (ifbarging) cout << "id " << id << " unblocking bargingLock" << endl;
    }

    if (!ifwaiting && (votersNum<maxGroupSize)) {
        // ifbarging = false;
        // cout << id << " is releasing lock with "<< j << endl;
        VOTER_LEAVE;
        voterOwnerLock.release();
        _Throw Failed();
    }

    if (waitingNum==0) ifwaiting = false;

    Tour retTour = curTour;

    // cout << id << " is releasing lock with "<< j << endl;
    VOTER_LEAVE;
    voterOwnerLock.release();
    return retTour;
}

void TallyVotes::done(unsigned int id){
    voterOwnerLock.acquire();
    // cout << "id " << id << " acquired the lock" << " if barging" << ifbarging << endl;
    // cout << votersNum << " " << maxGroupSize << endl;
    votersNum -= 1;
    if (votersNum<maxGroupSize){
        cout << "id " << id << " ifbarging " << ifbarging << endl;
        if (ifbarging) {
            // if (!voterConLock.empty()){
            //     ifbarging=voterConLock.signal();
            // } else {
            // // } else if(!bargingLock.empty()) {
            //     ifbarging=bargingLock.signal();
            //     // cout << "id " << id << " unblocking bargingLock" << endl;
            // } 
            ifbarging = false;
            bargingLock.wait( voterOwnerLock );
            // cout << "barging lock " << id << " is being unblocked" << " with " << ifbarging << endl;
            printer.print(id, Voter::Done);
        } 
        if (!voterConLock.empty()){
            voterConLock.signal();
        } else {
        // } else if(!bargingLock.empty()) {
            bargingLock.signal();
            // cout << "id " << id << " unblocking bargingLock" << endl;
        } 
        // ifbarging=false;
    }
    // cout << "id " << id << " released the lock" << endl;
    voterOwnerLock.release();
}

unsigned int TallyVotes::getGroupSize(){
    int s = prng(1,maxGroupSize);
    NEW_GROUP( s );
    return s;
}
