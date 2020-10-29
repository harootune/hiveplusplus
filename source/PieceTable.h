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
        std::vector<int> top(std::vector<int> coords);
        // do we need an overload for top?
        std::vector<std::vector<int>> adjacencies(std::string label, bool empty = false);
        std::vector<std::vector<int>> adjacencies(Position *pos, bool empty = false);

        void update(Move move, bool reversable = true);
        void remove(std::string pieceLabel);
        void undoLast();
        bool empty();
        std::string nextLabel(int code);
        std::vector<Piece*> getColorPieces(bool white);

        ~PieceTable();

    private: 
        std::map<std::string, Piece*> _labelToPiece;
        std::map<std::vector<int>, std::map<int, Piece*>> _coordsToPiece;
        std::stack<Move> _undoCache;

        void _storeUndo(Move move);
};


#endif