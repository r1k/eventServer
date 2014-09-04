#include <iostream>
#include <string>
#include <thread>

#include "CommandLineParse.h"
#include "ClientWebSocketHandler.h"
#include "EventEngine.h"
#include "HttpServer.h"
#include "CommandInterpreter.h"
#include "ConfigFileReader.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
namespace fs = boost::filesystem;

using namespace std;

string getDocRoot(char *argv0)
{
    fs::path full_path(fs::initial_path<fs::path>());

    full_path = fs::system_complete(fs::path(argv0));

    return full_path.parent_path().string() + "/doc_root/";
}

class cmdQuit : public Command
{
private:
    bool& main_quit;
public:
    cmdQuit(bool& mainLoop) : 
        Command("quit", "quits."),
        main_quit(mainLoop)
    {}

    void Run()
    {
        UI->stop();
        main_quit = true;
    }
    void Run(const command_list_t& args) { Run(); }
};

int main(int argc, char** argv)
{
    CommandLineParse clp(argc, argv);

    bool         finished = false;
    string       multicastIPAddress("0.0.0.0");
    int          multicastPortNumber = 5000;
    string       multicastInterface("0.0.0.0");
    bool         interfaceSet = false;
    int          basePort = 8080;
    bool         okay = true;
    const string doc_root = getDocRoot(argv[0]);
    cmdQuit      quitCommand(finished);

    okay = clp.GetStringValue(1, multicastIPAddress);
    okay = clp.GetNumValue(2, multicastPortNumber);
    okay = clp.GetNumValue(3, basePort);

    const int httpReqPort = basePort;
    const int webSocketPort = basePort + 1;
    
    if (!okay)
    {
        cerr << "Incorrect usage:" << endl;
        cerr << " eventServer <multicast IP Address> <multicast port> <base port> [options]" << endl;
        cerr << "     available options:" << endl;
        cerr << "         -i <interface address>" << endl << endl;
        cerr << " This program will open a port on base port and a further port on base port + 1" << endl;

        return -1;
    }

    interfaceSet = clp.GetStringValue("-i", multicastInterface);

    
    while (!finished)
    {
        try
        {
            HTTPServer httpServer("0.0.0.0", httpReqPort, doc_root);
            /*
            ClientWebSocketHandler webSockHander;
            appEngine server(multicastIPAddress, 
                             multicastPortNumber, 
                             multicastInterface, 
                             webSockHander);

            std::thread t(std::bind(&appEngine::packetProcessor, &server));
            webSockHander.run(webSocketPort);
            */
            CommandInterface ci;
            ci.AddCommand(&quitCommand);
            ci.run();
        }
        catch (exception& ex)
        {
            cout << "Error:" << __FILE__ << ":" << __LINE__ << ":Exception detected - " << ex.what() << endl;
            return -1;
        }
    }
    return 0;
}
