#include <ZobristHash.h>
#include <PieceInfo.h>
#include <Utils.h>
#include <iostream>
#include <queue>


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
    std::vector<int> tempCoords = Utils::concatCoords(coords, piece);
    _map.insert({tempCoords, hash});
};

unsigned long int ZobristTable::find(std::vector<int> coords, int piece)
{

    std::vector<int> tempCoords = Utils::concatCoords(coords, piece);
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
    std::vector<int> tempCoords = Utils::concatCoords(coords, piece);
    std::map<std::vector<int>, unsigned long int>::iterator it = _map.find(tempCoords);

    return it != _map.end();
};

ZobristHash::ZobristHash(std::map<int, int> pieceConfig)
{
    std::map<int, int>::iterator pieceIt;

    hash = 6543;
    
    _numPieces = 0;
    for (pieceIt = pieceConfig.begin(); pieceIt != pieceConfig.end(); pieceIt++)
    {
        _numPieces += pieceIt->second;
    };

    _next = 0;
    _depth = 0;
    _white = _getNextRand();

    _initTable(pieceConfig);
};

void ZobristHash::invertPiece(std::vector<int> coordinates, int code)
{
    unsigned long int targetHash = _bitTable.find(coordinates, code);
    if (targetHash != 0)
    {
        hash ^= targetHash;
    }
    else
    {
        // DEBUG - final version should throw an error
        std::cout << "err Zobrist hash miss." << std::endl;
    };
};

void ZobristHash::invertColor()
{
    hash ^= _white;
};

void ZobristHash::changeDepth(int depth)
{
    hash ^= _depth;
    hash ^= depth;
    _depth = depth;
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

    // While there are nodes in the queue
    while (!depthQ.empty())
    {
        current = depthQ.front();

        // for each piece in the piececonfig
        for (piece = pieceConfig.begin(); piece != pieceConfig.end(); piece++)
        {
            _bitTable.insert(current.coords, piece->first, _getNextRand());

            // if the piece is a beetle, we need to generate vertical coordinates up to the number of beetles
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

        // if current depth of the search is less than the depth limit of the search, continue the search across all neighbors
        // This is where we can replace depth with a distance check using max absolute value 
        if (current.depth < (_numPieces / 2) - 1)  
        {
            neighborGen = Position(current.coords);
            neighbors = neighborGen.getAllNeighbors();
            for (std::vector<int> neighbor: neighbors)
            {
                if (!_bitTable.check(neighbor, firstPiece))
                {
                    depthQ.push(QPair(current.depth + 1, neighbor));
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






