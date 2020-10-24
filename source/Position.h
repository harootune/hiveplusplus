#pragma once
#ifndef _POSITION_
#define _POSITION_
#include <vector>

enum Directions
{
    Up,
    UpRight,
    DownRight,
    Down,
    DownLeft,
    Upleft
};

class Position
{
    public:
        Position();
        Position(std::vector<int> coords);

        std::vector<int> getCoords();
        void setCoords(std::vector<int> c);
        std::vector<int> getNeighbor(int direction);
        std::vector<std::vector<int>> getAllNeighbors();

    private:
        std::vector<int> _coords;

        static std::vector<std::vector<int>> _translations;
};

#endif
