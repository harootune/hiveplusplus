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
        void insert(std::vector<int> coords, int piece, unsigned long int hash);
        unsigned long int find(std::vector<int> coords, int piece);
        bool check(std::vector<int> coords);
    
    private:
        std::map<int, std::map<int, std::map<int, std::map<int, std::map<int, unsigned long int>>>>>  _map;
};

class ZobristHash
{
    public:
        ZobristHash();
        ZobristHash(int numPieces);
        ZobristHash(std::map<int, int>);

        int getCount();
    
    private:
        int _numPieces;
        unsigned long int _next;
        ZobristTable _bitTable;

        unsigned long int _getNextRand();
        void _initTable(std::map<int, int>);

        // temporary piece config for debugging, REMOVE
        static std::map<int, int> _defaultConfig;
};

#endif