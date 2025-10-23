#pragma once

#include <fstream>
#include <iostream>
#include "MapOSC.hpp"

class OSC_readwriter {

public:
    OSC_readwriter(){}

    ~OSC_readwriter(){

        if (file.is_open() )
            file.close();

    }

    void openFile(std::string& fn)
    {
        filename = fn;
        file = std::fstream(filename, std::ios::in | std::ios::out | std::ios::binary ); // | std::ios::trunc // truc deletes the file if it exists

        if (file.is_open() )
        {
            std::cout << "opened file " << filename << std::endl;
            std::cout << "max size set to " << max_size_bytes << std::endl;

        }
        else
        {
            std::cout << "Error! File cannot be opened!" << std::endl;
        }

    }


    void writeOSC(MapOSC& osc)
    {
        const std::string writeStr = osc.getSerializedString();

        size_t new_size = size_in_bytes + writeStr.size() + 1;

        if( new_size <= max_size_bytes )
        {
            file.write( writeStr.c_str(), writeStr.size() );
            file.put('\r');

            size_in_bytes = new_size;
        } else {
            std::cout << "max size reached!" << std::endl;
        }

    }

private:
    std::string filename = "foo.osc";
    size_t size_in_bytes = 0;
    std::fstream file;

    size_t max_size_bytes = 3<<30; // max size 3 gb
};

