#ifndef UTILS_H
#define UTILS_H

#include <queue>
#include <iostream>
#include <vector>

namespace hotelService
{

    // Holds guest info and which employee served them
    struct guestInfo
    {
        unsigned int id;
        unsigned int bags;
        unsigned int room;
        unsigned int frontDeskService;
        unsigned int bellhopService;
    };

    // Args for guest threads with front desk and bellhop queues
    struct guestThreadArg
    {
        guestInfo guest;
        std::queue<guestInfo *> *line;
        std::queue<guestInfo *> *bellhopLine;
    };

    // Args for front desk threads with front desk queues and room availability data
    struct frontDeskThreadsArg
    {
        unsigned int id;
        std::vector<bool> *roomsAvail;
        std::queue<guestInfo *> *line;
    };

    // Args for guest thread with and bellhop queues
    struct bellHopThreadsArg
    {
        unsigned int id;
        std::queue<guestInfo *> *serviceLine;
    };

    // returns index of first empty room
    int firstEmptyRoom(std::vector<bool> *roomList)
    {
        for (int i = 0; i < roomList->size(); i++)
        {
            if (roomList->at(i) == true)
            {
                return i;
            }
        }

        return -1;
    }
}

#endif
