#include "Board.h"

Board::Board(std::map<int, int> pieceConfig)
{   
    _pieceConfig = pieceConfig;
};

// void Board::makeMove(std::string moveCode)
// {
//     std::vector<std::string> code = _splitMoveCode(moveCode);
    
//     if (_validateMove(code))
//     {
//         // move behavior
//     }
//     else
//     {
//         // throw some error
//     };
// };

// bool Board::_validateMove(std::vector<std::string> move)
// {
    
// };



std::vector<std::string> _splitMoveCode(std::string moveCode)
{
    std::vector<std::string> code;
    std::string codeComp = "";
    std::string::iterator codeIt;

    for (codeIt = moveCode.begin(); codeIt != moveCode.end(); codeIt++)
    {
        if (*codeIt == ' ')
        {
            code.push_back(codeComp);
            codeComp = "";
        }
        else
        {
            codeComp.push_back(*codeIt);
        };
    };

    return code;
};





