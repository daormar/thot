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
/* Module: BaseIncrPhraseModel                                      */
/*                                                                  */
/* Definitions file: BaseIncrPhraseModel.cc                         */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "BaseIncrPhraseModel.h"

//--------------- Function definitions

//-------------------------
int BaseIncrPhraseModel::trainBilPhrases(const Vector<Vector<std::string> >& srcPhrVec,
                                         const Vector<Vector<std::string> >& trgPhrVec,
                                         Count c/*=1*/,
                                         Count lowerBound/*=0*/,
                                         int verbose)
{
  if(srcPhrVec.size()!=trgPhrVec.size())
  {
    cerr<<"Error: different number of source and target phrases!"<<endl;
    return ERROR;
  }
  
  for(unsigned int i=0;i<srcPhrVec.size();++i)
  {
    if(srcPhrVec[i].size()>0 && trgPhrVec[i].size()>0)
    {
      if((double)c>=0.0)
      {
        strIncrCountsOfEntry(srcPhrVec[i],trgPhrVec[i],c);
      }
      else
      {
            // If c is negative, increase count only if lower bound is
            // not violated
        if(cHSrcHTrg(srcPhrVec[i],trgPhrVec[i])+c>lowerBound)
          strIncrCountsOfEntry(srcPhrVec[i],trgPhrVec[i],c);
      }
    }
    if(verbose)
    {
      for(unsigned int j=0;j<srcPhrVec[i].size();++j)
        cerr<<srcPhrVec[i][j]<<" ";
      cerr<<" |||";
      for(unsigned int j=0;j<trgPhrVec[i].size();++j)
        cerr<<" "<<trgPhrVec[i][j];
      cerr<<" ||| "<<cHSrcHTrg(srcPhrVec[i],trgPhrVec[i])<<endl;
    }
  }
  
  return OK;
}

//-------------------------
