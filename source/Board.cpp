#include "Board.h"
#include "PieceInfo.h"
#include <iostream>

Board::Board()
{
    _turn = 0;
    _white = true;   
};

Board::Board(std::map<int, int> pieceConfig)
{
    _turn = 0;
    _white = true;   
    _pieceConfig = pieceConfig;
};

void Board::makeMove(Move move)
{
    // std::vector<std::string> code = _splitMoveCode(moveCode);
    
    // if (_validateMove(code))
    // {
    //     // move behavior
    // }
    // else
    // {
    //     // throw some error
    // };
    _pieces.update(move);
    _turn++;
    _white = !_white;
};

void Board::undoLast()
{
    _pieces.undoLast();
    _turn--;
    _white = !_white;
};

bool Board::_validateMove(std::vector<std::string> move)
{
    return true;
};

std::set<std::vector<int>> Board::_oneHiveCheck(std::vector<int> start)
{
    _OneHiveInfo info;

    // this will break if there are no pieces on the board
    _oneHiveSearch(start, &info);

    return info.articulations;
};

void Board::_oneHiveSearch(std::vector<int> &location, _OneHiveInfo *info)
// This is an implementation of the articulation vertex discovery algorithm described in Skiena
{  
    Position current(location);
    std::vector<std::vector<int>> edges = _pieces.adjacencies(&current);
    std::vector<std::vector<int>>::iterator edgeIt;
    
    // early processing
    info->time++;
    info->entryTime[location] = info->time;
    info->discovered[location] = true;
    info->earliestAncestor[location] = location;

    // edge processing
    for (edgeIt = edges.begin(); edgeIt != edges.end(); edgeIt++)
    {
        if (!info->discovered[*edgeIt])
        {
            info->parent[*edgeIt] = location;
            _oneHiveEdge(location, *edgeIt, info);
            _oneHiveSearch(*edgeIt, info);
        }
        else if (!info->processed[*edgeIt] && info->parent[location] != *edgeIt)
        {
            _oneHiveEdge(location, *edgeIt, info);
        };
    };

    // late processing
    if (location == info->root)
    {
        if (info->outDegree[location] > 1)
        {
            info->articulations.insert(location);
        }; 
    }
    else if (info->parent[location] != info->root)
    {
        if (info->earliestAncestor[location] == info->parent[location])
        {
            info->articulations.insert(info->parent[location]);
        }
        else if (info->earliestAncestor[location] == location)
        {
            info->articulations.insert(info->parent[location]);

            if (info->outDegree[location] > 0)
            {
                info->articulations.insert(location);
            };
        };
    };

    // back up ancestors
    // if the earliest reachable ancestor of our current node is earlier than the node's parent
    // then parent's earliest reachable ancestor is this node's earliest reachable ancestor
    if (info->entryTime[info->earliestAncestor[location]] <
        info->entryTime[info->earliestAncestor[info->parent[location]]])
    {
        info->earliestAncestor[info->parent[location]] = info->earliestAncestor[location];
    };

    info->time++;
    info->processed[location] = true;
};

void Board::_oneHiveEdge(std::vector<int> &from, std::vector<int> &to, _OneHiveInfo *info)
{
    int edgeClass = _oneHiveClass(from, to, info);

    if (edgeClass == 0)
    {
        info->outDegree[from]++;
    }
    else if (edgeClass == 1 && info->parent[from] != to)
    {
        if (info->entryTime[to] < info->entryTime[info->earliestAncestor[from]])
        {
            info->earliestAncestor[from] = to;
        };
    };
};

int Board::_oneHiveClass(std::vector<int> &from, std::vector<int> &to, _OneHiveInfo *info)
{
    if (info->parent[to] == from) { return 0; }; // tree edge - this edge leads to an unexplored part of the tree
    if (info->discovered[to] && !info->processed[to]) { return 1; }; // back edge - this tree points back into the tree we've already explored
    return 2;
};

std::vector<Move> Board::_genPlacementMoves()
{
    std::map<int, int>::iterator configIt;
    std::vector<Move> moves;

    if (_turn == 0)
    {
        for (configIt = _pieceConfig.begin(); configIt != _pieceConfig.end(); configIt++)
        {
            if (configIt->first < PieceCodes::bQ && configIt->second > 0)
            {
                moves.push_back(Move(_pieces.nextLabel(configIt->first)));
            };
        };
    }
    else if (_turn == 1) // this doesn't work
    {
        std::vector<int> start {0, 0, 0 ,0};
        Piece *startPiece = _pieces.find(start);
        std::vector<int> neighbor;

        for (int i = 0; i < 6; i++)
        {
            neighbor = startPiece->getNeighbor(i);

            for (configIt = _pieceConfig.begin(); configIt != _pieceConfig.end(); configIt++)
            {
                if (configIt->first >= PieceCodes::bQ && configIt->second > 0)
                {
                    moves.push_back(Move(_pieces.nextLabel(configIt->first), startPiece->label, i, true ));
                };
            };
        };
    }
    else
    {
        bool open;
        int direction;
        Position toCheck;
        std::set<std::vector<int>> seen;
        std::vector<std::vector<int>> adjacentCoords; 
        std::vector<std::vector<int>>::iterator adjacentIt;
        std::vector<std::vector<int>> neighborCoords;
        std::vector<std::vector<int>>::iterator neighborIt;
        std::map<int, int>::iterator configIt;

        char key = _white ? 'w' : 'b';
        std::vector<Piece*>::iterator pieceIt;
        std::vector<Piece*> colorPieces = _pieces.getColorPieces(_white);

        // for every piece of a given color
        for (pieceIt = colorPieces.begin(); pieceIt != colorPieces.end(); pieceIt++)
        {
            // if it isn't topped
            if (!(*pieceIt)->isTopped) // double check this
            {
                // for every empty adjacent space
                neighborCoords = _pieces.adjacencies(*pieceIt, true);
                for (neighborIt = neighborCoords.begin(); neighborIt != neighborCoords.end(); neighborIt++)
                {
                    // if we have not checked this space before
                    if (seen.find(*neighborIt) == seen.end())
                    {
                        seen.insert(*neighborIt);
                        // if the adjacent space is not adjacent to any enemy pieces
                        open = true;
                        toCheck = Position(*neighborIt);
                        adjacentCoords = _pieces.adjacencies(&toCheck);
                        for (adjacentIt = adjacentCoords.begin(); adjacentIt != adjacentCoords.end(); adjacentIt++)
                        {
                            if (_pieces.find(*adjacentIt)->label[0] != key)
                            {
                                open = false;
                                break;
                            };
                        }
                        if (open)
                        {
                            direction = Position::findDirection((*pieceIt)->getCoords(), *neighborIt);

                            // for every piece
                            for (configIt = _pieceConfig.begin(); configIt != _pieceConfig.end(); configIt++)
                            {
                                // if the piece is on the current player's side
                                if ((configIt->first < PieceCodes::bQ && _white) ||
                                    (configIt->first >= PieceCodes::bQ && !_white))
                                {
                                    // if there are any pieces left
                                    if (configIt->second - _pieces.counts[configIt->first] > 0)
                                    {
                                        // store a corresponding move
                                        moves.push_back(Move(_pieces.nextLabel(configIt->first), (*pieceIt)->label, direction, true));
                                    };
                                };
                            };
                        };
                    };
                };
            };
        };
    };

    return moves;
};

std::vector<Move> Board::_genQueenMoves(std::string label)
{
    std::vector<Move> r;
    return r;
};

std::vector<Move> Board::_genAntMoves(std::string label)
{
    std::vector<Move> r;
    return r;
};

std::vector<Move> Board::_genBeetleMoves(std::string label)
{
    std::vector<Move> r;
    return r;
};

std::vector<Move> Board::_genHopperMoves(std::string label)
{
    std::vector<Move> r;
    return r;
};

std::vector<Move> Board::_genSpiderMoves(std::string label)
{
    std::vector<Move> r;
    return r;
};

std::vector<Move> Board::genAllMoves()
{
    std::vector<Move> moves;
    std::vector<Piece*> genTargets;
    std::vector<Piece*>::iterator targetIt;
    std::vector<Move> genResults;

    // get placement moves
    genResults = _genPlacementMoves();
    moves.insert(moves.end(), genResults.begin(), genResults.end());

    genTargets = _pieces.getColorPieces(_white);

    if (!genTargets.empty())
    {
        Piece *current;
        targetIt = genTargets.begin();
        std::set<std::vector<int>> pinned = _oneHiveCheck((*targetIt)->getCoords());

        for (targetIt; targetIt != genTargets.end(); targetIt++)
        {
            current = *targetIt;

            // universal checks
            if (pinned.find(current->getCoords()) == pinned.end())
            {
                switch (current->code % 5)
                {
                    case 0:
                        genResults = _genQueenMoves(current->label);
                        break;
                    case 1:
                        genResults = _genAntMoves(current->label);
                        break;
                    case 2:
                        genResults = _genBeetleMoves(current->label);
                        break;
                    case 3:
                        genResults = _genHopperMoves(current->label);
                        break;
                    case 4:
                        genResults = _genSpiderMoves(current->label);
                        break;
                    default:
                        // throw some error
                        std::cout << "Invalid code detected in genAllMoves." << std::endl;
                };
                moves.insert(moves.end(), genResults.begin(), genResults.end());
            };
        };
    };

    return moves;
};

std::vector<std::string> _splitMoveCode(std::string moveCode) // maybe a parser util
{
    std::vector<std::string> code;
    std::string codeComp = "";
    std::string::iterator codeIt;

    for (codeIt = moveCode.begin(); codeIt != moveCode.end(); codeIt++)
    {
        if (*codeIt == ' ')
        {
            code.push_back(codeComp);
            codeComp = "";
        }
        else
        {
            codeComp.push_back(*codeIt);
        };
    };

    return code;
};





