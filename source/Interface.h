#pragma once

#ifndef _UHPINTERFACE_
#define _UHPINTERFACE_
#include "Engine.h"

class UHPInterface
// A UHP command line through which the Engine can be interacted with
{
    public:
        /* Constructors */
        UHPInterface() {};
        UHPInterface(std::map<int, int> pieceConfig) { _game = Engine(pieceConfig); };

        /* Misc */
        // Initialize the command line
        void initTerminal();
        
        /* Destructor */
        ~UHPInterface() {};
    
    private:
        /* Member Variables */
        // The engine which this UHPInterface connects to
        Engine _game;

        /* Command Processing */
        // process an info command
        void _info(std::string input);
        // process a play command
        void _play(std::string input);
        // process a bestmove command
        void _bestMove(std::string input);
        // process an undo command
        void _undo(std::string input);
        // process a pass command
        void _pass(std::string input);
};

#endif