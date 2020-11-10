#include "source/Position.h"
#include "source/ZobristHash.h"
#include "source/Move.h"
#include "source/Board.h"
#include "source/Engine.h"
#include "source/PieceInfo.h"
#include "source/Interface.h"
#include "source/Utils.h"
#include <iostream>

int main()
{
    // testing

    std::map<int, int> defaultConfig
    {
        {PieceCodes::wQ, 1},
        {PieceCodes::wA, 3},
        {PieceCodes::wB, 2},
        {PieceCodes::wG, 3},
        {PieceCodes::wS, 2},
        {PieceCodes::bQ, 1},
        {PieceCodes::bA, 3},
        {PieceCodes::bB, 2},
        {PieceCodes::bG, 3},
        {PieceCodes::bS, 2}
    };

    UHPInterface inter(defaultConfig);
    inter.initTerminal();
};