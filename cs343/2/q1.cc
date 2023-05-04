#include <string>
#include <iostream>
#include <fstream>
#include<set>
#include <vector>

using namespace std;

_Coroutine Filter {
  protected:
	_Event Eof {};					// no more characters
	Filter * next;					// next filter in chain
	unsigned char ch;				// communication variable
	enum { ARTBITRARY_CHAR = '\001' };
  public:
	Filter( Filter * next ) : next( next ) {}
	Filter() : next( NULL ) {}
	void put( unsigned char c ) {
		ch = c;
		resume();
	}
};

_Coroutine Reader : public Filter {
	// private members
	istream *  input;
	void main(){
		for(;;){
			ch = input->get();
			if (input->fail()) {
				_Resume Eof() _At *next;
				next->put(ARTBITRARY_CHAR);
				break;
			}
			next->put(ch);
		}
	}
  public:
	Reader( Filter * f, istream & i ) : input( &i ) {
		this->next = f;
		resume(); // start the main program
	}
	~Reader(){
		if (next!=NULL) delete next;
	}
};

_Coroutine Writer : public Filter {
	// private members
	ostream * output;
	int totalNum;

	void main(){
		try {
			_Enable {
				for(;;){
					* output << ch;
					totalNum += 1;
					suspend(); 
				}
			}
		} _Catch( Eof&) {
			* output << totalNum << " " << "characters" << endl;
        }
	}
  public:
	Writer( ostream & o ) : output ( &o ), totalNum ( 0 ) {};
	~Writer(){
		if (next!=NULL) delete next;
	}
};

_Coroutine CaseOperation : public Filter {
	bool ifUpper;
	void main(){
		try {
            _Enable { 
                for(;;){
					if (('a' <= ch) && (ch <= 'z') && ifUpper){
						ch += 'A' - 'a';
					} else if (('A' <= ch) && (ch <= 'Z') && (!ifUpper)){
						ch += 'a' - 'A';
					}
					next->put(ch);
					suspend();
				}
            } // enable
        } _Catch( Eof& E) {
            _Resume E _At *next;
			next->put(ARTBITRARY_CHAR);
        }
	}
  public:
	CaseOperation(Filter * next, bool ifUpper ) : ifUpper( ifUpper ) {
		this->next = next;
	}
	~CaseOperation(){
		if (next!=NULL) delete next;
	}
};

_Coroutine ClearWhiteSpace : public Filter {
	char lastCh = ' ';
	bool ifEmpty = true;
	void main(){
		try {
            _Enable { 
                for(;;){
					if (ch==' '){
						lastCh = ch;
					} else if (ch=='\n'){
						if (!ifEmpty){
							next->put(ch);
						}
						ifEmpty = true;
						lastCh = ' ';
					} else {
						if ((lastCh==' ') && (!ifEmpty)){
							next->put(lastCh);
						}
						ifEmpty = false;
						lastCh = ch;
						next->put(ch);
					}
					suspend();
				}
            } // enable
        } _Catch( Eof& E) {
            _Resume E _At *next;
			next->put(ARTBITRARY_CHAR);
        }
	}
  public:
	ClearWhiteSpace(Filter * next ) {
		this->next = next;
	}
	~ClearWhiteSpace(){
		if (next!=NULL) delete next;
	}
};

_Coroutine Heighlight : public Filter {
	set<string> words;
	bool ifBold;

	string curWord = "";
	string boldStart = "\E[1m";
	string underscoreStart = "\E[4m";
	string end = "\E[m";
	
	void main(){
		try {
            _Enable { 
                for(;;){
					if ((('a' <= ch) && (ch <= 'z'))||
						(('A' <= ch) && (ch <= 'Z'))) {
							curWord += ch;
					} else {
						if (curWord != ""){
							if (words.find(curWord)!=words.end()){
								for(auto c: boldStart){
									next->put(c);
								}	
							}
							
							for(auto c: curWord){
								next->put(c);
							}
							if (words.find(curWord)!=words.end()){
								for(auto c: end){
									next->put(c);
								}
							}
						}
						curWord = "";
						next->put(ch);
					}
					suspend();
				}
            } // enable
        } _Catch( Eof& E) {
			if (curWord != ""){
				if (words.find(curWord)!=words.end()){
					string start;
					if (ifBold) start = boldStart;
					else start = underscoreStart;
					for(auto c: start){
						next->put(c);
					}
				}
				for(auto c: curWord){
					next->put(c);
				}
				if (words.find(curWord)!=words.end()){
					for(auto c: end){
						next->put(c);
					}
				}
			}
            _Resume E _At *next;
			next->put(ARTBITRARY_CHAR);
        }
	}
  public:
	Heighlight(Filter * next, set<string> words, bool ifBold ) : words( words ), ifBold( ifBold ) {
		this->next = next;
	}
	~Heighlight(){
		if (next!=NULL) delete next;
	}
};

int main( int argc, char * argv[] ) {
	istream * infile = &cin;							
	ostream * outfile = &cout;	
	int num = 0;
	for(int i = 0; i < argc; i++){
		if ((strcmp(argv[i],"-c")==0)||(strcmp(argv[i],"-w")==0)
		    || (strcmp(argv[i],"-H")==0)){
				num += 1;
		}
	}

	int positions[num];
	int curNum = 0;
	for(int i = 0; i < argc; i++){
		if ((strcmp(argv[i],"-c")==0)||(strcmp(argv[i],"-w")==0)
		    || (strcmp(argv[i],"-H")==0)){
				positions[curNum]=i;
				curNum += 1;
			}
	}

	Filter * next = NULL;
	try {
		// if there are no piplines
		if (num==0){
			int infilePos = 1;
			try {										
					infile = new ifstream( argv[infilePos] );
				} catch( uFile::Failure & ) {
					cerr << "Error! Could not open input file \"" << argv[infilePos] << "\"" << endl;
					throw 1;
				}
			if (infilePos + 1 < argc){
				int outfilePos = infilePos + 1;
				try {
						outfile = new ofstream( argv[outfilePos] );
					} catch( uFile::Failure & ) {			// open failed ?
						cerr << "Error! Could not open output file \"" << argv[outfilePos] << "\"" << endl;
						throw 1;
					}
			}
		}
		for(int curPos = num-1; curPos >= 0; curPos--){
			Filter * cur = NULL;
			int i = positions[curPos];
			if (curPos==num-1){
				int infilePos = i+1;
				if (strcmp(argv[i], "-c")==0) infilePos+=1;
				else if (strcmp(argv[i], "-H")==0){
					while (infilePos < argc) {
						if (strcmp(argv[infilePos],"#")==0){
							infilePos += 1;
							break;
						}
						infilePos += 1;
					}
				}
				try {										
						infile = new ifstream( argv[infilePos] );
					} catch( uFile::Failure & ) {
						cerr << "Error! Could not open input file \"" << argv[infilePos] << "\"" << endl;
						throw 1;
					}
				if (infilePos + 1 < argc){
					int outfilePos = infilePos + 1;
					try {
							outfile = new ofstream( argv[outfilePos] );
						} catch( uFile::Failure & ) {			// open failed ?
							cerr << "Error! Could not open output file \"" << argv[outfilePos] << "\"" << endl;
							throw 1;
						}
				}
				next = new Writer(*outfile);
			}
			if (strcmp(argv[i], "-c")==0){
				char * caseSign = argv[i+1];
				bool ifUpper = true;
				if (strcmp(caseSign,"l")==0) ifUpper = false;
				cur = new CaseOperation(next, ifUpper);
			} else if (strcmp(argv[i], "-w")==0){
				cur = new ClearWhiteSpace(next);
			} else {
				char * heightLightTpe = argv[i+1];
				bool ifBold = true;
				if (strcmp(heightLightTpe,"underline")==0) ifBold = false;
				set<string> words;
				int wordStart = i+2;
				while (wordStart < argc) {
					if (strcmp(argv[wordStart],"#")==0){
						break;
					}
					words.insert(argv[wordStart]);
					wordStart += 1;
				}
				cur = new Heighlight(next, words, ifBold);
			}
			next = cur;
		}						
	} catch( ... ) {
		cerr << "Usage: " << argv[0]
			<< " [ -filter-options ... ] [ infile [outfile] ]" << endl;
		exit( EXIT_FAILURE );// TERMINATE
	}

	Reader * reader = new Reader(next, *infile);

	if ( infile != &cin ) delete infile;
	if ( outfile != &cout ) delete outfile;
	delete reader;
} // main
