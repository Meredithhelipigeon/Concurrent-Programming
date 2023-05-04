#include "q3producer.h"
#include "q3consumer.h"
#include <iostream>
using namespace std;

static intmax_t convert( const char * str ) {			// convert C string to integer
	char * endptr;
	errno = 0;											// reset
	intmax_t val = strtoll( str, &endptr, 10 );			// attempt conversion
	if ( errno == ERANGE ) throw std::out_of_range("");
	if ( endptr == str ||								// conversion failed, no characters generated
		 *endptr != '\0' ) throw std::invalid_argument(""); // not at end of str ?
	return val;
} // convert

int main( int argc, char * argv[] ){
    // set seed for gobal prng
    set_seed(1000);
    int cons = 5;
    int prods = 3;
    int produce = 10;
    unsigned int buffersize = 10;
    int delays = -1;
    int processors = 1;

    try { // process command-line arguments
    switch ( argc ) { 
        case 7:
            if ( strcmp( argv[6], "d" ) != 0 ) {processors = convert( argv[6] ); if ( processors <= 0 ) throw 1;}
        case 6:
            if ( strcmp( argv[5], "d" ) != 0 ) {delays = convert( argv[5] ); if ( delays <= 0 ) throw 1;}
        case 5: 
            if ( strcmp( argv[4], "d" ) != 0 ) {buffersize = convert( argv[4] ); if ( buffersize <= 0 ) throw 1;}
        case 4: 
            if ( strcmp( argv[3], "d" ) != 0 ) {produce = convert( argv[3] ); if ( produce <= 0 ) throw 1;}
        case 3: 
            if ( strcmp( argv[2], "d" ) != 0 ) {prods = convert( argv[2] ); if ( prods <= 0 ) throw 1;}
        case 2: 
            if ( strcmp( argv[1], "d" ) != 0 ) {cons = convert( argv[1] ); if ( cons <= 0 ) throw 1; }}
    } catch( ... ) {
        cout << "Usage:" << argv[0] 
        << "[cons(>0)[prods(>0)[produce(>0)[buffersize(>0)[delays(>0)[processors(>0)]]]]]]" << endl; 
        exit( 1 );
    } // try
    
    // if delays is not spefified, delays=cons+prods
    if (delays == -1) delays = cons+prods;

    // multi-processor
    uProcessor p[processors-1] __attribute__(( unused ));

    BoundedBuffer<int> boundedbuffer(buffersize);
    
    // create producer tasks
    Producer *producers[prods];
    for( int i = 0; i < prods; i++){
        producers[i]= new Producer(boundedbuffer, produce, delays);
    }

    // create consumer tasks
    int counterSum[cons] = {0};
    Consumer *consumers[cons];
    for( int i = 0; i < cons; i++){
        consumers[i]= new Consumer(boundedbuffer, delays, counterSum[i]);
    }

    // use magic delete to wait for producer to terminate
    for( int i = 0; i < prods; i++){
        delete producers[i];
    }

    // wait for consumers finish consuming all the elements that producers produce
    while (boundedbuffer.blocks()!=0) {}
    boundedbuffer.poison();

    // throw Poison to all the consumers
    for( int i = 0; i < cons; i++){
        _Resume BoundedBuffer<int>::Poison() _At *consumers[i];
        delete consumers[i];
    }

    int total = 0;
    for( int i = 0; i < cons; i++){
        total += counterSum[i];
    }

    cout << "total value: " << total << endl;
}