#pragma once

#ifndef _HIVEBOARD_
#define _HIVEBOARD_
#include "ZobristHash.h"
#include "Position.h"
#include <map>
#include <vector>
#include <string>

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

        Board();
        Board(std::map<int, int> pieceConfig);

        void makeMove(std::string moveCode);
        void undoMove();


    private:
        // MoveGenerator _moveGen;
        std::map<int, int> _pieceConfig;
        std::map<int, int> _pieceCounts;
        std::map<std::vector<int>, Piece> _pieceLocations;
        std::multimap<std::vector<int>, std::vector<int>> _pieceAdjacencies;
        ZobristHash _hash;

};

#endif