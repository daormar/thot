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
 * @file IncrNgramLM.cc
 * 
 * @brief Definitions file for IncrNgramLM.h
 */

//--------------- Include files --------------------------------------

#include "IncrNgramLM.h"

//--------------- Classes --------------------------------------------

//---------------
Prob IncrNgramLM::pHTrgGivenHSrc(const std::vector<std::string>& hs,const std::string& ht)
{
  return this->_incrNgramLM<Count,Count>::pHTrgGivenHSrc(hs,ht);
}

//---------------
Prob IncrNgramLM::pTrgGivenSrc(const std::vector<WordIndex>& s,const WordIndex& t)
{
      // Remove extra BOS symbols
  bool found;
  std::vector<WordIndex> aux_s;
  if(s.size()>=2)
  {
    unsigned int i=0;
    while(i<s.size() && s[i]==this->getBosId(found))
    {
      ++i;
    }
    if(i>0) --i;
    for(;i<s.size();++i)
      aux_s.push_back(s[i]);
  }
  else aux_s=s;

      // Access table
  Prob p=this->tablePtr->pTrgGivenSrc(aux_s,t);
      // Obtain smoothing data
  std::vector<WordIndex> empty_hist;
  Count c=this->tablePtr->cSrc(empty_hist);
      // Return probability
  return (1.0-INLM_SMOOTHING_FACTOR)*(double)p + INLM_SMOOTHING_FACTOR*((double)1.0/(double)c);
}

//---------------
IncrNgramLM::~IncrNgramLM()
{
  
}

//------------------------------
