#pragma once

#ifndef _UHPINTERFACE_
#define _UHPINTERFACE_
#include "Board.h"

class UHPInterface
{
    public:
        UHPInterface() {};
        UHPInterface(std::map<int, int> pieceConfig) { _game = Board(pieceConfig); };

        void initTerminal();
        
        ~UHPInterface() {};
    
    private:
        Board _game;
        void _info(std::vector<std::string> tokens);
        void _play(std::vector<std::string> tokens);
};

#endif