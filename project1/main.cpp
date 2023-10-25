#include <unistd.h>
#include <stdio.h>
#include <stdexcept>

#include <iostream>
#include <time.h>

#include "Memory.h"

using std::cout;
using std::endl;

#define READ    0
#define WRITE   1


void checkMemory (int addr, bool k);


int main(int argc, char * argv[])
{
    const int WRITE_FLG = -1;

    if(argc <= 2)
    {
        printf("No arguments given\n");
        return 1;
    }

    srand(time(NULL));  //Setting random with unix time as seed

    int cpu_mem[2];
    int mem_cpu[2];
    if(pipe(cpu_mem) < 0 || pipe(mem_cpu) < 0)
        return 0;       //Pipe failed

    pid_t process = fork();

    //CPU
    if(process > 0)
    {
        //cli file error handling
        int *memStatus = new int;
        read(mem_cpu[READ], memStatus, sizeof(*memStatus));
        if(*memStatus == 0)
        {
            return 1;
        }

        int PC = 0,
            SP = 1000,
            IR = 0,
            AC = 0,
            X = 0,
            Y = 0;

        int op;
        int timer = 0;
        int timerInt = std::stoi(argv[2]);
        bool timerIntBool = false;
        bool kernel = false;

        while(true)
        {
            //Read instruction
            write(cpu_mem[WRITE], &PC, sizeof(PC));
            read(mem_cpu[READ], &IR, sizeof(IR));

            ++timer;
            //Check counter
            if(timer >= timerInt)
            {
                timer = 0;
                if(!kernel)
                {
                    kernel = true;
                    timerIntBool = true;
                    IR = 29;
                    --PC;
                }
            }

            switch(IR)
            {
                case 1:
                    ++PC;
                    write(cpu_mem[WRITE], &PC, sizeof(PC));
                    read(mem_cpu[READ], &AC, sizeof(AC));
                    break;
                case 2:
                    ++PC;
                    write(cpu_mem[WRITE], &PC, sizeof(PC));
                    read(mem_cpu[READ], &AC, sizeof(AC));
                    checkMemory(AC, kernel);
                    write(cpu_mem[WRITE], &AC, sizeof(AC));
                    read(mem_cpu[READ], &AC, sizeof(AC));
                    break;
                case 3:
                    ++PC;
                    write(cpu_mem[WRITE], &PC, sizeof(PC));
                    read(mem_cpu[READ], &AC, sizeof(AC));
                    checkMemory(AC, kernel);
                    write(cpu_mem[WRITE], &AC, sizeof(AC));
                    read(mem_cpu[READ], &AC, sizeof(AC));
                    checkMemory(AC, kernel);
                    write(cpu_mem[WRITE], &AC, sizeof(AC));
                    read(mem_cpu[READ], &AC, sizeof(AC));
                    break;
                case 4:
                    ++PC;
                    write(cpu_mem[WRITE], &PC, sizeof(PC));
                    read(mem_cpu[READ], &op, sizeof(AC));
                    op += X;
                    checkMemory(op, kernel);
                    write(cpu_mem[WRITE], &op, sizeof(AC));
                    read(mem_cpu[READ], &AC, sizeof(AC));
                    break;
                case 5:
                    ++PC;
                    write(cpu_mem[WRITE], &PC, sizeof(PC));
                    read(mem_cpu[READ], &op, sizeof(AC));
                    op += Y;
                    checkMemory(op, kernel);
                    write(cpu_mem[WRITE], &op, sizeof(AC));
                    read(mem_cpu[READ], &AC, sizeof(AC));
                    break;
                case 6:
                    op = SP + X;
                    checkMemory(op, kernel);
                    write(cpu_mem[WRITE], &op, sizeof(op));
                    read(mem_cpu[READ], &AC, sizeof(AC));
                    break;
                case 7:
                    ++PC;
                    write(cpu_mem[WRITE], &PC, sizeof(PC));
                    read(mem_cpu[READ], &op, sizeof(op));
                    checkMemory(op, kernel);
                    write(cpu_mem[WRITE], &WRITE_FLG, sizeof(WRITE_FLG));
                    write(cpu_mem[WRITE], &op, sizeof(op));
                    write(cpu_mem[WRITE], &AC, sizeof(AC));
                    break;
                case 8:
                    AC = (rand() % 100) + 1;
                    break;
                case 9:
                    ++PC;
                    write(cpu_mem[WRITE], &PC, sizeof(PC));
                    read(mem_cpu[READ], &op, sizeof(op));
                    if(op == 1)
                    {
                        printf("%i", AC);
                    }
                    else if(op == 2)
                    {
                        printf("%c", AC);
                    }
                    break;
                case 10:
                    AC += X;
                    break;
                case 11:
                    AC += Y;
                    break;
                case 12:
                    AC -= X;
                    break;
                case 13:
                    AC -= Y;
                    break;
                case 14:
                    X = AC;
                    break;
                case 15:
                    AC = X;
                    break;
                case 16:
                    Y = AC;
                    break;
                case 17:
                    AC = Y;
                    break;
                case 18:
                    SP = AC;
                    break;
                case 19:
                    AC = SP;
                    break;
                case 20:
                    ++PC;
                    write(cpu_mem[WRITE], &PC, sizeof(PC));
                    read(mem_cpu[READ], &PC, sizeof(PC));
                    checkMemory(PC, kernel);
                    --PC;
                    break;
                case 21:
                    ++PC;
                    write(cpu_mem[WRITE], &PC, sizeof(PC));
                    read(mem_cpu[READ], &op, sizeof(op));
                    checkMemory(op, kernel);
                    if(AC == 0)
                    {
                        PC = op - 1;
                    }
                    break;
                case 22:
                    ++PC;
                    write(cpu_mem[WRITE], &PC, sizeof(PC));
                    read(mem_cpu[READ], &op, sizeof(op));
                    checkMemory(op, kernel);
                    if(AC != 0)
                    {
                        PC = op - 1;
                    }
                    break;
                case 23:
                    ++PC;
                    write(cpu_mem[WRITE], &PC, sizeof(PC));
                    read(mem_cpu[READ], &op, sizeof(op));

                    ++PC;
                    --SP;
                    write(cpu_mem[WRITE], &WRITE_FLG, sizeof(WRITE_FLG));
                    write(cpu_mem[WRITE], &SP, sizeof(SP));
                    write(cpu_mem[WRITE], &PC, sizeof(PC));

                    PC = op - 1;
                    break;
                case 24:
                    write(cpu_mem[WRITE], &SP, sizeof(SP));
                    read(mem_cpu[READ], &PC, sizeof(PC));
                    --PC;
                    ++SP;
                    break;
                case 25:
                    ++X;
                    break;
                case 26:
                    --X;
                    break;
                case 27:
                    --SP;
                    write(cpu_mem[WRITE], &WRITE_FLG, sizeof(WRITE_FLG));
                    write(cpu_mem[WRITE], &SP, sizeof(SP));
                    write(cpu_mem[WRITE], &AC, sizeof(AC));
                    break;
                case 28:
                    write(cpu_mem[WRITE], &SP, sizeof(SP));
                    read(mem_cpu[READ], &AC, sizeof(AC));
                    ++SP;
                    break;
                case 29:
                    op = 1999;
                    write(cpu_mem[WRITE], &WRITE_FLG, sizeof(WRITE_FLG));
                    write(cpu_mem[WRITE], &op, sizeof(op));
                    write(cpu_mem[WRITE], &SP, sizeof(SP));
                    --op;
                    ++PC;
                    write(cpu_mem[WRITE], &WRITE_FLG, sizeof(WRITE_FLG));
                    write(cpu_mem[WRITE], &op, sizeof(op));
                    write(cpu_mem[WRITE], &PC, sizeof(PC));
                    SP = op;

                    kernel = true;
                    PC = (timerIntBool) ? 999 : 1499;
                    break;
                case 30:
                    write(cpu_mem[WRITE], &SP, sizeof(SP));
                    read(mem_cpu[READ], &PC, sizeof(PC));
                    --PC;
                    ++SP;
                    write(cpu_mem[WRITE], &SP, sizeof(SP));
                    read(mem_cpu[READ], &SP, sizeof(SP));
                    kernel = false;
                    timerIntBool = false;
                    break;
                case 50:
                    return 0;
            }
            ++PC;
        }

    }
    
    //Memory
    else if(process == 0)
    {
        Memory memory;
        int * fileRead;
        
        //cli error handling
        try
        {
            memory = Memory(argv[1]);
        }
        catch(std::invalid_argument &e)
        { 
            printf("%s", e.what());
            fileRead = new int(0);
            write(mem_cpu[WRITE], fileRead, sizeof(*fileRead));
        }
        fileRead = new int(1);
        write(mem_cpu[WRITE], fileRead, sizeof(*fileRead));

        int *head = new int;
        int *address = new int;
        int *data = new int;
        while(true)
        {
            read(cpu_mem[READ], head, sizeof(*head));
            //write to memory
            if(*head == WRITE_FLG)
            {
                read(cpu_mem[READ], address, sizeof(*address));
                read(cpu_mem[READ], data, sizeof(*data));

                memory.write(*address, *data);
            }
            //read from memory
            else
            {
                int data_mem = memory.read(*head);
                write(mem_cpu[WRITE], &data_mem, sizeof(data_mem));
            }
        }
    }
    else
    {
        printf("Failed\n");
        return 1;
    }
}

//Check to see if program is illegally accessing data, with system mode
//Call in CPU process only
void checkMemory (int addr, bool k)
{
    if(addr > 999 && !k)
    {
        printf("Memory violation: accessing system address %d in user mode\n", addr); 
        exit(1);
    }

    return;
};
