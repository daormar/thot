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
 * @file LevelDbDictFeat.h
 * 
 * @brief Declares the LevelDbDictFeat template class. This class implements a
 * dictionary feature with scores.
 */

#ifndef _LevelDbDictFeat_h
#define _LevelDbDictFeat_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "LevelDbDict.h"
#include "PhrScoreInfo.h"
#include "BasePbTransModelFeature.h"
#include "Score.h"
#include "ErrorDefs.h"
#include <map>

//--------------- Constants ------------------------------------------

//--------------- Classes --------------------------------------------

//--------------- LevelDbDictFeat class

/**
 * @brief The LevelDbDictFeat template class implements a
 * dictionary feature with scores.
 */

template<class SCORE_INFO>
class LevelDbDictFeat: public BasePbTransModelFeature<SCORE_INFO>
{
 public:

  typedef typename BasePbTransModelFeature<SCORE_INFO>::HypScoreInfo HypScoreInfo;

      // Constructors
  LevelDbDictFeat();
  LevelDbDictFeat(std::string modelFileName);

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


  LevelDbDict dict;

  int load(std::string modelFileName);
};

//--------------- LevelDbDictFeat class functions
//

template<class SCORE_INFO>
LevelDbDictFeat<SCORE_INFO>::LevelDbDictFeat()
{
}

//---------------------------------
template<class SCORE_INFO>
LevelDbDictFeat<SCORE_INFO>::LevelDbDictFeat(std::string modelFileName)
{
      // Load dictionary entries from file
  int ret=load(modelFileName);
  if(ret==THOT_ERROR)
    throw std::runtime_error("Error while initializing LevelDbDictFeat object");
}

//---------------------------------
template<class SCORE_INFO>
int LevelDbDictFeat<SCORE_INFO>::load(std::string modelFileName)
{
  int ret = dict.load(modelFileName);
  if(ret == THOT_ERROR) return THOT_ERROR;
  
  return THOT_OK;
}

//---------------------------------
template<class SCORE_INFO>
std::string LevelDbDictFeat<SCORE_INFO>::getFeatType(void)
{
  return "LevelDbDictFeat";
}

//---------------------------------
template<class SCORE_INFO>
Score LevelDbDictFeat<SCORE_INFO>::scorePhrasePairUnweighted(const std::vector<std::string>& srcPhrase,
                                                             const std::vector<std::string>& trgPhrase)
{
  bool found;
  Score scr=dict.getScore(srcPhrase,trgPhrase,found);
  if(found)
    return scr;
  else
    return FEAT_LGPROB_SMOOTH; 
}

//---------------------------------
template<class SCORE_INFO>
void LevelDbDictFeat<SCORE_INFO>::obtainTransOptions(const std::vector<std::string>& wordVec,
                                                     std::vector<std::vector<std::string> >& transOptVec)
{
  dict.getTransOptsForSource(wordVec,transOptVec);
}

#endif
