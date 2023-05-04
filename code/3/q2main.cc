#include "q2mergesort.h"
#include <iostream>
#include <fstream>
#include <math.h>
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

typedef TYPE T;
template<typename T>
void printArray(ostream *outfile, T arr[], unsigned int size ){
    if (size==0){
        *outfile << endl;
        return;
    }
    const int rowMaxLen = 22;
    for(unsigned int i = 0; i < size; i++){
        *outfile << arr[i];
        if (i==size-1){
            *outfile << endl;
            break;
        }
        if (i%rowMaxLen==rowMaxLen-1){
            *outfile << endl;
            *outfile << "  ";
        } else {
            *outfile << " ";
        }
    }
}

int main(int argc, char * argv[]) {
    // default value
    int size = -1;
    int depth = 0;
    istream * infile = &cin;
	ostream * outfile = &cout;

    try{
        // time mode
        if ((argc>1) && (strcmp(argv[1],"-t")==0)) {
            switch (argc){
                case 4: depth = convert( argv[3] );
                case 3: size = convert( argv[2] );
            }
            if ( depth < 0 ) throw 1;
            if ( size < 0 ) throw 1;

            int *values = new int[size];
            for (int i = 0; i < size; i++){
                values[i] = i;
            }
            if (size>0){
                unsigned int times = sqrt( size );
                for ( unsigned int counter = 0; counter < times; counter += 1 ) {
                    swap( values[0], values[rand() % size] ); }
            }
            int processors = pow(2,depth);
            uProcessor p[processors-1] __attribute__(( unused ));
            uTime start = uClock::currTime();
            mergesort(values, size, depth);
            cout << "Sort time " << uClock::currTime() - start << " sec." << endl;
            delete values;
        }
        // sort mode
        else {
            switch ( argc ) {
                case 4:
                    if ( strcmp( argv[3], "d" ) != 0 ) {
                        depth = convert(argv[3]);
                        if (depth<0) throw 1;
                    }
                case 3:
                    if ( strcmp( argv[2], "d" ) != 0 ) {
                        try {
                            outfile = new ofstream( argv[2] );
                        } catch( uFile::Failure & ) {			// open failed ?
                            cerr << "Error! Could not open output file \"" << argv[4] << "\"" << endl;
                            throw 1;
                        }
                    }
                case 2:
                    if ( strcmp( argv[1], "d" ) != 0 ) {	
                        try {										// open input file first as output creates file
                            infile = new ifstream( argv[1] );
                        } catch( uFile::Failure & ) {				// open failed ?
                            cerr << "Error! Could not open input file \"" << argv[1] << "\"" << endl;
                            throw 1;
                        }
                    } // if
                case 1:										// defaults
                    break;
                default:									// wrong number of options
                    throw 1;
                } // switch
            for ( ;; ) {
                // for each array with fixed size 'size'
                int size;
                *infile >> size;
                if ( infile->fail() ) break;
                if (size<0){
                    cerr << "size must >= 0 for each array."<< endl;
                    throw 1;
                }

                T *values = new T[size];
                for (int i = 0; i < size; i++) *infile >> values[i];
                // print the previous array
                printArray<T>(outfile, values, size);
                if (size > 0) {
                    mergesort<T>(values, size, depth); 
                }
                // print the updated array
                printArray<T>(outfile, values, size);
                *outfile << endl;
                delete values;
            }
            if ( infile != &cin ) delete infile;				// close file, do not delete cin!
	        if ( outfile != &cout ) delete outfile;				// close file, do not delete cout!
        }
    } catch(...){
        cout << "Usage: " << endl 
        << argv[0] << " -t size (>= 0) [ depth (>= 0) ]" << " or " << endl  
        << argv[0] << " [ unsorted-file | ’d’ [ sorted-file | ’d’ [ depth (>= 0) ] ] ]" << endl;
        exit(1);
    }
}