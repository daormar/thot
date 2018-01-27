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
 * @file _editDistBasedEcm.cc
 * 
 * @brief Definitions file for _editDistBasedEcm.h
 */

//--------------- Include files --------------------------------------

#include "_editDistBasedEcm.h"


//--------------- _editDistBasedEcm class functions
//

//---------------------------------------
void _editDistBasedEcm::correctStrGivenPrefOps(WordAndCharLevelOps wordCharOpsForSegm,
                                               std::vector<std::string> uncorrStrVec,
                                               std::vector<std::string> prefStrVec,
                                               std::vector<std::string>& correctedStrVec)
{
  std::string lasty;
  std::string::iterator iter;
  unsigned int i;
  unsigned int j;

  correctedStrVec.clear();

      // if the last word of the prefix ends with a blank character,
      // lasty will store that word without the blank
      // 
      // IMPORTANT NOTE: the corrected segment at this level will never
      // contain the blank character, because we don't know if the last
      // word of the corrected segment will also be the last word of the
      // translation
  if(prefStrVec.size()>0)
  {
    lasty=prefStrVec.back();
  
    if(lasty[lasty.size()-1]==' ')
    {
          // the last word of the prefix is complete
      while(lasty.size()>0 && lasty[lasty.size()-1]==' ')
      {
        iter=lasty.end();
        --iter;
        lasty.erase(iter);
      }
    }
  }
  
  i=0;
  j=0;
  for(unsigned int k=0;k<wordCharOpsForSegm.first.size();++k)
  {
    switch(wordCharOpsForSegm.first[k])
    {
      case INS_OP:
        if(prefStrVec[j][prefStrVec[j].size()-1]==' ')
        {
          correctedStrVec.push_back(lasty);
        }
        else
        {
          correctedStrVec.push_back(prefStrVec[j]);
        }
        ++j;
        break;
      case DEL_OP:
        ++i;
        break;
      case HIT_OP:
            // A correct word may have a suffix, because of this, it
            // could be necessary to invocate the
            // "correctWordGivenPrefOps" function
        if(j!=prefStrVec.size()-1)
        {
              // correct a word which is not the last prefix word
          correctedStrVec.push_back(prefStrVec[j]);
        }
        else
        {
              // obtain a correction of the last prefix word
          if(prefStrVec[j][prefStrVec[j].size()-1]==' ')
          {
            correctedStrVec.push_back(lasty);
          }
          else
          {
                // the last prefix word is a prefix
            std::string correctedLastWord;
            correctWordGivenPrefOps(wordCharOpsForSegm.second,
                                    uncorrStrVec[i],
                                    prefStrVec[j],
                                    correctedLastWord);
            correctedStrVec.push_back(correctedLastWord);
          }
        }
        ++i;
        ++j;
        break;        
      case SUBST_OP:
            // The correction of substituted word is the prefix word
        if(j!=prefStrVec.size()-1)
        {
              // correct a word which is not the last prefix word
          correctedStrVec.push_back(prefStrVec[j]);
        }
        else
        {
              // obtain a correction of the last prefix word
          if(prefStrVec[j][prefStrVec[j].size()-1]==' ')
          {
                // the last prefix word is complete
            correctedStrVec.push_back(lasty);
          }
          else
          {
                // the last prefix word is not complete
            correctedStrVec.push_back(prefStrVec[j]);
          }
        }
        ++i;
        ++j;
        break;
    }
  }
      // Add not corrected words
  for(;i<uncorrStrVec.size();++i)
  {
    correctedStrVec.push_back(uncorrStrVec[i]);
  }
}

//---------------------------------------
void _editDistBasedEcm::correctWordGivenPrefOps(std::vector<unsigned int> charOpsForWord,
                                                std::string word,
                                                std::string pref,
                                                std::string& correctedWord)
{
  unsigned int i;
  unsigned int j;
  correctedWord="";

  i=0;
  j=0;
  for(unsigned int k=0;k<charOpsForWord.size();++k)
  {
    switch(charOpsForWord[k])
    {
      case HIT_OP:
        correctedWord=correctedWord+word[i];
        ++i;
        ++j;
        break;
      case INS_OP:
        correctedWord=correctedWord+pref[j];
        ++j;
        break;
      case DEL_OP:
        ++i;
        break;
      case SUBST_OP:
        correctedWord=correctedWord+pref[j];
        ++i;
        ++j;
        break;
    }
  }
  for(;i<word.size();++i)
  {
    correctedWord=correctedWord+word[i];
  }
}
