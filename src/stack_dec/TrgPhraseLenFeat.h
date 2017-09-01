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
/* Prototypes file: TrgPhraseLenFeat.h                              */
/*                                                                  */
/* Description: Declares the TrgPhraseLenFeat template              */
/*              class. This class implements a target phrase        */
/*              length feature.                                     */
/*                                                                  */
/********************************************************************/

/**
 * @file TrgPhraseLenFeat.h
 * 
 * @brief Declares the TrgPhraseLenFeat template class. This class
 * implements a target phrase length feature.
 */

#ifndef _TrgPhraseLenFeat_h
#define _TrgPhraseLenFeat_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BasePhraseModel.h"
#include "PhrScoreInfo.h"
#include "BasePbTransModelFeature.h"

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- TrgPhraseLenFeat class

/**
 * @brief The TrgPhraseLenFeat template class is a base class for
 * implementing a word penalty feature.
 */

template<class SCORE_INFO>
class TrgPhraseLenFeat: public BasePbTransModelFeature<SCORE_INFO>
{
 public:

  typedef typename BasePbTransModelFeature<SCORE_INFO>::HypScoreInfo HypScoreInfo;

      // Constructor
  TrgPhraseLenFeat();
  
      // Feature information
  std::string getFeatType(void);

      // Scoring functions
  HypScoreInfo extensionScore(const std::vector<std::string>& srcSent,
                              const HypScoreInfo& predHypScrInf,
                              const PhrHypDataStr& predHypDataStr,
                              const PhrHypDataStr& newHypDataStr,
                              Score& unweightedScore);
  Score scorePhrasePairUnweighted(const std::vector<std::string>& srcPhrase,
                                  const std::vector<std::string>& trgPhrase);

      // Functions related to model pointers
  void link_pm(BasePhraseModel* _invPbModelPtr);

 protected:

  BasePhraseModel* invPbModelPtr;

  Score trgSegmLenScore(unsigned int x_k,
                        unsigned int x_km1,
                        unsigned int trgLen);
};

//--------------- TrgPhraseLenFeat class functions
//

template<class SCORE_INFO>
TrgPhraseLenFeat<SCORE_INFO>::TrgPhraseLenFeat()
{
  this->weight=1;
}

//---------------------------------
template<class SCORE_INFO>
std::string TrgPhraseLenFeat<SCORE_INFO>::getFeatType(void)
{
  return "TrgPhraseLenFeat";
}

//---------------------------------
template<class SCORE_INFO>
Score TrgPhraseLenFeat<SCORE_INFO>::scorePhrasePairUnweighted(const std::vector<std::string>& /*srcPhrase*/,
                                                              const std::vector<std::string>& /*trgPhrase*/)
{
  return 0;
}

//---------------------------------
template<class SCORE_INFO>
void TrgPhraseLenFeat<SCORE_INFO>::link_pm(BasePhraseModel* _invPbModelPtr)
{
  invPbModelPtr=_invPbModelPtr;
}

//---------------------------------------
template<class SCORE_INFO>
Score TrgPhraseLenFeat<SCORE_INFO>::trgSegmLenScore(unsigned int x_k,
                                                    unsigned int x_km1,
                                                    unsigned int trgLen)
{
  return (double)invPbModelPtr->srcSegmLenLgProb(x_k,x_km1,trgLen);
}

#endif
