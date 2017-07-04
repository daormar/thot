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
 
#ifndef _FeaturesInfo_h
#define _FeaturesInfo_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "SrcPosJumpFeat.h"
#include "InversePhraseModelFeat.h"
#include "DirectPhraseModelFeat.h"
#include "LangModelFeat.h"
#include "BasePbTransModelFeature.h"
#include "myVector.h"

//--------------- FeaturesInfo struct

template<class SCORE_INFO>
struct FeaturesInfo
{
  Vector<BasePbTransModelFeature<SCORE_INFO>* > featPtrVec;

      // Functions to get pointers to features
  Vector<LangModelFeat<SCORE_INFO>* > getLangModelFeatPtrs(void);
  Vector<DirectPhraseModelFeat<SCORE_INFO>* > getDirectPhraseModelFeatPtrs(void);
  Vector<InversePhraseModelFeat<SCORE_INFO>* > getInversePhraseModelFeatPtrs(void);
  Vector<SrcPosJumpFeat<SCORE_INFO>* > getSrcPosJumpFeatPtrs(void);
};

//---------------
template<class SCORE_INFO>
Vector<LangModelFeat<SCORE_INFO>* > FeaturesInfo<SCORE_INFO>::getLangModelFeatPtrs(void)
{
  Vector<LangModelFeat<SCORE_INFO>* > lmFeatPtrVec;
  for(unsigned int i=0;i<featPtrVec.size();++i)
  {
    LangModelFeat<SCORE_INFO>* lmFeatPtr=dynamic_cast<LangModelFeat<SCORE_INFO>* >(featPtrVec[i]);
    if(lmFeatPtr)
      lmFeatPtrVec.push_back(lmFeatPtr);
  }
  return lmFeatPtrVec;
}

//---------------
template<class SCORE_INFO>
Vector<DirectPhraseModelFeat<SCORE_INFO>* > FeaturesInfo<SCORE_INFO>::getDirectPhraseModelFeatPtrs(void)
{
  Vector<DirectPhraseModelFeat<SCORE_INFO>* > directPhraseModelFeatPtrVec;
  for(unsigned int i=0;i<featPtrVec.size();++i)
  {
    DirectPhraseModelFeat<SCORE_INFO>* directPhraseModelFeatPtr=dynamic_cast<DirectPhraseModelFeat<SCORE_INFO>* >(featPtrVec[i]);
    if(directPhraseModelFeatPtr)
      directPhraseModelFeatPtrVec.push_back(directPhraseModelFeatPtr);
  }
  return directPhraseModelFeatPtrVec;
}

//---------------
template<class SCORE_INFO>
Vector<InversePhraseModelFeat<SCORE_INFO>* > FeaturesInfo<SCORE_INFO>::getInversePhraseModelFeatPtrs(void)
{
  Vector<InversePhraseModelFeat<SCORE_INFO>* > inversePhraseModelFeatPtrVec;
  for(unsigned int i=0;i<featPtrVec.size();++i)
  {
    InversePhraseModelFeat<SCORE_INFO>* inversePhraseModelFeatPtr=dynamic_cast<InversePhraseModelFeat<SCORE_INFO>* >(featPtrVec[i]);
    if(inversePhraseModelFeatPtr)
      inversePhraseModelFeatPtrVec.push_back(inversePhraseModelFeatPtr);
  }
  return inversePhraseModelFeatPtrVec;
}

//---------------
template<class SCORE_INFO>
Vector<SrcPosJumpFeat<SCORE_INFO>* > FeaturesInfo<SCORE_INFO>::getSrcPosJumpFeatPtrs(void)
{
  Vector<SrcPosJumpFeat<SCORE_INFO>* > srcPosJumpFeatPtrVec;
  for(unsigned int i=0;i<featPtrVec.size();++i)
  {
    SrcPosJumpFeat<SCORE_INFO>* srcPosJumpFeatPtr=dynamic_cast<SrcPosJumpFeat<SCORE_INFO>* >(featPtrVec[i]);
    if(srcPosJumpFeatPtr)
      srcPosJumpFeatPtrVec.push_back(srcPosJumpFeatPtr);
  }
  return srcPosJumpFeatPtrVec;  
}

#endif
