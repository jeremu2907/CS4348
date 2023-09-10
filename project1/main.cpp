#include <iostream>
#include <fstream>
#include <sstream>

#include "Cpu.h"

using std::ifstream;
using std::stringstream;
using std::cout;
using std::endl;
using std::string;



int main()
{
    ifstream inFile("sample1.txt");
    string line;
    string instruction;
    stringstream sstream;

    Cpu cpu;

    // while(!inFile.eof()){
    //     std::getline(inFile, line);
    //     sstream.str(line);
        
    //     std::getline(sstream, instruction, ' ');
    //     cout << instruction << endl;
    //     sstream.clear();
    // }
}