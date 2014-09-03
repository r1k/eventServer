#pragma once
#include <string>
#include <list>
#include <iostream>

#include "_no_copy.h"

class Command
{
public:
    
    Command() {}
    Command(std::string n, std::string d)
    {
        set_Name(n);
        set_Description(d);
    }
    ~Command() {}

    virtual void Run() = 0;

    std::string get_Name()
    {
        return name;
    }
    void set_Name(std::string value)
    {
        name = value;
    }

    std::string get_Description()
    {
        return description;
    }
    void set_Description(std::string value)
    {
        description = value;
    }

protected:
    std::string name;
    std::string description;
};

class cmdOpen : public Command
{
public:
    cmdOpen()
        :
        Command("open", "opens a file")
    {
    }

    void Run()
    {
        std::cout << "running open command";
    }
};

class cmdClose : public Command
{
public:
    cmdClose()
        :
        Command("close", "closes a file")
    {
    }

    void Run()
    {
        std::cout << "running close command";
    }
};

class cmdCreate : public Command
{
public:
    cmdCreate()
        :
        Command("create", "creates a file")
    {
    }

    void Run()
    {
        std::cout << "running create command";
    }
};

class cmdUpdate : public Command
{
public:
    cmdUpdate()
        :
        Command("update", "updates a file")
    {
    }

    void Run()
    {
        std::cout << "running update command";
    }
};

class cmdRetrieve : public Command
{
public:
    cmdRetrieve()
        :
        Command("reterive", "retrieves a file")
    {
    }

    void Run()
    {
        std::cout << "running reterive command";
    }
};


class CInvoker : public _no_copy
{
public:
    CInvoker()
    {
        LoadCommands();
    }
private:
    // Array to hold list of commands
    std::list<Command*> listOfCommands;

    //Loads the commands to arrylist
    void LoadCommands()
    {
        listOfCommands.push_back(new cmdOpen());
        listOfCommands.push_back(new cmdClose());
        listOfCommands.push_back(new cmdCreate());
        listOfCommands.push_back(new cmdUpdate());
        listOfCommands.push_back(new cmdRetrieve());
    }

public:
    virtual Command* GetCommand(std::string name)
    {
        for (auto item : listOfCommands)
        {
            if (name.compare(item->get_Name()))
            {
                return item; //return the command found
            }
        }
        return nullptr; //return if no commands are found
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
