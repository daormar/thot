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
/* Module: IncrJelMerLevelDbNgramLM                                 */
/*                                                                  */
/* Definitions file: IncrJelMerLevelDbNgramLM.cc                    */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "IncrJelMerLevelDbNgramLM.h"

//--------------- Global variables -----------------------------------

//--------------- Function declarations 

//--------------- Constants


//--------------- Classes --------------------------------------------

//------------------------------
bool IncrJelMerLevelDbNgramLM::load(const char *fileName)
{
  bool retval;

      // Load vocabulary
  retval = loadVocab(fileName);
  if (retval == THOT_ERROR) return THOT_ERROR;

      // Load LevelDB ngram table
  retval = loadNgramTable(fileName);
  if (retval == THOT_ERROR) return THOT_ERROR;

      // Load weights
  retval = loadWeights(fileName);
  if (retval == THOT_ERROR) return THOT_ERROR;

  return THOT_OK;
}

//------------------------------
bool IncrJelMerLevelDbNgramLM::loadVocab(const char *fileName)
{
  std::string vocabFileName;

      // Load LevelDB ngram table
  std::string mainFileName;
  if(fileIsDescriptor(fileName, mainFileName))
  {
    std::string descFileName = fileName;
    std::string absolutizedMainFileName = absolutizeModelFileName(descFileName, mainFileName);
    vocabFileName = absolutizedMainFileName;
  }
  else
  {
    vocabFileName = fileName;
  }
  
      // Add vocab file extension
  vocabFileName += ".ldb_vcb";
  
  return this->encPtr->load(vocabFileName.c_str());
}

//------------------------------
bool IncrJelMerLevelDbNgramLM::loadNgramTable(const char *fileName)
{
  const char *ngramTableFileName;

      // Load LevelDB ngram table
  std::string mainFileName;
  if(fileIsDescriptor(fileName, mainFileName))
  {
    std::string descFileName = fileName;
    std::string absolutizedMainFileName = absolutizeModelFileName(descFileName, mainFileName);
    ngramTableFileName = absolutizedMainFileName.c_str();
  }
  else
  {
    ngramTableFileName = fileName;
  }

  return this->tablePtr->load(ngramTableFileName);
}

//------------------------------
IncrJelMerLevelDbNgramLM::~IncrJelMerLevelDbNgramLM()
{
  
}
