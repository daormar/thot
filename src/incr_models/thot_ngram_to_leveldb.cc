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
 
/********************************************************************/
/*                                                                  */
/* Module: thot_ngram_to_leveldb.cc                                 */
/*                                                                  */
/* Definitions file: thot_ngram_to_leveldb.cc                       */
/*                                                                  */
/* Description: Converts a ngram data to leveldb format.            */
/*                                                                  */   
/********************************************************************/


//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "LM_Defs.h"
#include "LevelDbNgramTable.h"
#include "im_pair.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include "options.h"
#include <awkInputStream.h>

//--------------- Constants ------------------------------------------


//--------------- Function Declarations ------------------------------

int TakeParameters(int argc, char *argv[]);
void printUsage(void);
int extractEntryInfo(awkInputStream& awk,
                     std::vector<WordIndex>& src,
                     WordIndex& trg,
                     LogCount& logJointCount,
                     std::map<std::string, WordIndex>& vocab);
int process_ttable(void);

//--------------- Type definitions -----------------------------------


//--------------- Global variables -----------------------------------

std::string outputFile;

//--------------- Function Definitions -------------------------------

//---------------
int main(int argc, char *argv[])
{
    if(TakeParameters(argc,argv) == THOT_OK)
        return process_ttable();
    else
        return THOT_ERROR;
}

//---------------
void saveVocabulary(std::map<std::string, WordIndex>& vocab)
{
    // Save vocabulary
    std::ofstream vocabFile;
    std::string vocabFileName = outputFile + ".ldb_vcb";
    vocabFile.open(vocabFileName.c_str());

    for (std::map<std::string, WordIndex>::iterator it = vocab.begin(); it != vocab.end(); it++)
    {
        vocabFile << it->first << " " << it->second << std::endl;
    }
        
    vocabFile.close();
}

//---------------
WordIndex getSymbolId(std::map<std::string, WordIndex> &vocab, std::string symbol)
{
    std::map<std::string, WordIndex>::iterator it = vocab.find(symbol);

    WordIndex wi = -1;

    if(it == vocab.end()) {
        wi = vocab.size();
        vocab[symbol] = wi;
    } else {
        wi = it->second;
    }

    return wi;
}

//---------------
int extractEntryInfo(awkInputStream& awk,
                     std::vector<WordIndex>& src,
                     WordIndex& trg,
                     im_pair<Count, Count>& inf,
                     std::map<std::string, WordIndex>& vocab)
{
    if (awk.NF < 3)
        return THOT_ERROR;

    // Obtain source phrase
    src.clear();

    for(unsigned int i = 1; i <= awk.NF - 3; ++i)
    {
        WordIndex wi = getSymbolId(vocab, awk.dollar(i));
        src.push_back(wi);
    }

    // Obtain target
    trg = getSymbolId(vocab, awk.dollar(awk.NF - 2));
    // Obtain count
    inf.first = atof(awk.dollar(awk.NF-1).c_str());
    inf.second = atof(awk.dollar(awk.NF).c_str());

    return THOT_OK;
}

//---------------
int process_ttable(void)
{
    // Read standard input
    awkInputStream awk;
    if (awk.open_stream(stdin) == THOT_ERROR)
    {
        std::cerr << "Error while reading from standard input!" << std::endl;
        return THOT_ERROR;
    }
    else
    {
        LevelDbNgramTable levelDbNt;
        int ret;
        std::map<std::string, WordIndex> vocab;

        if(levelDbNt.init(outputFile) == THOT_ERROR)
        {
            std::cerr << "Cannot create or recreate database (LevelDB) for language model" << std::endl;
            return THOT_ERROR;
        }

        // Define language model constants
        vocab[UNK_SYMBOL_STR] = UNK_SYMBOL;
        vocab[BOS_STR] = S_BEGIN;
        vocab[EOS_STR] = S_END;
        vocab[SP_SYM1_LM_STR] = SP_SYM1_LM;
        
        // Process translation table
        for(unsigned int i = 1; awk.getln(); i++)
        {
            std::vector<WordIndex> src;
            WordIndex trg;
            im_pair<Count, Count> inf;

            ret = extractEntryInfo(awk, src, trg, inf, vocab);

            if(ret == THOT_OK)
            {
                levelDbNt.addTableEntry(src, trg, inf);
            }
            else
            {
                std::cerr << "Cannot extract entry info" << std::endl;
            }

            if (i % 5000 == 0)
                std::cerr << "Processed " << i << " lines" << std::endl;
        }

        std::cerr << "levelDB size: " << levelDbNt.size() << std::endl;

        // Save vocabulary
        saveVocabulary(vocab);
        
        return THOT_OK;
    }
}

//---------------
int TakeParameters(int argc, char *argv[])
{
    int err;

    // Verify --help option
    err = readOption(argc, argv, "--help");

    if (err != -1)
    {
        printUsage();

        return THOT_ERROR;
    }

    // Takes the output files prefix
    err = readSTLstring(argc,argv, "-o", &outputFile);

    if (err == -1)
    {
        printUsage();

        return THOT_ERROR;
    }

    return THOT_OK;  
}

//---------------
void printUsage(void)
{
    printf("Usage: thot_ngram_to_leveldb -o <string> [--help]\n\n");
    printf("-o <string>                  Name of output file.\n\n");
    printf("--help                       Display this help and exit.\n\n");
}

//--------------------------------
