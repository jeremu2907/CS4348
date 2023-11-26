#include "Disk.h"
#include <vector>
#include <iostream>

Disk::Disk()
{
    storage = new std::vector<std::vector<char>>(BLOCKS, std::vector<char>(BLOCK_SIZE, '\0'));
    for(int i = 2; i < storage->at(1).size(); i++)
    {
        storage->at(1).at(i) = '0';
    }
    storage->at(1).at(0) = '1';   //File allocation table
    storage->at(1).at(1) = '1';   //Free space management
}

void Disk::write(int block, std::vector<char> val)
{
    storage->at(block) = val;
}

std::vector<char> Disk::read(int block)
{
    return storage->at(block);
}