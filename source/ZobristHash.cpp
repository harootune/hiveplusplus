#include "ZobristHash.h"
#include "PieceInfo.h"
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

void ZobristTable::insert(std::vector<int> coords, int piece, unsigned long int hash)
{
    // std::cout << coords[0] << ", " << coords[1] << ", " << coords[2] << ", " << coords[3] << std::endl; // debug
    _map[coords[0]][coords[1]][coords[2]][coords[3]].insert(std::make_pair(piece, hash));
};

unsigned long int ZobristTable::find(std::vector<int> coords, int piece)
{
    std::map<int, unsigned long int>::iterator it = _map[coords[0]][coords[1]][coords[2]][coords[3]].find(piece);
    if (it == _map[coords[0]][coords[1]][coords[2]][coords[3]].end())
    {
        return 0;
    }
    else
    {
        return it->second;
    };
};

bool ZobristTable::check(std::vector<int> coords)
{
    std::map<int, std::map<int, std::map<int, unsigned long int>>>::iterator it = _map[coords[0]][coords[1]].find(coords[2]);
    return it != _map[coords[0]][coords[1]].end();
}

std::map<int, int> ZobristHash::_defaultConfig
{
    {PieceCodes::WQueen, 1},
    {PieceCodes::WAnt, 3},
    {PieceCodes::WBeetle, 2},
    {PieceCodes::WHopper, 3},
    {PieceCodes::WSpider, 2},
    {PieceCodes::BQueen, 1},
    {PieceCodes::BAnt, 3},
    {PieceCodes::BBeetle, 2},
    {PieceCodes::BHopper, 3},
    {PieceCodes::BSpider, 2}
};

ZobristHash::ZobristHash()
{
    _next = 0;
    _numPieces = 22;
    
    _initTable(_defaultConfig);
};

ZobristHash::ZobristHash(int numPieces)
{
    std::map<int,int> tempConfig
    {
        {PieceCodes::WQueen, numPieces}
    };

    _next = 0;
    _numPieces = numPieces;
    
    _initTable(tempConfig);
};

ZobristHash::ZobristHash(std::map<int, int> pieceConfig)
{
    std::map<int, int>::iterator pieceIt;
    
    _numPieces = 0;
    for (pieceIt = pieceConfig.begin(); pieceIt != pieceConfig.end(); pieceIt++)
    {
        _numPieces += pieceIt->second;
    };

    _next = 0;

    _initTable(pieceConfig);
};

void ZobristHash::_initTable(std::map<int, int> pieceConfig)
{   
    int i;
    std::map<int, int>::iterator piece;

    Position neighborGen;
    std::vector<int> neighborVert;
    std::vector<std::vector<int>> neighbors;
    std::vector<std::vector<int>>::iterator neighborIt;

    QPair current(0, {0, 0, 0, 0});
    std::queue<QPair> depthQ; 
    depthQ.push(current);

    while (!depthQ.empty())
    {
        current = depthQ.front();

        for (piece = pieceConfig.begin(); piece != pieceConfig.end(); piece++)
        {
            _bitTable.insert(current.coords, piece->first, _getNextRand());

            if (piece->first == PieceCodes::WBeetle || piece->first == PieceCodes::BBeetle)
            {
                neighborVert = current.coords;

                // Generate vertical coordinates for beetles
                for (i = 1; i < pieceConfig[PieceCodes::WBeetle] + pieceConfig[PieceCodes::BBeetle]; i++)
                {
                    neighborVert[3] = i;
                    _bitTable.insert(neighborVert, piece->first, _getNextRand());
                };
            };
        };

        if (current.depth < (_numPieces / 2) - 1)  // determines bounds before we have to re-center - adjust as needed
        {
            neighborGen = Position(current.coords);
            neighbors = neighborGen.getAllNeighbors();
            for (neighborIt = neighbors.begin(); neighborIt != neighbors.end(); neighborIt++)
            {
                if (!_bitTable.check(*neighborIt))
                {
                    depthQ.push(QPair(current.depth + 1, *neighborIt));
                };
            };
        };

        depthQ.pop();
    };
};

unsigned long int ZobristHash::_getNextRand()
// This is a PRNG that is essentially an explicit reimplementation of rand() which allows overflows
// See: https://pubs.opengroup.org/onlinepubs/009695399/functions/rand.html
{
    _next = _next * 1103515245 + 12345;
    return _next;
};






