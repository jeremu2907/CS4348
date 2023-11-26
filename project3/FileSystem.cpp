#include "FileSystem.h"

#include <string>
#include <fstream>
#include <sstream>

#include <iostream>

int * FileSystem::getFileInfo(std::vector<char> v, std::string s)
{
    int startIdx;
    for (size_t i = 0; i < v.size(); ++i)
    {
        if (i + s.size() <= v.size() && std::equal(s.begin(), s.end(), v.begin() + i)) 
        {
            startIdx = static_cast<int>(i); // Found the start index of the sub-vector
        }
    }

    int endIdx;
    for (int i = startIdx; i < v.size(); i++)
    {
        if(v.at(i) == '\n')
        {
            endIdx = i;
        }
    }

    std::stringstream ss;
    ss << std::string(v.begin() + startIdx, v.begin() + endIdx);

    std::vector<std::string> tokens;
    std::string token;
    while (std::getline(ss, token, '\t'))
    {
        tokens.push_back(token);
    }


    int * data = new int[2]{std::stoi(tokens[tokens.size() - 2]), std::stoi(tokens[tokens.size() - 1])};

    return data;

    // return nullptr;
}

void FileSystem::displayFileTable()
{
    std::vector<char> data = disk.read(0);
    for( auto i : data)
    {
        std::cout << i;
    }
}

int Contiguous::findBlock(int numBlocks)
{
   int count = 0; // Counter for consecutive '0' values
    for (size_t i = 0; i < disk.storage->at(1).size(); ++i) {
        if (disk.storage->at(1).at(i) == 0) {
            ++count;
            if (count == numBlocks) {
                return static_cast<int>(i - numBlocks + 1); // Found the start index of the sub-vector
            }
        } else {
            count = 0; // Reset the counter when a non-'0' value is encountered
        }
    }
    return -1; // Return -1 if the sub-vector is not found
}

void Contiguous::copyToSim(std::string fileName, std::vector<char> val) 
{
    int numBlocks = (val.size() + Disk::BLOCK_SIZE) / Disk::BLOCK_SIZE;
    int block = Contiguous::findBlock(numBlocks);

    std::string tableEntry = fileName + "\t\t" + std::to_string(block) + "\t" + std::to_string(numBlocks) + "\n";

    //Update File table
    std::vector<char> data = disk.read(0);
    int tableWritePosition = findLastEntryTable();
    for (int i = 0; i < tableEntry.size(); i++)
    {
        data.at(tableWritePosition + i) = tableEntry[i];
    }
    disk.write(0, data);

    //Update Bit map
    data = disk.read(1);
    data.at(block) = '1';
    disk.write(1, data);

    // //Write file into storage
    auto beginIt = val.begin();
    auto endIt = (numBlocks == 1) ? val.end() : (val.begin() + Disk::BLOCK_SIZE);
    for (int i = 0; i < numBlocks; i++)
    {
        std::vector<char> writeData(beginIt, endIt);
        disk.write(block + i, writeData);

        beginIt = endIt;
        endIt = (endIt + Disk::BLOCK_SIZE >= val.end()) ? val.end() : endIt + Disk::BLOCK_SIZE;
    }
}

void Contiguous::copyToSystem (std::string source, std::string dest)
{
    //Find starting block and length
    std::vector<char> data = disk.read(0);
    int * fileInfo = getFileInfo(data, source);
    int block = fileInfo[0];
    int numBlocks = fileInfo[1];
    
    std::ofstream outFile(dest, std::ios::binary);

    for (int i = 0; i < numBlocks; i++)
    {
        std::vector<char> data = disk.read(block + i);
        outFile.write(data.data(), data.size());
    }

    outFile.close();
}

int Contiguous::findLastEntryTable()
{
    std::vector<char> data = disk.read(0);
    int pos;
    for(int i = data.size() - 1; i >= 0; i--)
    {
        if (data.at(i) == '\n')
        {
            pos = i + 1;
            break;
        }
    }

    return pos;
}

void Contiguous::displayFile(std::string fileName)
{
    //Find starting block and length
    std::vector<char> data = disk.read(0);
    int * fileInfo = getFileInfo(data, fileName);
    int block = fileInfo[0];
    int numBlocks = fileInfo[1];
}
