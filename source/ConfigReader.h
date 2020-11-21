#pragma once

#ifndef _CONFIGREADER_
#define _CONFIGREADER_
#include <map>

namespace ConfigReader
// A namespace which provides utilities for reading and validating .pconfig files
// NOTE: This is extremely fragile right now
{
    // read a config file
    std::map<int, int> readConfig(std::string path);
    // validate a config map
    bool validateConfig(std::map<int, int> config);
};

#endif