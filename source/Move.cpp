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