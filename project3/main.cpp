#include <iostream>
#include <fstream>
#include <vector>

#include "utils.h"
#include "FileSystem.h"

using std::cout;
using std::cin;
using std::endl;
using std::string;

void copyFileFromRealSystem(FileSystem * fileSystem);
void copyFileFromSimulation(FileSystem * fileSystem);
void displayFile(FileSystem * fileSystem);
void displayDiskBlock(FileSystem * fileSystem);
void deleteFile(FileSystem * fileSystem);

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
        fileSystem = new Indexed();
    }
    else
    {
        cout << "Invalid choice. Choices are:" << endl << "contiguous" << endl << "chained" << endl << "indexed" << endl;
        return EXIT_FAILURE;
    }

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
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            invalidChoice = true;
            continue;
        }

        switch(option)
        {
            case 1:
                displayFile(fileSystem);
                break;
            case 2:
                cout << endl;
                fileSystem->displayFileTable();
                cout << endl;
                break;
            case 3:
                cout << endl;
                cout << "Display Free Space Bitmap" << endl;
                fileSystem->displayBitMap();
                cout << endl << endl;
                break;
            case 4:
                displayDiskBlock(fileSystem);
                break;
            case 5:
                copyFileFromSimulation(fileSystem);
                break;
            case 6:
                copyFileFromRealSystem(fileSystem);
                break;
            case 7:
                deleteFile(fileSystem);
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
    std::ifstream inFile(source);

    if (!inFile)
    {
        cout << "Could not find source file.\n\n";
        return;
    }

    do
    {
        if(dest.size() > 8)
        {
            cout << "Simulation file name exceed 8 characters. Try again.\n";
        }
        cout << "\nCopy to: ";
        cin >> dest;
    } while (dest.size() > 8);

    string data = "";
    string buffer;
    while(std::getline(inFile, buffer))
    {
        data += buffer + "\n";
    }
    inFile.close();

    if(!fileSystem->copyToSim(dest, std::vector<char>(data.begin(), data.end())))
    {
        return;
    }
    printf("\nFile %s copied\n\n", source.c_str());
}

void displayFile(FileSystem * fileSystem)
{
    string source;
    cout << "\n\nDisplay File in Simulation";
    cout << "\nFile: ";
    cin >> source;
    cout << endl;
    fileSystem->displayFile(source);
    cout << endl;
    cout << endl;
}

void displayDiskBlock(FileSystem * fileSystem)
{
    int block;
    cout << "\n\nDisplay Disk Block";
    cout << "\nBlock Number: ";
    cin >> block;
    cout << endl;
    fileSystem->displayDiskBlock(block);
    cout << endl;
    cout << endl;
}

void deleteFile(FileSystem * fileSystem)
{
    string fileName;
    cout << "\n\nDelete File in Simulation";
    cout << "\nFile Name: ";
    cin >> fileName;
    if(!fileSystem->deleteFile(fileName))
    {
        return;
    }
    printf("File %s sucessully deleted.\n\n", fileName.c_str());
}
