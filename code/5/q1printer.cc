#include "q1printer.h"

void printHeader(unsigned int voters){
    cout << "TG" << "\t";
    for(unsigned int i = 0; i < voters; i++){
        cout << "V" << i;
        if (i==voters-1) cout << endl;
        else cout << "\t";
    }

    for(unsigned int i = 0; i < voters+1; i++){
        cout << "*******";
        if (i==voters) cout << endl;
        else cout << "\t";
    }
}

// printerNotPrintedBuffer is a helper function that helps to print out the buffer
// that hasn't been printed yet
void Printer::printNotPrintedBuffer(){
#ifdef NOOUTPUT
#else
    if (!nextGroupInfo.ifPrinted){
        cout << "N " << nextGroupInfo.groupSize;
        nextGroupInfo.ifPrinted = true;
    }
    int last = -1;
    for(int i = 0; i < voters; i++){
        if(!votersBuffer[i].ifPrinted){
            for(int j = last; j < i; j++){
                cout << "\t";
            }
            char curState = votersBuffer[i].state;
            switch(curState){
                case Voter::Vote:
                    cout << curState << " " << votersBuffer[i].curBallot.picture << "," 
                    << votersBuffer[i].curBallot.statue << ","
                    << votersBuffer[i].curBallot.giftshop;
                    break;
                case Voter::Block:
                case Voter::Unblock:
                    cout << curState << " " << votersBuffer[i].blockNum;
                    break;
                case Voter::Barging:
                    cout << curState << " " << votersBuffer[i].blockNum << " " << votersBuffer[i].groupno;
                    break;
                case Voter::Complete:
                    cout << curState << " " << votersBuffer[i].tour.tourkind;
                    break;
                case Voter::Going:
                    cout << curState << " " << votersBuffer[i].tour.tourkind << " " << votersBuffer[i].tour.groupno;
                    break;
                case Voter::Done:
                case Voter::Start:
                case Voter::Failed:
                case Voter::Terminated:
                    cout << curState;
                    break;
            }
            votersBuffer[i].ifPrinted = true;
            last = i;
        }
    }
    cout << endl;
#endif
}

Printer::Printer( unsigned int voters): voters{voters} {
#ifdef NOOUTPUT
#else
    votersBuffer = new VoterInfo[voters];
    for(unsigned int i = 0; i < voters; i++){
        votersBuffer[i] =  VoterInfo{i, Voter::Terminated, true};
    }
    printHeader(voters);
#endif
}

void Printer::print( Voter::States state, unsigned int nextGroupSize ){
#ifdef NOOUTPUT
#else
    if (!nextGroupInfo.ifPrinted){
        printNotPrintedBuffer();
    }
    nextGroupInfo.ifPrinted = false;
    nextGroupInfo.groupSize = nextGroupSize;
#endif
}

void Printer::print( unsigned int id, Voter::States state ) {
#ifdef NOOUTPUT
#else
    if (!votersBuffer[id].ifPrinted){
        printNotPrintedBuffer();
    }
    votersBuffer[id].ifPrinted = false;
    votersBuffer[id].state = state;
#endif
}

void Printer::print( unsigned int id, Voter::States state, TallyVotes::Ballot vote ) {
#ifdef NOOUTPUT
#else
    if (!votersBuffer[id].ifPrinted){
        printNotPrintedBuffer();
    }
    votersBuffer[id].ifPrinted = false;
    votersBuffer[id].curBallot = vote;
    votersBuffer[id].state = state;
#endif
}

void Printer::print( unsigned int id, Voter::States state, unsigned int numBlocked ) {
#ifdef NOOUTPUT
#else
    if (!votersBuffer[id].ifPrinted){
        printNotPrintedBuffer();
    }
    votersBuffer[id].ifPrinted = false;
    votersBuffer[id].blockNum = numBlocked;
    votersBuffer[id].state = state;
#endif
}

void Printer::print( unsigned int id, Voter::States state, unsigned int numBlocked, unsigned int group ){
#ifdef NOOUTPUT
#else
    if (!votersBuffer[id].ifPrinted){
        printNotPrintedBuffer();
    }
    votersBuffer[id].ifPrinted = false;
    votersBuffer[id].blockNum = numBlocked;
    votersBuffer[id].groupno = group;
    votersBuffer[id].state = state;
#endif
}

void Printer::print( unsigned int id, Voter::States state, TallyVotes::Tour tour ) {
#ifdef NOOUTPUT
#else
    if (!votersBuffer[id].ifPrinted){
        printNotPrintedBuffer();
    }
    votersBuffer[id].ifPrinted = false;
    votersBuffer[id].tour = tour;
    votersBuffer[id].state = state;
#endif
}

Printer::~Printer() {
#ifdef NOOUTPUT
#else
    printNotPrintedBuffer();
    cout << "*****************" << endl;
    cout << "All tours ended" << endl;
    delete []votersBuffer;
#endif
}
