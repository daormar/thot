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
/* Module: TrgPhraseLenFeat                                         */
/*                                                                  */
/* Definitions file: TrgPhraseLenFeat.cc                            */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "TrgPhraseLenFeat.h"

//--------------- TrgPhraseLenFeat class functions

template<>
TrgPhraseLenFeat<PhrScoreInfo>::HypScoreInfo
TrgPhraseLenFeat<PhrScoreInfo>::extensionScore(const Vector<std::string>& srcSent,
                                               const HypScoreInfo& predHypScrInf,
                                               const PhrHypDataStr& predHypDataStr,
                                               const PhrHypDataStr& newHypDataStr,
                                               Score& unweightedScore)
{
      // Obtain score for hypothesis extension
  unsigned int trglen=predHypDataStr.ntarget.size()-1;
  HypScoreInfo hypScrInf=predHypScrInf;
  unweightedScore=0;
    
  for(unsigned int i=predHypDataStr.sourceSegmentation.size();i<newHypDataStr.sourceSegmentation.size();++i)
  {
        // Initialize variables
    unsigned int trgLeft;
    unsigned int trgRight=newHypDataStr.targetSegmentCuts[i];
    if(i==0)
      trgLeft=1;
    else
      trgLeft=newHypDataStr.targetSegmentCuts[i-1]+1;
    unsigned int nextTrgPhraseLen=trgRight-trgLeft+1;

        // Update score
    Score iterScore=trgSegmLenScore(trglen+nextTrgPhraseLen,trglen,0);
    unweightedScore+= iterScore;
    hypScrInf.score+= weight*iterScore;
      
        // Increase trglen
    trglen+=nextTrgPhraseLen;
  }

      // NOTE: There are no additional score contributions when the
      // hypothesis is complete
    
  return hypScrInf;
}
