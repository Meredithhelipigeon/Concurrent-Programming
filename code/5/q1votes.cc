#include "q1votes.h"
#include "q1printer.h"

void Voter::main(){
		yield(prng(20));
		try{
			_Enable {
				for(int i = 0; i < nvotes; i++){
					printer.print(id, Start);
					yield(prng(5));
					// vote
					TallyVotes::Tour retTour = voteTallier.vote(id, cast());
					yield(prng(5));
                    printer.print(id, Going, retTour);
				}
			}
		} catch (TallyVotes::Failed){
			printer.print(id, Failed);
		}
		voteTallier.done(
			#if defined( TASK )			// task solution
		    id
		    #endif
		);
		printer.print(id, Terminated);
		return;
}