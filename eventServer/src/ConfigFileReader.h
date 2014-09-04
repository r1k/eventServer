#pragma once
#include <string>

#include "_no_copy.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

using boost::property_tree::ptree;


/*
Taken from the font of all knowledge - stackoverflow
http://stackoverflow.com/questions/16135285/iterate-over-ini-file-on-c-probably-using-boostproperty-treeptree
*/

class ini_reader : public _no_copy
{
public:
    ini_reader(const std::string filename)
    {
        read_ini(filename, pt);
    }

    int get_num_sections() const
    {
        return pt.size();
    }

    ptree& get_section(std::string sect_name)
    {
        return pt.get_child(sect_name);
    }

    std::string get_value(std::string name, ptree* section=nullptr)
    {
        ptree *root_node = &pt;
        if (section)
        {
            root_node = section;
        }

        return root_node->get_value(name);
    }

private:
    ptree pt;
};