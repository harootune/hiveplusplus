#pragma once

#ifndef _HIVEUTILS_
#define _HIVEUTILS_
#include <iostream>
#include <vector>


namespace Utils
// A set of utilities used in various places around the rest of the engine
{
    // a string tokenization function
    std::vector<std::string> tokenize(std::string input, char delimiter);
    // strips whitespace from either end of a string
    std::string strip(std::string input);
    // determines if a string is a MoveString as defined in the UHP
    bool isMoveString(std::string input);
    // convert a piece label to its corresponding piece code
    int labelToCode(std::string label);
    // concatenate a coordinate vector with a piece code (used in the ZobristHash)
    std::vector<int> concatCoords(std::vector<int> coords, int piece);
};

#endif