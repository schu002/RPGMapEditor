#ifndef ctokenizer_h
#define ctokenizer_h 1

#include <string>
using namespace std;

class CTokenizer
{
public:
	CTokenizer(const string &s);

	// Advance to next token, delimited by s:
	string    operator() (const char* s);
	string    operator()() { return operator()(" \t\n"); }

private:
	string	mString;
	const char *mPlace;

};

#endif
