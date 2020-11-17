#include <TransTable.h>
#include <iostream> // also debug

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
{
    int perMove = 16 + sizeof(PositionMove);
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
