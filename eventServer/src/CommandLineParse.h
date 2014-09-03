#pragma once

#include <cstdint>
#include <string>
using namespace std;

class CommandLineParse
{
public:
	CommandLineParse(const int argc, const char* const argv[]) : arg_num (argc), arg_array(argv){}
	virtual ~CommandLineParse();

	bool GetStringValue	( const string key, string &retvalue );
    bool GetStringValue ( const int argnum, string &retvalue);

	bool GetNumValue	( const string key, int &retvalue );
	bool GetNumValue	( const int argnum, int &retvalue );
    bool GetNumValue    ( const string key, int64_t *retvalue);
    bool GetNumValue    ( const string key, int *retvalue);
    bool GetNumValue    ( const int argnum, int *retvalue);

	bool Exists ( const string key );

	int NumArguments ( void ) const {return arg_num;}

private:
	int arg_num;
	const char *const *arg_array;
};
