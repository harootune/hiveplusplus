#pragma once

#ifndef _HIVEBOARD_
#define _HIVEBOARD_
#include "PieceTable.h"
#include "ZobristHash.h"
#include "Position.h"
#include <map>
#include <vector>
#include <string>
#include <stack>

enum GameStates
{
    Play,
    Win,
    Draw
};

class Board
{
    public:
        int gamestate;

        Board() {};
        Board(std::map<int, int> pieceConfig);

        // void makeMove(std::string moveCode);
        // void undoMove();

    private:
        std::map<int, int> _pieceConfig;
        // std::vector<std::string> _moveCache;

        ZobristHash _hash;
        PieceTable _pieces;
        // MoveGenerator _moveGen;

        bool _validateMove(std::vector<std::string> move);
};

#endif