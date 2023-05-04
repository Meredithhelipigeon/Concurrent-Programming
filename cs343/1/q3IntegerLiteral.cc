#include "q3IntegerLiteral.h"
#include <iostream>
using namespace std;

void IntegerLiteral::throwMatch(unsigned long int value){
    _Resume Match(value) _At resumer();
    suspend();
}

void IntegerLiteral::throwError(){
    _Resume Error() _At resumer();
    suspend();
}

// chech whether ch is digit
bool checkDigit(char ch){
    return ('0' <= ch) && (ch <= '9'); 
}

// chech whether ch is octal digit
bool checkOctalDigit(char ch){
    return ('0' <= ch) && (ch <= '7');
}

// chech whether ch is hexa digit
bool checkHexaDigit(char ch){
    return (('0' <= ch) && (ch <= '9')) 
           || (('a' <= ch) && (ch <= 'f'))
           || (('A' <= ch) && (ch <= 'F'));
}

// chech whether ch is unsigned digit
bool checkUnsigned(char ch) {
    return (ch == 'u') || (ch == 'U');
}

// chech whether ch is signed digit
bool checkSigned(char ch) {
    return (ch == 'l') || (ch == 'L');
}

void IntegerLiteral::main(){
    for(;;){
        // process the prefix for the literal
        if (ch == '0') { // octal-literal or hexadecimal
            integerString += ch;
            suspend();
            if ((ch == 'x') || (ch == 'X')){ // hexa
                literalType = HEXA;
                integerString += ch;
                suspend();
                if (checkHexaDigit(ch)){
                    integerString += ch;
                    suspend();
                } else throwError();
            } else { // octal
                literalType = OCTAL;
            }
        } else { // decimal
            literalType = DECIMAL;
            if (checkDigit(ch)) {
                integerString += ch;
                suspend();
            } else throwError();
        }
        
        // check the body of the interal
        int freq = 1;
        for(;;){
            if (((literalType==DECIMAL) && checkDigit(ch) && (freq <= 20))
               || ((literalType==OCTAL) && checkOctalDigit(ch) && (freq <= 22))
               || ((literalType==HEXA) && checkHexaDigit(ch) && (freq <= 16))){
                integerString += ch;
                freq += 1;
                suspend();
            } else break;
        }

        // check the optional integer suffix 
        if (checkUnsigned(ch)){
            integerString += ch;
            suspend();
            if (checkSigned(ch)){
                integerString += ch;
                suspend();
            }
        } else if (checkSigned(ch)){
            integerString += ch;
            suspend();
            if (checkUnsigned(ch)){
                integerString += ch;
                suspend();
            }
        }
        
        if (ch == EOT) {
            switch (literalType)
            {
            case DECIMAL:
                throwMatch(stoi(integerString));
                break;
            case HEXA:
                throwMatch(stoi(integerString,NULL,16));
            case OCTAL:
                throwMatch(stoi(integerString,NULL,8));
            default:
                break;
            }
        } else {
            throwError();
        }
    }
}
