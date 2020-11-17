#pragma once

#ifndef _TRANSTABLE_
#define _TRANSTABLE_
#include <Move.h>
#include <map>
#include <queue>

class TranspositionTable
{
    public:
        std::map<unsigned long int, PositionMove> table;

        PositionMove *find(unsigned long int zHash);
        void insert(unsigned long int zHash, PositionMove &move);
        void setMaxSize(int bytes);
    
    private:
        int _maxSize = 1048576;
        long int _insertionCount = 0;
        std::queue<unsigned long int> _insertionQ;

        void _updateSize(); 

};

#endif