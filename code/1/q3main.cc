#include "q3IntegerLiteral.h"
#include <fstream>
#include <iostream>
using namespace std;

int main(int argc, char** argv){
    istream *readFile = NULL;
    if (argc==2){
        readFile = new ifstream(argv[1]);
    } else if (argc==1) {
        readFile = &cin;
    } else {
        cerr << "Shell Interface: " <<  argv[0] << " [infile-file]" << endl;
        exit( EXIT_FAILURE );
    }
    string line;

    while (getline(*readFile, line)) {
        char ch;
        int size = line.length();
        int i = 0; // cur process position
        string curString = "";
        if (line=="") {
            cout << "Warning! Blank line." << endl;
            continue;
        }
        try {
            _Enable { 
                IntegerLiteral integerLiteral;
                for(; i < size; i++){
                    ch = line[i];
                    curString += ch;
                    integerLiteral.next(ch);
                }  
                integerLiteral.next('\003');
            } // enable
        } _Catch( IntegerLiteral::Match& M) {
            cout << "'" << curString << "' " << "yes, value " << M.value << endl;
        } _Catch( IntegerLiteral::Error&){
            cout <<  "'" << curString << "'" << "no";
            if (i < size - 1) {
                cout << " - extraneous characters '";
                for(int j = i+1; j < size; j++){
                    cout << line[j];
                }
                cout << "'";
            }
            cout << endl;
        } // try
    }
    if (readFile != &cin) delete readFile;
}
