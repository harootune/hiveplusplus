#pragma once

#ifndef _HIVEMOVE_
#define _HIVEMOVE_
#include <string>
#include <vector>


// TODO: These two move classes really should inherit from a base class

class LabelMove
// An object describing a piece placement or movement on the Board in terms of labels
{
    public:
        /* Member Variables */
        // Label of the piece being placed or moved
        std::string from;
        // Label of the piece used to find the relative destination of the move
        std::string to;
        // code of the piece being moved or placed
        int code;
        // direction from the "to" piece to the destination of the move
        int direction;
        // whether or not this is a pass move
        bool pass;
        // whether or not this is a new piece (placement move)
        bool newPiece;
        // whether or not this is the first piece of a game
        bool firstPiece;

        /* Constructors */
        LabelMove();
        LabelMove(std::string moveLabel);
        LabelMove(std::string moveLabel, std::string destLabel, int dir, bool isNew = false);

        /* Operators */
        bool operator==(const LabelMove &other);
        bool operator!=(const LabelMove &other);

        /* Misc */
        // Produce a MoveString representing this move
        std::string toString();
};


class PositionMove
// An object describing a piece placement or movement on the Board in terms concrete hex coordinates
{

    public:
        // score is included in positionmoves because they are stored in the transposition table
        int score;
        int code;
        std::vector<int> from;
        std::vector<int> to;
        bool pass;
        bool newPiece;
        bool firstPiece;

        /* Constructors */
        PositionMove();
        PositionMove(std::string pass);
        PositionMove(int c);
        PositionMove(int c, std::vector<int> fromCoords, std::vector<int> toCoords, bool newP = false);

        /* Operators */
        bool operator==(const PositionMove &other);
        bool operator!=(const PositionMove &other);
};

#endif