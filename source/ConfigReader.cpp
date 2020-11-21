#include <ConfigReader.h>
#include <Utils.h>
#include <PieceInfo.h>
#include <fstream>
#include <iostream>
#include <regex>
#include <set>


std::map<int, int> ConfigReader::readConfig(std::string path) 
{
    std::map<int, int> pieceConfig;

    std::ifstream configFile{path};

    // if we were unable to find any file at this location, break
    if (!configFile)
    {
        std::cout << "err Config file not found: " << path << std::endl;
        return pieceConfig; // returns a blank piece config which should fail any subsequent validation step
    }
    // otherwise, read the file found at path
    else
    {
        std::vector<int> vals;
        std::string currentLine;
        std::vector<std::string> tokens;

        std::regex pattern("[1]?[0-9]: [1]?[0-9]");

        // while there are still lines in the file, read them
        while (configFile)
        {
            // clear our buffer
            vals.clear();

            std::getline(configFile, currentLine);

            // jf the current line matches the line pattern
            if (std::regex_match(currentLine, pattern))
            {
                int temp;
                int buffer;
                
                tokens = Utils::tokenize(currentLine, ' ');
                
                // iterate through all space-separated tokens
                for (std::string token: tokens)
                {
                    buffer = 0;

                    // read a character
                    for (char c: token)
                    {
                        // skip over the colon
                        if (c != ':')
                        {
                            if (buffer > 0)
                            {
                                buffer *= 10;
                            };

                            // convert character to integer
                            temp = int(c) - 48;

                            // log an error message if the character was not an integer
                            if (temp < 0 || temp > 9)
                            {
                                std::cout << "err Invalid character found in config file: " << c <<std::endl;
                                pieceConfig.clear();
                                return pieceConfig; // return a blank config, which will fail a subsequent validation step
                            };

                            buffer += temp;
                        };
                    };

                    vals.push_back(buffer);
                };
            }
            // log an error message if a line does not match the line pattern
            else
            {
                std::cout << "err Invalid line in config file: " << currentLine << std::endl;
                pieceConfig.clear();
                return pieceConfig; // return a blank config, which will fail a subsequent validation step
            };

            pieceConfig[vals[0]] = vals[1];
        };

        return pieceConfig;
    };
};


bool ConfigReader::validateConfig(std::map<int, int> config)
{
    std::set<int> seen;
    std::map<int, int>::iterator configIt;

    bool wQ = false;
    bool bQ = false;
    
    for (configIt = config.begin(); configIt != config.end(); configIt++)
    {
        // return false on invalid piece codes
        if (configIt->first < PieceCodes::wQ || configIt->first > PieceCodes::bS)
        {
            return false;
        };

        if (seen.find(configIt->first) != seen.end())
        {
            return false;
        };

        // ensure there is precisely one queen of each color
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