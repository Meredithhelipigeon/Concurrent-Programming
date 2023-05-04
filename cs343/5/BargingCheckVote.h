#include <iostream>

// Preprocessor variables declared in a program can have no value (empty), but variables declared with compiler flag -D
// have a value of one, unless explicitly set. So the following trick concatenates a 0 at the end of the variable, so if
// the variable has no value it becomes 0. Otherwise, the value is increased by 10. For example, if the variable is 1,
// it becomes 10 => do the abort; any other value means do not do the abort.

#define DO_EXPAND( VAL ) VAL ## 0
#define EXPAND( VAL ) DO_EXPAND( VAL )

#ifdef BARGINGCHECK
	#define BCHECK( stmt ) stmt
	#if EXPAND(BARGINGCHECK) != 10
		#define __BCHECK_PRT__( msg ) abort( msg )
	#else
		#define __BCHECK_PRT__( msg ) std::osacquire( std::cerr ) << msg << std::endl
	#endif
#else
	#define BCHECK( stmt )
#endif // BARGINGCHECK

#define __BCHECK_DECL__ \
unsigned int __exit_call_count__ = 0, __callCnt__ = 0, __next_group__ = 0, __group_end__ = 0; \
bool shouldUpdate = false; \
\
void __update__( unsigned int group ) { \
	        __next_group__ = group; \
		if ( shouldUpdate ) { \
			__group_end__ += group; \
			shouldUpdate = false; \
		} else shouldUpdate = true; \
		if ( __group_end__ == 0 ) __group_end__ = group; \
} \
unsigned int __prodEnter__() { \
	return __callCnt__++; \
} \
void __prodleave__( unsigned int ticket ) { \
	__exit_call_count__++; \
	if ( ticket > __group_end__ ) { __BCHECK_PRT__( "**** BARGING ERROR **** voter over voter" ); } \
	if ( __exit_call_count__ == __group_end__) { \
		if ( shouldUpdate ) { \
                	__group_end__ += __next_group__; \
			shouldUpdate = false; \
        	} else shouldUpdate = true; \
	} \
}

#define BCHECK_DECL \
	BCHECK( __BCHECK_DECL__ )
#define VOTER_ENTER \
	BCHECK( unsigned int __voter_ticket__ = __prodEnter__( ); )
#define VOTER_LEAVE \
	BCHECK( __prodleave__( __voter_ticket__ ); ) 
#define NEW_GROUP( group ) \
	BCHECK( __update__( group ); )
