#include "Position.h"

std::vector<std::vector<int>> Position::_translations 
{
    {0, 1, -1},
    {1, 0, -1},
    {1, -1, 0},
    {0, -1, 1},
    {-1, 0, 1},
    {-1, 1, 0}
};

Position::Position()
{
    _coords = {0, 0, 0};
}

Position::Position(std::vector<int> coords)
{
    _coords = coords;
};

std::vector<int> Position::getCoords()
{
    return _coords;
};

void Position::setCoords(std::vector<int> c)
{
    _coords = c;
};

std::vector<int> Position::getNeighbor(int direction)
{
    std::vector<int> neighbor = _coords;
    std::vector<int> &translation = Position::_translations[direction];

    for (int i = 0; i < 3; i++)
    {
        neighbor[i] += translation[i];
    };

    return neighbor;
};

std::vector<std::vector<int>> Position::getAllNeighbors()
{
    std::vector<std::vector<int>> neighbors;

    for (int i = 0; i < 6; i++)
    {
        neighbors.push_back(getNeighbor(i));
    };

    return neighbors;
};





