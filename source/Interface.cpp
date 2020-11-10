#include "Interface.h"
#include "Utils.h"
#include <iostream>
#include <regex>


void UHPInterface::initTerminal()
{
    std::string input;
    std::vector<std::string> tokens;

    // Terminal loop
    while (true)
    {
        std::getline(std::cin, input);
        input = Utils::strip(input);
        tokens = Utils::tokenize(input, ' ');
        
        // TODO: Length check
        if (tokens[0] == "info") { _info(input); }
        else if (tokens[0] == "play") { _play(input); }
        else if (tokens[0] == "exit") { exit(0); }
        else
        {
            if (tokens[0].size() > 0)
            {
                std::cout << "Unknown command " << tokens[0] << std::endl;
                std::cout << "ok" << std::endl;
            };
            continue;
        };
    };  
};

void UHPInterface::_info(std::string input)
{
    std::vector<std::string> tokens = Utils::tokenize(input, ' ');

    std::cout << "id hive++ indev" << std::endl;

    if (tokens.size() > 1)
    {
        std::cout << "Note: Arguments discarded. info takes no arguments." << std::endl;
    };

    std::cout << "ok" << std::endl;  
};

void UHPInterface::_play(std::string input)
{
    std::string cleanInput = input.substr(4, std::string::npos);
    std::vector<std::string> tokens = Utils::tokenize(cleanInput, ';');

    if (tokens.size() != 1)
    {
        std::cout << "Too many arguments. Usage: play [MoveString/pass]" << std::endl;
    }
    else
    {
        std::string second = Utils::strip(tokens[0]);
        if (second == "pass")
        { 
            // implement passing behavior
            std::cout << "Passing..." << std::endl;
        }
        else if (Utils::isMoveString(second))
        {
            // check for validity of move in Board object
            _game.makeMove(second);
        }
        else
        {
            std::cout << "Argument not recognized. Usage: play [MoveString/pass]" << std::endl;
        };
    };
    
    std::cout << "ok" << std::endl;
};
