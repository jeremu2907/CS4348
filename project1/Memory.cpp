#ifndef MEMORY_H
#define MEMORY_H

#include <iostream>
#include <fstream>
#include <sstream>

#include "Memory.h"

using std::ifstream;
using std::stringstream;
using std::cout;
using std::endl;
using std::string;

Memory::Memory(){}

Memory::Memory(string filename)
{
    ifstream inFile;
    inFile.open(filename);
    if(!inFile)
    {
        throw std::invalid_argument("File not found\n");
        return;
    }

    string line;
    string data;
    stringstream sstream;

    int i = 0;
    while(std::getline(inFile, line)){
        sstream.str(line);

        if(sstream.str() == "")
        {
            sstream.clear();
            continue;
        }

        std::getline(sstream, data, ' ');
        if(data.size() == 0)
        {
            continue;
        }
        if(data[0] == '.')
        {
            data = data.substr(1, data.size() - 1);
            // cout << "JUMP TO ADDRES" << data << endl;
            i = stoi(data);
            sstream.clear();
            continue;
        }
        write(i, std::stoi(data));
        i++;
        sstream.clear();
    }

    inFile.close();
}

void
Memory::write(int index, int value)
{
    mem[index] = value;
}

int
Memory::read(int index)
{
    return mem[index];
}
#endif
