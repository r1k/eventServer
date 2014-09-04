#pragma once
#include <string>
#include <sstream>
#include <list>
#include <iostream>
#include <mutex>

#include <vector>

#include "_no_copy.h"

class ui;
class Command;
// Interface classes (sort of)
typedef std::list<Command*> command_object_list_t;
typedef std::vector<std::string> command_list_t;

class Command
{
public:
    
    Command() {}
    Command(std::string n, std::string d) : name(n), description(d) { }
    ~Command() {}

    virtual void Run() = 0;
    virtual void Run(const command_list_t& args) = 0;

    std::string get_Name() { return name; }
    void set_Name(std::string value) { name = value; }

    std::string get_Description() { return description; }
    void set_Description(std::string value) { description = value; }

    void set_ui(ui *const _ui) { UI = _ui; }

protected:

    ui *UI;
    std::string name;
    std::string description;
};

class ui : public _no_copy
{
public:
    virtual void AddCommand(Command* c) = 0;
    virtual Command* GetCommand(const std::string& name) = 0;
    virtual command_object_list_t& GetCommandList() = 0;

    virtual void writeline(const std::string& s) = 0;

    virtual void stop() = 0;
};


// Now for the inplementations

// a couple of  basic commands
class cmdOpen : public Command
{
public:
    cmdOpen() : Command("open", "opens a file") {}

    void Run()
    {
        UI->writeline("running open command");
    }
    void Run(const command_list_t& args) { Run(); }
};

class cmdClose : public Command
{
public:
    cmdClose() : Command("close", "closes a file") {}

    void Run()
    {
        UI->writeline("running close command");
    }
    void Run(const command_list_t& args) { Run(); }
};

class cmdHelp : public Command
{
public:
    cmdHelp() : Command("help", "displays the commands") {}

    void Run()
    {
        UI->writeline("List of commands");

        std::list<Command *>& cmds = UI->GetCommandList();
        for (auto cmd : cmds)
        {
            std::stringstream ss;
            ss << "\t" << cmd->get_Name();
            UI->writeline(ss.str());
        }
    }
    void Run(const command_list_t& args)
    {
        std::list<Command *>& cmds = UI->GetCommandList();
        for (auto cmd : cmds)
        {
            std::string cmd_name = cmd->get_Name();
            if (cmd_name.compare(0, cmd_name.length(), args[0]) == 0)
            {
                std::stringstream ss;
                ss << "\t" << cmd->get_Name() << "\t\t-\t\t" << cmd->get_Description();
                UI->writeline(ss.str());
            }
        }
    }
};

class CommandInterface : public ui
{
private:
    // Array to hold list of commands
    command_object_list_t listOfCommands;
    std::mutex listofCommands_mutex;

    std::mutex outstream_mutex;
    std::istream& instream;
    std::ostream& outstream;

    void split_command_line(const std::string cmdl, std::string& command, command_list_t& args);

    bool done = false;

public:
    CommandInterface(std::istream& i = std::cin, std::ostream &o = std::cout) :
        instream(i),
        outstream(o)
    {
        CommandInitialise();
    }

    void CommandInitialise();

    virtual Command* GetCommand(const std::string& name);
    command_object_list_t& GetCommandList() { return listOfCommands; }

    virtual void AddCommand(Command* c);

    virtual void writeline(const std::string& s);
    virtual void write(const std::string& s);
    virtual std::string readline();

    int  run();
    void stop() { done = true; }
    
};
