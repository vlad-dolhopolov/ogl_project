#include "Util.h"

#include <iostream>
#include <fstream>

std::vector<std::string> LoadStrings(const std::string& fname)
{
    std::vector<std::string> names;

    // open the file
    std::ifstream f(fname);

    // check if file opened ok
    if (!f) {
        std::cerr << "Error: Failed to open " << fname << std::endl;
        std::exit(1);
    }

    // read lines until the end of file
    std::string line;
    while (std::getline(f, line)) {
        // ignore blank lines and comments starting with '#'
        if (!line.empty() && line[0] != '#') {
            names.push_back(line);
        }
    }

    return names;
}
