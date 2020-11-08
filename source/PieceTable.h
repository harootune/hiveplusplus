#pragma once

#ifndef _PIECETABLE_
#define _PIECETABLE_
#include "Position.h"
#include "Move.h"
#include <vector>
#include <map>
#include <stack>
#include <set>

class Board;

struct _OneHiveInfo
{
    int time = 0;
    std::set<std::vector<int>> articulations;
    std::map<std::vector<int>, int> entryTime;
    std::map<std::vector<int>, int> earliestTime;
    std::map<std::vector<int>, bool> visited;
};

class PieceTable
{
    public:
        std::vector<Move> _undoCache; // move back

        std::map<int, int> counts;
        bool wQueen = false;
        bool bQueen = false;
        Board *board = nullptr;

        PieceTable() {};
        PieceTable(Board *b) { board = b; };

        Piece *find(std::string label);
        Piece *find(std::vector<int> coords);
        std::vector<int> top(std::vector<int> coords);
        // do we need an overload for top?
        std::vector<std::vector<int>> adjacencies(std::string label, bool empty = false);
        std::vector<std::vector<int>> adjacencies(Position *pos, bool empty = false);
        
        void update(Move move, bool reversible = true);
        void remove(std::string pieceLabel);
        void undoLast();
        bool empty();
        Piece* getFirst();
        std::string nextLabel(int code);
        std::vector<Piece*> getColorPieces();
        std::vector<Piece*> getColorPieces(bool white);
        std::vector<Piece*> getAllPieces();
        std::set<std::vector<int>> getPinned();
        
        int checkGameState();

        // this should get separated out into some other class
        static int checkMateScore;
        static int drawScore;
        static std::vector<int> baseScores;
        static std::vector<std::vector<int>> offScores;
        static std::vector<std::vector<int>> defScores;
        int score ();
        int score (bool white);

        // debug
        std::string coordsMapToString();


        ~PieceTable();

    private: 
        std::map<std::string, Piece*> _labelToPiece;
        std::map<std::vector<int>, std::map<int, Piece*>> _coordsToPiece;

        void _storeUndo(Move move);
        void _pinSearch(std::vector<int> &parent, std::vector<int> &location, _OneHiveInfo *info);
};


#endif