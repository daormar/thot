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
 * @file SrfNodeInfoMap.cc
 * 
 * @brief Definitions file for SrfNodeInfoMap.h
 */

//--------------- Include files --------------------------------------

#include "SrfNodeInfoMap.h"

//--------------- Global variables -----------------------------------


//--------------- SrfNodeInfoMap class method definitions

//-------------------------
SrfNodeInfoMap::SrfNodeInfoMap(void)
{
}

//-------------------------
void SrfNodeInfoMap::insert(const SrfNodeKey& k,
                            const SrfNodeInfo& sni)
{
  bitsetSniMap[k]=sni;
}
   
//-------------------------
SrfNodeInfo SrfNodeInfoMap::find(const SrfNodeKey& k,
                                 bool& found)const
{
  BitsetSniMap::const_iterator bsmIter=bitsetSniMap.find(k);
  if(bsmIter!=bitsetSniMap.end())
  {
    found=true;
    return bsmIter->second;
  }
  else
  {
    found=false;
    SrfNodeInfo sni;
    return sni;
  }
}

//-------------------------
size_t SrfNodeInfoMap::numNodesWithLeafs(void)
{
  size_t result=0;
  BitsetSniMap::const_iterator bsmIter;
  for(bsmIter=bitsetSniMap.begin();bsmIter!=bitsetSniMap.end();++bsmIter)
  {
    if(bsmIter->second.estLogNumLeafs<0)
      ++result;
  }
  return result;
}

//-------------------------
size_t SrfNodeInfoMap::size(void)
{
  return bitsetSniMap.size();
}

//-------------------------
void SrfNodeInfoMap::clear(void)
{
  bitsetSniMap.clear();
}
