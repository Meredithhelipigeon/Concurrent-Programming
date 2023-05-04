#include "q2player.h"
#include <iostream>
#include <ostream>
using namespace std;

unsigned int Player::numOfPlayers;
unsigned int Player::numOfDrunkPeople;
int Player::startId = -1;

Player::Player( PRNG & prng, Printer &printer, unsigned int id ): prng (&prng) ,printer (&printer), id (id) {}

void Player::players( unsigned int num ){
    Player::numOfPlayers = num;
    Player::numOfDrunkPeople = 0;
    startId = -1;
}

void Player::start( Player &lp, Player &rp){
    this->lplayer = &lp;
    this->rplayer = &rp;
}

void Player::main() {
    try {
        _Enable{
            for(;;){
                if ((Player::numOfPlayers==1) && (!ifTerminated)){
                    Player::numOfPlayers -= 1;
                    _Resume Stop() _At *rplayer;
                    printer->prt(id, 0, numOfPlayers);
                    rplayer->play(0);
                    break;
                }

                int remainingNum = deck;
                if (!ifTerminated){
                    PRNG & prngFunc = *prng;

                    int chosenNum = min((int)prngFunc(1,MAXIMUM_CHOSEN_CARDS_NUM), remainingNum);
                    remainingNum -= chosenNum;
                    // death check
                    if (deck%DEATH_DECK_DIVISOR==0){
                        ifTerminated = true;
                        Player::numOfPlayers -= 1;
                    }
                    printer->prt(id, chosenNum, numOfPlayers);
                }

                if ((remainingNum <= 0) || (Player::numOfPlayers==0)){
                    _Resume Stop() _At *rplayer;
                    rplayer->play(remainingNum);
                    break;
                }

                PRNG & prngFunc = *prng;
                if ((!ifTerminated) && (prngFunc(10)==0)) {
                    numOfDrunkPeople += 1;
                    _Resume Drink() _At *rplayer;
                    printer->prt(id);
                    rplayer->drink();
                    numOfDrunkPeople -= 1;
                }

                if (remainingNum%2==0){
                    rplayer->play(remainingNum);
                } else {
                    lplayer->play(remainingNum);
                }
            }
        }
    } _CatchResume(Drink&) {
        if (numOfDrunkPeople<numOfPlayers){
            if (!ifTerminated) { 
                numOfDrunkPeople += 1;
                printer->prt(id);
            }
            _Resume Drink() _At *rplayer;
            rplayer->drink();
            if (!ifTerminated) { numOfDrunkPeople -= 1;}
        }
        suspend();
    }_Catch(Stop&){
        if (id!=startId){
            _Resume Stop() _At *rplayer;
            rplayer->play(0);
        }
    }
}

void Player::play( unsigned int deck ){
    if (!ifTerminated){
        if (startId==-1){
            startId = id;
        }
        this->deck = deck;
        resume();
    } else {
        this->deck = deck;
        resume();
    }
}

void Player::drink(){
    resume();
}
