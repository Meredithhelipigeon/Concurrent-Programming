#include <string>
using namespace std;

_Coroutine IntegerLiteral {
	typedef enum { DECIMAL, OCTAL, HEXA, UNDEFINED } LiteralType ;
	char ch;											// character passed by cocaller	string integerString;
	LiteralType literalType;
	string integerString;
	void throwMatch(unsigned long int value);
	void throwError();

	void main(); // coroutine main
  public:
	enum { EOT = '\003' };								// end of text

	_Event Match {										// last character match
	  public:
	  unsigned long int value;							// value of integer literal
	  Match( unsigned long int value ) : value( value ) {}
	};
	_Event Error {
		public:
		Error( ) {}
	};									// last character invalid

	void next( char c ) {
		ch = c;											// communication input
		resume();										// activate
	} // IntegerLiteral::next

	IntegerLiteral() {
		this->literalType = UNDEFINED;
		this->integerString = "";
    }
}; // IntegerLiteral
