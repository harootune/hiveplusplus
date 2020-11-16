#pragma once

#ifndef _UHPINTERFACE_
#define _UHPINTERFACE_
#include <Engine.h>

namespace StandardPConfigs
{
    extern std::map<int, int> base;
};

class UHPInterface
// A UHP command line through which the Engine can be interacted with
{
    public:
        /* Constructors */
        UHPInterface() { _active = false; };

        /* Misc */
        // Initialize the command line
        void initTerminal();
        
        /* Destructor */
        ~UHPInterface() {};
    
    private:
        /* Member Variables */
        // The engine which this UHPInterface connects to
        Engine _game;
        bool _active;
        std::string _mode;
        std::string _customPath;


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
        // process a validmoves command
        void _validMoves(std::string input);
        // process a newgame command
        void _newGame(std::string input);
        void _options(std::string input);
        bool _initGame(std::string input);
};

#endif