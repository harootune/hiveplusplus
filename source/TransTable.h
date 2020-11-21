#pragma once

#ifndef _TRANSTABLE_
#define _TRANSTABLE_
#include <Move.h>
#include <map>
#include <queue>


class TranspositionTable
// A transposition table holding best moves found at different transpositions and search depths
{
    public:
        /* Member Variables */
        // The map which holds best moves
        std::map<unsigned long int, PositionMove> table;

        /* Constructor */
        TranspositionTable();

        /* Table Manipualtion */
        // Find a move in the transposition table given a zobrist hash. Returns nullptr if none.
        PositionMove *find(unsigned long int zHash);
        // Insert a PositionMove at the specified zHash
        void insert(unsigned long int zHash, PositionMove &move);
        // Set the maximum size of the table in bytes
        void setMaxSize(int bytes);
    
    private:
        /* Member Variables */
        // Maximum size of the table in bytes
        int _maxSize;
        // The number of moves currently in the table
        long int _insertionCount;
        // A queue of keys inserted into the table in chronological order
        std::queue<unsigned long int> _insertionQ;

        /* Misc */
        // Reduce the tables size to the value specified in _maxSize
        void _updateSize(); 

};

#endif