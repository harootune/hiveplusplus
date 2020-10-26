#pragma once

#ifndef _PIECEINFO_
#define _PIECEINFO_
#include <vector>
#include <string>

enum PieceCodes
{
    WQueen,
    WAnt,
    WBeetle,
    WHopper,
    WSpider,
    BQueen,
    BAnt,
    BBeetle,
    BHopper,
    BSpider
};

extern std::vector<std::string> PieceNames;

#endif