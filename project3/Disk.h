#ifndef DISK_H
#define DISK_H

#include <vector>

class Disk
{
    static const unsigned int BLOCKS = 256;
    static const unsigned int BLOCK_SIZE = 512;

    private:
        std::vector<std::vector<char>> * storage;
        
        void write(int block, std::vector<char> val);
        std::vector<char> read(int block);

    public:
        Disk();

    friend class FileSystem;
    friend class Contiguous;
    friend class Indexed;
    friend class Chained;
};

#endif