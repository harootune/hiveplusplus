#include "Board.h"
#include "PieceInfo.h"
#include "Utils.h"
#include <iostream>
#include <algorithm>

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
    _pieces.update(move);
    _turn++;
    _white = !_white;
};


void Board::makeMove(std::string moveString)
{
    makeMove(_stringToMove(moveString));
};


void Board::undoLast()
{
    _pieces.undoLast();
    _turn--;
    _white = !_white;
};

int Board::score()
{
    std::set<std::vector<int>> pinned {};

    if (_turn != 1)
    {
        // this is not a good solution
        pinned = _oneHiveCheck(_pieces.getFirst()->getCoords());
    };

    return _pieces.score(_white, pinned);
};


bool Board::_validateMove(std::vector<std::string> move)
{
    return true;
};


std::set<std::vector<int>> Board::_oneHiveCheck(std::vector<int> start)
// returns a set of articulation vertices for the current board.
{
    _OneHiveInfo info;
    std::vector<int> rootParent {-1, -1, -1, -1};

    // this will break if there are no pieces on the board
    _oneHiveSearch(rootParent, start, &info);

    return info.articulations;
};


void Board::_oneHiveSearch(std::vector<int> &parent, std::vector<int> &location, _OneHiveInfo *info)
// This is an implementation of a linear-time articulation vertext discovery algorithm described here:
// https://cp-algorithms.com/graph/cutpoints.html
{
    
    // increment our timer and update our contextual information about this node
    Position current(location);
    int children = 0; // child count of current node
    info->time++;
    info->visited[location] = true;
    info->entryTime[location] = info->time;
    info->earliestTime[location] = info->time;
    std::vector<int> rootParent {-1, -1, -1, -1};

    // iterate through every adjacency
    std::vector<std::vector<int>> adjacencies = _pieces.adjacencies(&current);
    std::vector<std::vector<int>>::iterator adjacentIt = adjacencies.begin();

    for (adjacentIt = adjacencies.begin(); adjacentIt != adjacencies.end(); adjacentIt++)
    {
        if (*adjacentIt != parent) // ignore the edge back to the parent
        {
            if (info->visited[*adjacentIt]) // this is a back edge
            {
                // earliest we can get back to is the entry time of the back node
                info->earliestTime[location] = std::min(info->earliestTime[location], info->entryTime[*adjacentIt]);
            }
            else // this is a tree edge
            {  
                // recur the dfs
                _oneHiveSearch(location, *adjacentIt, info);

                // earliest we can get back to is the earliest node the child can get to
                info->earliestTime[location] = std::min(info->earliestTime[location], info->earliestTime[*adjacentIt]);

                // if we can go no further back then the current node, this is an articulation vertex
                if (info->earliestTime[*adjacentIt] >= info->entryTime[location] && parent != rootParent)
                {
                    info->articulations.insert(location);
                }
                children++;
            };
        };
    };

    
    if (parent == rootParent && children > 1)
    {
        info->articulations.insert(location);
    };
};

Move Board::_stringToMove(std::string moveString)
{
    // maybe regex check to start?s

    std::vector<std::string> components = Utils::tokenize(moveString, ' '); // need a better whitespace check
    
    // a failed tokenization returns a vector containing a single empty string
    if (components[0] == "")
    {  
        // throw some error - this should never occur unless makeMove is called by something without a regex check
        std::cout << "Empty movestring detected in Board::_stringToMove" << std::endl;
        return Move();
    }
    else
    {
        // this is a first move
        if (components.size() == 1)
        {
            return Move(components[0]);
        }
        // this is a normal move
        else
        {
            int direction;
            std::string destination = "";
            bool newPiece = _pieces.find(components[0]) == nullptr;

            switch (components[1][0])
            {
                case '\\':
                    direction = 4;
                    destination = components[1].substr(1, 2);
                    break;
                case '-':
                    direction = 3;
                    destination = components[1].substr(1, 2);
                    break; 
                case '/':
                    direction = 2;
                    destination = components[1].substr(1, 2);
                    break; 
                default:
                    break;
            };

            if (destination == "")
            {
                switch (components[1][2])
                {
                    case '\\':
                        direction = 1;
                        destination = components[1].substr(0, 2);
                        break;
                    case '-':
                        direction = 0;
                        destination = components[1].substr(0, 2);
                        break; 
                    case '/':
                        direction = 5;
                        destination = components[1].substr(0, 2);
                        break; 
                    default:
                        break;
                };
            };

            return Move(components[0], destination, direction, newPiece);
        };
    };
};

std::vector<Move> Board::_genPlacementMoves()
{
    std::map<int, int>::iterator configIt;
    std::vector<Move> moves;

    if (_turn == 0)
    {
        for (configIt = _pieceConfig.begin(); configIt != _pieceConfig.end(); configIt++)
        {
            if (PieceCodes::wQ < configIt->first && configIt->first < PieceCodes::bQ && configIt->second > 0) // no queen moves on first turn
            {
                moves.push_back(Move(_pieces.nextLabel(configIt->first)));
            };
        };
    }
    else if (_turn == 1)
    {
        std::vector<int> start {0, 0, 0 ,0};
        Piece *startPiece = _pieces.find(start);
        std::vector<int> neighbor;

        for (int i = 0; i < 6; i++)
        {
            neighbor = startPiece->getNeighbor(i);

            for (configIt = _pieceConfig.begin(); configIt != _pieceConfig.end(); configIt++)
            {
                if (configIt->first > PieceCodes::bQ && configIt->second > 0) // same queen restriction
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
            // if it isn't topped, continue
            if (!(*pieceIt)->isTopped) // double check this
            {
                // for every empty adjacent space
                neighborCoords = _pieces.adjacencies(*pieceIt, true);
                for (neighborIt = neighborCoords.begin(); neighborIt != neighborCoords.end(); neighborIt++)
                {
                    // if we have not checked this space before, continue
                    if (seen.find(*neighborIt) == seen.end())
                    {
                        seen.insert(*neighborIt);
                        // if the adjacent space is not adjacent to any enemy pieces, continue
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
                                // if the piece is on the current player's side, continue
                                if ((configIt->first < PieceCodes::bQ && _white) ||
                                    (configIt->first >= PieceCodes::bQ && !_white))
                                {
                                    // if there are any pieces left, continue
                                    if (configIt->second - _pieces.counts[configIt->first] > 0)
                                    {
                                        //enforcing Queen at turn 4
                                        if ((6 <= _turn <= 7 && configIt->first % 5 == 0) || // if this is a queen piece, it is the 4th turn, and we havent placed a queen piece yet
                                            (_turn < 6 || _turn > 7)) // or if its just any other turn, continue
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
    };

    return moves;
};


void Board::_moveSearch(std::string label, int code, Position *current, 
                        std::vector<Move> &moves, std::set<std::vector<int>> &seen, int depth)
{
    if (depth > 0 || code % 5 == 1)
    {
        int direction;
        int left;
        int right;

        Position nextSpace;
        std::vector<std::vector<int>> nextAdj;
        std::vector<std::vector<int>>::iterator nextIt;
        Piece *nextReference;
        Piece *original;
        std::vector<std::vector<int>> empties;
        std::vector<std::vector<int>>::iterator emptyIt;
        
        // if we are currently working with a beetle, we want to look at the top of every nearby piece
        if (code % 5 == 2)
        {
            std::vector<int> topCoord;

            for (int i = 0; i < 6; i++)
            {
                topCoord = _pieces.top(current->getNeighbor(i));
                empties.push_back(topCoord);
            };
        }
        else
        {
            empties = _pieces.adjacencies(current, true);
        }
        
        // for every empty adjacency to our current space
        for (emptyIt = empties.begin(); emptyIt != empties.end(); emptyIt++)
        {   
            // if we haven't moved through this spot already, continue
            if (seen.find(*emptyIt) == seen.end())
            {
                // find the direction of the empty spot and its neighbors
                direction = Position::findDirection(current->getCoords(), *emptyIt);
                left = direction == 0 ? 5 : direction - 1;
                right = direction == 5 ? 0 : direction + 1;

                // if at least one neighbor is empty, continue
                if (_pieces.find(current->getNeighbor(left)) == nullptr ||
                    _pieces.find(current->getNeighbor(right)) == nullptr)
                
                {
                    nextSpace = Position(*emptyIt);
                    nextAdj = _pieces.adjacencies(&nextSpace);

                    // if the empty spot has a neighbor, continue
                    if (!nextAdj.empty())
                    {
                        // check if the neighbor is our original piece
                        nextReference = nullptr;
                        original = _pieces.find(label);

                        for (nextIt = nextAdj.begin(); nextIt != nextAdj.end(); nextIt++)
                        {
                            if (*nextIt != original->getCoords())
                            {
                                nextReference = _pieces.find(*nextIt);
                                break;
                            };
                        };

                        //if we found a non-original-piece neighbor, continue
                        if (nextReference != nullptr)
                        {
                            direction = Position::findDirection(nextReference->getCoords(), nextSpace.getCoords());
                            
                            // spider check
                            if (code % 5 == 4)
                            {
                                if (depth == 1)
                                {
                                    moves.push_back(Move(label, nextReference->label, direction));
                                };
                            }
                            else
                            {
                                moves.push_back(Move(label, nextReference->label, direction));
                            };
                            
                            // note that we've now seen this location
                            seen.insert(*emptyIt);

                            // continue the search
                            _moveSearch(label, code, &nextSpace, moves, seen, depth - 1);
                        };
                    };
                };
            };
        };
    };
};


void Board::_hopperSearch(std::string label, int direction, Position *current, std::vector<Move> &moves)
{
    // if we're currently over a piece, continue searching
    if (_pieces.find(current->getCoords()))
    {
        Position next = current->getNeighbor(direction);
        _hopperSearch(label, direction, &next, moves);
    }
    // otherwise, we've found a stopping point
    else
    {
        std::vector<std::vector<int>> adjacencies = _pieces.adjacencies(current);
        Piece *ref = _pieces.find(adjacencies[0]);
        int refDir = Position::findDirection(ref->getCoords(), current->getCoords());
        moves.push_back(Move(label, ref->label, refDir));
    };
};


std::vector<Move> Board::_genQueenMoves(std::string label)
{
    std::vector<Move> moves;
    std::set<std::vector<int>> seen;
    Piece *current = _pieces.find(label);
    _moveSearch(label, current->code, current, moves, seen, 1);
    return moves;
};


std::vector<Move> Board::_genAntMoves(std::string label)
{
    std::vector<Move> moves;
    std::set<std::vector<int>> seen;
    Piece *current = _pieces.find(label);
    _moveSearch(label, current->code, current, moves, seen, -1);
    return moves;
};


std::vector<Move> Board::_genBeetleMoves(std::string label)
{
    std::vector<Move> moves;
    std::set<std::vector<int>> seen;
    Piece *current = _pieces.find(label);
    _moveSearch(label, current->code, current, moves, seen, 1);
    return moves;
};


std::vector<Move> Board::_genHopperMoves(std::string label)
{
    
    std::vector<Move> moves;
    int direction;
    Piece *current = _pieces.find(label);
    std::vector<std::vector<int>> adjacencies = _pieces.adjacencies(current);
    std::vector<std::vector<int>>::iterator adjacentIt = adjacencies.begin();

    // initiate a hopper search for every direction with an adjacency
    for (adjacentIt; adjacentIt != adjacencies.end(); adjacentIt++)
    {
        direction = Position::findDirection(current->getCoords(), *adjacentIt);
        _hopperSearch(label, direction, current, moves);
    };

    return moves;
};


std::vector<Move> Board::_genSpiderMoves(std::string label)
{
    std::vector<Move> moves;
    std::set<std::vector<int>> seen;
    Piece *current = _pieces.find(label);
    _moveSearch(label, current->code, current, moves, seen, 3);
    return moves;
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

    if ((_white && _pieces.wQueen) || // if white + wQ on board
        (!_white && _pieces.bQueen)) // or black + bQ on board, add piece movement
    {
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
    };

    return moves;
};







