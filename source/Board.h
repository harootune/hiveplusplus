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
    Play,
    Win,
    Draw
};

struct _OneHiveInfo
{
    int time = 0;
    std::set<std::vector<int>> articulations;
    std::map<std::vector<int>, int> entryTime;
    std::map<std::vector<int>, int> earliestTime;
    std::map<std::vector<int>, bool> visited;
};

class Board
{
    public:
        int gamestate;

        Board();
        Board(std::map<int, int> pieceConfig);

        std::vector<Move> genAllMoves();
  
        void makeMove(Move move);
        void makeMove(std::string moveString); 
        void undoLast();

    private:
        bool _white;
        int _turn;
        std::map<int, int> _pieceConfig;
        // std::vector<std::string> _moveCache;

        ZobristHash _hash;
        PieceTable _pieces;

        bool _validateMove(std::vector<std::string> move);
        
        // one hive rule enforcement
        std::set<std::vector<int>> _oneHiveCheck(std::vector<int> start);
        void _oneHiveSearch(std::vector<int> &parent, std::vector<int> &location, _OneHiveInfo *info);
                            
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
};

#endif