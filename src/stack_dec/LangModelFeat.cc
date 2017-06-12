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
/* Module: LangModelFeat                                            */
/*                                                                  */
/* Definitions file: LangModelFeat.cc                               */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "LangModelFeat.h"

//--------------- LangModelFeat class functions

template<>
typename LangModelFeat<PhrScoreInfo>::HypScoreInfo
LangModelFeat<PhrScoreInfo>::nullHypScore(const HypScoreInfo& predHypScrInf,
                                          Score& unweightedScore)
{
  unweightedScore=0;
      // Obtain language model state for null hypothesis
  HypScoreInfo hypScrInf=predHypScrInf;
      // lModelPtr->getStateForBeginOfSentence(hypScrInf.lmHist);
  return hypScrInf;
}

//---------------
template<>
typename LangModelFeat<PhrScoreInfo>::HypScoreInfo
LangModelFeat<PhrScoreInfo>::extensionScore(const Vector<std::string>& srcSent,
                                            const HypScoreInfo& predHypScrInf,
                                            const PhrHypDataStr& predHypDataStr,
                                            const PhrHypDataStr& newHypDataStr,
                                            Score& unweightedScore)
{
      // Obtain score for hypothesis extension
  HypScoreInfo hypScrInf=predHypScrInf;
  unweightedScore=0;

      // Obtain current partial translation
  Vector<std::string> currPartialTrans;
  obtainCurrPartialTrans(predHypDataStr,currPartialTrans);
    
  for(unsigned int i=predHypDataStr.sourceSegmentation.size();i<newHypDataStr.sourceSegmentation.size();++i)
  {
        // Initialize variables
    unsigned int trgLeft;
    unsigned int trgRight=newHypDataStr.targetSegmentCuts[i];
    if(i==0)
      trgLeft=1;
    else
      trgLeft=newHypDataStr.targetSegmentCuts[i-1]+1;
    Vector<std::string> trgPhrase;
    for(unsigned int k=trgLeft;k<=trgRight;++k)
      trgPhrase.push_back(newHypDataStr.ntarget[k]);

        // Obtain state info
    LM_State state;    
    getStateForWordSeqStr(currPartialTrans,state);
      
        // Update score
//      Score iterScore=getNgramScoreGivenState(trgPhrase,hypScrInf.lmHist);
    Score iterScore=getNgramScoreGivenState(trgPhrase,state);
    unweightedScore+= iterScore;
    hypScrInf.score+= weight*iterScore;
        // Update current partial translation
    updateCurrPartialTranslation(trgPhrase,currPartialTrans);
  }

      // Check if new hypothesis is complete
  if(numberOfSrcWordsCovered(newHypDataStr)==srcSent.size())
  {
        // Obtain state info
    LM_State state;    
    getStateForWordSeqStr(currPartialTrans,state);

        // Obtain score contribution for complete hypothesis
//      Score scrCompl=getEosScoreGivenState(hypScrInf.lmHist);
    Score scrCompl=getEosScoreGivenState(state);
    unweightedScore+= scrCompl;
    hypScrInf.score+= weight*scrCompl;
  }
    
  return hypScrInf;
}
