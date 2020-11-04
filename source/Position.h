#pragma once

#ifndef _POSITION_
#define _POSITION_
#include <vector>
#include <string>
#include <map>

enum Directions
{
    Right,
    DownRight,
    DownLeft,
    Left,
    UpLeft,
    UpRight
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
        virtual int findDistance(Position *other);

        static std::vector<int> convertDirection(int dir);
        static int convertTranslation(const std::vector<int> &trans);
        static int findDirection(const std::vector<int> &from, const std::vector<int> &to);

        ~Position() {};

    protected:
        std::vector<int> _coords;

        const static std::vector<std::vector<int>> _dirToTranslations;
        const static std::map<std::vector<int>, int> _translationsToDir;

        
};

class Piece: public Position
{
    public:
        Piece();
        Piece(std::vector<int> coords);
        Piece(std::vector<int> coords, int c, std::string l);

        int code;
        std::string label;
        bool isTopped;
        bool white;

        ~Piece() {};
};

#endif
