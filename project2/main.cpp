#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <queue>

#include "utils.h"

using std::string;

sem_t frontDeskSem;
sem_t bellhopsSem;
sem_t endOfLine;

void *frontDeskEmployee(void *arg)
{
    frontDeskStruct *info = (frontDeskStruct *) arg;

    printf("Front desk employee %d created\n", info->id);

    while(true)
    {
        sem_post(&frontDeskSem);
        
    }
    pthread_exit(NULL);
}

void *guest(void *arg)
{
    guestThreadStruct * info = (guestThreadStruct *) arg;
    printf("Guest %d created\n", info->guest->id);
    info->line->push(info->guest);

    sem_wait(&frontDeskSem);
    printf("Guest %d retires for the evening\n", info->guest->id);
    pthread_exit(NULL);
}

void *bellhop(void *arg)
{
    pthread_exit(NULL);
}


int main(int argc, char** argv)
{
    srand(time(NULL));

    const int NUMBER_OF_GUESTS = 25;
    const int NUMBER_OF_FRONTDESK_EMPLOYEES = 2;
    const int NUMBER_OF_BELLHOPS = 2;

    pthread_t frontDeskThreads[NUMBER_OF_FRONTDESK_EMPLOYEES];
    pthread_t bellhopThreads[NUMBER_OF_BELLHOPS];
    pthread_t guestThreads[NUMBER_OF_GUESTS];

    std::queue<guestInfo *> *line = new std::queue<guestInfo *>();

    bool availableRooms[25] = {false};


    printf("Simulation starts\n");

    sem_init(&frontDeskSem, 0, 2);
    //Creating Front Desk Employee
    frontDeskStruct *employeeStruct;
    for(int i = 0; i < NUMBER_OF_FRONTDESK_EMPLOYEES; ++i)
    {
        employeeStruct = new frontDeskStruct;
        employeeStruct->id = i;
        employeeStruct->roomsAvail = availableRooms;
        employeeStruct->line = line;
        pthread_create(&frontDeskThreads[i], NULL, frontDeskEmployee, (void *) employeeStruct);
    }

    //Creating Bellhops
    sem_init(&bellhopsSem, 0, 2);
    for(int i = 0; i < NUMBER_OF_BELLHOPS; ++i)
    {
        printf("Bellhop %d created\n", i);
        pthread_create(&bellhopThreads[i], NULL, bellhop, NULL);
    }

    sem_init(&endOfLine, 0, 25);
    //Creating Guests
    guestThreadStruct * guestThreadArg;
    for(int i = 0; i < NUMBER_OF_GUESTS; ++i)
    {
        guestThreadArg = new guestThreadStruct;
        guestThreadArg->guest = new guestInfo;
        guestThreadArg->guest->id = i;
        guestThreadArg->guest->bags = rand()%6;
        guestThreadArg->line = line;
        pthread_create(&guestThreads[i], NULL, guest, (void *) guestThreadArg);
    }



    //Joining Front Desk Employee
    for(int i = 0; i < NUMBER_OF_FRONTDESK_EMPLOYEES; ++i)
    {
        pthread_join(frontDeskThreads[i], NULL);
    }

    //Joining Bellhops
    for(int i = 0; i < NUMBER_OF_BELLHOPS; ++i)
    {
        pthread_join(bellhopThreads[i], NULL);
    }

    //Joining Guest threads
    for(int i = 0; i < NUMBER_OF_GUESTS; ++i)
    {
        printf("Guest %d joined\n", i);
        pthread_join(guestThreads[i], NULL);
    }

    printf("Simulation ends\n");

    return EXIT_SUCCESS;
}
