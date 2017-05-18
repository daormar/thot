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
/* Module: WordPenaltyFeat                                          */
/*                                                                  */
/* Definitions file: WordPenaltyFeat.cc                             */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "WordPenaltyFeat.h"

//--------------- WordPenaltyFeat class functions

template<>
typename WordPenaltyFeat<PhrScoreInfo>::HypScoreInfo
WordPenaltyFeat<PhrScoreInfo>::extensionScore(const Vector<std::string>& srcSent,
                                              const HypScoreInfo& predHypScrInf,
                                              const PhrHypDataStr& predHypDataStr,
                                              const PhrHypDataStr& newHypDataStr,
                                              Score& unweightedScore)
{
      // Check if function was called to score the null hypothesis
  if(predHypDataStr.sourceSegmentation.empty() && newHypDataStr.sourceSegmentation.empty())
  {
        // Obtain score for null hypothesis
    unweightedScore=wpModelPtr->sumWordPenaltyScore(0);
    HypScoreInfo hypScrInf=predHypScrInf;
    hypScrInf.score+=weight*unweightedScore;
    return hypScrInf;
  }
  else
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
      Score iterScore=wpModelPtr->sumWordPenaltyScore(trglen+nextTrgPhraseLen) - wpModelPtr->sumWordPenaltyScore(trglen);
      unweightedScore+= iterScore;
      hypScrInf.score+= weight*iterScore;

          // Increase trglen
      trglen+=nextTrgPhraseLen;
    }

        // Check if new hypothesis is complete
    if(numberOfSrcWordsCovered(newHypDataStr)==srcSent.size())
    {
        // Obtain score contribution for complete hypothesis
      Score scrCompl=wpModelPtr->wordPenaltyScore(trglen) - wpModelPtr->sumWordPenaltyScore(trglen);
      unweightedScore+= scrCompl;
      hypScrInf.score+= weight*scrCompl;
    }
    
    return hypScrInf;
  }
}
