#pragma once

#ifndef _ZHASH_
#define _ZHASH_
#include "Position.h"
#include <map>
#include <vector>


struct QPair
{
    // can be replaced with a distance calculation
    QPair();
    QPair(int d, std::vector<int> c);

    int depth;
    std::vector<int> coords;
};

class ZobristTable
// A table containing 8-byte bit strings describing unique position/piece combinations
{
    public:
        /* Operations on table contents*/
        // Insert a new bit string
        void insert(std::vector<int> coords, int piece, unsigned long int hash);
        // Retrieve a bit string associated with a coord/piece combination
        unsigned long int find(std::vector<int> coords, int piece);
        // Check if a coord/piece combo is present in the table -- maybe unnecessary?
        bool check(std::vector<int> coords, int piece);
    
    private:
        /* Member Variables */
        // The map constituting the table's physical data structure
        std::map<std::vector<int>, unsigned long int>  _map;
};

class ZobristHash
// A class tracking a given Hive game's current hash
{
    public:
        /* Constructors */
        ZobristHash() {};
        ZobristHash(std::map<int, int>);
    
    private:
        /* Member Variables */
        // Total number of pieces - used to determine maximum board radius
        int _numPieces;
        // The current PRNG bitstring
        unsigned long int _next;
        // The ZobristTable tracked by this Hash
        ZobristTable _bitTable;
        
        /* Misc */
        // Incremememt the PRNG
        unsigned long int _getNextRand();
        // Initialize the ZobristTable
        void _initTable(std::map<int, int>);
};

#endif