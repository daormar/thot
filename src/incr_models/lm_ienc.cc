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
 * @file lm_ienc.cc
 * 
 * @brief Definitions file for lm_ienc.h
 */

//--------------- Include files --------------------------------------

#include "lm_ienc.h"

//--------------- lm_ienc class functions
//

//---------------
lm_ienc::lm_ienc():vecx_x_incr_enc<std::string,WordIndex>()
{
      // Introduce standard symbols
  addHTrgCode(UNK_SYMBOL_STR,UNK_SYMBOL);
  addHTrgCode(BOS_STR,S_BEGIN);
  addHTrgCode(EOS_STR,S_END);
  addHTrgCode(SP_SYM1_LM_STR,SP_SYM1_LM);

      // Initialize x_object
  WordIndex maxIdx=UNK_SYMBOL;
  if(maxIdx<S_BEGIN) maxIdx=S_BEGIN;
  if(maxIdx<S_END) maxIdx=S_END;
  if(maxIdx<SP_SYM1_LM) maxIdx=SP_SYM1_LM;

  x_object=maxIdx;
}

//---------------
bool lm_ienc::HighSrc_to_Src(const std::vector<std::string>& hs,
                             std::vector<WordIndex>& s)
{
  std::map<std::string,WordIndex>::iterator iter;
  unsigned int i;
  bool retval=true;
  
  s.clear();
  for(i=0;i<hs.size();++i)
  {
    iter=hx_to_x.find(hs[i]);
    if(iter==hx_to_x.end())
    {
      retval=false;
      s.push_back(UNK_SYMBOL);
    }
    else
    {
      s.push_back(iter->second);
    }
  }
  return retval;
}

//---------------
bool lm_ienc::HighTrg_to_Trg(const std::string& ht,
                             WordIndex& t)
{
  std::map<std::string,WordIndex>::iterator iter;

  iter=hx_to_x.find(ht);
  if(iter==hx_to_x.end())
  {
    t=UNK_SYMBOL;
    return false;
  }
  else
  {
    t=iter->second;
    return true;
  }
}
