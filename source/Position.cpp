#include <Position.h>
#include <PieceInfo.h>
#include <algorithm>


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
    {{1, 0, -1}, 1},
    {{1, -1, 0}, 2},
    {{0, -1, 1}, 3},
    {{-1, 0, 1}, 4},
    {{-1, 1, 0}, 5},
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
    std::string errorRepr;

    int acc = 0;

    // fail if not an xyzv coordinate
    if (c.size() != 4)
    {
        errorRepr = "";
        for (int el: c)
        {
            errorRepr += std::to_string(el) + ", ";
        };

        throw (std::runtime_error("Cannot build a hex position from coordinates: " + errorRepr));        
    };

    for (int i = 0; i < 3; i++)
    {
        acc += c[i];
    };

    // fail if xyz doesnt equal 0
    if (acc != 0)
    {
        errorRepr = "";
        for (int el: c)
        {
            errorRepr += std::to_string(el) + ", ";
        };

        throw (std::runtime_error("Hex coordinates x,y,z must equal 0. Given: " + errorRepr));
    };

    _coords = c;
};


std::vector<int> Position::getNeighbor(int direction)
{
    std::vector<int> neighbor = _coords;
    std::vector<int> translation = convertDirection(direction);

    // Apply translation found by convertDirection
    for (int i = 0; i < 3; i++)
    {
        neighbor[i] += translation[i];
    };
    neighbor[3] = 0; // Ensures that we always look at the lowest level of the board -- maybe unnecessary?

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
    try
    {
        return _dirToTranslations[dir];
    }
    catch (std::out_of_range &e)
    {
        throw (std::runtime_error("Attempted conversion of invalid direction: " + std::to_string(dir)));
    };
};


int Position::convertTranslation(const std::vector<int> &trans)
{
    std::map<std::vector<int>, int>::const_iterator check = _translationsToDir.find(trans);
    
    if (check == _translationsToDir.end())
    {
        std::string errorRepr = "";
        for (int el: trans)
        {
            errorRepr += std::to_string(el) + ", ";
        };
        throw (std::runtime_error("Attempted conversion of invalid translation: " + errorRepr));
    };

    return _translationsToDir.find(trans)->second;
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


int Position::findDistance(Position *other)
// The distance in # of hex pieces (min translations from one Position to another)
// can be found by taking the maximum absolute value among the x, y, and z coordinates
{
    int check;

    int distance = 0;
    std::vector<int> otherCoords = other->getCoords();
    std::vector<int> distanceDiff {
        otherCoords[0] - _coords[0],
        otherCoords[1] - _coords[1],
        otherCoords[2] - _coords[2],
    };

    for (int dim: distanceDiff)
    {
        check = std::abs(dim);
        distance = std::max(distance, check);
    };

    return distance;
};


Piece::Piece()
{
    code = -1;
    label = "EMPTY";
    isTopped = false;
    _coords = {0, 0, 0, 0};
};


Piece::Piece(std::vector<int> &coords)
{
    code = -1;
    label = "EMPTY";
    isTopped = false;
    _coords = coords;
};


Piece::Piece(std::vector<int> &coords, int c, std::string l)
{
    code = c;
    label = l;
    white = code < PieceCodes::bQ;
    isTopped = false;
    _coords = coords;
};




