#pragma once

#ifndef _HIVEBOARD_
#define _HIVEBOARD_
#include "PieceTable.h"
#include "ZobristHash.h"
#include "Position.h"
#include "Move.h"
#include <map>
#include <vector>
#include <string>
#include <stack>
#include <set>

enum GameStates
{
    NotStarted,
    InProgress,
    Draw,
    BCapture,
    WCapture
};

class Board
{
    public:
        int turn;
        bool white;
        int gamestate;

        Board();
        Board(std::map<int, int> pieceConfig);

        std::vector<Move> genAllMoves();
  
        void makeMove(Move move);
        void makeMove(std::string moveString); 
        void undoLast();
        Move recommendMove();

        std::string toString();
        
        int score();

    private:
        std::map<int, int> _pieceConfig;
        // std::vector<std::string> _moveCache;

        ZobristHash _hash;
        PieceTable _pieces;

        bool _validateMove(std::vector<std::string> move);
                            
        // move generation helpers
        Move _stringToMove(std::string moveString);
        void _moveSearch(std::string label, int code, Position *current,
                        std::vector<Move> &moves, std::set<std::vector<int>> &seen, int depth);
        void _hopperSearch(std::string label, int direction, Position *current, std::vector<Move> &moves);
        std::vector<Move> _genPlacementMoves();
        std::vector<Move> _genQueenMoves(std::string label);
        std::vector<Move> _genAntMoves(std::string label);
        std::vector<Move> _genBeetleMoves(std::string label);
        std::vector<Move> _genHopperMoves(std::string label);
        std::vector<Move> _genSpiderMoves(std::string label);

        // negamax search
        Move _negaMax(int alpha, int beta, int depth);
        int _negaMaxSearch(int alpha, int beta, int depth, int i);
};

#endif