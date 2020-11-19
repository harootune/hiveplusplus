#pragma once

#ifndef _HIVEBOARD_
#define _HIVEBOARD_
#include <Board.h>
#include <ZobristHash.h>
#include <TransTable.h>
#include <Position.h>
#include <Move.h>
#include <map>
#include <vector>
#include <string>
#include <stack>
#include <set>
#include <chrono>

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
        std::vector<LabelMove> genAllMoves();

        /* Board Manipulation */
        // Make a move, updating the Board and game-related variables, given a move object
        void makeMove(LabelMove &move);
        // Make a move, updating the Board and game-related variables, given a MoveString
        void makeMove(std::string moveString); 
        // Undo the last move made
        void undoLast();

        /* Move Recommendation */
        // Recommend a best move derived from a negamax search
        LabelMove recommendMove(int depth = 4, int duration = 0);

        /* Misc */
        // Produce a GameString representing the current board
        std::string toString();
        // Reset the game to a beginning state
        void reset();
        // Checks if a given move is legal in the current turn
        LabelMove *validateMove(LabelMove *move); // rework this
        // Converts a move string to a Move object -- maybe move to utils?
        LabelMove stringToMove(std::string moveString);
        // Change the size of the composited transposition table
        void setTableSize(int bytes) { _transTable.setMaxSize(bytes); };

        
        /* DEBUG */
        int score();
        std::string toCoordString();

    private:
        /* Member Variables */
        // The current configuration of pieces
        std::map<int, int> _pieceConfig;
        // Tracks the current Zobrist Hash for this engine
        ZobristHash _hash;
        // Transposition table
        TranspositionTable _transTable;
        // Tracks the location of pieces and allows for easy traversal between them
        Board _board;
                            
        /* Move Generation */
        // A board traversal used to find valid move destinations for all but the grasshopper
        void _moveSearch(std::string label, int code, Position *current,
                        std::vector<LabelMove> &moves, std::set<std::vector<int>> &seen, int depth);
        // A board traversal used to find valid move destinations for grasshoppers
        void _hopperSearch(std::string label, int direction, Position *current, std::vector<LabelMove> &moves);
        // Generates all legal placement moves for the current player
        std::vector<LabelMove> _genPlacementMoves();
        // Generates all legal moves for a target queen piece
        std::vector<LabelMove> _genQueenMoves(std::string label);
        // Generates all legal moves for a target ant piece
        std::vector<LabelMove> _genAntMoves(std::string label);
        // Generates all legal moves for a target beetle piece
        std::vector<LabelMove> _genBeetleMoves(std::string label);
        // Generates all legal moves for a target grasshopper piece
        std::vector<LabelMove> _genHopperMoves(std::string label);
        // Generates all legal moves for a target spider piece
        std::vector<LabelMove> _genSpiderMoves(std::string label);

        /* Recentering */
        void _recenter();

        /* Move Recommendation */
        // The root call of the negamax search
        LabelMove _negaMax(int alpha, int beta, int maxDepth, int duration, 
                            std::chrono::time_point<std::chrono::high_resolution_clock> &start,  
                            std::vector<PositionMove> &killerMoves);
        // The recursive component of the negamax search
        int _negaMaxSearch(int alpha, int beta, int depth, int maxDepth, int duration, 
                            std::chrono::time_point<std::chrono::high_resolution_clock> &start, 
                            std::vector<PositionMove> &killerMoves);
        // Rotate a coordinate clockwise around the origin
        void _rotate(std::vector<int> &coords);
        // Mirror a coordinate across the origin
        void _mirror(std::vector<int> &coords);
        // Apply a combinations of mirrors or rotations represented with a 3-item int vector
        void _applyTransformation(std::vector<int> &coords, std::vector<int> &transformation, bool reverse = false);
        // Find a rotation/mirror combination with a valid transposition, if any
        std::vector<int> _findTransposedRotation(int maxDepth, int currentDepth);
        // Find an exact transposition of the current game and search state
        PositionMove _findExactTransposition(std::vector<int> &transformation);
        // Find all transpositions of the current gamestate, regardless of search state
        std::vector<LabelMove> _findAdjacentTranspositions(std::vector<int> &transformation, int maxDepth, int currentDepth);
        // Store a best move to a transformed transposition in the trans table
        void _storeTransformedBest(PositionMove bestMove, std::vector<int> transformKey);

        /* Misc */
        LabelMove _labelNonMove;
        PositionMove _positionNonMove;
        int _defaultDepth = 1000000;
};

#endif