#include "ConfigReader.h"
#include "Utils.h"
#include "PieceInfo.h"
#include <fstream>
#include <iostream>
#include <regex>
#include <set>

std::map<int, int> ConfigReader::readConfig(std::string path)
{
    std::map<int, int> pieceConfig;
    std::ifstream configFile{path};

    if (!configFile)
    {
        // TODO: Throw some error
        std::cerr << "Config file not found: " << path << std::endl;
        return pieceConfig;
    }
    else
    {
        std::vector<int> vals;
        std::string currentLine;
        std::vector<std::string> tokens;
        std::regex pattern("[1]?[0-9]: [1]?[0-9]");

        while (configFile)
        {
            vals.clear();

            std::getline(configFile, currentLine);
            if (std::regex_match(currentLine, pattern))
            {
                int temp;
                int buffer;
                
                tokens = Utils::tokenize(currentLine, ' ');
                
                for (std::string token: tokens)
                {
                    buffer = 0;

                    for (char c: token)
                    {
                        if (c != ':')
                        {
                            if (buffer > 0)
                            {
                                buffer *= 10;
                            };

                            temp = int(c) - 48;
                            if (temp < 0 || temp > 9)
                            {
                                std::cerr << "Invalid character found in config file: " << c <<std::endl;
                                pieceConfig.clear();
                                return pieceConfig;
                            };

                            buffer += temp;
                        };
                    };

                    vals.push_back(buffer);
                };
            }
            else
            {
                std::cerr << "Invalid line in config file: " << currentLine << std::endl;
                pieceConfig.clear();
                return pieceConfig; 
            };

            pieceConfig[vals[0]] = vals[1];
        };

        return pieceConfig;
    };
};


bool ConfigReader::validateConfig(std::map<int, int> config)
{
    std::set<int> seen;
    bool wQ = false;
    bool bQ = false;
    std::map<int, int>::iterator configIt = config.begin();

    for (configIt; configIt != config.end(); configIt++)
    {
        if (configIt->first < PieceCodes::wQ || configIt->first > PieceCodes::bS)
        {
            return false;
        };

        if (seen.find(configIt->first) != seen.end())
        {
            return false;
        };

        if (configIt->first % 5 == PieceCodes::wQ)
        {
            if (configIt->first == PieceCodes::wQ)
            {
                wQ = configIt->second == 1;
            }
            else
            {
                bQ = configIt->second == 1;
            };
        };
    };

    return wQ && bQ;
};