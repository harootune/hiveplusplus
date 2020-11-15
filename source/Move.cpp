#include "Move.h"
#include "Utils.h"
#include "PieceInfo.h"

LabelMove::LabelMove()
{
    from = "NULL";
    to = "NULL";
    code = -5;
    direction = -5;
    newPiece = false;
    firstPiece = false;
};

LabelMove::LabelMove(std::string moveLabel, std::string destLabel, int dir, bool isNew)
{
    from = moveLabel;
    to = destLabel;
    code = Utils::labelToCode(moveLabel);
    direction = dir;
    newPiece = isNew;
    firstPiece = false;
};

LabelMove::LabelMove(std::string moveLabel)
// specifically for a first move
{
    from = moveLabel;
    to = moveLabel;
    code = Utils::labelToCode(moveLabel);
    direction = -1;
    newPiece = true;
    firstPiece = true;
};

bool LabelMove::operator==(const LabelMove &other)
{
    return (
        from == other.from &&
        to == other.to &&
        code == other.code &&
        direction == other.direction &&
        newPiece == other.newPiece &&
        firstPiece == other.firstPiece
    );
};

bool LabelMove::operator!=(const LabelMove &other)
{
    bool check = this->operator==(other);
    return !check;
};

std::string LabelMove::toString()
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

PositionMove::PositionMove()
{
    score = 0;
    code = -5;
    from = {-1, -1, -1, -1};
    to = {-1, -1, -1, -1};
    newPiece = false;
    firstPiece = false; 
};

PositionMove::PositionMove(int c)
{
    score = 0;
    code = c;
    from = {0, 0, 0, 0};
    to = {0, 0, 0, 0};
    newPiece = true;
    firstPiece = true;
};

PositionMove::PositionMove(int c, std::vector<int> fromCoords, std::vector<int> toCoords, bool newP)
{
    score = 0;
    code = c;
    from = fromCoords;
    to = toCoords;
    newPiece = newP;
    firstPiece = false;
};

bool PositionMove::operator==(const PositionMove &other) // does not compare scores
{
    return code == other.code &&
            from == other.from &&
            to == other.to &&
            newPiece == other.newPiece &&
            firstPiece == other.firstPiece;
};

bool PositionMove::operator!=(const PositionMove &other)
{
    return !operator==(other);
};



