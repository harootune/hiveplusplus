#pragma once

#ifndef _PIECEINFO_
#define _PIECEINFO_
#include <vector>
#include <map>
#include <string>

/* This header defines several useful global data structures for use in translating
between piece names and piece codes */

enum PieceCodes
// Integral codes for each piece type and color (piece codes)
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

// Piece codes -> piece names
extern const std::vector<std::string> PieceNames;

// Piece names -> piece codes
extern const std::map<std::string, int> LabelCodes;

#endif