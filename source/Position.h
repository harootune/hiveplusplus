#pragma once
#ifndef _POSITION_
#define _POSITION_
#include <vector>
#include <string>

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

        virtual std::vector<int> getCoords();
        virtual void setCoords(std::vector<int> c);
        virtual std::vector<int> getNeighbor(int direction);
        virtual std::vector<std::vector<int>> getAllNeighbors();

        ~Position() {};

    protected:
        std::vector<int> _coords;

        const static std::vector<std::vector<int>> _translations;
};

class Piece: public Position
{
    public:
        Piece();
        Piece(std::vector<int> coords);
        Piece(std::vector<int> coords, int c, std::string l);

        int code;
        std::string label;

        ~Piece() {};
};

#endif
