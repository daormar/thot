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

#include "BaseSmtModelFeature.h"

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- DirectPhraseModelFeat class

/**
 * @brief The DirectPhraseModelFeat template class is a base class for
 * implementing a direct phrase model feature.
 */

template<class HYPOTHESIS>
class DirectPhraseModelFeat
{
 public:

      // TO-BE-DONE

      // Feature information
  int getFeatType(void);

      // Scoring functions
  Score extensionScore(const Hypothesis& pred_hyp,
                       const HypDataType& new_hypd);

 protected:

};

//--------------- DirectPhraseModelFeat class functions
//

template<class HYPOTHESIS>
std::string DirectPhraseModelFeat<HYPOTHESIS>::getFeatType(void)
{
  return "DirectPhraseModelFeat"
}

//---------------------------------
template<class HYPOTHESIS>
Score DirectPhraseModelFeat<HYPOTHESIS>::extensionScore(const Hypothesis& pred_hyp,
                                                        const HypDataType& new_hypd)
{
  
}

#endif
