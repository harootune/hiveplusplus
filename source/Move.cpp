#include "Move.h"
#include "Utils.h"
#include "PieceInfo.h"

Move::Move(std::string moveLabel, std::string destLabel, int dir, bool isNew)
{
    from = moveLabel;
    to = destLabel;
    code = Utils::labelToCode(moveLabel);
    direction = dir;
    newPiece = isNew;
    firstPiece = false;
};

Move::Move(std::string moveLabel)
// specifically for a first move
{
    from = moveLabel;
    to = moveLabel;
    code = Utils::labelToCode(moveLabel);
    direction = -1;
    newPiece = true;
    firstPiece = true;
};


std::string Move::toString()
{
    if (from == to)
    {
        return from;
    }
    else
    { 
        std::string repr = from + " ";

        switch (direction) {
            case 4:
                repr += "\\";
                break;
            case 3:
                repr += "-";
                break;
            case 2:
                repr += "/";
                break;
            default:
                break;
        };

        repr += to;

        switch (direction) {
            case 5:
                repr += "/";
                break;
            case 0:
                repr += "-";
                break;
            case 1:
                repr += "\\";
                break;
            default:
                break;
        };

        return repr;
    };
};