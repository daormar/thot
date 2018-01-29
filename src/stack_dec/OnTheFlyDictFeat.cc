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
 * @file OnTheFlyDictFeat.cc
 * 
 * @brief Definitions file for OnTheFlyDictFeat.h
 */

//--------------- Include files --------------------------------------

#include "OnTheFlyDictFeat.h"

//--------------- OnTheFlyDictFeat class functions

template<>
OnTheFlyDictFeat<PhrScoreInfo>::HypScoreInfo
OnTheFlyDictFeat<PhrScoreInfo>::extensionScore(const std::vector<std::string>& srcSent,
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
        // Obtain source phrase boundaries
    unsigned int srcLeft=newHypDataStr.sourceSegmentation[i].first;
    unsigned int srcRight=newHypDataStr.sourceSegmentation[i].second;

        // Obtain source phrase
    std::vector<std::string> srcPhrase;
    for(unsigned int k=srcLeft;k<=srcRight;++k)
      srcPhrase.push_back(srcSent[k-1]);

        // Obtain target phrase boundaries
    unsigned int trgLeft;
    unsigned int trgRight=newHypDataStr.targetSegmentCuts[i];
    if(i==0)
      trgLeft=1;
    else
      trgLeft=newHypDataStr.targetSegmentCuts[i-1]+1;

        // Obtain target phrase
    std::vector<std::string> trgPhrase;
    for(unsigned int k=trgLeft;k<=trgRight;++k)
    {
      trgPhrase.push_back(newHypDataStr.ntarget[k]);
    }

        // Update score
    Score iterScore=scorePhrasePairUnweighted(srcPhrase,trgPhrase);
   unweightedScore+= iterScore;
   hypScrInf.score+= weight*iterScore;
  }

      // NOTE: There are no additional score contributions when the
      // hypothesis is complete
    
  return hypScrInf;
}
