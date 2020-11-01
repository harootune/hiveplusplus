#pragma once

#ifndef _HIVEUTILS_
#define _HIVEUTILS_
#include <iostream>
#include <vector>

namespace Utils
{
    std::vector<std::string> tokenize(std::string input, char delimiter);
    std::string strip(std::string input);
    bool isGameString(std::string input);
    int labelToCode(std::string label);
};

#endif