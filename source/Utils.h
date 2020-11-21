#pragma once

#ifndef _HIVEUTILS_
#define _HIVEUTILS_
#include <Board.h>
#include <Move.h>
#include <iostream>
#include <vector>
#include <chrono>


namespace Utils
// A set of utilities used in various places around the rest of the program
{
    /* String Manipulation */
    // a string tokenization function
    std::vector<std::string> tokenize(std::string input, char delimiter);
    // strips whitespace from either end of a string
    std::string strip(std::string input);
    
    /* UHP String Definition Checks */
    bool isMoveString(std::string input);
    bool isGameTypeString(std::string input);
    bool isGameStateString(std::string input);
    bool isMoveString(std::string input);
    bool isTurnString(std::string input);
    bool isGameString(std::string input);
    bool isTimeString(std::string input); // not formally defined in UHP - constitutes a HH:MM:SS timecode

    /* Move Conversion */
    // positionmove -> labelmove
    LabelMove toLabelMove(PositionMove &positionMove, Board &board);
    // labelmove -> positionmove
    PositionMove toPositionMove(LabelMove &labelMove, Board &board);

    /* Misc */
    // convert a piece label to its corresponding piece code
    int labelToCode(std::string label);
    // concatenate a coordinate vector with a piece code (used in the ZobristHash)
    std::vector<int> concatCoords(std::vector<int> coords, int piece);
    // extract the number of seconds from a TimeString
    int extractSeconds(std::string input);
    // check if a search duration has been met
    bool checkDuration(int duration, std::chrono::time_point<std::chrono::high_resolution_clock> &start);
};

#endif