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
/* Module: DirectPhraseModelFeat                                    */
/*                                                                  */
/* Prototypes file: DirectPhraseModelFeat.h                         */
/*                                                                  */
/* Description: Declares the DirectPhraseModelFeat template         */
/*              class. This class implements a direct phrase model  */
/*              feature.                                            */
/*                                                                  */
/********************************************************************/

/**
 * @file DirectPhraseModelFeat.h
 * 
 * @brief Declares the DirectPhraseModelFeat template class. This class
 * implements a direct phrase model feature.
 */

#ifndef _DirectPhraseModelFeat_h
#define _DirectPhraseModelFeat_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include THOT_PPINFO_H // Define PpInfo type. It is set in
                       // configure by checking PPINFO_H variable
                       // (default value: PpInfo.h)
#include "BaseSwAligModel.h"
#include "BasePhraseModel.h"
#include "PhraseBasedTmHypRec.h"
#include "BasePbTransModelFeature.h"

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- DirectPhraseModelFeat class

/**
 * @brief The DirectPhraseModelFeat template class is a base class for
 * implementing a direct phrase model feature.
 */

template<class SCORE_INFO>
class DirectPhraseModelFeat: public BasePbTransModelFeature<SCORE_INFO>
{
 public:

      // TO-BE-DONE
  typedef typename BasePbTransModelFeature<SCORE_INFO>::HypScoreInfo HypScoreInfo;

      // Constructor
  DirectPhraseModelFeat();

      // Feature information
  std::string getFeatType(void);

      // Scoring functions
  HypScoreInfo extensionScore(unsigned int srcSentLen,
                              const HypScoreInfo& predHypScrInf,
                              const PhrHypDataStr& predHypDataStr,
                              const PhrHypDataStr& newHypDataStr,
                              Score& unweightedScore);

      // Link pointer
  void link_pm(BasePhraseModel* _pbModelPtr);
  void link_swm(BaseSwAligModel<PpInfo>* _swAligModelPtr);

 protected:

  BasePhraseModel* pbModelPtr;
  BaseSwAligModel<PpInfo>* swAligModelPtr;

};

//--------------- WordPenaltyFeat class functions
//

template<class SCORE_INFO>
DirectPhraseModelFeat<SCORE_INFO>::DirectPhraseModelFeat()
{
  this->weight=1.0;
}

//---------------------------------
template<class SCORE_INFO>
std::string DirectPhraseModelFeat<SCORE_INFO>::getFeatType(void)
{
  return "DirectPhraseModelFeat";
}

//---------------------------------
template<class SCORE_INFO>
void DirectPhraseModelFeat<SCORE_INFO>::link_pm(BasePhraseModel* _pbModelPtr)
{
  pbModelPtr=_pbModelPtr;
}

//---------------------------------
template<class SCORE_INFO>
void DirectPhraseModelFeat<SCORE_INFO>::link_swm(BaseSwAligModel<PpInfo>* _swAligModelPtr)
{
  swAligModelPtr=_swAligModelPtr;
}

#endif
