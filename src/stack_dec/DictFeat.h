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
 * @file DictFeat.h
 * 
 * @brief Declares the DictFeat template class. This class implements a
 * dictionary feature with scores.
 */

#ifndef _DictFeat_h
#define _DictFeat_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "PhrScoreInfo.h"
#include "BasePbTransModelFeature.h"
#include "Score.h"
#include <map>

//--------------- Constants ------------------------------------------

//--------------- Classes --------------------------------------------

//--------------- DictFeat class

/**
 * @brief The DictFeat template class implements a
 * dictionary feature with scores.
 */

template<class SCORE_INFO>
class DictFeat: public BasePbTransModelFeature<SCORE_INFO>
{
 public:

  typedef typename BasePbTransModelFeature<SCORE_INFO>::HypScoreInfo HypScoreInfo;

      // Constructor
  DictFeat();

      // Feature information
  std::string getFeatType(void);

      // Scoring functions
  HypScoreInfo extensionScore(const std::vector<std::string>& srcSent,
                              const HypScoreInfo& predHypScrInf,
                              const PhrHypDataStr& predHypDataStr,
                              const PhrHypDataStr& newHypDataStr,
                              float weight,
                              Score& unweightedScore);
  Score scorePhrasePairUnweighted(const std::vector<std::string>& srcPhrase,
                                  const std::vector<std::string>& trgPhrase);

      // Functions to obtain translation options
  void obtainTransOptions(const std::vector<std::string>& wordVec,
                          std::vector<std::vector<std::string> >& transOptVec);
  
 protected:

  typedef std::map<std::vector<std::string>,Score> TransOptions;
  typedef std::map<std::vector<std::string>, TransOptions> Dict;

  Dict dict;
  
};

//--------------- WordPenaltyFeat class functions
//

template<class SCORE_INFO>
DictFeat<SCORE_INFO>::DictFeat()
{
}

//---------------------------------
template<class SCORE_INFO>
std::string DictFeat<SCORE_INFO>::getFeatType(void)
{
  return "DictFeat";
}

//---------------------------------
template<class SCORE_INFO>
Score DictFeat<SCORE_INFO>::scorePhrasePairUnweighted(const std::vector<std::string>& srcPhrase,
                                                      const std::vector<std::string>& trgPhrase)
{
  Dict::const_iterator dictIter=dict.find(srcPhrase);
  if(dictIter==dict.end())
    return FEAT_LGPROB_SMOOTH; 
  else
  {
    TransOptions::const_iterator trOptIter=dictIter->second.find(trgPhrase);
    if(trOptIter==dictIter->second.end())
      return FEAT_LGPROB_SMOOTH;
    else
      return trOptIter->second;
  }
}

//---------------------------------
template<class SCORE_INFO>
void DictFeat<SCORE_INFO>::obtainTransOptions(const std::vector<std::string>& wordVec,
                                              std::vector<std::vector<std::string> >& transOptVec)
{
      // Clear output variable
  transOptVec.clear();

      // Find translation options for wordVec
  Dict::const_iterator dictIter=dict.find(wordVec);
  if(dictIter!=dict.end())
  {
        // Put translation options in output variable
    TransOptions::const_iterator trOptIter;
    for(trOptIter=dictIter->second.begin();trOptIter!=dictIter->second.end();++trOptIter)
      transOptVec.push_back(trOptIter->first);
  }  
}

#endif
