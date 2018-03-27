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
 * @file LevelDbDictFeatPhrScoreInfoFactory.cc
 * 
 * @brief Factory for DictFeat objects.
 */

//--------------- Include files --------------------------------------

#include "PhrScoreInfo.h"
#include "LevelDbDictFeat.h"
#include <string.h>

//--------------- Function definitions

extern "C" BasePbTransModelFeature<PhrScoreInfo>* create(const char* str)
{
  if(strlen(str)==0)
    return new LevelDbDictFeat<PhrScoreInfo>;
  else
  {
    LevelDbDictFeat<PhrScoreInfo>* levelDbDictFeatPtr;
    try
    {
      levelDbDictFeatPtr= new LevelDbDictFeat<PhrScoreInfo>(str);
    }
    catch(const std::exception& e)
    {
      return NULL;
    }
    
    return levelDbDictFeatPtr;
  }
}

//---------------
extern "C" const char* type_id(void)
{
  return "LevelDbDictFeat";
}
