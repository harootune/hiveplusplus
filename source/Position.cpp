#include "Position.h"

const std::vector<std::vector<int>> Position::_dirToTranslations
{
    {0, 1, -1},
    {1, 0, -1},
    {1, -1, 0},
    {0, -1, 1},
    {-1, 0, 1},
    {-1, 1, 0}
};

const std::map<std::vector<int>, int> Position::_translationsToDir
{
    {{0, 1, -1}, 0},
    {{1, 0, -1}, 2},
    {{1, -1, 0}, 3},
    {{0, -1, 1}, 4},
    {{-1, 0, 1}, 5},
    {{-1, 1, 0}, 6},
};

Position::Position()
{
    _coords = {0, 0, 0, 0};
};

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
    std::vector<int> translation = convertDirection(direction);

    for (int i = 0; i < 3; i++)
    {
        neighbor[i] += translation[i];
    };
    neighbor[3] = 0; // this ensures that we always explore the lowest level of the board

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

std::vector<int> Position::convertDirection(int dir)
{
    return _dirToTranslations[dir]; // checking
};

int Position::convertTranslation(const std::vector<int> &trans)
{
    return _translationsToDir.find(trans)->second; // checking
};

int Position::findDirection(const std::vector<int> &from, const std::vector<int> &to)
{
    return convertTranslation(
        {
            to[0] - from[0],
            to[1] - from[1],
            to[2] - from[2]
        }
    );
};

Piece::Piece()
{
    code = -1;
    label = "EMPTY";
    isTopped = false;
    _coords = {0, 0, 0, 0};
};

Piece::Piece(std::vector<int> coords)
{
    code = -1;
    label = "EMPTY";
    isTopped = false;
    _coords = coords;
};

Piece::Piece(std::vector<int> coords, int c, std::string l)
{
    code = c;
    label = l;
    isTopped = false;
    _coords = coords;
};




