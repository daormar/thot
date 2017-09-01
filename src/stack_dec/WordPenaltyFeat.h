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
/* Prototypes file: WordPenaltyFeat.h                               */
/*                                                                  */
/* Description: Declares the WordPenaltyFeat template               */
/*              class. This class implements a word penalty         */
/*              feature.                                            */
/*                                                                  */
/********************************************************************/

/**
 * @file WordPenaltyFeat.h
 * 
 * @brief Declares the WordPenaltyFeat template class. This class
 * implements a word penalty feature.
 */

#ifndef _WordPenaltyFeat_h
#define _WordPenaltyFeat_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseWordPenaltyModel.h"
#include "PhrScoreInfo.h"
#include "BasePbTransModelFeature.h"

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- WordPenaltyFeat class

/**
 * @brief The WordPenaltyFeat template class is a base class for
 * implementing a word penalty feature.
 */

template<class SCORE_INFO>
class WordPenaltyFeat: public BasePbTransModelFeature<SCORE_INFO>
{
 public:

  typedef typename BasePbTransModelFeature<SCORE_INFO>::HypScoreInfo HypScoreInfo;

      // Constructor
  WordPenaltyFeat();
  
      // Feature information
  std::string getFeatType(void);

      // Scoring functions
  HypScoreInfo nullHypScore(const HypScoreInfo& predHypScrInf,
                            Score& unweightedScore);
  HypScoreInfo extensionScore(const std::vector<std::string>& srcSent,
                              const HypScoreInfo& predHypScrInf,
                              const PhrHypDataStr& predHypDataStr,
                              const PhrHypDataStr& newHypDataStr,
                              Score& unweightedScore);
  Score scorePhrasePairUnweighted(const std::vector<std::string>& srcPhrase,
                                  const std::vector<std::string>& trgPhrase);
  
      // Link pointer
  void link_wpm(BaseWordPenaltyModel* _wpModelPtr);
  
 protected:

  BaseWordPenaltyModel* wpModelPtr;
};

//--------------- WordPenaltyFeat class functions
//

template<class SCORE_INFO>
WordPenaltyFeat<SCORE_INFO>::WordPenaltyFeat()
{
  this->weight=0.000001;
}

//---------------------------------
template<class SCORE_INFO>
std::string WordPenaltyFeat<SCORE_INFO>::getFeatType(void)
{
  return "DirectPhraseModelFeat";
}

//---------------------------------
template<class SCORE_INFO>
Score WordPenaltyFeat<SCORE_INFO>::scorePhrasePairUnweighted(const std::vector<std::string>& /*srcPhrase*/,
                                                             const std::vector<std::string>& trgPhrase)
{
  return (double)wpModelPtr->wordPenaltyScore(trgPhrase.size());
}

//---------------------------------
template<class SCORE_INFO>
void WordPenaltyFeat<SCORE_INFO>::link_wpm(BaseWordPenaltyModel* _wpModelPtr)
{
  wpModelPtr=_wpModelPtr;
}

#endif
