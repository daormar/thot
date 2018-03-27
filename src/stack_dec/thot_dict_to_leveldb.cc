/*
thot package for statistical machine translation
Copyright (C) 2013 Daniel Ortiz-Mart\'inez
 
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
 * @file thot_dict_to_leveldb.cc
 * 
 * @brief Converts a dictionary with scores to leveldb format.
 */

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <LevelDbDict.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "options.h"
#include <AwkInputStream.h>

//--------------- Constants ------------------------------------------


//--------------- Function Declarations ------------------------------

int TakeParameters(int argc, char *argv[]);
void printUsage(void);
int extractEntryInfo(AwkInputStream& awk,
                     std::vector<std::string>& srcPhr,
                     std::vector<std::string>& trgPhr,
                     Score& scr);
int process_ttable(void);

//--------------- Type definitions -----------------------------------


//--------------- Global variables -----------------------------------

std::string outputFile;

//--------------- Function Definitions -------------------------------

//---------------
int main(int argc, char *argv[])
{
  if(TakeParameters(argc,argv) == THOT_OK)
  {
    return process_ttable();
  }
  else return THOT_ERROR;
}

//---------------
int extractEntryInfo(AwkInputStream& awk,
                     std::vector<std::string>& srcPhr,
                     std::vector<std::string>& trgPhr,
                     Score& scr)
{
  unsigned int i;

      // Obtain source phrase
  srcPhr.clear();
  for(i = 1; i <= awk.NF; ++i)
  {
    if(awk.dollar(i) == "|||")
      break;
    else
      srcPhr.push_back(awk.dollar(i));
  }
  if(i == awk.NF)
    return THOT_ERROR;

      // Obtain target phrase
  trgPhr.clear();
  i += 1;
  for(; i <= awk.NF; ++i)
  {
    if(awk.dollar(i) == "|||")
      break;
    else
      trgPhr.push_back(awk.dollar(i));
  }
  if(i != awk.NF - 1)
    return THOT_ERROR;

      // Obtain score
  scr = atof(awk.dollar(awk.NF).c_str());

  return THOT_OK;
}

//---------------
int process_ttable(void)
{
      // Read standard input
  AwkInputStream awk;
  if(awk.open_stream(stdin) == THOT_ERROR)
  {
    std::cerr << "Error while reading from standard input!" << std::endl;
    return THOT_ERROR;
  }
  else
  {
    LevelDbDict levelDbDict;
    if(levelDbDict.init(outputFile) == THOT_ERROR)
    {
      std::cerr << "Cannot create or recreate database (LevelDB)" << std::endl;
      return THOT_ERROR;
    }
    
        // Process translation table
    int i = 0;
    while(awk.getln())
    {
      // if(awk.FNR % 1000 ==0)
      //   std::cerr<<"Processing entry "<<awk.FNR<<std::endl;

      std::vector<std::string> srcPhr;
      std::vector<std::string> trgPhr;
      Score scr;
      int ret = extractEntryInfo(awk, srcPhr, trgPhr, scr);
      if(ret == THOT_OK)
        levelDbDict.addDictEntry(srcPhr, trgPhr, scr);
      else
        std::cerr << "Cannot extract entry info" << std::endl;
      i++;

      if (i % 5000 == 0)
        std::cerr << "Processed " << i << " lines" << std::endl;
    }

    std::cerr << "levelDB size: " << levelDbDict.size() << std::endl;
    
    return THOT_OK;
  }
}

//---------------
int TakeParameters(int argc,char *argv[])
{
  int err;

      /* Verify --help option */
  err=readOption(argc, argv, "--help");
  if(err != -1)
  {
    printUsage();
    return THOT_ERROR;
  }

      /* Takes the output files prefix */
  err = readSTLstring(argc,argv, "-o", &outputFile);
  if(err == -1)
  {
    printUsage();
    return THOT_ERROR;
  }

  return THOT_OK;  
}

//---------------
void printUsage(void)
{
  printf("Usage: thot_ttable_to_leveldb -o <string> [--help]\n\n");
  printf("-o <string>                   Name of output file.\n\n");
  printf("--help                        Display this help and exit.\n\n");
}

//--------------------------------
