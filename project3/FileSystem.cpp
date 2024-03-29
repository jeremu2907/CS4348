#include "FileSystem.h"

#include <string>
#include <sstream>
#include <iostream>
#include <cctype>

int * FileSystem::getFileInfo(std::vector<char> v, std::string s)
{
    int startIdx = -1;
    for (size_t i = 0; i < v.size(); i += 16)
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
    int * data = new int[2];
    while (std::getline(ss, token, ' '))
    {
        tokens.push_back(token);
    }

    int count = 1;
    for (int i = tokens.size() - 1; i >= 0; i--)
    {
        if(tokens[i].size() > 0)
        {
            data[count] = std::stoi(tokens[i]);
            count--;
            if(count < 0)
            {
                break;
            }
        }
    }

    return data;
}

int FileSystem::findLastEntryTable()
{
    std::vector<char> data = disk.read(0);
    int pos = 0;
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

void FileSystem::removeFileInfo(std::string s)
{
    std::vector<char> data = disk.read(0);
    int startIdx;
    for (size_t i = 0; i < data.size(); ++i)
    {
        if ((i + s.size() <= data.size()) && std::equal(s.begin(), s.end(), data.begin() + i)) 
        {
            startIdx = static_cast<int>(i); // Found the start index of the sub-vector
            break;
        }
    }

    int endIdx;
    for (int i = startIdx; i < data.size(); ++i)
    {
        if(data.at(i) == '\n')
        {
            data.at(i) = '\0';
            endIdx = i;
            break;
        }
        data.at(i) = '\0';
    }

    std::vector<char> firstHalf(data.begin(), data.begin() + startIdx);
    std::vector<char> secondHalf(data.begin() + endIdx + 1, data.end());
    firstHalf.insert(firstHalf.end(), secondHalf.begin(), secondHalf.end());
    data = firstHalf;

    disk.write(0, data);
}

void FileSystem::displayFileTable()
{
    std::vector<char> data = disk.read(0);
    std::cout << data.data();
}

void FileSystem::displayBitMap()
{
    std::vector<char> data = disk.read(1);

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            std::cout << data.at(i*32 + j);
        }
        std::cout << std::endl;
    }
}

void FileSystem::displayDiskBlock(int block)
{
    std::vector<char> data = disk.read(block);
    for (auto i : data)
    {
        std::cout << i;
    }
}