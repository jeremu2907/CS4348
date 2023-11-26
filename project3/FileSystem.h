#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "Disk.h"

#include <string>

class FileSystem
{
    protected:
        Disk disk;
        int * getFileInfo(std::vector<char> v, std::string s);
        void removeFileInfo(std::string s);

    public:
        void displayFileTable();
        void displayBitMap();
        void displayDiskBlock(int block);

        virtual bool copyToSim (std::string fileName, std::vector<char> val) {return false;};
        virtual void copyToSystem (std::string systemFileName, std::string simFileName) {};
        virtual void displayFile(std::string fileName) {};
        virtual void deleteFile(std::string fileName) {};
};

class Contiguous : public FileSystem
{
    private:
        int findBlock(int blockSize);
        int findLastEntryTable();

    public:
        bool copyToSim (std::string fileName, std::vector<char> val) override;
        void copyToSystem (std::string systemFileName, std::string simFileName) override;
        void displayFile(std::string fileName) override;
        void deleteFile(std::string fileName) override;
};

#endif