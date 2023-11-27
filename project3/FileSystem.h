#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "Disk.h"

#include <string>

class FileSystem
{
    protected:
        Disk disk;
        void removeFileInfo(std::string s);
        int findLastEntryTable();

    public:
        void displayBitMap();
        void displayDiskBlock(int block);
        void displayFileTable();

        virtual int * getFileInfo(std::vector<char> v, std::string s);
        virtual bool copyToSim (std::string fileName, std::vector<char> val) {return false;};
        virtual void copyToSystem (std::string systemFileName, std::string simFileName) {};
        virtual void displayFile(std::string fileName) {};
        virtual bool deleteFile(std::string fileName) {return false;};
};

class Contiguous : public FileSystem
{
    private:
        int findBlock(int blockSize);

    public:
        bool copyToSim (std::string fileName, std::vector<char> val) override;
        void copyToSystem (std::string source, std::string dest) override;
        void displayFile(std::string fileName) override;
        bool deleteFile(std::string fileName) override;
};

class Indexed : public FileSystem
{
    private:
        std::vector<int> findBlock(int blockSize);
        std::vector<int> getBlockList(std::vector<char> s);

    public:
        int * getFileInfo(std::vector<char> v, std::string s) override;
        bool copyToSim (std::string fileName, std::vector<char> val) override;
        void copyToSystem (std::string source, std::string dest) override;
        void displayFile(std::string fileName) override;
        bool deleteFile(std::string fileName) override;
};

#endif