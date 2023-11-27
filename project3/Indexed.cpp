#include "FileSystem.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>

int * Indexed::getFileInfo(std::vector<char> v, std::string s)
{
    int startIdx = -1;
    for (size_t i = 0; i < v.size(); i += 13)
    {
        if (
            (i + s.size() <= v.size()) &&
            std::equal(s.begin(), s.end(), v.begin() + i) &&
            (v[i + s.size()] == ' ')
        ) 
        {
            startIdx = static_cast<int>(i); // Found the start index of the sub-vector
            break;
        }
    }

    if (startIdx == -1)
    {
        return new int(-1);
    }

    int endIdx;
    for (int i = startIdx; i < v.size(); ++i)
    {
        if(v.at(i) == '\n')
        {
            endIdx = i;
            break;
        }
    }

    std::stringstream ss;
    ss << std::string(v.begin() + startIdx, v.begin() + endIdx);

    std::vector<std::string> tokens;
    std::string token;
    while (std::getline(ss, token, ' '))
    {
        tokens.push_back(token);
    }

    int * data = new int[1]{std::stoi(tokens[tokens.size() - 1])};

    return data;
}

std::vector<int> Indexed::findBlock(int numBlocks)
{
    ++numBlocks;    //Acount for index block
    std::vector<char> data = disk.read(1);
    std::vector<int> blocks;

    int currBlock = 2;
    while(blocks.size() < numBlocks && currBlock < data.size())
    {
        if (data.at(currBlock) == '0')
        {
            blocks.push_back(currBlock);
        }
        ++currBlock;
    }

    return blocks;
}

std::vector<int> Indexed::getBlockList(std::vector<char> s)
{
    std::string charString(s.begin(), s.end());

    // Create a stringstream from the string
    std::stringstream ss(charString);

    // Vector to store the extracted numbers
    std::vector<int> intVector;

    // Temporary variable to store each number
    int number;

    // Extract numbers from stringstream
    while (ss >> number) {
        intVector.push_back(number);

        // Check for newline character '\n' and consume it
        if (ss.peek() == '\n') {
            ss.ignore();
        }
    }

    return intVector;
}

bool Indexed::copyToSim (std::string fileName, std::vector<char> val)
{
    int numBlocks = (val.size() + Disk::BLOCK_SIZE) / Disk::BLOCK_SIZE;
    std::vector<int> blockList = Indexed::findBlock(numBlocks);

    if (numBlocks > 10)
    {
        std::cout << "File exceeded 10 blocks. Try another file.\n\n";
        return false;
    }

    if (blockList.size() == 0)
    {
        std::cout << "Not enough space in disk.\n\n";
        return false;
    }

    //Writing the index block
    //Update bitmap
    std::string indexString = "";
    std::vector<char> bitmap = disk.read(1);
    for(int i = 1; i < blockList.size(); i++)
    {
        indexString += std::to_string(blockList.at(i)) + "\n";
        bitmap.at(blockList.at(i)) = '1';
    }
    bitmap.at(blockList.at(0)) = '1';
    disk.write(1, bitmap);

    std::vector<char> indexBlockData(indexString.size());
    for (int i = 0; i < indexString.size(); i++)
    {
        indexBlockData.at(i) = indexString[i];
    }
    disk.write(blockList.at(0), indexBlockData);

    //Update File table
    std::stringstream ss;
    ss  << std::setw(8) << std::left << fileName << " " 
        << std::setw(3) << std::right << std::to_string(blockList[0])
        << "\n";
    std::string tableEntry = ss.str();
    std::vector<char> data = disk.read(0);
    int tableWritePosition = findLastEntryTable();
    for (int i = 0; i < tableEntry.size(); i++)
    {
        data.at(tableWritePosition + i) = tableEntry[i];
    }
    disk.write(0, data);

    //Write file into storage
    auto beginIt = val.begin();
    auto endIt = (numBlocks == 1) ? val.end() : (val.begin() + Disk::BLOCK_SIZE);
    for (int i = 1; i <= numBlocks; i++)
    {
        std::vector<char> writeData(beginIt, endIt);
        disk.write(blockList[i], writeData);

        beginIt = endIt;
        endIt = (endIt + Disk::BLOCK_SIZE >= val.end()) ? val.end() : endIt + Disk::BLOCK_SIZE;
    }

    return true;
}

void Indexed::copyToSystem (std::string source, std::string dest)
{
    //Find starting block and length
    std::vector<char> data = disk.read(0);
    int * fileInfo = Indexed::getFileInfo(data, source);
    if (*fileInfo == -1)
    {
        std::cout << "File not found. \n\n";
        return;
    }
    int indexBlock = fileInfo[0];
    
    std::ofstream outFile(dest, std::ios::binary);

    std::vector<int> blockList = getBlockList(disk.read(indexBlock));

    for (int i = 0; i < blockList.size(); i++)
    {
        std::vector<char> data = disk.read(blockList[i]);
        outFile.write(data.data(), data.size());
    }

    outFile.close();
}

void Indexed::displayFile(std::string fileName)
{
    //Find starting block and length
    std::vector<char> data = disk.read(0);
    int * fileInfo = Indexed::getFileInfo(data, fileName);
    if (*fileInfo == -1)
    {
        std::cout << "File not found.\n\n";
        return;
    }
    int indexBlock = fileInfo[0];

    std::vector<int> blockList = getBlockList(disk.read(indexBlock));
    
    for (int i = 0; i < blockList.size(); i++)
    {
        std::vector<char> data = disk.read(blockList[i]);
        std::cout << data.data();
    }
}

bool Indexed::deleteFile(std::string fileName)
{
    std::vector<char> data = disk.read(0);
    int * fileInfo = getFileInfo(data, fileName);
    if (*fileInfo == -1)
    {
        std::cout << "File not found.\n\n";
        return false;
    }
    int indexBlock = fileInfo[0];

    std::vector<int> blockList = getBlockList(disk.read(indexBlock));

    data = disk.read(1);
    for (int i = 0; i < blockList.size(); i++)
    {
        data.at(blockList.at(i)) = '0';
    }
    data.at(indexBlock) = '0';
    disk.write(1, data);

    removeFileInfo(fileName);
    return true;
}