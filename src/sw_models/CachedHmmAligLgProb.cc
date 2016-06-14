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
 
/*********************************************************************/
/*                                                                   */
/* Module: CachedHmmAligLgProb                                       */
/*                                                                   */
/* Definitions file: CachedHmmAligLgProb.cc                          */
/*                                                                   */
/*********************************************************************/


//--------------- Include files ---------------------------------------

#include "CachedHmmAligLgProb.h"


//--------------- CachedHmmAligLgProb function declarations 

//-------------------------
void CachedHmmAligLgProb::makeRoomGivenNSrcSentLen(PositionIndex nsrclen)
{
  if(cachedLgProbs.size()<=nsrclen)
  {
    Vector<Vector<double> > lpVecVec;
    while(cachedLgProbs.size()<=nsrclen)
      cachedLgProbs.push_back(lpVecVec);

    for(unsigned int i=0;i<cachedLgProbs.size();++i)
    {
      Vector<double> lpVec;
      while(cachedLgProbs[i].size()<=nsrclen)
        cachedLgProbs[i].push_back(lpVec);

      for(unsigned int j=0;j<cachedLgProbs[i].size();++j)
      {
        while(cachedLgProbs[i][j].size()<=nsrclen)
          cachedLgProbs[i][j].push_back((double)CACHED_HMM_ALIG_LGPROB_VIT_INVALID_VAL);
      }
    }
  }
}

//-------------------------
bool CachedHmmAligLgProb::isDefined(PositionIndex prev_i,
                                    PositionIndex slen,
                                    PositionIndex i)
{
  if(cachedLgProbs.size()>prev_i && cachedLgProbs[prev_i].size()>slen && cachedLgProbs[prev_i][slen].size()>i)
  {
    if(cachedLgProbs[prev_i][slen][i]>=(double)CACHED_HMM_ALIG_LGPROB_VIT_INVALID_VAL)
      return false;
    else
      return true;
  }
  else
  {
    return false;
  }
}

//-------------------------
void CachedHmmAligLgProb::set_boundary_check(PositionIndex prev_i,
                                             PositionIndex slen,
                                             PositionIndex i,
                                             double lp)
{
      // Make room in cachedLgProbs if necessary
  Vector<Vector<double> > lpVecVec;
  while(cachedLgProbs.size()<=prev_i)
    cachedLgProbs.push_back(lpVecVec);

  Vector<double> lpVec;
  while(cachedLgProbs[prev_i].size()<=slen)
    cachedLgProbs[prev_i].push_back(lpVec);

  while(cachedLgProbs[prev_i][slen].size()<=i)
    cachedLgProbs[prev_i][slen].push_back((double)CACHED_HMM_ALIG_LGPROB_VIT_INVALID_VAL);

      // Set value
  cachedLgProbs[prev_i][slen][i]=lp;
}

//-------------------------
void CachedHmmAligLgProb::set(PositionIndex prev_i,
                              PositionIndex slen,
                              PositionIndex i,
                              double lp)
{
  cachedLgProbs[prev_i][slen][i]=lp;
}

//-------------------------
double CachedHmmAligLgProb::get(PositionIndex prev_i,
                                PositionIndex slen,
                                PositionIndex i)
{
  return cachedLgProbs[prev_i][slen][i];
}

//-------------------------
void CachedHmmAligLgProb::clear(void)
{
  cachedLgProbs.clear();
}
