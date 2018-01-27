/*
thot package for statistical machine translation
Copyright (C) 2017 Adam Harasimowicz
 
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public License
as published by the Free Software Foundation; either version 3
of the License, or (at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.
 
You should have received a copy of the GNU Lesser General Public License
along with this program; If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file thot_lextable_to_leveldb.cc
 * 
 * @brief Convert lex table file in binary format to LevelDB format.
 */

//--------------- Include files ---------------------------------------

#include "IncrLexLevelDbTable.h"

#include <fstream>
#include <stdlib.h>
#include "options.h"

//--------------- Function Declarations -------------------------------

int convert(void);
int takeParameters(int argc, char *argv[]);
void printUsage(void);
void printVersion(void);

//--------------- Global variables ------------------------------------

std::string inputFile;
std::string outputPath;

//--------------- Function Definitions --------------------------------

//--------------- main function
int main(int argc, char *argv[])
{
    if(takeParameters(argc, argv) == THOT_OK)
    {
        return convert();
    }
    else return THOT_ERROR;
}

//--------------- convert function
int convert()
{
    IncrLexLevelDbTable lexTable;
    lexTable.init(outputPath.c_str());

    std::ifstream inF (inputFile.c_str(), std::ios::in | std::ios::binary);
    if (!inF)
    {
        std::cerr << "Error in lexical nd file, file " << inputFile << " does not exist." << std::endl;
        return THOT_ERROR;
    }
    else
    {
            // Read binary data from file and fill lexical table
        bool end = false;
        while(!end)
        {
            WordIndex s;
            WordIndex t;
            float numer;
            float denom;
            if(inF.read((char*) &s, sizeof(WordIndex)))
            {
                inF.read((char*) &t, sizeof(WordIndex));
                inF.read((char*) &numer, sizeof(float));
                inF.read((char*) &denom, sizeof(float));
                lexTable.setLexNumDen(s, t, numer, denom);
            }
            else end = true;
        }

        return THOT_OK;
    }
}

//--------------- takeParameters function
int takeParameters(int argc, char *argv[])
{
    int err;

        /* Verify --help option */
    err = readOption(argc, argv, "--help");
    if(err != -1)
    {
        printUsage();
        return THOT_ERROR;
    }

        /* Verify --version option */
    err = readOption(argc, argv, "--version");
    if (err != -1)
    {
        printVersion();
        return THOT_ERROR;
    }

        /* Takes the input file path */
    err = readSTLstring(argc, argv, "-i", &inputFile);
    if(err == -1)
    {
        printUsage();
        return THOT_ERROR;
    }

        /* Takes the output path */
    err = readSTLstring(argc, argv, "-o", &outputPath);
    if(err == -1)
    {
        printUsage();
        return THOT_ERROR;
    }

    return THOT_OK;  
}

//--------------- printUsage function
void printUsage(void)
{
    std::cerr << "Usage: thot_lextable_to_leveldb -i <string> -o <string>" << std::endl;
    std::cerr << "                   [-v] [--help] [--version]" << std::endl << std::endl;
    std::cerr << "-i <string>        Input file with lex table in binary format" << std::endl;
    std::cerr << "-o <string>        Output path for LevelDB with lex table" << std::endl;
    std::cerr << "--help             Display this help and exit." << std::endl;
    std::cerr << "--version          Output version information and exit." << std::endl;
}

//--------------- printVersion function
void printVersion(void)
{
    std::cerr << "thot_lextable_to_leveldb is part of the thot package " << std::endl;
    std::cerr << "thot version " << THOT_VERSION << std::endl;
    std::cerr << "thot is GNU software written by Daniel Ortiz" << std::endl;
}
