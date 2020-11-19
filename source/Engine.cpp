#include <Engine.h>
#include <PieceInfo.h>
#include <Utils.h>
#include <iostream>
#include <algorithm>
#include <regex>

Engine::Engine(std::map<int, int> pieceConfig)
{
    turn = 0;
    white = true;   
    gamestate = GameStates::NotStarted;
    _pieceConfig = pieceConfig;
    _hash = ZobristHash(pieceConfig);
    _hash.changeDepth(_defaultDepth);
    _hash.invertColor(); // sets to white
};


void Engine::makeMove(LabelMove &move)
{
    // DEBUG
    // std::cout << "MAKING MOVE: " << move.toString() << std::endl;
    // std::cout << "HISTORY BEFORE MOVE: " << toString() << std::endl;
    // std::cout << "COORDINATE MAP BEFORE MOVE: " << toCoordString() << std::endl;
    
    Piece *target;
    Position origin({0, 0, 0, 0});

    if (!move.newPiece && !move.pass)
    {
        target = _board.find(move.from);
        _hash.invertPiece(target->getCoords(), move.code); // before move
    };

    history.push_back(move.toString());
    _board.update(move);
    gamestate = _board.checkGameState();
    turn++;
    white = !white;
    _hash.invertColor();

    if (!move.pass)
    {
        target = _board.find(move.from);
        _hash.invertPiece(target->getCoords(), move.code); // after move
        
        if (target->findDistance(&origin) == _hash.radius)
        {
            _recenter();
        };
    };
    
    // DEBUG
    // std::cout << "HISTORY AFTER MOVE: " << toString() << std::endl;
    // std::cout << "COORDINATE MAP AFTER MOVE: " << toCoordString() << std::endl;
};


void Engine::makeMove(std::string moveString)
{
    LabelMove tempMove = stringToMove(moveString);
    makeMove(tempMove);
};


void Engine::undoLast()
{
    LabelMove undo = _board.getLastUndo();

    // DEBUG
    // std::cout << "UNDOING MOVE: " << undo.toString() << std::endl;
    // std::cout << "HISTORY BEFORE UNDO: " << toString() << std::endl;
    // std::cout << "COORDINATE MAP BEFORE UNDO: " << toCoordString() << std::endl;

    if (!undo.pass)
    {
        _hash.invertPiece(_board.find(undo.from)->getCoords(), undo.code);
    };

    history.pop_back();
    _board.undoLast();
    gamestate = _board.checkGameState();
    turn--;
    white = !white;
    _hash.invertColor();

    if (!undo.newPiece && !undo.pass)
    {
        _hash.invertPiece(_board.find(undo.from)->getCoords(), undo.code);
    };

    // DEBUG
    // std::cout << "HISTORY AFTER UNDO: " << toString() << std::endl;
    // std::cout << "COORDINATE MAP UNDO: " << toCoordString() << std::endl;
};


int Engine::score()
{
    return _board.score(white);
};


LabelMove *Engine::validateMove(LabelMove *move)
{
    Piece *onBoard = _board.find(move->from);

    if (*move == _labelNonMove)
    {
        return nullptr;
    };

    if (move->pass)
    {
        std::vector<LabelMove> moves = genAllMoves();
        if (moves[0] == *move) // if there are no moves available, genAllMoves returns a vector with a pass move as its only member
        {
            return move;
        }
        else
        {
            return nullptr; // if there is a choice to move, disallow passing
        };
    };

    if ((!white && move->code < PieceCodes::bQ) ||
        (white && move->code >= PieceCodes::bQ))
    {
        return nullptr;
    };

    //Is this asking us to place a piece which is already on the board?
    if (move->newPiece)
    {
        // if a piece with this label is on the board, do this
        if (onBoard != nullptr)
        {
            // if there are still more pieces in this category to place, do this
            if (_pieceConfig[move->code] - _board.counts[move->code] > 0)
            {
                // update the from label with the next valid label
                move->from = _board.nextLabel(move->code);
            }
            else
            {
                // otherwise, return nullptr
                return nullptr;
            };
        }
        else if (move->from != _board.nextLabel(move->code))
        {
            move->from = _board.nextLabel(move->code);
        };

        // check to see if this is a valid placement move
        PositionMove check;
        PositionMove candidate = Utils::toPositionMove(*move, _board);
        std::vector<LabelMove> placements = _genPlacementMoves();

        for (LabelMove m: placements)
        {
            check = Utils::toPositionMove(m, _board);

            if (check == candidate)
            {
                return move;
            };
        };

        return nullptr;
    }
    else
    {
        if ((!_board.wQueen && move->code < PieceCodes::bQ) ||
            (!_board.bQueen && move->code >= PieceCodes::bQ))
        {
            return nullptr;
        };

        if (onBoard != nullptr)
        {
            if (onBoard->isTopped)
            {
                return nullptr;
            };
            
            std::set<std::vector<int>> pinned = _board.getPinned();
            if (pinned.find(onBoard->getCoords()) != pinned.end())
            {
                return nullptr;
            };


            std::vector<LabelMove> possibleMoves;
            switch (move->code % 5)
            {
                case PieceCodes::wQ:
                    possibleMoves = _genQueenMoves(move->from);
                    break;
                case PieceCodes::wA:
                    possibleMoves = _genAntMoves(move->from);
                    break;
                case PieceCodes::wB:
                    possibleMoves = _genBeetleMoves(move->from);
                    break;
                case PieceCodes::wG:
                    possibleMoves = _genHopperMoves(move->from);
                    break;
                case PieceCodes::wS:
                    possibleMoves = _genSpiderMoves(move->from);
                    break;
                default:
                    std::cout << "err Invalid piece code detected in validateMove" << std::endl;
                    return nullptr;
                    break;
            };

            PositionMove check;
            PositionMove candidate = Utils::toPositionMove(*move, _board);

            for (LabelMove m: possibleMoves)
            {
                check = Utils::toPositionMove(m, _board);

                if (check == candidate)
                {
                    return move;
                };
            };
            
            return nullptr;
        }
        else
        {
            return nullptr;
        };
    };
};

LabelMove Engine::stringToMove(std::string moveString)
{
    // maybe regex check to start?s

    std::vector<std::string> components = Utils::tokenize(moveString, ' '); // need a better whitespace check
    
    // a failed tokenization returns a vector containing a single empty string
    if (components[0] == "")
    {  
        // throw some error - this should never occur unless makeMove is called by something without a regex check
        std::cout << "err Empty movestring detected in Board::_stringToMove" << std::endl;
        return LabelMove();
    }
    else
    {
        // this is a first move or a pass move
        if (components.size() == 1)
        {
            return LabelMove(components[0]);
        }
        // this is a normal move
        else
        {
            int direction = -1;
            int labelLength = 3;
            std::string destination = "";
            bool newPiece = _board.find(components[0]) == nullptr;

            // queen check
            std::regex pattern("[bw]Q");
            if (std::regex_search(components[1], pattern) == true)
            {
                labelLength = 2;
            };

            // find direction if direction symbol precedes the destination piece
            switch (components[1][0])
            {
                case '\\':
                    direction = Directions::UpLeft;
                    destination = components[1].substr(1, labelLength);
                    break;
                case '-':
                    direction = Directions::Left;
                    destination = components[1].substr(1, labelLength);
                    break; 
                case '/':
                    direction = Directions::DownLeft;
                    destination = components[1].substr(1, labelLength);
                    break; 
                default:
                    break;
            };
            // find direction if direction symbol comes after the destination piece
            if (destination == "")
            {
                switch (components[1][labelLength])
                {
                    case '\\':
                        direction = Directions::DownRight;
                        destination = components[1].substr(0, labelLength);
                        break;
                    case '-':
                        direction = Directions::Right;
                        destination = components[1].substr(0, labelLength);
                        break; 
                    case '/':
                        direction = Directions::UpRight;
                        destination = components[1].substr(0, labelLength);
                        break; 
                    default:
                        break;
                };
            };
            
            // Malformed moveString
            if (direction == -1)
            {
                return _labelNonMove;
            };
            
            destination = Utils::strip(destination);
            return LabelMove(components[0], destination, direction, newPiece);
        };
    };
};

std::vector<LabelMove> Engine::_genPlacementMoves()
{
    std::map<int, int>::iterator configIt;
    std::vector<LabelMove> moves;

    if (turn == 0)
    {
        for (configIt = _pieceConfig.begin(); configIt != _pieceConfig.end(); configIt++)
        {
            if (PieceCodes::wQ < configIt->first && configIt->first < PieceCodes::bQ && configIt->second > 0) // no queen moves on first turn
            {
                moves.push_back(LabelMove(_board.nextLabel(configIt->first)));
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
                    moves.push_back(LabelMove(_board.nextLabel(configIt->first), startPiece->label, i, true));
                };
            };
        };
    }
    else
    {
        bool open;
        bool queenCheck;
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
        std::vector<Piece*> colorPieces = _board.getColorPieces(white);

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
                                    if (configIt->second - _board.counts[configIt->first] > 0)
                                    {
                                        queenCheck = false;
                                        //enforcing Queen at turn 4
                                        if (turn == 6 && !_board.wQueen)
                                        {
                                            if (configIt->first == PieceCodes::wQ)
                                            {
                                                queenCheck = true;
                                            };
                                        }
                                        else if (turn == 7 && !_board.bQueen)
                                        {
                                            if (configIt->first == PieceCodes::bQ)
                                            {
                                                queenCheck = true;
                                            };
                                        }
                                        else
                                        {
                                            queenCheck = true;
                                        };

                                        if (queenCheck)
                                        {
                                        // store a corresponding move
                                            moves.push_back(LabelMove(_board.nextLabel(configIt->first), (*pieceIt)->label, direction, true));
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
                        std::vector<LabelMove> &moves, std::set<std::vector<int>> &seen, int depth)
{
    if (depth > 0 || code % 5 == 1)
    {
        int direction;
        int left;
        int right;
        Piece* leftBlock;
        Piece* rightBlock;

        Position nextSpace;
        std::vector<std::vector<int>> nextAdj;
        std::vector<std::vector<int>>::iterator nextIt;
        Piece *nextReference;
        Piece *original;
        std::vector<std::vector<int>> empties;
        std::vector<std::vector<int>>::iterator emptyIt;
        
        empties = _board.adjacencies(current, true);

        // if we are currently working with a beetle, we want to look at the top of every nearby piece
        if (code % 5 == 2)
        {
            for (std::vector<int> occ: _board.adjacencies(current))
            {
                occ[3] += 1;
                empties.push_back(occ);
            };
        };

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
                leftBlock = _board.find(current->getNeighbor(left));
                rightBlock = _board.find(current->getNeighbor(right));

                // double check potential blocking pieces for beetles
                // beetles are slowly driving me insane. Can't wait for ladybugs!
                if (code % 5 == 2)
                {
                    std::vector<int> topCoord;
                

                    if (leftBlock != nullptr)
                    {
                        topCoord = leftBlock->getCoords();
                        topCoord[3] = std::max((*emptyIt)[3], current->getCoords()[3]);
                        leftBlock = _board.find(topCoord);
                    };

                    if (rightBlock != nullptr)
                    {
                        topCoord = rightBlock->getCoords();
                        topCoord[3] = std::max((*emptyIt)[3], current->getCoords()[3]);
                        rightBlock = _board.find(topCoord);
                    };
                };

                // if at least one neighbor is empty, continue
                if (leftBlock == nullptr || rightBlock == nullptr)
                {
                    nextSpace = Position(*emptyIt);
                    nextAdj = _board.adjacencies(&nextSpace);

                    // if the empty spot has a neighbor, continue
                    if (!nextAdj.empty())
                    {
                        // check if the neighbor is our original piece
                        nextReference = nullptr;

                        // Add a bottom piece if our piece is elevated
                        original = _board.find(label);
                        if (original->getCoords()[3] > 0)
                        {
                            std::vector<int> underCoords = original->getCoords();
                            underCoords[3]--;
                            nextAdj.push_back(underCoords);
                        };

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
                                    moves.push_back(LabelMove(label, nextReference->label, direction));
                                };
                            }
                            else
                            {
                                moves.push_back(LabelMove(label, nextReference->label, direction));
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


void Engine::_hopperSearch(std::string label, int direction, Position *current, std::vector<LabelMove> &moves)
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
        moves.push_back(LabelMove(label, ref->label, refDir));
    };
};


std::vector<LabelMove> Engine::_genQueenMoves(std::string label)
{
    std::vector<LabelMove> moves;
    std::set<std::vector<int>> seen;
    Piece *current = _board.find(label);
    _moveSearch(label, current->code, current, moves, seen, 1);
    return moves;
};


std::vector<LabelMove> Engine::_genAntMoves(std::string label)
{
    std::vector<LabelMove> moves;
    std::set<std::vector<int>> seen;
    Piece *current = _board.find(label);
    _moveSearch(label, current->code, current, moves, seen, -1);
    return moves;
};


std::vector<LabelMove> Engine::_genBeetleMoves(std::string label)
{
    std::vector<LabelMove> moves;
    std::set<std::vector<int>> seen;
    Piece *current = _board.find(label);
    _moveSearch(label, current->code, current, moves, seen, 1);
    return moves;
};


std::vector<LabelMove> Engine::_genHopperMoves(std::string label)
{
    std::vector<LabelMove> moves;
    int direction;
    Piece *current = _board.find(label);
    std::vector<std::vector<int>> adjacencies = _board.adjacencies(current);
    std::vector<std::vector<int>>::iterator adjacentIt = adjacencies.begin();

    // initiate a hopper search for every direction with an adjacency
    for (; adjacentIt != adjacencies.end(); adjacentIt++)
    {
        direction = Position::findDirection(current->getCoords(), *adjacentIt);
        _hopperSearch(label, direction, current, moves);
    };

    return moves;
};


std::vector<LabelMove> Engine::_genSpiderMoves(std::string label)
{
    std::vector<LabelMove> moves;
    std::set<std::vector<int>> seen;
    Piece *current = _board.find(label);
    _moveSearch(label, current->code, current, moves, seen, 3);
    return moves;
};


std::vector<LabelMove> Engine::genAllMoves()
{
    std::vector<LabelMove> moves;
    std::vector<Piece*> genTargets;
    std::vector<Piece*>::iterator targetIt;
    std::vector<LabelMove> genResults;

    // get placement moves
    genResults = _genPlacementMoves();
    moves.insert(moves.end(), genResults.begin(), genResults.end());

    if ((white && _board.wQueen) || // if white + wQ on board
        (!white && _board.bQueen)) // or black + bQ on board, add piece movement
    {
        genTargets = _board.getColorPieces(white);

        if (!genTargets.empty())
        {
            Piece *current;
            targetIt = genTargets.begin();
            std::set<std::vector<int>> pinned = _board.getPinned();

            for (; targetIt != genTargets.end(); targetIt++) // maybe convert
            {
                current = *targetIt;
    
                // universal checks
                if (pinned.find(current->getCoords()) == pinned.end() && !current->isTopped)
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
                            std::cout << "err Invalid code detected in genAllMoves." << std::endl;
                    };
                    moves.insert(moves.end(), genResults.begin(), genResults.end());
                };
            };
        };
    };

    if (moves.empty())
    {
        moves.push_back(LabelMove("pass"));
    };

    return moves;
};

void Engine::_recenter()
{
    std::vector<int> centroid = _board.getCenter();
    std::vector<Piece*> pieces = _board.getAllPieces();

    for (Piece *piece: pieces)
    {
        _hash.invertPiece(piece->getCoords(), piece->code);
    };

    _board.recenter(centroid);

    for (Piece *piece: pieces)
    {
        _hash.invertPiece(piece->getCoords(), piece->code);
    };
};

LabelMove Engine::recommendMove(int depth, int duration)
{
    LabelMove bestMove;
    std::vector<PositionMove> killerMoves;
    std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();
    int alpha = -1000000;
    int beta = 1000000;

    if (_board.getRadius() > _hash.radius - depth)
    {
        _recenter();
    };

    for (int i = 1; i <= depth; i++)
    {
        killerMoves.push_back(_positionNonMove);
        bestMove = _negaMax(alpha, beta, i, duration, start, killerMoves);
        std::cout << "note Best move at depth " << i << ": " << bestMove.toString() << std::endl;

        if (Utils::checkDuration(duration, start)) 
        {
            break; 
        };
    };

    _hash.changeDepth(_defaultDepth);

    // DEBUG TIMER
    std::cout << "TIME ELAPSED: " << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - start).count() << 'S' << std::endl;

    return bestMove;
};

LabelMove Engine::_negaMax(int alpha, int beta, int maxDepth, int duration, 
                            std::chrono::time_point<std::chrono::high_resolution_clock> &start,  
                            std::vector<PositionMove> &killerMoves)
{
    PositionMove tableMove = _positionNonMove;
    std::vector<int> transformKey;
    LabelMove best;
    int val;
    int bestVal = -1000000;
    bool early = false;
    std::vector<LabelMove> moves = genAllMoves();

    _hash.changeDepth(maxDepth);

    transformKey = _findTransposedRotation(maxDepth, 0);

    if (!transformKey.empty())
    {
        tableMove = _findExactTransposition(transformKey);
        if (tableMove != _positionNonMove)
        {
            std::cout << "EXACT TRANSPOSITION" << std::endl; // DEBUG
            return Utils::toLabelMove(tableMove, _board);
        };

        std::vector<LabelMove> tableMoves = _findAdjacentTranspositions(transformKey, maxDepth, 0);
        for (LabelMove move: tableMoves)
        {
            std::cout << "ADJACENT TRANSPOSITION" << std::endl; // DEBUG
            moves.push_back(move); // could be a std::move I think
        };
    };

    for (LabelMove m: moves)
    {
        makeMove(m);

        val = -_negaMaxSearch(-beta, -alpha, 0, maxDepth-1, duration,  start, killerMoves);
        if (val > bestVal)
        {
            best = m;
            bestVal = val;
        };

        undoLast();

        if (Utils::checkDuration(duration, start)) 
        {
            std::cout << "note Time limit reached, returning best known move." << std::endl;
            early = true; 
            break; 
        };
    };

    _hash.changeDepth(maxDepth);

    if (!early) 
    {
        PositionMove positionBest = Utils::toPositionMove(best, _board);
        positionBest.score = bestVal;

        if (!transformKey.empty())
        {
            _storeTransformedBest(positionBest, transformKey);
        }
        else
        {   
            _transTable.insert(_hash.hash, positionBest);
        };
    };

    return best;
};

int Engine::_negaMaxSearch(int alpha, int beta, int depth, int maxDepth, int duration, 
                            std::chrono::time_point<std::chrono::high_resolution_clock> &start, 
                            std::vector<PositionMove> &killerMoves)
{
    if (gamestate > GameStates::InProgress || depth == maxDepth)
    {
        return score();    
    };
    
    int val = -1000000;
    int curVal;
    bool failHigh = false;
    bool early = false;
    LabelMove best;
    PositionMove tableMove = _positionNonMove;
    std::vector<int> transformKey;
    std::vector<LabelMove> moves = genAllMoves();
    std::vector<LabelMove>::reverse_iterator moveIt;

    _hash.changeDepth(maxDepth-depth);

    // check if any exact or adjacent transposition of any transformation exists in the trans table
    transformKey = _findTransposedRotation(maxDepth, depth);
    
    // if a transformKey was found, check for an exact transposition at that transformation
    if (!transformKey.empty())
    {
        tableMove = _findExactTransposition(transformKey);
        if (tableMove != _positionNonMove)
        {
            std::cout << "EXACT TRANSPOSITION" << std::endl; // DEBUG
            return tableMove.score;
        };
    };

    // find a killer move, if any
    if (killerMoves[depth] != _positionNonMove)
    {
        LabelMove killer = Utils::toLabelMove(killerMoves[depth], _board);
        LabelMove *killerRef = validateMove(&killer);
        if (killerRef != nullptr)
        {
            std::cout << "LEGAL KILLER" << std::endl; // DEBUG
            moves.push_back(killer);
        };
    };

    // find transtable moves of different depths
    if (!transformKey.empty())
    {
        std::vector<LabelMove> tableMoves = _findAdjacentTranspositions(transformKey, maxDepth, depth);
        for (LabelMove move: tableMoves)
        {
            std::cout << "ADJACENT TRANSPOSITION" << std::endl; // DEBUG
            moves.push_back(move); // could be a std::move I think
        };
    };

    _hash.changeDepth(maxDepth-depth);

    for (moveIt = moves.rbegin(); moveIt != moves.rend(); moveIt++)
    {
        makeMove(*moveIt);
        curVal = -_negaMaxSearch(-beta, -alpha, depth+1, maxDepth, duration, start, killerMoves);
        
        if (val < curVal)
        {
            val = curVal;
            best = *moveIt;
        };

        alpha = std::max(alpha, val);
        
        undoLast();

        if (alpha >= beta)
        {
            std::cout << "FAILED HIGH" << std::endl; // DEBUG
            failHigh = true;
            killerMoves[depth] = Utils::toPositionMove(*moveIt, _board);
            break;  
        };

        if (Utils::checkDuration(duration, start)) 
        {
            early = true; 
            break; 
        };
    };

    _hash.changeDepth(maxDepth-depth);

    // store an analyzed best move
    if (!failHigh && !early)
    {
        PositionMove positionBest = Utils::toPositionMove(best, _board);
        positionBest.score = val;

        if (!transformKey.empty())
        {
            _storeTransformedBest(positionBest, transformKey);
        }
        else
        {   
            _transTable.insert(_hash.hash, positionBest);
        };
    };

    return val;
};

std::string Engine::toString()
{
    std::string repr = "";

    std::string status;
    switch(gamestate)
    {
        case GameStates::NotStarted:
            status = "NotStarted";
            break;
        case GameStates::InProgress:
            status = "InProgress";
            break;
        case GameStates::Draw:
            status = "Draw";
            break;
        case GameStates::WhiteWins:
            status = "WhiteWins";
            break;
        case GameStates::BlackWins:
            status = "BlackWins";
            break;
        default:
            status = "ERRORSTATUS";
            break;
    };
    repr += status + ";";

    if (white)
    {
        repr += "White[" + std::to_string(turn / 2 + 1) + "];";
    }
    else
    {
        repr += "Black[" + std::to_string((turn / 2) + 1) + "];";
    };

    for (std::string moveString: history)
    {
        repr += moveString += ";";
    };
    return repr.substr(0, repr.size()-1);
};

void Engine::_rotate(std::vector<int> &coords)
{
    int temp;
    temp = coords[0];
    coords[0] = -coords[2];
    coords[2] = -coords[1];
    coords[1] = -temp;
};

void Engine::_mirror(std::vector<int> &coords)
{
    int temp;
    temp = coords[1];
    coords[0] = -coords[0];
    coords[1] = -coords[2];
    coords[2] = -temp;
};

void Engine::_applyTransformation(std::vector<int> &coords, std::vector<int> &transformation, bool reverse)
{
    if (reverse)
    {   
        int position;
        int sign;
        std::vector<int> result(3, 0);

        for (int i = 0; i < 3; i++)
        {
            position = std::abs(transformation[i]);
            sign = transformation[i] < 0 ? -1 : 1;
            result[position - 1] = sign * (i + 1);
        };

        _applyTransformation(coords, result);
    }
    else
    {
        std::vector<int> temp;
        std::vector<int> signs;
        std::vector<int> positions;
        for (int el: transformation)
        {
            signs.push_back(el < 0 ? -1 : 1);
            positions.push_back(std::abs(el) - 1);
        };

        temp = {signs[0] * coords[positions[0]], 
                signs[1] * coords[positions[1]], 
                signs[2] * coords[positions[2]]};

        for (int i = 0; i < 3; i++)
        {
            coords[i] = temp[i];
        };
    };
};


std::vector<int> Engine::_findTransposedRotation(int maxDepth, int currentDepth)
{
    PositionMove *target;

    unsigned long int originalHash = _hash.hash;
    std::vector<Piece*> pieces = _board.getAllPieces();
    std::vector<std::vector<int>> coords;
    for (Piece *piece: pieces)
    {
        coords.push_back(piece->getCoords());
    };
    std::vector<int> transformation {1, 2, 3}; // starting from this code ensures we always check from a {1, 2, 3} transposition first
    
    // a bit clunky, but this allows us to avoid an unecessary initial transformation
    // and prioritize searching the untransformed hash + its mirror first
    // check the initial hash at all depths
    for (int i = 1; i <= maxDepth; i++)
    {
        _hash.changeDepth(i);
        target = _transTable.find(_hash.hash);
        if (target != nullptr)
        {
            _hash.changeDepth(maxDepth - currentDepth); // this is to reset the hash's depth variable
            _hash.hash = originalHash;
            return transformation;
        };
    };

    // Check the first mirror at all depths
    _mirror(transformation);
    for (unsigned int i = 0; i < coords.size(); i++)
    {
        _hash.invertPiece(coords[i], pieces[i]->code);
        _mirror(coords[i]);
        _hash.invertPiece(coords[i], pieces[i]->code);
    };

    for (int i = 1; i <= maxDepth; i++)
    {
        _hash.changeDepth(i);
        target = _transTable.find(_hash.hash);
        if (target != nullptr)
        {
            _hash.changeDepth(maxDepth - currentDepth); // this is to reset the hash's depth variable
            _hash.hash = originalHash;
            return transformation;
        };
    };

    // check the rest
    for (int i = 0; i < 2; i++)
    {
        _mirror(transformation);

        for (unsigned int j = 0; j < pieces.size(); j++)
        {
            _hash.invertPiece(coords[j], pieces[j]->code);
            _mirror(coords[j]);
        };

        for (int k = 0; k < 5; k++)
        {
            _rotate(transformation);

            for (unsigned int l = 0; l < pieces.size(); l++)
            {
                _rotate(coords[l]);
                _hash.invertPiece(coords[l], pieces[l]->code);
            };

            for (int m = 1; m <= maxDepth; m++)
            {
                _hash.changeDepth(m);
                target = _transTable.find(_hash.hash);
                if (target != nullptr)
                {
                    _hash.changeDepth(maxDepth - currentDepth); // this is to reset the hash's depth variable
                    _hash.hash = originalHash;
                    return transformation;
                };
            };
        };
    };

    _hash.changeDepth(maxDepth - currentDepth); // this is to reset the hash's depth variable
    _hash.hash = originalHash;
    return {}; // return a blank transformation if we didnt find anything
};

PositionMove Engine::_findExactTransposition(std::vector<int> &transformation)
{
    PositionMove *target;
    unsigned long int originalHash = _hash.hash;
    std::vector<Piece*> pieces = _board.getAllPieces();
    std::vector<std::vector<int>> coords;
    for (Piece *piece: pieces)
    {
        coords.push_back(piece->getCoords());
    };

    for (unsigned int i = 0; i < pieces.size(); i++)
    {
        _hash.invertPiece(pieces[i]->getCoords(), pieces[i]->code);
        _applyTransformation(coords[i], transformation);
        _hash.invertPiece(coords[i], pieces[i]->code);
    };

    target = _transTable.find(_hash.hash);
    _hash.hash = originalHash;


    if (target == nullptr)
    {
        return _positionNonMove; // return a nonmove if we didnt find anything
    }
    else
    {
        PositionMove ret(*target);
        _applyTransformation(ret.from, transformation, true);
        _applyTransformation(ret.to, transformation, true);

        return ret; 
    };
};

std::vector<LabelMove> Engine::_findAdjacentTranspositions(std::vector<int> &transformation, int maxDepth, int currentDepth)
{
    PositionMove *target;
    PositionMove temp;
    std::vector<PositionMove> accumulator;
    std::vector<LabelMove> retVector;
    unsigned long int originalHash = _hash.hash;
    std::vector<Piece*> pieces = _board.getAllPieces();
    std::vector<std::vector<int>> coords;
    for (Piece *piece: pieces)
    {
        coords.push_back(piece->getCoords());
    };

    for (unsigned int i = 0; i < pieces.size(); i++)
    {
        _hash.invertPiece(pieces[i]->getCoords(), pieces[i]->code);
        _applyTransformation(coords[i], transformation);
        _hash.invertPiece(coords[i], pieces[i]->code);
    };

    for (int j = 1; j <= maxDepth; j++)
    {
        _hash.changeDepth(j);
        target = _transTable.find(_hash.hash);
        if (target != nullptr)
        {
            accumulator.emplace_back(*target);
        };
    };

    for (unsigned int k = 0; k < accumulator.size(); k++)
    {
        _applyTransformation(accumulator[k].from, transformation, true);
        _applyTransformation(accumulator[k].to, transformation, true);

        retVector.push_back(Utils::toLabelMove(accumulator[k], _board));
    };

    _hash.changeDepth(maxDepth - currentDepth);
    _hash.hash = originalHash;
    return retVector;
};

 void Engine::_storeTransformedBest(PositionMove bestMove, std::vector<int> transformKey)
 {
    unsigned long int originalHash = _hash.hash;
    std::vector<Piece*> pieces = _board.getAllPieces();
    std::vector<std::vector<int>> coords;
    for (Piece *piece: pieces)
    {
        coords.push_back(piece->getCoords());
    };

    for (unsigned int i = 0; i < pieces.size(); i++)
    {
        _hash.invertPiece(coords[i], pieces[i]->code);
        _applyTransformation(coords[i], transformKey);
        _hash.invertPiece(coords[i], pieces[i]->code);
    };

    _applyTransformation(bestMove.to, transformKey);
    _applyTransformation(bestMove.from, transformKey);

    _transTable.insert(_hash.hash, bestMove);
    
    _hash.hash = originalHash;
 };


void Engine::reset()
{
    std::vector<Piece*> pieces = _board.getAllPieces();
    for (Piece* piece: pieces)
    {
        _hash.invertPiece(piece->getCoords(), piece->code);
    };

    if (!white)
    {
        _hash.invertColor();
    };

    history.clear();
    _board.clear();
    turn = 0;
    gamestate = GameStates::NotStarted;
    white = true;
};


std::string Engine::toCoordString()
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



