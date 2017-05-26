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
/* Module: SrcPosJumpFeat                                         */
/*                                                                  */
/* Prototypes file: SrcPosJumpFeat.h                              */
/*                                                                  */
/* Description: Declares the SrcPosJumpFeat template              */
/*              class. This class implements a source phrase        */
/*              length feature.                                     */
/*                                                                  */
/********************************************************************/

/**
 * @file SrcPosJumpFeat.h
 * 
 * @brief Declares the SrcPosJumpFeat template class. This class
 * implements a reordering feature.
 */

#ifndef _SrcPosJumpFeat_h
#define _SrcPosJumpFeat_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BasePhraseModel.h"
#include "PhrScoreInfo.h"
#include "BasePbTransModelFeature.h"

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- SrcPosJumpFeat class

/**
 * @brief The SrcPosJumpFeat template class is a base class for
 * implementing a reordering feature.
 */

template<class SCORE_INFO>
class SrcPosJumpFeat: public BasePbTransModelFeature<SCORE_INFO>
{
 public:

  typedef typename BasePbTransModelFeature<SCORE_INFO>::HypScoreInfo HypScoreInfo;

      // Constructor
  SrcPosJumpFeat();
  
      // Feature information
  std::string getFeatType(void);

      // Scoring functions
  HypScoreInfo extensionScore(const Vector<std::string>& srcSent,
                              const HypScoreInfo& predHypScrInf,
                              const PhrHypDataStr& predHypDataStr,
                              const PhrHypDataStr& newHypDataStr,
                              Score& unweightedScore);
  Score scorePhrasePair(const Vector<std::string>& srcPhrase,
                        const Vector<std::string>& trgPhrase);

      // Functions related to model pointers
  void link_pm(BasePhraseModel* _invPbModelPtr);

 protected:

  BasePhraseModel* invPbModelPtr;

  Score srcJumpScore(unsigned int offset);
};

//--------------- SrcPosJumpFeat class functions
//

template<class SCORE_INFO>
SrcPosJumpFeat<SCORE_INFO>::SrcPosJumpFeat()
{
  this->weight=1;
}

//---------------------------------
template<class SCORE_INFO>
std::string SrcPosJumpFeat<SCORE_INFO>::getFeatType(void)
{
  return "SrcPosJumpFeat";
}

//---------------------------------
template<class SCORE_INFO>
Score SrcPosJumpFeat<SCORE_INFO>::scorePhrasePair(const Vector<std::string>& /*srcPhrase*/,
                                                  const Vector<std::string>& trgPhrase)
{
  return 0;
}

//---------------------------------
template<class SCORE_INFO>
void SrcPosJumpFeat<SCORE_INFO>::link_pm(BasePhraseModel* _invPbModelPtr)
{
  invPbModelPtr=_invPbModelPtr;
}

//---------------------------------------
template<class SCORE_INFO>
Score SrcPosJumpFeat<SCORE_INFO>::srcJumpScore(unsigned int offset)
{
  return (double)this->invPbModelPtr->trgCutsLgProb(offset);
}

#endif
