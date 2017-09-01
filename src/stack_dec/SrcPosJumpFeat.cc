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
/* Module: SrcPosJumpFeat                                           */
/*                                                                  */
/* Definitions file: SrcPosJumpFeat.cc                              */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "SrcPosJumpFeat.h"

//--------------- SrcPosJumpFeat class functions

template<>
SrcPosJumpFeat<PhrScoreInfo>::HypScoreInfo
SrcPosJumpFeat<PhrScoreInfo>::extensionScore(const std::vector<std::string>& /*srcSent*/,
                                             const HypScoreInfo& predHypScrInf,
                                             const PhrHypDataStr& predHypDataStr,
                                             const PhrHypDataStr& newHypDataStr,
                                             Score& unweightedScore)
{
      // Obtain score for hypothesis extension
  HypScoreInfo hypScrInf=predHypScrInf;
  unweightedScore=0;
    
  for(unsigned int i=predHypDataStr.sourceSegmentation.size();i<newHypDataStr.sourceSegmentation.size();++i)
  {
        // Initialize variables
    unsigned int srcLeft=newHypDataStr.sourceSegmentation[i].first;
    int lastSrcPosStart=srcLeft;
    int prevSrcPosEnd;
    if(i>0) prevSrcPosEnd=newHypDataStr.sourceSegmentation[i-1].second;
    else prevSrcPosEnd=0;

        // Update score
    Score iterScore=srcJumpScore(abs(lastSrcPosStart-(prevSrcPosEnd+1)));
    unweightedScore+= iterScore;
    hypScrInf.score+= weight*iterScore;
  }

      // NOTE: There are no additional score contributions when the
      // hypothesis is complete
    
  return hypScrInf;
}
