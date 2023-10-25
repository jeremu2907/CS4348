#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <queue>
#include <fcntl.h>

#include "utils.h"

const unsigned int HOTEL_ROOM_NUMBER = 100;
const unsigned int NUMBER_OF_GUESTS = 25;
const unsigned int NUMBER_OF_FRONTDESK_EMPLOYEES = 2;
const unsigned int NUMBER_OF_BELLHOPS = 2;

sem_t *guestInLine;
sem_t *guestInBellhopLine;

sem_t *guestCheckedIn[NUMBER_OF_GUESTS];
sem_t *guestEnteredRoom[NUMBER_OF_GUESTS];
sem_t *bellhopReceived[NUMBER_OF_GUESTS];
sem_t *bellhopDelivered[NUMBER_OF_GUESTS];
sem_t *bellhopTip[NUMBER_OF_GUESTS];

sem_t *guestLineQueueMutex;
sem_t *guestBellhopQueueMutex;
sem_t *frontDeskRoomMutex;

void setUpSem();
void cleanUpSem();

void *frontDeskEmployee(void *arg);
void *guest(void *arg);
void *bellhop(void *arg);

int main(int argc, char **argv)
{
    // Making sure to clean any named semaphores in the system with the same name
    cleanUpSem();
    // Creating semaphores used per each guest threads
    setUpSem();

    srand(time(NULL));

    // Each thread has a special *arg struct to pass data - see utils for structs
    pthread_t frontDeskThreads[NUMBER_OF_FRONTDESK_EMPLOYEES];
    pthread_t bellhopThreads[NUMBER_OF_BELLHOPS];
    pthread_t guestThreads[NUMBER_OF_GUESTS];

    // shared data between threads
    // frontDeskLine is the queue used to store guests lining to be serviced by front desk
    // bellhopLine is the queue used to store guests lining to be serviced by bellhop
    // availableRooms store which room is available that can be looked up - see utils.h
    std::queue<hotelService::guestInfo *> *frontDeskLine = new std::queue<hotelService::guestInfo *>();
    std::queue<hotelService::guestInfo *> *bellhopLine = new std::queue<hotelService::guestInfo *>();
    std::vector<bool> *availableRooms = new std::vector<bool>(HOTEL_ROOM_NUMBER, true);

    printf("Simulation starts\n");

    // Creating Front Desk Employee
    for (int i = 0; i < NUMBER_OF_FRONTDESK_EMPLOYEES; ++i)
    {
        hotelService::frontDeskThreadsArg *frontDeskArg = new hotelService::frontDeskThreadsArg;
        frontDeskArg->id = i;
        frontDeskArg->roomsAvail = availableRooms;
        frontDeskArg->line = frontDeskLine;
        pthread_create(&frontDeskThreads[i], NULL, frontDeskEmployee, (void *)frontDeskArg);
    }

    // Creating Bellhops
    for (int i = 0; i < NUMBER_OF_BELLHOPS; ++i)
    {
        hotelService::bellHopThreadsArg *bellHopArg = new hotelService::bellHopThreadsArg;
        bellHopArg->id = i;
        bellHopArg->serviceLine = bellhopLine;
        pthread_create(&bellhopThreads[i], NULL, bellhop, (void *)bellHopArg);
    }

    // Creating Guests
    for (int i = 0; i < NUMBER_OF_GUESTS; ++i)
    {
        hotelService::guestThreadArg *guestArg = new hotelService::guestThreadArg;
        guestArg->guest.id = i;
        guestArg->guest.bags = rand() % 6;
        guestArg->guest.room = i;
        guestArg->guest.frontDeskService = 1;
        guestArg->line = frontDeskLine;
        guestArg->bellhopLine = bellhopLine;
        pthread_create(&guestThreads[i], NULL, guest, (void *)guestArg);
    }

    // Joining Guest threads
    for (int i = 0; i < NUMBER_OF_GUESTS; ++i)
    {
        pthread_join(guestThreads[i], NULL);
        printf("Guest %d joined\n", i);
    }

    // Cleaning up named semaphores
    cleanUpSem();
    printf("Simulation ends\n");

    return EXIT_SUCCESS;
}

void *frontDeskEmployee(void *arg)
{
    hotelService::frontDeskThreadsArg *args = (hotelService::frontDeskThreadsArg *)arg;
    printf("Front desk employee %d created\n", args->id);

    while (true)
    {
        // Getting the next available room in hand while waiting for next guest
        sem_wait(frontDeskRoomMutex);
        int emptyRoom = hotelService::firstEmptyRoom(args->roomsAvail);
        args->roomsAvail->at(emptyRoom) = false;
        sem_post(frontDeskRoomMutex);

        sem_wait(guestInLine);

        // Serving the guest in front of line
        sem_wait(guestLineQueueMutex);
        hotelService::guestInfo *guest = args->line->front();
        guest->room = emptyRoom + 1; // Room number starts at 1
        guest->frontDeskService = args->id;
        args->line->pop();
        sem_post(guestLineQueueMutex);

        printf("Front desk employee %d registers guest %d and assigns room %d\n", args->id, guest->id, guest->room);

        // Signalling guest they are checked in
        sem_post(guestCheckedIn[guest->id]);
    }

    pthread_exit(NULL);
}

void *guest(void *arg)
{
    hotelService::guestThreadArg *args = (hotelService::guestThreadArg *)arg;
    hotelService::guestInfo *guest = &args->guest;
    printf("Guest %d created\n", guest->id);
    printf("Guest %d enters hotel with %d bag\n", guest->id, guest->bags);

    // Guest enters front desk queue
    sem_wait(guestLineQueueMutex);
    args->line->push(guest);
    sem_post(guestLineQueueMutex);

    // Signalling that guest is in line to front desk
    sem_post(guestInLine);
    sem_wait(guestCheckedIn[guest->id]);
    printf("Guest %d receives room key for room %d from front desk employee %d\n", guest->id, guest->room, guest->frontDeskService);

    // Requiring bellhop service
    if (guest->bags > 2)
    {
        // Enters bellhop queue
        sem_wait(guestBellhopQueueMutex);
        args->bellhopLine->push(guest);
        printf("Guest %d requests help with bags\n", guest->id);
        sem_post(guestBellhopQueueMutex);

        // Signalling belhop guest waiting
        sem_post(guestInBellhopLine);

        // Once bellhop receive bags, guest enters room
        sem_wait(bellhopReceived[guest->id]);
        printf("Guest %d enters room %d\n", guest->id, guest->room);
        sem_post(guestEnteredRoom[guest->id]);

        // Once bellhop delivers bag, guest gives tip
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
    hotelService::bellHopThreadsArg *args = (hotelService::bellHopThreadsArg *)arg;
    printf("Bellhop %d created\n", args->id);

    while (true)
    {
        // Waiting for next guest in line
        sem_wait(guestInBellhopLine);

        // Extracting guest data
        sem_wait(guestBellhopQueueMutex);
        hotelService::guestInfo *guest = args->serviceLine->front();
        args->serviceLine->pop();
        sem_post(guestBellhopQueueMutex);
        guest->bellhopService = args->id;

        printf("Bellhop %d receives bags from guest %d\n", args->id, guest->id);
        sem_post(bellhopReceived[guest->id]);

        // Wait til guest enter room then deliver bag
        sem_wait(guestEnteredRoom[guest->id]);

        // Signaling guest bag delivered
        printf("Bellhop %d delivers bags to guest %d\n", args->id, guest->id);
        sem_post(bellhopDelivered[guest->id]);
        sem_wait(bellhopTip[guest->id]);
    }

    pthread_exit(NULL);
}

// Settingup named sems
void setUpSem()
{
    guestInLine = sem_open("/guestInLine", O_CREAT, 0644, 0);
    guestInBellhopLine = sem_open("/guestInBellhopLine", O_CREAT, 0644, 0);

    guestLineQueueMutex = sem_open("/guestLineQueueMutex", O_CREAT, 0644, 1);
    guestBellhopQueueMutex = sem_open("/guestBellhopQueueMutex", O_CREAT, 0644, 1);
    frontDeskRoomMutex = sem_open("/frontDeskRoomMutex", O_CREAT, 0644, 1);

    // Each guest threads have separate semaphores at different stage of checking in to ensure threads
    // dont signal other threads unintentionally
    for (int i = 0; i < NUMBER_OF_GUESTS; i++)
    {
        guestCheckedIn[i] = sem_open(("/guestCheckedIn" + std::to_string(i)).c_str(), O_CREAT, 0644, 0);
        guestEnteredRoom[i] = sem_open(("/guestEnteredRoom" + std::to_string(i)).c_str(), O_CREAT, 0644, 0);
        bellhopReceived[i] = sem_open(("/bellhopReceived" + std::to_string(i)).c_str(), O_CREAT, 0644, 0);
        bellhopTip[i] = sem_open(("/bellhopTip" + std::to_string(i)).c_str(), O_CREAT, 0644, 0);
        bellhopDelivered[i] = sem_open(("/bellhopDelivered" + std::to_string(i)).c_str(), O_CREAT, 0644, 0);
    }
}

// Cleaning named sems
void cleanUpSem()
{
    sem_unlink("/frontDeskSem");
    sem_unlink("/guestInLine");
    sem_unlink("/guestLineQueueMutex");
    sem_unlink("/frontDeskRoomMutex");
    sem_unlink("/guestBellhopQueueMutex");
    sem_unlink("/guestInBellhopLine");

    for (int i = 0; i < NUMBER_OF_GUESTS; i++)
    {
        sem_unlink((("/guestCheckedIn" + std::to_string(i)).c_str()));
        sem_unlink(("/guestEnteredRoom" + std::to_string(i)).c_str());
        sem_unlink(("/bellhopReceived" + std::to_string(i)).c_str());
        sem_unlink(("/bellhopTip" + std::to_string(i)).c_str());
        sem_unlink(("/bellhopDelivered" + std::to_string(i)).c_str());
    }
}
