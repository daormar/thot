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
#include "AwkInputStream.h"
#include "Score.h"
#include "ErrorDefs.h"
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

      // Constructors
  DictFeat();
  DictFeat(std::string modelFileName);

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

  int load(std::string modelFileName);
  int extractEntryInfo(AwkInputStream& awk,
                       std::vector<std::string>& srcPhr,
                       std::vector<std::string>& trgPhr,
                       Score& scr);
  void insertEntry(const std::vector<std::string>& srcPhr,
                   const std::vector<std::string>& trgPhr,
                   Score scr);
};

//--------------- DictFeat class functions
//

template<class SCORE_INFO>
DictFeat<SCORE_INFO>::DictFeat()
{
}

//---------------------------------
template<class SCORE_INFO>
DictFeat<SCORE_INFO>::DictFeat(std::string modelFileName)
{
      // Load dictionary entries from file
  int ret=load(modelFileName);
  if(ret==THOT_ERROR)
    throw std::runtime_error("Error while initializing DictFeat object");
}

//---------------------------------
template<class SCORE_INFO>
int DictFeat<SCORE_INFO>::load(std::string modelFileName)
{
  AwkInputStream awk;
  if(awk.open(modelFileName.c_str())==THOT_ERROR)
  {
    return THOT_ERROR;
  }
  else
  {
        // Read entries for each model
    while(awk.getln())
    {
          // Extract entry information
      std::vector<std::string> srcPhr;
      std::vector<std::string> trgPhr;
      Score scr;
      extractEntryInfo(awk,srcPhr,trgPhr,scr);

          // Insert information into dictionary
      insertEntry(srcPhr,trgPhr,scr);
    }
  }
  
  return THOT_OK;
}

//---------------------------------
template<class SCORE_INFO>
int DictFeat<SCORE_INFO>::extractEntryInfo(AwkInputStream& awk,
                                           std::vector<std::string>& srcPhr,
                                           std::vector<std::string>& trgPhr,
                                           Score& scr)
{
  unsigned int i;

      // Obtain source phrase
  srcPhr.clear();
  for(i = 1; i <= awk.NF; ++i)
  {
    if(awk.dollar(i) == "|||")
      break;
    else
      srcPhr.push_back(awk.dollar(i));
  }
  if(i == awk.NF)
    return THOT_ERROR;

      // Obtain target phrase
  trgPhr.clear();
  i += 1;
  for(; i <= awk.NF; ++i)
  {
    if(awk.dollar(i) == "|||")
      break;
    else
      trgPhr.push_back(awk.dollar(i));
  }
  if(i != awk.NF - 1)
    return THOT_ERROR;

      // Obtain score
  scr = atof(awk.dollar(awk.NF).c_str());

  return THOT_OK;
}

//---------------------------------
template<class SCORE_INFO>
void DictFeat<SCORE_INFO>::insertEntry(const std::vector<std::string>& srcPhr,
                                       const std::vector<std::string>& trgPhr,
                                       Score scr)
{
  Dict::iterator dictIter=dict.find(srcPhr);
  if(dictIter==dict.end())
  {
        // There is no entry for source phrase
    TransOptions trOpts;
    trOpts.insert(std::make_pair(trgPhr,scr));
    dict.insert(std::make_pair(srcPhr,trOpts));
  }
  else
  {
        // Given source phrase has an entry in dictionary
    dictIter->second[trgPhr]=scr;
  }
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
