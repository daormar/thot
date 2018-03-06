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
 * @file BasePbTransModelFeature.h
 * 
 * @brief Declares the BasePbTransModelFeature abstract template class,
 * this class is a base class for implementing different kinds of
 * features to be used in phrase based translation models.
 */

#ifndef _BasePbTransModelFeature_h
#define _BasePbTransModelFeature_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "PhrHypDataStr.h"
#include "Score.h"
#include <set>
#include <string>

//--------------- Constants ------------------------------------------

#define FEAT_LGPROB_SMOOTH         -9999999

//--------------- Classes --------------------------------------------

//--------------- BasePbTransModelFeature class

/**
 * @brief The BasePbTransModelFeature abstract template class is a base
 * class for implementing different kinds of features to be used in
 * phrase based translation models.
 */

template<class SCORE_INFO>
class BasePbTransModelFeature
{
 public:

  typedef SCORE_INFO HypScoreInfo;

      // Feature information
  void setFeatName(std::string fname);
  std::string getFeatName(void);
  virtual std::string getFeatType(void)=0;

      // Scoring functions
  virtual HypScoreInfo nullHypScore(const HypScoreInfo& predHypScrInf,
                                    float weight,
                                    Score& unweightedScore);
  virtual HypScoreInfo extensionScore(const std::vector<std::string>& srcSent,
                                      const HypScoreInfo& predHypScrInf,
                                      const PhrHypDataStr& predHypDataStr,
                                      const PhrHypDataStr& newHypDataStr,
                                      float weight,
                                      Score& unweightedScore)=0;
  virtual Score scorePhrasePairUnweighted(const std::vector<std::string>& srcPhrase,
                                          const std::vector<std::string>& trgPhrase)=0;

      // Functions to obtain translation options
  virtual void obtainTransOptions(const std::vector<std::string>& wordVec,
                                  std::vector<std::vector<std::string> >& transOptVec);

      // Destructor
  virtual ~BasePbTransModelFeature(){};

 protected:

  std::string name;

      // Auxiliary functions
  unsigned int numberOfSrcWordsCovered(const PhrHypDataStr& hypdStr)const;
};

//--------------- BasePbTransModelFeature class functions
//

//---------------------------------
template<class SCORE_INFO>
void BasePbTransModelFeature<SCORE_INFO>::setFeatName(std::string fname)
{
  name=fname;
}

//---------------------------------
template<class SCORE_INFO>
std::string BasePbTransModelFeature<SCORE_INFO>::getFeatName(void)
{
  return name;
}

//---------------------------------
template<class SCORE_INFO>
typename BasePbTransModelFeature<SCORE_INFO>::HypScoreInfo
BasePbTransModelFeature<SCORE_INFO>::nullHypScore(const HypScoreInfo& predHypScrInf,
                                                  float /*weight*/,
                                                  Score& unweightedScore)
{
  unweightedScore=0;
  HypScoreInfo hypScrInf=predHypScrInf;
  return hypScrInf;
}

//---------------------------------
template<class SCORE_INFO>
void BasePbTransModelFeature<SCORE_INFO>::obtainTransOptions(const std::vector<std::string>& /*wordVec*/,
                                                             std::vector<std::vector<std::string> >& transOptVec)
{
      // Standard features do not provide translation options
  transOptVec.clear();
}

//---------------------------------
template<class SCORE_INFO>
unsigned int BasePbTransModelFeature<SCORE_INFO>::numberOfSrcWordsCovered(const PhrHypDataStr& hypdStr)const
{
  unsigned int n=0;
  for(unsigned int k=0;k<hypdStr.sourceSegmentation.size();k++)
	n+=hypdStr.sourceSegmentation[k].second-hypdStr.sourceSegmentation[k].first+1;
  return n;
}

#endif
