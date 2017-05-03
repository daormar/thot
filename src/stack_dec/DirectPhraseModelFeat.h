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

#include "PhraseBasedTmHypRec.h"
#include "BaseSmtModelFeature.h"

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- DirectPhraseModelFeat class

/**
 * @brief The DirectPhraseModelFeat template class is a base class for
 * implementing a direct phrase model feature.
 */

template<class EQCLASS_FUNC>
class DirectPhraseModelFeat: public BaseSmtModelFeature<PhraseBasedTmHypRec<EQCLASS_FUNC> >
{
 public:

      // TO-BE-DONE
  typedef typename BaseSmtModelFeature<PhraseBasedTmHypRec<EQCLASS_FUNC> >::Hypothesis Hypothesis;
  typedef typename BaseSmtModelFeature<PhraseBasedTmHypRec<EQCLASS_FUNC> >::HypScoreInfo HypScoreInfo;
  typedef typename BaseSmtModelFeature<PhraseBasedTmHypRec<EQCLASS_FUNC> >::HypDataType HypDataType;

      // Feature information
  std::string getFeatType(void);

      // Scoring functions
  HypScoreInfo extensionScore(const Hypothesis& pred_hyp,
                              const HypDataType& new_hypd);

 protected:

};

//--------------- DirectPhraseModelFeat class functions
//

template<class EQCLASS_FUNC>
std::string DirectPhraseModelFeat<EQCLASS_FUNC>::getFeatType(void)
{
  return "DirectPhraseModelFeat";
}

//---------------------------------
template<class EQCLASS_FUNC>
typename DirectPhraseModelFeat<EQCLASS_FUNC>::HypScoreInfo
DirectPhraseModelFeat<EQCLASS_FUNC>::extensionScore(const Hypothesis& pred_hyp,
                                                    const HypDataType& new_hypd)
{
  
}

#endif
