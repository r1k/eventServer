
#include<boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include "CommandInterpreter.h"

using namespace boost::algorithm;

void CommandInterface::CommandInitialise()
{
    AddCommand(new cmdHelp());
    AddCommand(new cmdOpen());
    AddCommand(new cmdClose());
}

void CommandInterface::split_command_line(const std::string cmdl, std::string& command, command_list_t& args)
{
    const char* whitespace = " \n\r\t";
    split(args, cmdl, is_any_of(whitespace), boost::token_compress_on);

    if (args.size())
    {
        command = args[0];
        args.erase(args.begin(), args.begin() + 1);
    }
    else
    {
        command = "";
    }
}

Command* CommandInterface::GetCommand(const std::string& name)
{
    std::lock_guard<std::mutex> lock(listofCommands_mutex);
    for (auto item : listOfCommands)
    {
        if (name.compare(item->get_Name()) == 0)
        {
            return item; //return the command found
        }
    }
    return nullptr; //return if no commands are found
}

void CommandInterface::AddCommand(Command* c)
{
    c->set_ui(this);
    std::lock_guard<std::mutex> lock(listofCommands_mutex);
    listOfCommands.push_back(c);
}

void CommandInterface::writeline(const std::string& s)
{
    std::lock_guard<std::mutex> lock(outstream_mutex);
    outstream << s << "\n";
}

void CommandInterface::write(const std::string& s)
{
    std::lock_guard<std::mutex> lock(outstream_mutex);
    outstream << s;
}

std::string CommandInterface::readline()
{
    std::string line;
    std::getline(std::cin, line);
    return line;
}

int CommandInterface::run()
{
    const std::string prompt(">>>");
    try
    {
        writeline("Welcome.");
        writeline("eventServer command prompt:");

        while (!done)
        {
            // output prompt
            write(prompt);
            // read a line
            std::string line = readline();
            // work out what command it is
            trim(line);
            if (!line.length())
            {
                continue;
            }

            std::string cmnd;
            command_list_t args;
            split_command_line(line, cmnd, args);
            // execute command
            Command* command = GetCommand(cmnd);
            if (command)
            {
                if (args.size())
                {
                    command->Run(args);
                }
                else
                {
                    command->Run();
                }
            }
            else
            {
                writeline("Unknown command");
            }

            // rinse repeat
        }

        return 0;
    }
    catch (...)
    {
        return -1;
    }
}