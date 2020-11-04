#include "PieceTable.h"
#include "PieceInfo.h"
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
            outerCoords.push_back(topPiece->first);
        }
        else
        {
            outerCoords.push_back(0);
        }
        return outerCoords;
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
        std::vector<int> oldCoords;

        target = find(move.from);
        oldCoords = target->getCoords();
        newCoords= find(move.to)->getNeighbor(move.direction);
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

    delete target;
};

void PieceTable::undoLast()
{
    Move *undo = &_undoCache.top();

    if (undo->newPiece)
    {
        remove(undo->from);
    } 
    else
    {
        update(*undo, false); // is this dereference appropriate?
    };

    _undoCache.pop();
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
        _undoCache.push(move); // NOT A MOVE
    }
    else
    {
        Piece *target = find(move.from);
        Piece *oldNeighbor = nullptr;

        for (int i = 0; i < 6; i++)
        {
            oldNeighbor = find(target->getNeighbor(i));
            
            if (oldNeighbor != nullptr)
            {
                int reverseDirection = std::abs(i - 3);
                Move reverseMove(target->label, oldNeighbor->label, reverseDirection);
                _undoCache.push(reverseMove); // maybe a move
            };   
        };
    };
};

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

int PieceTable::score(bool white, std::set<std::vector<int>> pinned)
{
    int score = 0;
    int sign = white ? 1 : -1;
    bool wCapture = false;
    bool bCapture = false;

    if (wQueen)
    {
        std::vector<std::vector<int>> adj = adjacencies("wQ");
        if (adj.size() == 6)
        {
            wCapture = true;
        }
        else
        {
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
    };

    if (bQueen)
    {
        std::vector<std::vector<int>> adj = adjacencies("bQ");
        if (adj.size() == 6)
        {
            bCapture = true;
        }
        else
        {
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
    };

    if (bCapture && wCapture)
    {
        return PieceTable::drawScore;
    }
    else if (bCapture)
    {
        return sign * PieceTable::checkMateScore;
    }
    else if (wCapture)
    {
        return sign * -PieceTable::checkMateScore;
    };

    // no captures or draws
    int tempScore;
    int distance;
    std::vector<Piece*> pieces = getAllPieces();
    
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







