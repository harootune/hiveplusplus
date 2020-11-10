#include "Engine.h"
#include "PieceInfo.h"
#include "Utils.h"
#include <iostream>
#include <algorithm>


Engine::Engine(std::map<int, int> pieceConfig)
{
    turn = 0;
    white = true;   
    _pieceConfig = pieceConfig;
    _board = Board(this);
};


void Engine::makeMove(Move move)
{
    // DEBUG
    std::cout << turn << " " << toString() << std::endl;
    std::cout << "COORDS MAP BEFORE MOVE " << _board.coordsMapToString() << std::endl;

    _board.update(move);
    gamestate = _board.checkGameState();
    turn++;
    white = !white;

    // DEBUG
    std::cout << turn << " " << move.toString() << std::endl;
    std::cout << "COORDS MAP AFTER MOVE " << _board.coordsMapToString() << std::endl;
};


void Engine::makeMove(std::string moveString)
{
    makeMove(_stringToMove(moveString));
};


void Engine::undoLast()
{
    // DEBUG
    std::cout << turn << " UNDO " << toString() << std::endl;
    std::cout << "COORDS MAP BEFORE UNDO " << _board.coordsMapToString() << std::endl;
    
    _board.undoLast();
    gamestate = _board.checkGameState();
    turn--;
    white = !white;

    // DEBUG
    std::cout << "COORDS MAP AFTER UNDO " << _board.coordsMapToString() << std::endl;
    if (_board._undoCache.size() != turn)
    {
        std::cout << "FAILURE AFTER UNDO" << std::endl;;
    };

};


int Engine::score()
{
    return _board.score();
};


bool Engine::_validateMove(std::vector<std::string> move)
{
    return true;
};

Move Engine::_stringToMove(std::string moveString)
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
            bool newPiece = _board.find(components[0]) == nullptr;

            // find direction if direction symbol precedes the destination piece
            switch (components[1][0])
            {
                case '\\':
                    direction = Directions::UpLeft;
                    destination = components[1].substr(1, 2);
                    break;
                case '-':
                    direction = Directions::Left;
                    destination = components[1].substr(1, 2);
                    break; 
                case '/':
                    direction = Directions::DownLeft;
                    destination = components[1].substr(1, 2);
                    break; 
                default:
                    break;
            };
            // find direction if direction symbol comes after the destination piece
            if (destination == "")
            {
                switch (components[1][2])
                {
                    case '\\':
                        direction = Directions::DownRight;
                        destination = components[1].substr(0, 2);
                        break;
                    case '-':
                        direction = Directions::Right;
                        destination = components[1].substr(0, 2);
                        break; 
                    case '/':
                        direction = Directions::UpRight;
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

std::vector<Move> Engine::_genPlacementMoves()
{
    std::map<int, int>::iterator configIt;
    std::vector<Move> moves;

    if (turn == 0)
    {
        for (configIt = _pieceConfig.begin(); configIt != _pieceConfig.end(); configIt++)
        {
            if (PieceCodes::wQ < configIt->first && configIt->first < PieceCodes::bQ && configIt->second > 0) // no queen moves on first turn
            {
                moves.push_back(Move(_board.nextLabel(configIt->first)));
            };
        };
    }
    else if (turn == 1)
    {
        std::vector<int> start {0, 0, 0 ,0};
        Piece *startPiece = _board.find(start);
        std::vector<int> neighbor;

        for (int i = 0; i < 6; i++)
        {
            neighbor = startPiece->getNeighbor(i);

            for (configIt = _pieceConfig.begin(); configIt != _pieceConfig.end(); configIt++)
            {
                if (configIt->first > PieceCodes::bQ && configIt->second > 0) // same queen restriction
                {
                    moves.push_back(Move(_board.nextLabel(configIt->first), startPiece->label, i, true));
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

        char key = white ? 'w' : 'b';
        std::vector<Piece*>::iterator pieceIt;
        std::vector<Piece*> colorPieces = _board.getColorPieces();

        // for every piece of a given color
        for (pieceIt = colorPieces.begin(); pieceIt != colorPieces.end(); pieceIt++)
        {
            // if it isn't topped, continue
            if (!(*pieceIt)->isTopped) // double check this
            {
                // for every empty adjacent space
                neighborCoords = _board.adjacencies(*pieceIt, true);
                for (neighborIt = neighborCoords.begin(); neighborIt != neighborCoords.end(); neighborIt++)
                {
                    // if we have not checked this space before, continue
                    if (seen.find(*neighborIt) == seen.end())
                    {
                        seen.insert(*neighborIt);
                        // if the adjacent space is not adjacent to any enemy pieces, continue
                        open = true;
                        toCheck = Position(*neighborIt);
                        adjacentCoords = _board.adjacencies(&toCheck);
                        for (adjacentIt = adjacentCoords.begin(); adjacentIt != adjacentCoords.end(); adjacentIt++)
                        {
                            if (_board.find(*adjacentIt)->label[0] != key)
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
                                if ((configIt->first < PieceCodes::bQ && white) ||
                                    (configIt->first >= PieceCodes::bQ && !white))
                                {
                                    // if there are any pieces left, continue
                                    int check = configIt->second - _board.counts[configIt->first];
                                    if (configIt->second - _board.counts[configIt->first] > 0)
                                    {
                                        //enforcing Queen at turn 4
                                        if ((turn == 6 && configIt->first == 0) ||
                                            (turn == 7 && configIt->first == 5) ||
                                            turn < 6 || turn < 7)
                                        {
                                            // store a corresponding move
                                            moves.push_back(Move(_board.nextLabel(configIt->first), (*pieceIt)->label, direction, true));
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


void Engine::_moveSearch(std::string label, int code, Position *current, 
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
                topCoord = _board.top(current->getNeighbor(i));
                empties.push_back(topCoord);
            };
        }
        else
        {
            empties = _board.adjacencies(current, true);
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
                if (_board.find(current->getNeighbor(left)) == nullptr ||
                    _board.find(current->getNeighbor(right)) == nullptr)
                
                {
                    nextSpace = Position(*emptyIt);
                    nextAdj = _board.adjacencies(&nextSpace);

                    // if the empty spot has a neighbor, continue
                    if (!nextAdj.empty())
                    {
                        // check if the neighbor is our original piece
                        nextReference = nullptr;
                        original = _board.find(label);

                        for (nextIt = nextAdj.begin(); nextIt != nextAdj.end(); nextIt++)
                        {
                            if (*nextIt != original->getCoords())
                            {
                                nextReference = _board.find(*nextIt);
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


void Engine::_hopperSearch(std::string label, int direction, Position *current, std::vector<Move> &moves)
{
    // if we're currently over a piece, continue searching
    if (_board.find(current->getCoords()))
    {
        Position next = current->getNeighbor(direction);
        _hopperSearch(label, direction, &next, moves);
    }
    // otherwise, we've found a stopping point
    else
    {
        std::vector<std::vector<int>> adjacencies = _board.adjacencies(current);
        Piece *ref = _board.find(adjacencies[0]);
        int refDir = Position::findDirection(ref->getCoords(), current->getCoords());
        moves.push_back(Move(label, ref->label, refDir));
    };
};


std::vector<Move> Engine::_genQueenMoves(std::string label)
{
    std::vector<Move> moves;
    std::set<std::vector<int>> seen;
    Piece *current = _board.find(label);
    _moveSearch(label, current->code, current, moves, seen, 1);
    return moves;
};


std::vector<Move> Engine::_genAntMoves(std::string label)
{
    std::vector<Move> moves;
    std::set<std::vector<int>> seen;
    Piece *current = _board.find(label);
    _moveSearch(label, current->code, current, moves, seen, -1);
    return moves;
};


std::vector<Move> Engine::_genBeetleMoves(std::string label)
{
    std::vector<Move> moves;
    std::set<std::vector<int>> seen;
    Piece *current = _board.find(label);
    _moveSearch(label, current->code, current, moves, seen, 1);
    return moves;
};


std::vector<Move> Engine::_genHopperMoves(std::string label)
{
    std::vector<Move> moves;
    int direction;
    Piece *current = _board.find(label);
    std::vector<std::vector<int>> adjacencies = _board.adjacencies(current);
    std::vector<std::vector<int>>::iterator adjacentIt = adjacencies.begin();

    // initiate a hopper search for every direction with an adjacency
    for (adjacentIt; adjacentIt != adjacencies.end(); adjacentIt++)
    {
        direction = Position::findDirection(current->getCoords(), *adjacentIt);
        _hopperSearch(label, direction, current, moves);
    };

    return moves;
};


std::vector<Move> Engine::_genSpiderMoves(std::string label)
{
    std::vector<Move> moves;
    std::set<std::vector<int>> seen;
    Piece *current = _board.find(label);
    _moveSearch(label, current->code, current, moves, seen, 3);
    return moves;
};


std::vector<Move> Engine::genAllMoves()
{
    std::vector<Move> moves;
    std::vector<Piece*> genTargets;
    std::vector<Piece*>::iterator targetIt;
    std::vector<Move> genResults;

    // get placement moves
    genResults = _genPlacementMoves();
    moves.insert(moves.end(), genResults.begin(), genResults.end());

    if ((white && _board.wQueen) || // if white + wQ on board
        (!white && _board.bQueen)) // or black + bQ on board, add piece movement
    {
        genTargets = _board.getColorPieces();

        if (!genTargets.empty())
        {
            Piece *current;
            targetIt = genTargets.begin();
            std::set<std::vector<int>> pinned = _board.getPinned();

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

Move Engine::recommendMove()
{
    Move bestMove;
    int alpha = -1000000;
    int beta = 1000000;

    for (int i = 1; i <= 3; i++)
    {
        bestMove = _negaMax(alpha, beta, i);
    };

    return bestMove;
};

Move Engine::_negaMax(int alpha, int beta, int depth)
{
    Move best;
    int val;
    int bestVal = -1000000;
    std::vector<Move> moves = genAllMoves();

    if (_board._undoCache.size() != turn)
    {
        std::cout << "FAILURE BEFORE INIT" << std::endl;
    };

    for (Move m: moves)
    {
        makeMove(m);

        val = -_negaMaxSearch(-beta, -alpha, depth-1);
        if (val > bestVal)
        {
            best = m;
            bestVal = val;
        };

        undoLast();
    };

    if (_board._undoCache.size() != turn)
    {
        std::cout << "FAILURE AFTER INIT" << std::endl;
    };

    return best;
};

int Engine::_negaMaxSearch(int alpha, int beta, int depth)
{
    // DEBUG
    if (_board._undoCache.size() != turn)
    {
        std::cout << "FAILURE BEFORE SEARCH" << std::endl;
    };

    if (gamestate > GameStates::InProgress || depth == 0)
    {
        return score();    
    };

    int newAlpha = alpha;
    int val = -1000000;
    std::vector<Move> moves = genAllMoves();

    for (Move m: moves)
    {
        makeMove(m);

        val = std::max(val, -_negaMaxSearch(-beta, -newAlpha, depth-1));
        newAlpha = std::max(newAlpha, val);
        
        undoLast();

        if (newAlpha >= beta)
        {
            std::cout << "FAILED HIGH" << std::endl;  
            // failing high - we will want to do more with this
            break;  
        };
    };

    // DEBUG
    if (_board._undoCache.size() != turn)
    {
        std::cout << "FAILURE AFTER SEARCH" << std::endl;
    };

    return val;

};


std::string Engine::toString()
{
    std::string repr = "";

    std::vector<Piece*> pcs = _board.getAllPieces();
    for (Piece *p: pcs)
    {
        repr += " " + p->label + " " + 
                std::to_string(p->getCoords()[0]) + "," +
                std::to_string(p->getCoords()[1]) + "," +
                std::to_string(p->getCoords()[2]) + "," +
                std::to_string(p->getCoords()[3]) + ":";
    };

    return repr;
};


