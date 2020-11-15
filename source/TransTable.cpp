#include "TransTable.h"

PositionMove *TranspositionTable::find(unsigned long int zHash)
{
    try
    {
        return &table.at(zHash);
    }
    catch (std::out_of_range &e)
    {
        return nullptr;
    };
};

void TranspositionTable::insert(unsigned long int zHash, PositionMove &move)
{
    table[zHash] = move;
};