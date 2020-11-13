#pragma once

#ifndef _HIVEBOARD_
#define _HIVEBOARD_
#include "Board.h"
#include "ZobristHash.h"
#include "Position.h"
#include "Move.h"
#include <map>
#include <vector>
#include <string>
#include <stack>
#include <set>

enum GameStates
// An enumeration of possible gamestates (gamestate codes)
{
    NotStarted,
    InProgress,
    Draw,
    WhiteWins,
    BlackWins
};

class Engine
// The Hive Engine, which tracks a board and handles move recommendation
{
    public:
        /* Member Variables */
        // The number of turns which have passed
        int turn;
        // Color of the current player (true == white, false == black)
        bool white;
        // The current gamestate code
        int gamestate;
        // The move history of this engine
        std::vector<std::string> history;

        /* Constructors */
        Engine() {};
        Engine(std::map<int, int> pieceConfig);

        /* Move Generation */
        // Generate all moves for the current player on the current board
        std::vector<Move> genAllMoves();

        /* Board Manipulation */
        // Make a move, updating the Board and game-related variables, given a move object
        void makeMove(Move move);
        // Make a move, updating the Board and game-related variables, given a MoveString
        void makeMove(std::string moveString); 
        // Undo the last move made
        void undoLast();

        /* Move Recommendation */
        // Recommend a best move derived from a negamax search
        Move recommendMove();

        /* Misc */
        // Produce a GameString representing the current board
        std::string toString();
        // Reset the game to a beginning state
        void reset();
        // Checks if a given move is legal in the current turn
        Move *validateMove(Move *move);
        Move *validateMove(std::string moveString);
        
        /* DEBUG */
        int score();
        std::string toCoordString();

    private:
        /* Member Variables */
        // The current configuration of pieces
        std::map<int, int> _pieceConfig;\
        // Tracks the current Zobrist Hash for this engine
        ZobristHash _hash;
        // Tracks the location of pieces and allows for easy traversal between them
        Board _board;
                            
        /* Move Generation */
        // A board traversal used to find valid move destinations for all but the grasshopper
        void _moveSearch(std::string label, int code, Position *current,
                        std::vector<Move> &moves, std::set<std::vector<int>> &seen, int depth);
        // A board traversal used to find valid move destinations for grasshoppers
        void _hopperSearch(std::string label, int direction, Position *current, std::vector<Move> &moves);
        // Generates all legal placement moves for the current player
        std::vector<Move> _genPlacementMoves();
        // Generates all legal moves for a target queen piece
        std::vector<Move> _genQueenMoves(std::string label);
        // Generates all legal moves for a target ant piece
        std::vector<Move> _genAntMoves(std::string label);
        // Generates all legal moves for a target beetle piece
        std::vector<Move> _genBeetleMoves(std::string label);
        // Generates all legal moves for a target grasshopper piece
        std::vector<Move> _genHopperMoves(std::string label);
        // Generates all legal moves for a target spider piece
        std::vector<Move> _genSpiderMoves(std::string label);

        /* Move Recommendation */
        // The root call of the negamax search
        Move _negaMax(int alpha, int beta, int depth);
        // The recursive component of the negamax search
        int _negaMaxSearch(int alpha, int beta, int depth);
        
        /* Misc */
        // Converts a move string to a Move object -- maybe move to utils?
        Move _stringToMove(std::string moveString);
};

#endif