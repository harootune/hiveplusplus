#pragma once

#ifndef _ZHASH_
#define _ZHASH_
#include <Position.h>
#include <map>
#include <vector>


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
        /* Member Variables */
        // The maximum radius of the board that the hash value can track
        int radius;
        // The current hash value stored in this hash object
        unsigned long int hash;

        /* Constructors */
        ZobristHash() { hash = 6543; };
        ZobristHash(std::map<int, int>);

        /* Hash Manipulators */
        // Turn a piece at a certain location on or off
        void invertPiece(std::vector<int> coordinates, int code);
        // invert the active player hash (switch between white and black)
        void invertColor();
        // change the depth of the active hash
        void changeDepth(int depth);
        
    private:
        /* Member Variables */
        // The current PRNG bitstring
        unsigned long int _next;
        // The current depth of the hash
        int _depth;
        // The hash value associated with the white player - if off, current player is black
        unsigned long int _white;
        // The ZobristTable composited by this Hash
        ZobristTable _bitTable;
        
        /* Misc */
        // Incremememt the PRNG
        unsigned long int _getNextRand();
        // Initialize the ZobristTable
        void _initTable(std::map<int, int>);
};

#endif