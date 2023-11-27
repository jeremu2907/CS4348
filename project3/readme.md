### Project3

#### Running manually
- step 1 - compile:
    ```bash
        g++ utils.cpp Disk.cpp FileSystem.cpp Contiguous.cpp Chained.cpp Indexed.cpp main.cpp -std=c++11 -o main
    ```
- step 2 - execute:
    ```bash
        ./main [file system]
    ```
[file system] should be either 'contiguous', 'chained', or 'indexed'. No option will end execution. Wrong arg will produce error

#### Running with provided script
    ```bash
        ./run.sh [file system]
    ```
Make sure script has execute permission
[file system] should be either 'contiguous', 'chained', or 'indexed'. No option will end execution. Wrong arg will produce error