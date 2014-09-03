#ifndef MESSAGES_H_
#define MESSAGES_H_

class client_requests_if
{
public:
	typedef enum {REFRESH_ALL, GET_PIDS} request_t;

	virtual ~client_requests_if() = 0;

	request_t get_type() { return type; }
private:

	request_t type;
};

class refresh_all_request : public client_requests_if
{

};

#endif /* MESSAGES_H_ */
