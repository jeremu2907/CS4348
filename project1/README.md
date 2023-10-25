### Env parameters
Cmake Version minimum is 3.25
C++ 11 standard
exec binary proj_1

### Program Files:
main.cpp        Main logic of the project where the parent process is the CPU and child process is Memory
Memory.h        Memory Class, supporting 2 operations write and read. It also checks for filestream error
Memory.cpp      Implemetation file for Memory Class
CMakeLists.txt  (Not required to run the project) makefile Build File

sample*.txt     sample files

### Building

##### Option 1: CMake
```bash
cmake .
make
```
##### Option 2: Manual
```bash
g++ -std=c++11 Memory.cpp main.cpp -o proj_1
```

##### Other platforms:
Please consult your system help page to build c++ files

### Run
./proj_1 [sample_file_name] [timer_val]

sample_file_name must be a file name that exists in the directory
timer_val must be an integer > 2