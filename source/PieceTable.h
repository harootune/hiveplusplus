#pragma once

#ifndef _PIECETABLE_
#define _PIECETABLE_
#include "Position.h"
#include "Move.h"
#include <vector>
#include <map>
#include <stack>

class PieceTable
{
    public:
        std::map<int, int> counts;

        Piece *find(std::string label);
        Piece *find(std::vector<int> coords);
        void update(Move move, bool reversable = true);
        void remove(std::string pieceLabel);
        void undoLast();
        bool empty();
        std::string nextLabel(int code);

        ~PieceTable();

    private: 
        std::map<std::string, Piece*> _labelToPiece;
        std::map<std::vector<int>, std::map<int, Piece*>> _coordsToPiece;
        std::stack<Move> _undoCache;

        void _storeUndo(Move move);
};


#endif