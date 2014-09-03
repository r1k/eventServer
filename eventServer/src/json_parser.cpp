#include "string.h"
#include "json_spirit_writer_template.h"
#include "json_parser.h"

using namespace std;


int json_parser::to_buffer(const CBitrateEvent &bre)
{
	json_spirit::Object message;
	message.push_back( json_spirit::Pair("type", "bitrate_event"));
	message.push_back( json_spirit::Pair("pid", (unsigned short)bre.Pid));
	message.push_back( json_spirit::Pair("bitrate", (unsigned long long)bre.Bitrate));

	this->s = json_spirit::write_string(json_spirit::Value(message), true);

	return this->s.length();
}

int json_parser::to_buffer(const CPidEvent &pe)
{
    json_spirit::Object message;
    message.push_back( json_spirit::Pair("type", "pid_gone"));
    message.push_back( json_spirit::Pair("pid", (unsigned short)pe.Pid));

    this->s = json_spirit::write_string(json_spirit::Value(message), true);

    return this->s.length();
}

int json_parser::from_buffer(const void *const buf, const int buf_length, client_requests_if *req)
{
	// interpret and return an object of the appropriate type
	return 0;
}



