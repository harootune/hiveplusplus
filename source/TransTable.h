#pragma once

#ifndef _TRANSTABLE_
#define _TRANSTABLE_
#include "Move.h"
#include <map>

class TranspositionTable
{
    // memory management will go here
    public:
        std::map<unsigned long int, PositionMove> table;
        PositionMove *find(unsigned long int zHash);
        void insert(unsigned long int zHash, PositionMove &move);
};

#endif