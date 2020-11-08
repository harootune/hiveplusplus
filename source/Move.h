#pragma once

#ifndef _HIVEMOVE_
#define _HIVEMOVE_
#include <string>

class Move
{
    public:
        std::string from;
        std::string to;
        int code;
        int direction;
        bool newPiece;
        bool firstPiece;

        Move() {};
        Move(std::string moveLabel);
        Move(std::string moveLabel, std::string destLabel, int dir, bool isNew = false);

        std::string toString();
};

#endif