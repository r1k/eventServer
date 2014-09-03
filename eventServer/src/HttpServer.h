#pragma once

#include <iostream>
#include <thread>

#include "_no_copy.h"

#include <boost/asio.hpp>
#include "boost_http/server/server.hpp"
using namespace std;

class HTTPServer : public _no_copy
{
private:
    unique_ptr<http::server::server> server;
    unique_ptr<thread> thrd;
public:
    HTTPServer(const string listening_interface, const int port, const string doc_root)
    {
        try
        {
            stringstream port_s;
            port_s << port;
            const string port_string(port_s.str());
            server = make_unique<http::server::server>(listening_interface, port_string, doc_root);
            thrd = make_unique<thread>(&HTTPServer::run, this);
        }
        catch (std::exception& e)
        {
            std::cerr << "exception: " << e.what() << "\n";
        }
    }

    void run() { server->run(); }

    ~HTTPServer()
    {
        server->stop();
        thrd->join();
    }
};