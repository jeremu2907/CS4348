#include <iostream>
#include <fstream>
#include <vector>

#include "utils.h"
#include "Disk.h"
#include "FileSystem.h"

using std::cout;
using std::cin;
using std::endl;
using std::string;

void copyFileFromRealSystem(FileSystem * fileSystem);
void copyFileFromSimulation(FileSystem * fileSystem);

int main (int argc, char * argv[])
{
    if (argc != 2)
    {
        return EXIT_FAILURE;
    }

    std::string allocationMethod(argv[1]);
    FileSystem * fileSystem = nullptr;

    if (allocationMethod == "contiguous")
    {
        fileSystem = new Contiguous();
    } 
    else if (allocationMethod == "chained")
    {

    }
    else if (allocationMethod == "indexed")
    {

    }
    else
    {
        cout << "Invalid choice. Choices are:" << endl << "contiguous" << endl << "chained" << endl << "indexed" << endl;
        return EXIT_FAILURE;
    }

    Disk disk;

    int option;
    bool invalidChoice = false;
    while(option != 8)
    {
        utils::menu(invalidChoice);
        invalidChoice = false;
        cout << "Choice: ";
        cin >> option;

        if(cin.fail() || option > 8 || option < 1)
        {
            cin.clear();
            cin.ignore();
            invalidChoice = true;
            continue;
        }

        switch(option)
        {
            case 1:
                break;
            case 2:
                cout << endl;
                fileSystem->displayFileTable();
                cout << endl;
                break;
            case 3:
                break;
            case 4:
                break;
            case 5:
                copyFileFromSimulation(fileSystem);
                break;
            case 6:
                copyFileFromRealSystem(fileSystem);
                break;
            case 7:
                break;
            case 8:
                break;
        }
    }

    return EXIT_SUCCESS;
}

void copyFileFromSimulation(FileSystem * fileSystem)
{
    string source;
    string dest;
    cout << "\n\nCopy File From Simulation";
    cout << "\nCopy from: ";
    cin >> source;
    cout << "\nCopy to: ";
    cin >> dest;

    fileSystem->copyToSystem(source, dest);
    
    printf("\nFile %s copied\n\n", source.c_str());
}

void copyFileFromRealSystem(FileSystem * fileSystem)
{
    string source;
    string dest;
    cout << "\n\nCopy File From System";
    cout << "\nCopy from: ";
    cin >> source;
    cout << "\nCopy to: ";
    cin >> dest;

    std::ifstream inFile(source);

    if (!inFile)
    {
        exit(EXIT_FAILURE);
    }

    string data = "";
    string buffer;
    while(std::getline(inFile, buffer))
    {
        data += buffer + "\n";
    }

    fileSystem->copyToSim(dest, std::vector<char>(data.begin(), data.end()));
    inFile.close();
    printf("\nFile %s copied\n\n", source.c_str());
}