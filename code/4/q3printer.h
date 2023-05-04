#ifndef PRINTER_H
#define PRINTER_H
#include"q3votes.h"
#include "q3tallyVotes.h"
#include <string>
#include <iostream>
#include <ostream>
using namespace std;

struct VoterInfo {
	unsigned int id;
	Voter::States state;
	bool ifPrinted;
	int blockNum;
	int groupno;
	TallyVotes::Ballot curBallot;
	TallyVotes::Tour tour;
};

struct GroupInfo{
	bool ifPrinted;
	int groupSize;
};

_Monitor Printer {
	VoterInfo * votersBuffer;
	GroupInfo nextGroupInfo {true, -1};
	int voters;
	// helper function for the printer
	void printNotPrintedBuffer();

  public:
	Printer( unsigned int voters );
	~Printer();
	void print( Voter::States state, unsigned int nextGroupSize );
	void print( unsigned int id, Voter::States state );
	void print( unsigned int id, Voter::States state, TallyVotes::Tour tour );
	void print( unsigned int id, Voter::States state, TallyVotes::Ballot vote );
	void print( unsigned int id, Voter::States state, unsigned int numBlocked );
	void print( unsigned int id, Voter::States state, unsigned int numBlocked, unsigned int group );
};
#endif