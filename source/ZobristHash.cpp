#include "ZobristHash.h"
#include <iostream>
#include <queue>

QPair::QPair()
{
    depth = 0;
    coords = {0, 0, 0};
};

QPair::QPair(int d, std::vector<int> c)
{
    depth = d;
    coords = c;
};

ZobristHash::ZobristHash()
{
    _numPieces = 11;
    _initTable();
};

ZobristHash::ZobristHash(int numPieces)
{
    _numPieces = numPieces;
    _initTable();
};

void ZobristHash::_initTable()
{   
    Position neighborGen;
    std::vector<std::vector<int>> neighbors;
    std::vector<std::vector<int>>::iterator neighborIt;

    QPair current(0, {0, 0, 0});
    std::queue<QPair> depthQ; 
    depthQ.push(current);

    while (!depthQ.empty())
    {
        current = depthQ.front();
        _bitTable.insert(current.coords);

        if (current.depth < _numPieces - 1)
        {
            neighborGen = Position(current.coords);
            neighbors = neighborGen.getAllNeighbors();
            for (neighborIt = neighbors.begin(); neighborIt < neighbors.end(); neighborIt++)
            {
                if (!_bitTable.find(*neighborIt))
                {
                    depthQ.push(QPair(current.depth + 1, *neighborIt));
                };
            };
        }

        depthQ.pop();
    };
};

void ZobristTable::insert(std::vector<int> coords)
{
    std::cout << coords[0] << ", " << coords[1] << ", " << coords[2] << std::endl; // debug
    _map[coords[0]][coords[1]].insert(std::make_pair(coords[2], true));
};

bool ZobristTable::find(std::vector<int> coords)
{
    std::map<int, bool>::iterator it = _map[coords[0]][coords[1]].find(coords[2]);
    return it != _map[coords[0]][coords[1]].end();
};





