#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <queue>
#include <fcntl.h>

#include "utils.h"

using std::string;

sem_t * frontDeskSem = sem_open("/frontDeskSem", O_CREAT, 0644, 0);
sem_t * guestInLine = sem_open("/guestInLine", O_CREAT, 0644, 0);
sem_t * bellhopSem = sem_open("/bellhopSem", O_CREAT, 0644, 0);
sem_t * guestInBellhopLine = sem_open("/guestInBellhopLine", O_CREAT, 0644, 0);
sem_t * guestEnteredRoom = sem_open("/guestEnteredRoom", O_CREAT, 0644, 0);
sem_t * bellhopReceived = sem_open("/bellhopReceived", O_CREAT, 0644, 0);
sem_t * bellhopTip = sem_open("/bellhopTip", O_CREAT, 0644, 0);

sem_t * guestLineQueueMutex = sem_open("/guestLineQueueMutex", O_CREAT, 0644, 1);
sem_t * guestBellhopQueueMutex = sem_open("/guestBellhopQueueMutex", O_CREAT, 0644, 1);
sem_t * frontDeskRoomMutex = sem_open("/frontDeskRoomMutex", O_CREAT, 0644, 1);


const unsigned int HOTEL_ROOM_NUMBER = 100;
const int NUMBER_OF_GUESTS = 25;
const int NUMBER_OF_FRONTDESK_EMPLOYEES = 2;
const int NUMBER_OF_BELLHOPS = 2;


void *frontDeskEmployee(void *arg)
{
    frontDeskThreadsArg * args = (frontDeskThreadsArg *) arg;
    printf("Front desk employee %d created\n", args->id);

    while(true)
    {   
        
        sem_wait(frontDeskRoomMutex);
        int emptyRoom = hotelService::firstEmptyRoom(args->roomsAvail);
        args->roomsAvail->at(emptyRoom) = false;
        if(hotelService::occupiedRooms(args->roomsAvail) > NUMBER_OF_GUESTS)
        {
            sem_post(frontDeskRoomMutex);
            // sem_post(frontDeskSem);
            break;
        }
        sem_post(frontDeskRoomMutex);

        sem_wait(guestInLine);

        sem_wait(guestLineQueueMutex);
        guestInfo * guest = args->line->front();
        guest->room = emptyRoom + 1;
        guest->frontDeskService = args->id;
        args->line->pop();
        sem_post(guestLineQueueMutex);

        printf("Front desk employee %d registers guest %d and assigns room %d\n", args->id, guest->id, guest->room);

        sem_post(frontDeskSem);
    }

    pthread_exit(NULL);
}

void *guest(void *arg)
{
    guestThreadArg * args = (guestThreadArg *) arg;
    guestInfo * guest = &args->guest;
    printf("Guest %d created\n", guest->id);
    printf("Guest %d enters hotel with %d bag\n", guest->id, guest->bags);


    sem_wait(guestLineQueueMutex);
    args->line->push(guest);
    sem_post(guestLineQueueMutex);

    sem_post(guestInLine);
    sem_wait(frontDeskSem);
    
    printf("Guest %d receives room key for room %d from front desk employee %d\n", guest->id, guest->room, guest->frontDeskService);

    if(guest->bags > 2)
    {
        sem_wait(guestBellhopQueueMutex);
        args->bellhopLine->push(guest);
        printf("Guest %d requests help with bags\n", guest->id);
        sem_post(guestBellhopQueueMutex);

        sem_post(guestInBellhopLine);
        sem_wait(bellhopReceived);

        sem_post(guestEnteredRoom);
        printf("Guest %d enters room %d\n", guest->id, guest->room);

        sem_wait(bellhopSem);

        sem_post(bellhopTip);
        printf("Guest %d receives bags from bellhop %d and gives tip\n", guest->id, guest->bellhopService);
    }
    else
    {
        printf("Guest %d enters room %d\n", guest->id, guest->room);
    }
    
    printf("Guest %d retires for the evening\n", guest->id);
    pthread_exit(NULL);
}

void *bellhop(void *arg)
{
    bellHopThreadsArg * args = (bellHopThreadsArg *) arg;
    printf("Bellhop %d created\n", args->id);

    while(true)
    {   
        // sem_wait(frontDeskRoomMutex);
        // sem_wait(guestBellhopQueueMutex);
        // sem_wait(guestLineQueueMutex);
        if(
            (hotelService::occupiedRooms(args->roomsAvail) > NUMBER_OF_GUESTS) &&
            args->line->empty() &&
            args->serviceLine->empty()
        )
        {
            // sem_post(guestLineQueueMutex);
            // sem_post(guestBellhopQueueMutex);
            // sem_post(frontDeskRoomMutex);
            break;
        }
        // sem_post(guestLineQueueMutex);
        // sem_post(guestBellhopQueueMutex);
        // sem_post(frontDeskRoomMutex);
        
        sem_wait(guestInBellhopLine);

        sem_wait(guestBellhopQueueMutex);
        guestInfo * guest = args->serviceLine->front();
        args->serviceLine->pop();
        sem_post(guestBellhopQueueMutex);
        guest->bellhopService = args->id;

        sem_post(bellhopReceived);
        printf("Bellhop %d receives bags from guest %d\n", args->id, guest->id);

        sem_wait(guestEnteredRoom);

        sem_post(bellhopSem);
        printf("Bellhop %d delivers bags to guest %d\n", args->id, guest->id);

        sem_wait(bellhopTip);
    }

    pthread_exit(NULL);
}

int main(int argc, char** argv)
{
    srand(time(NULL));

    pthread_t frontDeskThreads[NUMBER_OF_FRONTDESK_EMPLOYEES];
    pthread_t bellhopThreads[NUMBER_OF_BELLHOPS];
    pthread_t guestThreads[NUMBER_OF_GUESTS];

    std::queue<guestInfo *> *frontDeskLine = new std::queue<guestInfo *>();
    std::queue<guestInfo *> *bellhopLine = new std::queue<guestInfo *>();
    std::vector<bool> *availableRooms = new std::vector<bool>(HOTEL_ROOM_NUMBER, true);

    printf("Simulation starts\n");

    //Creating Front Desk Employee
    for(int i = 0; i < NUMBER_OF_FRONTDESK_EMPLOYEES; ++i)
    {
        frontDeskThreadsArg *frontDeskArg = new frontDeskThreadsArg;
        frontDeskArg->id = i;
        frontDeskArg->roomsAvail = availableRooms;
        frontDeskArg->line = frontDeskLine;
        pthread_create(&frontDeskThreads[i], NULL, frontDeskEmployee, (void *) frontDeskArg);
    }

    // Creating Bellhops
    for(int i = 0; i < NUMBER_OF_BELLHOPS; ++i)
    {
        bellHopThreadsArg * bellHopArg = new bellHopThreadsArg;
        bellHopArg->id = i;
        bellHopArg->serviceLine = bellhopLine;
        bellHopArg->line = frontDeskLine;
        bellHopArg->roomsAvail = availableRooms;
        pthread_create(&bellhopThreads[i], NULL, bellhop, (void *) bellHopArg);
    }

    //Creating Guests
    for(int i = 0; i < NUMBER_OF_GUESTS; ++i)
    {
        guestThreadArg * guestArg = new guestThreadArg;
        guestArg->guest.id = i;
        guestArg->guest.bags = 6 * rand();
        guestArg->guest.room = i;
        guestArg->guest.frontDeskService = 1;
        guestArg->line = frontDeskLine;
        guestArg->bellhopLine = bellhopLine;
        pthread_create(&guestThreads[i], NULL, guest, (void *) guestArg);
    }

    //Joining Front Desk Employee
    for(int i = 0; i < NUMBER_OF_FRONTDESK_EMPLOYEES; ++i)
    {
        pthread_join(frontDeskThreads[i], NULL);
    }

    // Joining Bellhops
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

    sem_unlink("/frontDeskSem");
    sem_unlink("/guestInLine");
    sem_unlink("/guestLineQueueMutex");
    sem_unlink("/frontDeskRoomMutex");
    sem_unlink("/guestBellhopQueueMutex");
    sem_unlink("/bellhopSem");
    sem_unlink("/guestInBellhopLine");
    sem_unlink("/guestEnteredRoom");
    sem_unlink("/bellhopReceived");
    sem_unlink("/bellhopTip");

    sem_close(frontDeskSem);
    sem_close(guestInLine);
    sem_close(guestLineQueueMutex);
    sem_close(frontDeskRoomMutex);
    sem_close(bellhopReceived);
    sem_close(guestBellhopQueueMutex);
    sem_close(bellhopSem);
    sem_close(guestInBellhopLine);
    sem_close(guestEnteredRoom);
    sem_close(bellhopTip);

    return EXIT_SUCCESS;
}
