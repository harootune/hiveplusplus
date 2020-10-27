#pragma once

#ifndef _PIECEINFO_
#define _PIECEINFO_
#include <vector>
#include <map>
#include <string>

enum PieceCodes
{
    wQ,
    wA,
    wB,
    wG,
    wS,
    bQ,
    bA,
    bB,
    bG,
    bS
};

extern std::vector<std::string> PieceNames;
extern std::map<std::string, int> LabelCodes;

#endif