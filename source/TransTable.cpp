#include <TransTable.h>
#include <iostream> // also debug


TranspositionTable::TranspositionTable()
{
    _maxSize = 1048576;
    _insertionCount = 0;
};


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
    _insertionCount++;
    table[zHash] = move;
    _insertionQ.push(zHash);
    _updateSize();
};


void TranspositionTable::setMaxSize(int bytes)
{
    _maxSize = bytes;
    _updateSize();
};


void TranspositionTable::_updateSize()
// This ignores a certain amount of overhead inherent to the table
{
    int perMove = 16 + sizeof(PositionMove); // each entry requires 2 unsigned long ints + the size of a move
    int currentSize = _insertionCount * perMove;

    // DEBUG
    // std::cout << currentSize <<std::endl;

    while (currentSize > _maxSize)
    {
        table.erase(_insertionQ.front());
        _insertionQ.pop();
        _insertionCount--;
        currentSize = _insertionCount * perMove;
    };

    // DEBUG
    // std::cout << currentSize <<std::endl;
};
