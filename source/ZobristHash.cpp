#include "ZobristHash.h"
#include "PieceInfo.h"
#include <iostream>
#include <queue>

std::vector<int> concatCoords(std::vector<int> coords, int piece)
{
    std::vector<int> tempCoords = coords;
    tempCoords.push_back(piece);
    
    return tempCoords;
};

QPair::QPair()
{
    depth = 0;
    coords = {0, 0, 0, 0};
};

QPair::QPair(int d, std::vector<int> c)
{
    depth = d;
    coords = c;
};

void ZobristTable::insert(std::vector<int> coords, int piece, unsigned long int hash)
{
    // std::cout << coords[0] << ", " << coords[1] << ", " << coords[2] << ", " << coords[3] << std::endl; // debug
    std::vector<int> tempCoords = concatCoords(coords, piece);
    _map.insert({tempCoords, hash});
};

unsigned long int ZobristTable::find(std::vector<int> coords, int piece)
{

    std::vector<int> tempCoords = concatCoords(coords, piece);
    std::map<std::vector<int>, unsigned long int>::iterator it = _map.find(tempCoords);

    if (it == _map.end())
    {
        return 0;
    }
    else
    {
        return it->second;
    };
};

bool ZobristTable::check(std::vector<int> coords, int piece)
{
    std::vector<int> tempCoords = concatCoords(coords, piece);
    std::map<std::vector<int>, unsigned long int>::iterator it = _map.find(tempCoords);

    return it != _map.end();
}

std::map<int, int> ZobristHash::_defaultConfig
{
    {PieceCodes::wQ, 1},
    {PieceCodes::wA, 3},
    {PieceCodes::wB, 2},
    {PieceCodes::wG, 3},
    {PieceCodes::wS, 2},
    {PieceCodes::bQ, 1},
    {PieceCodes::bA, 3},
    {PieceCodes::bB, 2},
    {PieceCodes::bG, 3},
    {PieceCodes::bS, 2}
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
        {PieceCodes::wQ, numPieces}
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
    int firstPiece = pieceConfig.begin()->first;

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

            if (piece->first == PieceCodes::wB || piece->first == PieceCodes::bB)
            {
                neighborVert = current.coords;

                // Generate vertical coordinates for beetles
                for (i = 1; i <= pieceConfig[PieceCodes::wB] + pieceConfig[PieceCodes::bB]; i++)
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
                if (!_bitTable.check(*neighborIt, firstPiece))
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






