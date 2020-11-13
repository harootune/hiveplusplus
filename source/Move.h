#pragma once

#ifndef _HIVEMOVE_
#define _HIVEMOVE_
#include <string>

class Move
// An object describing a piece placement or movement on the Board -- could probably be a struct
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
        // whether or not this is a new piece (placement move)
        bool newPiece;
        // whether or not this is the first piece of a game
        bool firstPiece;

        /* Constructors */
        Move() {};
        Move(std::string moveLabel);
        Move(std::string moveLabel, std::string destLabel, int dir, bool isNew = false);

        /* Operators */
        bool operator==(const Move &other);

        /* Misc */
        // Produce a MoveString representing this move
        std::string toString();
};

#endif