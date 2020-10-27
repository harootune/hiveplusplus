#include "PieceInfo.h"
#include <vector>
#include <map>

std::vector<std::string> PieceNames
{
    "wQ",
    "wA",
    "wB",
    "wG",
    "wS",
    "bQ",
    "bA",
    "bB",
    "bG",
    "bS"
};

std::map<std::string, int> LabelCodes
{
    {"wQ", 0},
    {"wA", 1},
    {"wB", 2},
    {"wG", 3},
    {"wS", 4},
    {"bQ", 5},
    {"bA", 6},
    {"bB", 7},
    {"bG", 8},
    {"bS", 9},
};