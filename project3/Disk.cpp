#include "Disk.h"
#include <vector>

Disk::Disk()
{
    storage = new std::vector<std::vector<char>>(BLOCKS, std::vector<char>(BLOCK_SIZE, '\0'));
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