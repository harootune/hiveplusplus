#include "PieceTable.h"
#include "PieceInfo.h"
#include "Board.h"
#include "Utils.h"
#include <stdexcept>
#include <cmath>

Piece *PieceTable::find(std::string label)
{
    try
    {
        return _labelToPiece.at(label);
    }
    catch (std::out_of_range &e)
    {
        return nullptr;
    };
};

Piece *PieceTable::find(std::vector<int> coords)
{
    try
    {
        std::map<int, Piece*> inner = _coordsToPiece.at(
            {coords[0], coords[1], coords[2]}
        );
        return inner.at(coords[3]);
    }
    catch (std::out_of_range &e)
    {
        return nullptr;
    };
};

std::vector<int> PieceTable::top(std::vector<int> coords)
{
    std::vector<int> outerCoords;
    outerCoords.insert(outerCoords.begin(), coords.begin(), coords.begin()+3);

    try
    {
        std::map<int, Piece*> inner = _coordsToPiece.at(outerCoords);
        if (!inner.empty())
        {
            std::map<int, Piece*>::reverse_iterator topPiece = inner.rbegin();
            for (topPiece; topPiece != inner.rend(); topPiece++)
            {
                if (topPiece->second != nullptr)
                {
                    outerCoords.push_back(topPiece->first+1);
                    return outerCoords;
                };
            };
            outerCoords.push_back(0);
            return outerCoords;
        }
        else
        {
            outerCoords.push_back(0);
            return outerCoords;
        }
    }
    catch (std::out_of_range &e)
    {
        outerCoords.push_back(0);
        return outerCoords;
    };
};

std::vector<std::vector<int>> PieceTable::adjacencies(std::string label, bool empty)
{
    Piece *neighbor;
    std::vector<std::vector<int>> existingNeighbors;

    Piece *target = find(label);
    // empty check here
    std::vector<std::vector<int>> neighborCoords = target->getAllNeighbors();
    
    for (int i = 0; i < 6; i++)
    {
        neighborCoords[i] = top(neighborCoords[i]);
        neighbor = find(neighborCoords[i]);
        if ((neighbor != nullptr && !empty) ||
            (neighbor == nullptr && empty))
        {
            existingNeighbors.push_back(neighborCoords[i]);
        };
    };

    return existingNeighbors;
};

std::vector<std::vector<int>> PieceTable::adjacencies(Position *pos, bool empty)
{
    Piece *neighbor;
    std::vector<std::vector<int>> existingNeighbors;

    std::vector<std::vector<int>> neighborCoords = pos->getAllNeighbors();
    
    for (int i = 0; i < 6; i++)
    {
        neighborCoords[i] = top(neighborCoords[i]);
        neighbor = find(neighborCoords[i]);
        if ((neighbor != nullptr && !empty) ||
            (neighbor == nullptr && empty))
        {
            existingNeighbors.push_back(neighborCoords[i]);
        };
    };

    return existingNeighbors;
};

void PieceTable::update(Move move, bool reversible)
{
    int i;
    Piece *target;
    Piece *underPiece;
    std::vector<int> outer;
    std::vector<int> newCoords;
    std::vector<int> underCoords;
    
    if (reversible)
    {
        _storeUndo(move);
    };

    if (!move.newPiece)
    {
        if (!reversible) // debug
        {
            std::cout << "Candidate" << std::endl;
        };

        std::vector<int> oldCoords;

        target = find(move.from);
        oldCoords = target->getCoords();
        newCoords= top(find(move.to)->getNeighbor(move.direction));
        target->setCoords(newCoords);

        // adding new coordinate mapping
        outer = {newCoords[0], newCoords[1], newCoords[2]};
        _coordsToPiece[outer][newCoords[3]] = target;
        
        // erasing old coordinate mapping
        outer = {oldCoords[0], oldCoords[1], oldCoords[2]};
        // this leaves behind the x ,y, z key but breaks searches to full coords by removing v
        _coordsToPiece[outer].erase(oldCoords[3]);

        // track topping
        if (newCoords[3] > 0)
        {
            underCoords = newCoords;
            underCoords[3]--;
            underPiece = find(underCoords);
            underPiece->isTopped = true; // letting this fail for now to alert to junk underCoords
        };

        if (oldCoords[3] > 0)
        {
            underCoords = oldCoords;
            underCoords[3]--;
            underPiece = find(underCoords);
            underPiece->isTopped = false;
        };
    }
    else
    {
        if (!move.firstPiece)
        {
            newCoords = find(move.to)->getNeighbor(move.direction);
            target = new Piece(newCoords, move.code, move.from);

            outer = {newCoords[0], newCoords[1], newCoords[2]};
            _coordsToPiece[outer][newCoords[3]] = target;

            _labelToPiece[move.from] = target;
        }
        else
        {
            newCoords = {0, 0, 0, 0};
            target = new Piece(newCoords, move.code, move.from);

            outer = {newCoords[0], newCoords[1], newCoords[2]};
            _coordsToPiece[outer][newCoords[3]] = target;

            _labelToPiece[move.from] = target;
        };

        counts[move.code]++;

        // queen update
        if (move.code == PieceCodes::wQ)
        {
            wQueen = true;
        }
        else if (move.code == PieceCodes::bQ)
        {
            bQueen = true;
        };
    };
};

void PieceTable::remove(std::string pieceLabel)
{
    Piece *target = find(pieceLabel);
    std::vector<int> coords = target->getCoords();
    std::vector<int> outer {coords[0], coords[1], coords[2]};

    if (coords[3] > 0)
    {
        std::vector<int> underCoords = coords;
        underCoords[3]--;
        Piece *underPiece = find(underCoords); // also letting this fail on bad underCoords
        underPiece->isTopped = false;
    };
    
    _coordsToPiece[outer].erase(coords[3]);
    _labelToPiece.erase(pieceLabel);
    counts[target->code]--;

    if (pieceLabel == "wQ")
    {
        wQueen = false;
    }
    else if (pieceLabel == "bQ")
    {
        bQueen = false;
    };

    delete target; // suspicious
};

void PieceTable::undoLast()
{
    Move *undo = &_undoCache.back();

    std::cout << board->turn << " UNDO " <<  undo->toString() << std::endl;

    int code = Utils::labelToCode(undo->from);
    int count = counts[code];

    if (undo->newPiece)
    {
        remove(undo->from);
    } 
    else
    {
        update(*undo, false); // is this dereference appropriate?
    };

    int count2 = counts[code];

    _undoCache.pop_back();
};

bool PieceTable::empty()
{
    if (counts.empty())
    {
        return true;
    }
    else
    {
        std::map<int, int>::iterator countIt;

        for (countIt = counts.begin(); countIt != counts.end(); countIt++)
        {
            if (countIt->second != 0)
            {
                return false;
            };
        };
        return true;
    };
};

Piece *PieceTable::getFirst()
{
    if (empty())
    {
        return nullptr;
    }
    else
    {
        Piece *target = _labelToPiece.begin()->second;
        return target;
    };
};

std::string PieceTable::nextLabel(int code)
{
    std::string label = PieceNames[code];

    if (code % 5 != 0)
    {
        label += std::to_string(counts[code] + 1);
    }
    return label;
};

void PieceTable::_storeUndo(Move move)
{
    if (move.newPiece)
    {
        _undoCache.push_back(move); // NOT A MOVE
    }
    else
    {
        Piece *target = find(move.from);
        Piece *oldNeighbor = nullptr;

        bool found = false;

        for (int i = 0; i < 6; i++)
        {
            oldNeighbor = find(target->getNeighbor(i));
            
            if (oldNeighbor != nullptr)
            {
                int reverseDirection = (3 + i) % 6;
                Move reverseMove(target->label, oldNeighbor->label, reverseDirection);
                _undoCache.push_back(reverseMove); // maybe a move
                found = true;
                break;
            };   
        };

        if (!found) { std::cout << "FAILED STOREUNDO" << std::endl; };
    };
};

std::vector<Piece*> PieceTable::getColorPieces()
{
    if (board != nullptr)
    {
        return getColorPieces(board->white);
    }
    else
    {
        // PLACEHOLDER: throw some error
        std::vector<Piece*> badReturn;
        return badReturn;
    };
}

std::vector<Piece*> PieceTable::getColorPieces(bool white)
{
    std::map<std::string, Piece*>::iterator labelIt;
    std::vector<Piece*> targets;
    char key = white ? 'w' : 'b';

    for (labelIt = _labelToPiece.begin(); labelIt != _labelToPiece.end(); labelIt++)
    {
        if (labelIt->first[0] == key)
        {
            targets.push_back(labelIt->second);
        };
    };

    return targets;
};

std::vector<Piece*> PieceTable::getAllPieces()
{
    std::map<std::string, Piece*>::iterator labelIt;
    std::vector<Piece*> targets;

    for (labelIt = _labelToPiece.begin(); labelIt != _labelToPiece.end(); labelIt++)
    {

        targets.push_back(labelIt->second);

    };

    return targets;   
};

std::set<std::vector<int>> PieceTable::getPinned()
// returns a set of articulation vertices for the current board.
{
    if (empty())
    {
        return {};
    };

    _OneHiveInfo info;
    std::vector<int> rootParent {-1, -1, -1, -1};
    std::vector<int> start = getFirst()->getCoords();

    // this will break if there are no pieces on the board
    _pinSearch(rootParent, start, &info);

    return info.articulations;
};


void PieceTable::_pinSearch(std::vector<int> &parent, std::vector<int> &location, _OneHiveInfo *info)
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
    std::vector<std::vector<int>> adj = adjacencies(&current);
    std::vector<std::vector<int>>::iterator adjacentIt = adj.begin();

    for (adjacentIt = adj.begin(); adjacentIt != adj.end(); adjacentIt++)
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
                _pinSearch(location, *adjacentIt, info);

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

int PieceTable::checkGameState()
{
    // check for empty
    
    if (empty()) { return 0; };

    // else

    bool wCapture = false;
    bool bCapture = false;
    std::vector<std::vector<int>> adj;

    if (wQueen)
    {
        adj = adjacencies("wQ");
        if (adj.size() == 6)
        {
            wCapture = true;
        };
    };

    if (bQueen)
    {
        adj = adjacencies("bQ");
        if (adj.size() == 6)
        {
            bCapture = true;
        };
    };

    // enum for organization?
    if (bCapture && wCapture)
    {
        return 2;
    }
    else if (bCapture)
    {
        return 3;
    }
    else if (wCapture)
    {
        return 4;
    }
    else
    {
        return 1;
    };
};

// board scoring

int PieceTable::checkMateScore = 100000;

int PieceTable::drawScore = 1;

std::vector<int> PieceTable::baseScores
{
    10,
    10,
    10,
    10,
    10
};

std::vector<std::vector<int>> PieceTable::offScores
{
    {0, -100, -40, 0, 0},
    {0, 100, 80, 50, 10},
    {80, 60, 40, 20, 10},
    {0, 60, 40, 20, 10},
    {0, 60, 40, 20, 10}
};

std::vector<std::vector<int>> PieceTable::defScores
{
    {0, 0, 0, 0},
    {0, 50, 20, 10},
    {180, 200, 50, 10},
    {0, 160, 0, 0},
    {0, 50, 20, 10}
};

int PieceTable::score()
{
    if (board != nullptr)
    {
        return score(board->white);
    }
    else
    {
        // PLACEHOLDER: throw some error
        return -1234512345;
    };
};

int PieceTable::score(bool white)
{
    int sign = white ? 1 : -1;
    std::set<std::vector<int>> pinned = getPinned();
    
    switch (board->gamestate)
    {
        case 2:
            return drawScore;
        case 3:
            return sign * checkMateScore;
        case 4:
            return sign * -checkMateScore;
        default:
            break;
    };

    // no captures or draws

    int tempScore;
    int distance;
    std::vector<std::vector<int>> adj;
    int score = 0;
    std::vector<Piece*> pieces = getAllPieces();

    if (wQueen)
    {
         adj = adjacencies("wQ");

        // for every adjacent coordinate with a piece
        for (std::vector<int> el: adj) 
        {
            // if that piece is white, do this
            if (find(el)->code < PieceCodes::bQ)
            {
                // if the piece is white, +/- by 50
                score -= 50;
            }
            // otherwise
            else
            {
                // if the piece is black, +/- by 100 
                score -= 100;
            };
        };
    };

    if (bQueen)
    {
        adj = adjacencies("bQ");

        // for every adjacent coordinate with a piece
        for (std::vector<int> el: adj) 
        {
            // if that piece is white, do this
            if (find(el)->code < PieceCodes::bQ)
            {
                // if the piece is white, +/- by 100
                score += 100;
            }
            // otherwise
            else
            {
                // if the piece is black, +/- by 50
                score += 50;
            };
        };
    };
    
    for (Piece* p: pieces)
    {
        tempScore = 0;

        // if the piece is the same color as our current player, do this
        if (p->white)
        {
            // add base score
            tempScore += baseScores[p->code % 5];

            // offensive scores
            if (bQueen)
            {
                distance = p->findDistance(find("bQ"));
                if (distance < 5)
                {
                    tempScore += offScores[p->code % 5][distance];
                };
            };

            // defensive scores
            if (wQueen)
            {
                distance = p->findDistance(find("wQ"));
                if (distance < 4)
                {
                    tempScore += defScores[p->code % 5][distance];
                };
            };
        }
        else
        {
            tempScore -= baseScores[p->code % 5];

            if (wQueen)
            {
                distance = p->findDistance(find("wQ"));
                if (distance < 5)
                {
                    tempScore -= offScores[p->code % 5][distance];
                };
            };

            if (bQueen)
            {
                distance = p->findDistance(find("bQ"));
                if (distance < 4)
                {
                    tempScore -= defScores[p->code % 5][distance];
                };
            };
        };

        if (pinned.find(p->getCoords()) != pinned.end())
        {
            tempScore /= 2; // halve scores for pinned pieces
        };

        score += tempScore;
    };

    return sign * score;
};

std::string PieceTable::coordsMapToString()
{
    std::map<std::vector<int>, std::map<int, Piece*>>::iterator outerIt;
    std::map<int, Piece*>::iterator innerIt;
    std::string repr = "";

    for (outerIt = _coordsToPiece.begin(); outerIt != _coordsToPiece.end(); outerIt++)
    {
        for (innerIt = outerIt->second.begin(); innerIt != outerIt->second.end(); innerIt++)
        {
            if (innerIt->second != nullptr)
            {
                repr += " " + innerIt->second->label + " " + 
                    std::to_string(outerIt->first[0]) + "," +
                    std::to_string(outerIt->first[1]) + "," +
                    std::to_string(outerIt->first[2]) + "," +
                    std::to_string(innerIt->first) + ":";
            };
        };
    };

    return repr;
};

PieceTable::~PieceTable()
{
    std::vector<Piece*> toDelete;
    std::vector<Piece*>::iterator deleteIt;
    std::map<std::string, Piece*>::iterator labelIt;

    for (labelIt = _labelToPiece.begin(); labelIt != _labelToPiece.end(); labelIt++)
    {
        toDelete.push_back(labelIt->second);
    };

    for (deleteIt = toDelete.begin(); deleteIt != toDelete.end(); deleteIt++)
    {
        delete *deleteIt;
    };
};







