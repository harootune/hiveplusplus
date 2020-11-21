#pragma once

#ifndef _POSITION_
#define _POSITION_
#include <vector>
#include <string>
#include <map>


/* A note about hex coordinates

The hex coordinate system used by this engine consists of four axes: x, y, z, and v. Commenting in the
source code may refer to these as xyzv coordinates, or a subset of xyzv coordinates.

x, y, and z can be thought to describe the same plane usual cartesian coordinates do, but in a way that is
more convenient for describing hexagon's positions relative to one another. They are arranged as such
(forgive the limitations of ascii art)

y+__         0
 |\      _________
     \  /         \
     5 /           \ 1
      /             \  ____\ x+
      \             /      /
     4 \           / 2
     /  \_________/        
 |/_          
z+           3  

In words, x increases as one moves to the right, y increases as one moves out from face 5, and z increases
as one moves out from face 4. The v axis describes the vertical height of a hexagon, in practical terms how
highly stacked a given hexagonal tile is on the Hive board.

This coordinate system has a number of interesting properties. Most importantly, all valid hex coordinates in this
system have xyz coordinates which, when summed, equal 0. This is used in several places throughout the program to
do convenient things, like find the approximate centroid of a board or transform the hash to find equivalent
board states in the transposition table.
*/


enum Directions
// Enumeration of directions around a hexagon (direction codes)
{
    Right,
    DownRight,
    DownLeft,
    Left,
    UpLeft,
    UpRight
};


class Position
// The base position class simulating hexagonal coordinates along 4 axes
{
    public:
        /* Constructors */
        Position();
        Position(std::vector<int> coords);

        /* Variable Access */
        virtual std::vector<int> getCoords();
        virtual void setCoords(std::vector<int> c);

        /* Interaction with other Positions/Coords */
        // Find neighbor coordinates in a certain direction
        virtual std::vector<int> getNeighbor(int direction);
        // Find all neighbor coordinates
        virtual std::vector<std::vector<int>> getAllNeighbors();
        // Find the distance between this Position and another
        virtual int findDistance(Position *other);
        
        /* Static Converters */
        // direction code to the associated translation in hexspace
        static std::vector<int> convertDirection(int dir);
        // translation in hexspace to the associated direction code
        static int convertTranslation(const std::vector<int> &trans);
        // direction between two adjacent hex coordinates
        static int findDirection(const std::vector<int> &from, const std::vector<int> &to);

        /* Destructor */
        virtual ~Position() {};

    protected:
        /* Member Variables */
        // Coordinates of the Position in hexspace
        std::vector<int> _coords;

        /* Static Variables */
        // A vector which serves as a map of direction codes to translations in hexspace
        const static std::vector<std::vector<int>> _dirToTranslations;
        // A map of translations in hexspace to direction codes
        const static std::map<std::vector<int>, int> _translationsToDir;
};


class Piece: public Position
// A Position with a Hive piece in it
{
    public:
        /* Constructors */
        Piece();
        Piece(std::vector<int> &coords);
        Piece(std::vector<int> &coords, int c, std::string l);

        /* Member Variables */
        // Code of the piece at this location
        int code;
        // Label of the piece at this location
        std::string label;
        // Whether or not the piece is topped by another piece
        bool isTopped;
        // Whether or not the piece is white
        bool white;

        /* Destructor */
        ~Piece() {};
};

#endif
