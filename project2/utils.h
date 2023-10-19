#ifndef UTILS_H
#define UTILS_H

#include <queue>
#include <iostream>
#include <vector>

struct guestInfo
{
    unsigned int id;
    unsigned int bags;
    unsigned int room;
    unsigned int frontDeskService;
    unsigned int bellhopService;
};

struct guestThreadArg
{
    guestInfo guest;
    std::queue<guestInfo *> *line;
    std::queue<guestInfo *> *bellhopLine;
};

struct frontDeskThreadsArg
{
    unsigned int id;
    bool endShift = false;
    std::vector<bool> * roomsAvail;
    std::queue<guestInfo *> *line;
};

struct bellHopThreadsArg
{
    unsigned int id;
    bool endShift = false;
    std::queue<guestInfo *> *line;
    std::queue<guestInfo *> *serviceLine;
    std::vector<bool> * roomsAvail;
};

namespace hotelService{

    int firstEmptyRoom(std::vector<bool> *roomList)
    {
        for(int i = 0; i < roomList->size(); i++)
        {
            if(roomList->at(i) == true)
            {
                return i;
            }
        }

        return -1;
    }

    int occupiedRooms(std::vector<bool> *roomList)
    {
        int num = 0;
        for(auto i : *roomList)
        {
            if(i == false)
            {
                num++;
            }
        }

        return num;
    }
}


#endif
