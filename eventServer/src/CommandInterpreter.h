#pragma once
#include <string>
#include <list>
#include <iostream>

#include <mutex>

#include "_no_copy.h"

class Command
{
public:
    
    Command() {}
    Command(std::string n, std::string d) : name(n), description(d) { }
    ~Command() {}

    virtual void Run() = 0;

    std::string get_Name() { return name; }
    void set_Name(std::string value) { name = value; }

    std::string get_Description() { return description; }
    void set_Description(std::string value) { description = value; }

protected:
    std::string name;
    std::string description;
};

class ui : public _no_copy
{
public:
    virtual void AddCommand(Command* c) = 0;
    virtual Command* GetCommand(std::string name) = 0;

    virtual void write(std::string s) = 0;
};

class cmdOpen : public Command
{
public:
    cmdOpen() : Command("open", "opens a file") {}

    void Run()
    {
        std::cout << "running open command";
    }
};

class cmdClose : public Command
{
public:
    cmdClose() : Command("close", "closes a file") {}

    void Run()
    {
        std::cout << "running close command";
    }
};

class CommandInterface : public ui
{
public:
    CommandInterface(std::istream& i=std::cin, std::ostream &o=std::cout) :
        instream(i),
        outstream(o)
    {
        LoadCommands();
    }
private:
    // Array to hold list of commands
    std::list<Command*> listOfCommands;
    std::mutex listofCommands_mutex;

    std::mutex outstream_mutex;
    std::istream& instream;
    std::ostream& outstream;

    //Loads the commands to arrylist
    void LoadCommands()
    {
        std::lock_guard<std::mutex> lock(listofCommands_mutex);
        listOfCommands.push_back(new cmdOpen());
        listOfCommands.push_back(new cmdClose());
    }

public:
    virtual Command* GetCommand(std::string name)
    {
        std::lock_guard<std::mutex> lock(listofCommands_mutex);
        for (auto item : listOfCommands)
        {
            if (name.compare(item->get_Name()))
            {
                return item; //return the command found
            }
        }
        return nullptr; //return if no commands are found
    }

    virtual void AddCommand(Command* c)
    {
        std::lock_guard<std::mutex> lock(listofCommands_mutex);
        listOfCommands.push_back(c);
    }

    virtual void write(std::string s)
    {
        std::lock_guard<std::mutex> lock(outstream_mutex);
        outstream << s << "\n";
    }

    virtual std::string readline()
    {
        return string("");
    }

    void run()
    {

    }
};

    //class Program
    //{
    //    static void Main(string[] args)
    //    {
    //        //Command pattern example
    //        CInvoker cmdInvoker = new CInvoker();
    //        ICommand cmd1 = cmdInvoker.GetCommand("open");
    //        cmd1.Run();
    //        cmdInvoker.GetCommand("update").Run();
    //        //or
    //        new CInvoker().GetCommand("close").Run();
    //    }
    //}
