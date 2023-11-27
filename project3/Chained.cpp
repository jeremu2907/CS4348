#include "FileSystem.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>

std::vector<int> Chained::findBlock(int numBlocks)
{
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

bool Chained::copyToSim (std::string fileName, std::vector<char> val)
{
    int numBlocks = (val.size() + Disk::BLOCK_SIZE) / Disk::BLOCK_SIZE;
    numBlocks = (numBlocks * 3 + val.size() + Disk::BLOCK_SIZE) / Disk::BLOCK_SIZE;
    std::vector<int> blockList = Chained::findBlock(numBlocks);

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
    for(int i = 0; i < blockList.size(); i++)
    {
        bitmap.at(blockList.at(i)) = '1';
    }
    disk.write(1, bitmap);

    //Update File table
    std::stringstream ss;
    ss  << std::setw(8) << std::left << fileName << " " 
        << std::setw(3) << std::right << std::to_string(blockList.at(0)) << " "
        << std::setw(2) << std::right << std::to_string(numBlocks)
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
    auto endIt = (numBlocks == 1) ? val.end() : (val.begin() + Disk::BLOCK_SIZE - 3);
    for (int i = 0; i < blockList.size(); i++)
    {   
        std::string nextBlock = "   ";
        if(i < blockList.size() - 1)
        {
            std::stringstream ss;
            ss << std::setw(3) << std::right << std::to_string(blockList.at(i + 1));
            nextBlock = ss.str();
        }
        std::vector<char> writeData(beginIt, endIt);
        writeData.insert(writeData.end(), nextBlock.begin(), nextBlock.end());
        disk.write(blockList[i], writeData);

        beginIt = endIt;
        endIt = (endIt + Disk::BLOCK_SIZE >= val.end() + 3) ? val.end() : endIt + Disk::BLOCK_SIZE - 3;
    }

    return true;
}

void Chained::copyToSystem (std::string source, std::string dest)
{
    
    //Find starting block and length
    std::vector<char> data = disk.read(0);

    int * fileInfo = getFileInfo(data, source);
    if (*fileInfo == -1)
    {
        std::cout << "File not found. \n\n";
        return;
    }
    int block = fileInfo[0];
    int numBlocks = fileInfo[1];

    std::ofstream outFile(dest, std::ios::binary);

    for (int i = 0; i < numBlocks; i++)
    {
        std::vector<char> data = disk.read(block);

        std::vector<char> printData(data.begin(), data.end() - 3);
        std::string nextBlock = "";
        if (i < numBlocks - 1)
        {
            for(int j = data.size() - 1; j >= data.size() - 4; j--)
            {
                if (data.at(j) == ' ')
                {
                    break;
                }
                nextBlock = data.at(j) + nextBlock;
            }
            block = std::stoi(nextBlock);
        }

        outFile.write(printData.data(), printData.size());
    }

    outFile.close();
}

void Chained::displayFile(std::string fileName)
{
    //Find starting block and length
    std::vector<char> data = disk.read(0);

    int * fileInfo = getFileInfo(data, fileName);
    if (*fileInfo == -1)
    {
        std::cout << "File not found. \n\n";
        return;
    }
    int block = fileInfo[0];
    int numBlocks = fileInfo[1];

    for (int i = 0; i < numBlocks; i++)
    {
        std::vector<char> data = disk.read(block);

        std::vector<char> printData(data.begin(), data.end() - 3);
        std::string nextBlock = "";
        if (i < numBlocks - 1)
        {
            for(int j = data.size() - 1; j >= data.size() - 4; j--)
            {
                if (data.at(j) == ' ')
                {
                    break;
                }
                nextBlock = data.at(j) + nextBlock;
            }
            block = std::stoi(nextBlock);
        }
        
        std::cout << data.data();
    }
}

bool Chained::deleteFile(std::string fileName)
{
    //Find starting block and length
    std::vector<char> data = disk.read(0);
    std::vector<char> bitmap = disk.read(1);

    int * fileInfo = getFileInfo(data, fileName);
    if (*fileInfo == -1)
    {
        std::cout << "File not found. \n\n";
        return false;
    }
    int block = fileInfo[0];
    int numBlocks = fileInfo[1];

    for (int i = 0; i < numBlocks; i++)
    {   
        bitmap.at(block) = '0';
        std::vector<char> data = disk.read(block);
        std::string nextBlock = "";
        if (i < numBlocks - 1)
        {
            for(int j = data.size() - 1; j >= data.size() - 4; j--)
            {
                if (data.at(j) == ' ')
                {
                    break;
                }
                nextBlock = data.at(j) + nextBlock;
            }
            block = std::stoi(nextBlock);
        }
    }

    removeFileInfo(fileName);
    disk.write(1, bitmap);

    return true;
}