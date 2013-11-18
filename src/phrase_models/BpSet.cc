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
 
/********************************************************************/
/*                                                                  */
/* Module: BpSet                                                    */
/*                                                                  */
/* Definitions file: BpSet.cc                                       */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "BpSet.h"

//--------------- Global variables -----------------------------------


//--------------- BpSet class method definitions

//-------------------------
BpSet::BpSet(void)
{
}

//-------------------------
void BpSet::incrPair(PositionIndex x1,
                     PositionIndex x2,
                     PositionIndex y1,
                     PositionIndex y2,
                     float lc/*=1*/)
{
      // Make room if necessary
  Vector<Vector<BpSetInfo > > vvp;
  while(consPairs.size()<=x1)
    consPairs.push_back(vvp);
  
  Vector<BpSetInfo> vp;
  while(consPairs[x1].size()<=x2)
    consPairs[x1].push_back(vp);

      // Check if it is present
  bool found=false;
  for(unsigned int i=0;i<consPairs[x1][x2].size();++i)
  {
    if(consPairs[x1][x2][i].tpair.first==y1 && consPairs[x1][x2][i].tpair.second==y2)
    {
      consPairs[x1][x2][i].lc=MathFuncs::lns_sumlog(consPairs[x1][x2][i].lc,lc);
      found=true;
      break;
    }
  }

      // If not present, insert it
  if(!found)
  {
    BpSetInfo bsi;
    bsi.tpair.first=y1;
    bsi.tpair.second=y2;
    bsi.lc=lc;
    consPairs[x1][x2].push_back(bsi);
  }
}

//-------------------------
void BpSet::retrieveTrgPhrasesGivenSrc(PositionIndex x1,
                                       PositionIndex x2,
                                       BpSetInfoCont& trgPhrases)const
{
  trgPhrases.clear();
  if(consPairs.size()>x1 && consPairs[x1].size()>x2)
  {
    trgPhrases=consPairs[x1][x2];
  }
}

//-------------------------
PositionIndex BpSet::getx1Max(void)const
{
  if(!consPairs.empty())
    return consPairs.size()-1;
  else return 0;
}

//-------------------------
PositionIndex BpSet::getx2Max(PositionIndex x1)const
{
  if(consPairs.size()>x1 && !consPairs[x1].empty())
    return consPairs[x1].size()-1;
  else return 0;
}

//-------------------------
void BpSet::obtainUnion(const BpSet& b)
{
  PositionIndex x1Max=b.getx1Max();
  for(PositionIndex i=0;i<=x1Max;++i)
  {
    PositionIndex x2Max=b.getx2Max(i);
    for(PositionIndex j=0;j<=x2Max;++j)
    {
      BpSetInfoCont trgPhrases;
      b.retrieveTrgPhrasesGivenSrc(i,j,trgPhrases);
      for(BpSetInfoCont::const_iterator bsicIter=trgPhrases.begin();bsicIter!=trgPhrases.end();++bsicIter)
      {
        incrPair(i,j,bsicIter->tpair.first,bsicIter->tpair.second,bsicIter->lc);
      }
    }
  }
}

//-------------------------
void BpSet::clear(void)
{
  consPairs.clear();
}
