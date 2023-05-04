#include "q2printer.h"
#include <iostream>
#include <ostream>
#include <string>
using namespace std;

unsigned int Printer::noOfCards = -1;

Printer::Printer( const unsigned int NoOfPlayers, const unsigned int NoOfCards ){
    if ((int)noOfCards!=-1){
        cout << endl << endl;
    }
    this->noOfCards = NoOfCards;
    buffer = new printerInfo[NoOfPlayers];
    this->noOfPlayers = NoOfPlayers;
    for(unsigned int i = 0; i < NoOfPlayers; i++){
        buffer[i]= printerInfo{0, NoOfCards, true, false, false, false};
    }
    printHeader(NoOfPlayers, NoOfCards);
}

void Printer::prt( unsigned int id, int took, unsigned int RemainingPlayers ){
    if (buffer[id].ifPrint==false){
        printNotPrintedBuffer();
    }
    buffer[id].took = took;
    noOfCards -= took;
    buffer[id].remaining = noOfCards;
    buffer[id].ifPrint = false;
    
    // The game will terminate in such case
    if ((RemainingPlayers==0) || (buffer[id].remaining==0)){
        buffer[id].ifDead = true;
        if (RemainingPlayers==0) buffer[id].ifLastPlyer = true;
        printNotPrintedBuffer();
        return;
    }
}

void Printer::prt( unsigned int id) {
    if (buffer[id].ifPrint==false){
        printNotPrintedBuffer();
    }
    buffer[id].ifDrunk = true;
    buffer[id].ifPrint = false;
}

void Printer::printHeader(const unsigned int NoOfPlayers, const unsigned int NoOfCards){
    cout << "Players: " << NoOfPlayers << "\t" << "Cards: " << NoOfCards << "\n";
    for(unsigned int i = 0; i < NoOfPlayers; i++){
        cout << "P" << i;
        if (i==NoOfPlayers-1) cout << endl;
        else cout << "\t";
    }
}

void Printer::printNotPrintedBuffer(){
    int i = 0;
    int last = 0;
    for(auto info = buffer; info != buffer+noOfPlayers; info++){
        if (!info->ifPrint) {
            for(int j = last; j<i; j++){
                cout << "\t";
            }
            if (info->ifDead) {
                if (info->ifLastPlyer){
                    cout << "#" << noOfCards << "#";
                    if ((info->remaining+info->took)%7==0) cout << "X";
                } else if (info->remaining==0){
                    cout << info->took << ":" << info->remaining << "#";
                    if ((info->remaining+info->took)%7==0) cout << "X";
                }
            } else if (info->ifDrunk) {
                cout << "D";
                info->ifDrunk = false;
            } else {
                string sign = ">";
                if (info->remaining%2!=0) sign = "<";
                cout << info->took << ":" << info->remaining << sign;
                if ((info->remaining+info->took)%7==0) cout << "X";
            }
            info->ifPrint = true;
            last = i;
        } 
        i += 1;
    }
    cout << endl;
}
