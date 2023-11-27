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
        virtual bool copyToSystem (std::string systemFileName, std::string simFileName) {return false;};
        virtual void displayFile(std::string fileName) {};
        virtual bool deleteFile(std::string fileName) {return false;};
};

class Contiguous : public FileSystem
{
    private:
        int findBlock(int numBlocks);

    public:
        bool copyToSim (std::string fileName, std::vector<char> val) override;
        bool copyToSystem (std::string source, std::string dest) override;
        void displayFile(std::string fileName) override;
        bool deleteFile(std::string fileName) override;
};

class Indexed : public FileSystem
{
    private:
        std::vector<int> findBlock(int numBlocks);
        std::vector<int> getBlockList(std::vector<char> s);

    public:
        int * getFileInfo(std::vector<char> v, std::string s) override;
        bool copyToSim (std::string fileName, std::vector<char> val) override;
        bool copyToSystem (std::string source, std::string dest) override;
        void displayFile(std::string fileName) override;
        bool deleteFile(std::string fileName) override;
};

class Chained : public FileSystem
{
    private:
        std::vector<int> findBlock(int numBlocks);

    public:
        bool copyToSim (std::string fileName, std::vector<char> val) override;
        bool copyToSystem (std::string source, std::string dest) override;
        void displayFile(std::string fileName) override;
        bool deleteFile(std::string fileName) override;
};

#endif