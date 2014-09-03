#ifndef JSON_PARSER_H_
#define JSON_PARSER_H_

#include "ThemeliosConfig.h"

#include <string.h>

#include "mpeg/events/CTransportStreamEvent.h"
#include "mpeg/events/CBitrateEvent.h"
#include "mpeg/events/CPidEvent.h"
#include "messages.h"

using namespace std;
using namespace themelios;

class json_parser
{
public:
	json_parser() : s(){}
	virtual ~json_parser(){}

	int to_buffer(const CBitrateEvent &bre);
	int to_buffer(const CPidEvent &pe);

	int from_buffer(const void *const buf, const int buf_length, client_requests_if *req);

	const char *buf() { return this->s.c_str(); }
	int buf_length() { return this->s.length(); }

private:
	json_parser(const json_parser&);
	json_parser& operator=(const json_parser&);

	string s;
};

#endif /* JSON_PARSER_H_ */
