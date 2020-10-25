#pragma once

#ifndef _ZHASH_
#define _ZHASH_
#include "Position.h"
#include <map>
#include <vector>

struct QPair
{
    QPair();
    QPair(int d, std::vector<int> c);

    int depth;
    std::vector<int> coords;
};

class ZobristTable
{
    public:
        void insert(std::vector<int> coords);
        bool find(std::vector<int> coords);
    
    private:
        std::map<int, std::map<int, std::map<int, bool>>>  _map;
};

class ZobristHash
{
    public:
        ZobristHash();
        ZobristHash(int numPieces);

        int getCount();
    
    private:
        int _numPieces;
        ZobristTable _bitTable;

        void _initTable();
};

#endif