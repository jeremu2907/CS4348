#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <queue>
#include <fcntl.h>

#include "utils.h"

const unsigned int HOTEL_ROOM_NUMBER = 100;
const int NUMBER_OF_GUESTS = 25;
const int NUMBER_OF_FRONTDESK_EMPLOYEES = 2;
const int NUMBER_OF_BELLHOPS = 2;

sem_t * guestInLine = sem_open("/guestInLine", O_CREAT, 0644, 0);
sem_t * guestInBellhopLine = sem_open("/guestInBellhopLine", O_CREAT, 0644, 0);

sem_t * guestCheckedIn[NUMBER_OF_GUESTS];
sem_t * guestEnteredRoom[NUMBER_OF_GUESTS];
sem_t * bellhopReceived[NUMBER_OF_GUESTS];
sem_t * bellhopDelivered[NUMBER_OF_GUESTS];
sem_t * bellhopTip[NUMBER_OF_GUESTS];

sem_t * guestLineQueueMutex = sem_open("/guestLineQueueMutex", O_CREAT, 0644, 1);
sem_t * guestBellhopQueueMutex = sem_open("/guestBellhopQueueMutex", O_CREAT, 0644, 1);
sem_t * frontDeskRoomMutex = sem_open("/frontDeskRoomMutex", O_CREAT, 0644, 1);

void createArrSem();
void cleanUp();
void closeSem();

void * frontDeskEmployee(void *arg);
void * guest(void *arg);
void * bellhop(void *arg);

int main(int argc, char** argv)
{
    cleanUp();

    srand(time(NULL));

    pthread_t frontDeskThreads[NUMBER_OF_FRONTDESK_EMPLOYEES];
    pthread_t bellhopThreads[NUMBER_OF_BELLHOPS];
    pthread_t guestThreads[NUMBER_OF_GUESTS];

    createArrSem();

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
        guestArg->guest.bags = rand() % 6;
        guestArg->guest.room = i;
        guestArg->guest.frontDeskService = 1;
        guestArg->line = frontDeskLine;
        guestArg->bellhopLine = bellhopLine;
        pthread_create(&guestThreads[i], NULL, guest, (void *) guestArg);
    }

    //Joining Guest threads
    for(int i = 0; i < NUMBER_OF_GUESTS; ++i)
    {
        pthread_join(guestThreads[i], NULL);
        printf("Guest %d joined\n", i);
    }

    cleanUp();

    printf("Simulation ends\n");

    return EXIT_SUCCESS;
}


void *frontDeskEmployee(void *arg)
{
    frontDeskThreadsArg * args = (frontDeskThreadsArg *) arg;
    printf("Front desk employee %d created\n", args->id);

    while(true)
    {   
        
        sem_wait(frontDeskRoomMutex);
        int emptyRoom = hotelService::firstEmptyRoom(args->roomsAvail);
        args->roomsAvail->at(emptyRoom) = false;
        sem_post(frontDeskRoomMutex);

        sem_wait(guestInLine);

        sem_wait(guestLineQueueMutex);
        guestInfo * guest = args->line->front();
        guest->room = emptyRoom + 1;
        guest->frontDeskService = args->id;
        args->line->pop();
        sem_post(guestLineQueueMutex);

        printf("Front desk employee %d registers guest %d and assigns room %d\n", args->id, guest->id, guest->room);

        sem_post(guestCheckedIn[guest->id]);
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
    sem_wait(guestCheckedIn[guest->id]);
    printf("Guest %d receives room key for room %d from front desk employee %d\n", guest->id, guest->room, guest->frontDeskService);

    if(guest->bags > 2)
    {
        sem_wait(guestBellhopQueueMutex);
        args->bellhopLine->push(guest);
        printf("Guest %d requests help with bags\n", guest->id);
        sem_post(guestBellhopQueueMutex);

        sem_post(guestInBellhopLine);

        sem_wait(bellhopReceived[guest->id]);
        printf("Guest %d enters room %d\n", guest->id, guest->room);
        sem_post(guestEnteredRoom[guest->id]);

        sem_wait(bellhopDelivered[guest->id]);
        printf("Guest %d receives bags from bellhop %d and gives tip\n", guest->id, guest->bellhopService);

        sem_post(bellhopTip[guest->id]);
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
        sem_wait(guestInBellhopLine);

        sem_wait(guestBellhopQueueMutex);
        guestInfo * guest = args->serviceLine->front();
        args->serviceLine->pop();
        sem_post(guestBellhopQueueMutex);
        guest->bellhopService = args->id;

        printf("Bellhop %d receives bags from guest %d\n", args->id, guest->id);
        sem_post(bellhopReceived[guest->id]);

        sem_wait(guestEnteredRoom[guest->id]);

        printf("Bellhop %d delivers bags to guest %d\n", args->id, guest->id);
        sem_post(bellhopDelivered[guest->id]);

        sem_wait(bellhopTip[guest->id]);
    }

    pthread_exit(NULL);
}

void createArrSem()
{
    for(int i = 0; i < NUMBER_OF_GUESTS; i++)
    {
        guestCheckedIn[i] = sem_open(("/guestCheckedIn" + std::to_string(i)).c_str(), O_CREAT, 0644, 0);
        guestEnteredRoom[i] = sem_open(("/guestEnteredRoom" + std::to_string(i)).c_str(), O_CREAT, 0644, 0);
        bellhopReceived[i] = sem_open(("/bellhopReceived" + std::to_string(i)).c_str(), O_CREAT, 0644, 0);
        bellhopTip[i] = sem_open(("/bellhopTip" + std::to_string(i)).c_str(), O_CREAT, 0644, 0);
        bellhopDelivered[i] = sem_open(("/bellhopDelivered" + std::to_string(i)).c_str(), O_CREAT, 0644, 0);
    }
}

void cleanUp()
{
    sem_unlink("/frontDeskSem");
    sem_unlink("/guestInLine");
    sem_unlink("/guestLineQueueMutex");
    sem_unlink("/frontDeskRoomMutex");
    sem_unlink("/guestBellhopQueueMutex");
    sem_unlink("/guestInBellhopLine");

    for(int i = 0; i < NUMBER_OF_GUESTS; i++)
    {
        sem_unlink((("/guestCheckedIn" + std::to_string(i)).c_str()));
        sem_unlink(("/guestEnteredRoom" + std::to_string(i)).c_str());
        sem_unlink(("/bellhopReceived" + std::to_string(i)).c_str());
        sem_unlink(("/bellhopTip" + std::to_string(i)).c_str());
        sem_unlink(("/bellhopDelivered" + std::to_string(i)).c_str());
    }
}

void closeSem()
{
    sem_close(guestInLine);
    sem_close(guestLineQueueMutex);
    sem_close(frontDeskRoomMutex);
    sem_close(guestBellhopQueueMutex);
    sem_close(guestInBellhopLine);

    for(int i; i < NUMBER_OF_GUESTS; i++)
    {
        sem_close(guestCheckedIn[i]);
        sem_close(guestEnteredRoom[i]);
        sem_close(bellhopReceived[i]);
        sem_close(bellhopTip[i]);
        sem_close(bellhopDelivered[i]);
    }
}
