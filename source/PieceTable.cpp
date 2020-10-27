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

void PieceTable::update(Move move, bool reversable)
{
    int i;
    Piece *target;
    std::vector<int> outer;
    std::vector<int> newCoords;
    
    if (reversable)
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
    };
};

void PieceTable::remove(std::string pieceLabel)
{
    Piece *target = find(pieceLabel);
    std::vector<int> coords = target->getCoords();
    std::vector<int> outer {coords[0], coords[1], coords[2]};
    
    _coordsToPiece[outer].erase(coords[3]);
    _labelToPiece.erase(pieceLabel);
    counts[target->code]--;
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

std::string PieceTable::nextLabel(int code)
{
    std::string label = PieceNames[code];
    label += std::to_string(counts[code] + 1);
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







