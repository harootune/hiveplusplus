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
    int time;
    std::set<std::vector<int>> articulations;
    std::map<std::vector<int>, int> entryTime;
    std::map<std::vector<int>, int> outDegree;
    std::map<std::vector<int>, std::vector<int>> parent;
    std::map<std::vector<int>, std::vector<int>> earliestAncestor;
    std::map<std::vector<int>, bool> discovered;
    std::map<std::vector<int>, bool> processed;
    std::vector<int> root;

    _OneHiveInfo() {};
    _OneHiveInfo(std::vector<int> r) { root = r; };
};

class Board
{
    public:
        int gamestate;

        Board();
        Board(std::map<int, int> pieceConfig);

        std::vector<Move> genAllMoves();

        void makeMove(Move move);
        void undoLast();

    private:
        bool _white;
        int _turn;
        std::map<int, int> _pieceConfig;
        // std::vector<std::string> _moveCache;

        ZobristHash _hash;
        PieceTable _pieces;

        // move generation helpers
        bool _validateMove(std::vector<std::string> move);
        std::set<std::vector<int>> _oneHiveCheck(std::vector<int> start);
        void _oneHiveSearch(std::vector<int> &location, _OneHiveInfo *info);
        void _oneHiveEdge(std::vector<int> &from, std::vector<int> &to, _OneHiveInfo *info);
        int _oneHiveClass(std::vector<int> &from, std::vector<int> &to, _OneHiveInfo *info);
                            

        std::vector<Move> _genPlacementMoves();
        std::vector<Move> _genQueenMoves(std::string label);
        std::vector<Move> _genAntMoves(std::string label);
        std::vector<Move> _genBeetleMoves(std::string label);
        std::vector<Move> _genHopperMoves(std::string label);
        std::vector<Move> _genSpiderMoves(std::string label);
};

#endif