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
 * @file OnTheFlyDictFeat.h
 * 
 * @brief Declares the OnTheFlyDictFeat template class. This class
 * implements a dictionary feature created on-the-fly.
 */

#ifndef _OnTheFlyDictFeat_h
#define _OnTheFlyDictFeat_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "PhrScoreInfo.h"
#include "BasePbTransModelFeature.h"
#include <map>

//--------------- Constants ------------------------------------------

#define ONTHEFLY_DICT_SMOOTHING_SCORE 0

//--------------- Classes --------------------------------------------

//--------------- OnTheFlyDictFeat class

/**
 * @brief The OnTheFlyDictFeat template class is a base class for
 * implementing a direct phrase model feature.
 */

template<class SCORE_INFO>
class OnTheFlyDictFeat: public BasePbTransModelFeature<SCORE_INFO>
{
 public:

  typedef typename BasePbTransModelFeature<SCORE_INFO>::HypScoreInfo HypScoreInfo;

      // Constructor
  OnTheFlyDictFeat();

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

      // Functions to work with translation options
  void obtainTransOptions(const std::vector<std::string>& wordVec,
                          std::vector<std::vector<std::string> >& transOptVec);
  std::string getTransOptInfo(const std::vector<std::string>& srcPhrase,
                              const std::vector<std::string>& trgPhrase,
                              bool& found);
  
      // Functions to handle dictionary content
  void addTransOption(const std::vector<std::string>& srcPhrase,
                      const std::vector<std::string>& trgPhrase,
                      Score score,
                      std::string transOptInfo);
  void clearDict(void);
  
 protected:

  typedef struct{
    Score score;
    std::string info;
  } TransOptData;
  typedef std::map<std::vector<std::string>,TransOptData> DictEntry;
  typedef std::map<std::vector<std::string>,DictEntry> Dict;

  Dict dict;
};

//--------------- WordPenaltyFeat class functions
//

template<class SCORE_INFO>
OnTheFlyDictFeat<SCORE_INFO>::OnTheFlyDictFeat()
{
}

//---------------------------------
template<class SCORE_INFO>
std::string OnTheFlyDictFeat<SCORE_INFO>::getFeatType(void)
{
  return "OnTheFlyDictFeat";
}

//---------------------------------
template<class SCORE_INFO>
Score OnTheFlyDictFeat<SCORE_INFO>::scorePhrasePairUnweighted(const std::vector<std::string>& srcPhrase,
                                                              const std::vector<std::string>& trgPhrase)
{
      // Look for srcPhrase in dictionary
  typename Dict::const_iterator dictIter=dict.find(srcPhrase);
  if(dictIter!=dict.end())
  {
        // Look for trgPhrase in dictionary entry
    typename DictEntry::const_iterator dictEntryIter=dictIter->second.find(trgPhrase);
    if(dictEntryIter!=dictIter->second.end())
    {
      return dictEntryIter->second.score;
    }
    else
    {
      return ONTHEFLY_DICT_SMOOTHING_SCORE;
    }
  }
  else
  {
    return ONTHEFLY_DICT_SMOOTHING_SCORE;
  }
}

//---------------------------------
template<class SCORE_INFO>
void OnTheFlyDictFeat<SCORE_INFO>::obtainTransOptions(const std::vector<std::string>& wordVec,
                                                      std::vector<std::vector<std::string> >& transOptVec)
{
      // Look for srcPhrase in dictionary
  typename Dict::const_iterator dictIter=dict.find(wordVec);
  if(dictIter!=dict.end())
  {
    transOptVec.clear();
    typename DictEntry::const_iterator dictEntryIter;
    for(dictEntryIter=dictIter->second.begin();dictEntryIter!=dictIter->second.end();++dictEntryIter)
    {
      transOptVec.push_back(dictEntryIter->first);
    }
  }
  else
  {
        // No entries for srcPhrase were found
    transOptVec.clear();
  }
}

//---------------------------------
template<class SCORE_INFO>
std::string OnTheFlyDictFeat<SCORE_INFO>::getTransOptInfo(const std::vector<std::string>& srcPhrase,
                                                          const std::vector<std::string>& trgPhrase,
                                                          bool& found)
{
      // Look for srcPhrase in dictionary
  typename Dict::const_iterator dictIter=dict.find(srcPhrase);
  if(dictIter!=dict.end())
  {
    typename DictEntry::const_iterator dictEntryIter=dictIter->second.find(trgPhrase);
    if(dictEntryIter!=dictIter->second.end())
    {
          // trgPhrase is contained as entry
      found=true;
      return dictEntryIter->second.info;
    }
    else
    {
          // trgPhrase not contained as entry
      found=false;
      return "";
    }    
  }
  else
  {
        // No entries for srcPhrase were found
    found=false;
    return "";
  }
}

//---------------------------------
template<class SCORE_INFO>
void OnTheFlyDictFeat<SCORE_INFO>::addTransOption(const std::vector<std::string>& srcPhrase,
                                                  const std::vector<std::string>& trgPhrase,
                                                  Score score,
                                                  std::string transOptInfo)
{
      // Look for srcPhrase in dictionary
  typename Dict::iterator dictIter=dict.find(srcPhrase);
  if(dictIter!=dict.end())
  {
    TransOptData trOptData;
    trOptData.score=score;
    trOptData.info=transOptInfo;
    dictIter->second[trgPhrase]=trOptData;
  }
  else
  {
        // No entries for srcPhrase were found
    DictEntry dictEntry;
    TransOptData trOptData;
    trOptData.score=score;
    trOptData.info=transOptInfo;
    dictEntry[trgPhrase]=trOptData;
    dict[srcPhrase]=dictEntry;
  }
}

//---------------------------------
template<class SCORE_INFO>
void OnTheFlyDictFeat<SCORE_INFO>::clearDict(void)
{
  dict.clear();
}

#endif
