#include "Move.h"
#include "PieceInfo.h"

int labelToCode(std::string label)  // candidate for parse tools
{
  std::string prefix = label.substr(0, 2);
  return LabelCodes[prefix];  
};

Move::Move(std::string moveLabel, std::string destLabel, int dir, bool isNew)
{
    from = moveLabel;
    to = destLabel;
    code = labelToCode(moveLabel);
    direction = dir;
    newPiece = isNew;
    firstPiece = false;
};

Move::Move(std::string moveLabel)
// specifically for a first move
{
    from = moveLabel;
    to = moveLabel;
    code = labelToCode(moveLabel);
    direction = -1;
    newPiece = true;
    firstPiece = true;
};