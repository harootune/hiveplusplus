#pragma once

#ifndef _HIVESCORE_
#define _HIVESCORE_
#include <vector>

namespace Scores
{
    const int checkMateScore = 10000;

    const int drawScore = 1;

    const std::vector<std::vector<int>> baseScores
    {
        {0, 10, 10, 20, 20, 30, 30, 100, 100, 100, 100}, // Queen
        {0, 0, 0, 0, 0, 10, 10, 20, 20, 30, 30}, // Ant
        {0, 10, 10, 20, 20, 20, 20, 20, 20, 30, 30}, // Beetle
        {0, 10, 10, 10, 20, 30, 30, 20, 20, 20, 20}, // Hopper
        {0, 30, 30, 30, 30, 20, 20, 20, 10, 10, 10}, // Spider
    };

    const std::vector<std::vector<int>> offScores
    {
        {0, -50, -30},
        {0, 40, 0},
        {250, 80, 0},
        {0, 30, 0},
        {0, 40, 0}
    };  

    const std::vector<std::vector<int>> defScores
    {
        {0, 0, 0},
        {0, 20, 0},
        {0, 30, 20},
        {0, 30, 0},
        {0, 10, 0}
    };
};

#endif