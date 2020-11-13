#pragma once

#ifndef _PIECETABLE_
#define _PIECETABLE_
#include "Position.h"
#include "Move.h"
#include <vector>
#include <map>
#include <stack>
#include <set>


struct _OneHiveInfo
// A structure used to store information relevant to the articulation vertex search used to enforce the one hive rule
{
    int time = 0; // current time of the search
    std::set<std::vector<int>> articulations; // located articulation vertices
    std::map<std::vector<int>, int> entryTime; // map of entry points for each vertex
    std::map<std::vector<int>, int> earliestTime; // map of earliest reachable times for each vertex
    std::map<std::vector<int>, bool> visited; // map tracking which vertices have been visited thus far
};

class Board
{
    public:
        /* DEBUG - MOVE BACK TO PRIVATE ASAP */
        // A series of Move objects allowing the Board to restore earlier piece arrangements
        // Note that these are not NECESSARILY the same move objects as used to make moves forward in time
        // Non-placement moves in particular require that we store a "reverse" move
        std::vector<Move> _undoCache; // move back

        /* Member Variables */
        // Counts for each piece code
        std::map<int, int> counts;
        // Whether or not the white queen is present on board
        bool wQueen = false;
        // Whether or not the black queen is present on board
        bool bQueen = false;

        /* Piece Location */
        // Find a piece using a label
        Piece *find(std::string label);
        // Find a piece using a coordinate
        Piece *find(std::vector<int> coords);
        // Find the piece at the highest v coordinate for a certain x, y, z (expects full x, y, z, v input)
        std::vector<int> top(std::vector<int> coords);
        // Find empty (empty = true) or occupied adjacent hexes to a given piece identified with a label
        std::vector<std::vector<int>> adjacencies(std::string label, bool empty = false);
        // find empty (empty = true) or occuped adjacent hexes to a given piece identified with a position -- should this be a coordinate instead of a position?
        std::vector<std::vector<int>> adjacencies(Position *pos, bool empty = false);
        // returns a pointer to the first piece in the table (used to get an arbitrary valid piece)
        Piece* getFirst();
        // get all pieces of the specified color (true == white, false == black)
        std::vector<Piece*> getColorPieces(bool white);
        // get all pieces on the board
        std::vector<Piece*> getAllPieces();
        // get all pieces pinned due to the one hive rule
        std::set<std::vector<int>> getPinned();
        
        /* Piece Manipulation */
        // Make a move described with a Move object. If reversible = true, a reverse Move will be stored in the undocache
        void update(Move move, bool reversible = true);
        // Remove a piece from the board
        void remove(std::string pieceLabel);
        // Undo the most recent Move
        void undoLast();

        /* Scoring -- should this be a composited object? */
        // checkmate scores
        static int checkMateScore;
        // score for a draw
        static int drawScore;
        // base scores for each piece code
        static std::vector<int> baseScores;
        // scaling offense bonuses per piece code
        static std::vector<std::vector<int>> offScores;
        // scaling defense bonuses per piece code
        static std::vector<std::vector<int>> defScores;
        // get the score for the specified color (true == white, false == black)
        int score (bool white);

        /* Misc */
        // return true if the board is empty, false otherwise
        bool empty();
        // reset the board to an empty state
        void clear();
        // return the next label in sequence for a given piece code
        std::string nextLabel(int code);
        // returns a code indicating the current gamestate of the board (captures, draws, etc)
        int checkGameState();

        /* DEBUG */
        std::string coordsMapToString();

        /* Destructor */
        ~Board();

    private: 
        /* Member Variables */
        // A map of labels to piece pointers
        std::map<std::string, Piece*> _labelToPiece;
        // A map of coordinates to piece pointers. Made a nested map to allow easy access to other pieces at same x, y, z
        std::map<std::vector<int>, std::map<int, Piece*>> _coordsToPiece;

        /* Misc */
        // Store a reverse Move in the _undoCache
        void _storeUndo(Move move);
        // Recursive search for articulation vertices, used with getPinned
        void _pinSearch(std::vector<int> &parent, std::vector<int> &location, _OneHiveInfo *info);
        // Delete all dynamically allocated Piece objects to prevent memory leakage
        void _clearPieces();
};


#endif