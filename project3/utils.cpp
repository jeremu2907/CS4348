#include "utils.h"

void utils::menu(bool error = false)
{
    if (error)
    {
        printf("INVALID INPUT\n\n");
    }

    printf(
        "1) Display a file\n" 
        "2) Display the file table\n"
        "3) Display the free space bitmap\n"
        "4) Display a disk block\n"
        "5) Copy a file from the simulation to a file on the real system\n"
        "6) Copy a file from the real system to a file in the simulation\n"
        "7) Delete a file\n"
        "8) Exit\n"
    );
}