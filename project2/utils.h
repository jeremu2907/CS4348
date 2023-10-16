#ifndef UTILS_H
#define UTILS_H

#include <queue>

struct guestInfo
{
    unsigned int id;
    unsigned int bags;
    unsigned int room;
};

struct guestThreadStruct
{
    guestInfo * guest;
    std::queue<guestInfo *> *line;
};

struct frontDeskStruct
{
    unsigned int id;
    bool * roomsAvail;
    std::queue<guestInfo *> *line;
};

#endif
