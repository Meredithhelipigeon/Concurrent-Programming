#include "q1tallyVotes.h"
#include "q1printer.h"

TallyVotes::TallyVotes( unsigned int voters, unsigned int group, Printer & printer ):
maxGroupSize{group}, printer{printer} , votersNum{voters} {
    currentGroupSize = getGroupSize();
    printer.print(Voter::States::NextTourSize, currentGroupSize);
}

TallyVotes::Tour TallyVotes::vote(unsigned int id, Ballot ballot){
    // voter votes by passing the voterBallot and id to the administrative
    TallyVotes::voterBallot = ballot;
    TallyVotes::voterId = id;

    group.wait(id);

    // quorem failure; throw Failed in this case
    if (votersNum<maxGroupSize) _Throw Failed();
    Tour retTour = curTour;

    return retTour;
}

void TallyVotes::done( unsigned int id ){
    // pass doneId to administrative
    TallyVotes::doneId = id;
    votersNum -= 1;
}

unsigned int TallyVotes::getGroupSize(){
    int newGroupSize = prng(1,maxGroupSize);
    return newGroupSize;
}

void TallyVotes::main(){
    for(;;) {
        _Accept( ~TallyVotes ){
            break;
        } or _Accept( done ){
            printer.print(doneId, Voter::Done);
            // wake everyone up if there is a quorem failure in done
            if (votersNum<maxGroupSize) {
                while (!group.empty()){
                    waitingNum -= 1;
                    printer.print(group.front(), Voter::Unblock, waitingNum);
                    group.signalBlock();
                }
            }
        } or _Accept( vote ){
            // wake this task up immediately if there is a quorem failure in vote
            if (votersNum<maxGroupSize) {
                // since we blocked everyone in task when calling wait()
                waitingNum += 1;
                printer.print(voterId, Voter::Block, waitingNum);
                while (!group.empty()){
                    waitingNum -= 1;
                    printer.print(group.front(), Voter::Unblock, waitingNum);
                    group.signalBlock();
                }
                group.signalBlock();
                continue;
            }

            curBallot.picture += voterBallot.picture;
            curBallot.statue += voterBallot.statue;
            curBallot.giftshop += voterBallot.giftshop;
            formedGroupSize += 1;
            printer.print(voterId, Voter::Vote, voterBallot);

            // since we blocked everyone in task when calling wait()
            waitingNum += 1;
            printer.print(voterId, Voter::Block, waitingNum);

            if (formedGroupSize==currentGroupSize){
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
                printer.print(voterId, Voter::Complete, curTour);
                // set new group
                formedGroupSize = 0;
                currentGroupSize = getGroupSize();
                printer.print(Voter::States::NextTourSize, currentGroupSize);
                // reset ballot
                curBallot = (Ballot) {0,0,0};
                // wake the formed group people up
                while( !group.empty() ) {
                    waitingNum -= 1;
                    printer.print(group.front(), Voter::Unblock, waitingNum);
                    group.signalBlock();
                }
            }
        } 
    }
}
