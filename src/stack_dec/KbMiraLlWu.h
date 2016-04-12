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
/* Module: KbMiraLlWu                                               */
/*                                                                  */
/* Prototypes file: KbMiraLlWu.h                                    */
/*                                                                  */
/* Description: Declares the KbMiraLlWu class implementing          */
/*              the K-best MIRA algorithm for weight                */
/*              updating.                                           */
/*                                                                  */
/********************************************************************/

/**
 * @file KbMiraLlWu.h
 *
 * @brief Declares the KbMiraLlWu class implementing the K-best MIRA
 * algorithm for weight updating.
 */

#ifndef _KbMiraLlWu_h
#define _KbMiraLlWu_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseLogLinWeightUpdater.h"

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- KbMiraLlWu template class

/**
 * @brief Class implementing the K-best MIRA algorithm.
 */

class KbMiraLlWu: public BaseLogLinWeightUpdater
{
 public:

      // Compute new weights for an individual sentence
  void update(const std::string& reference,
              const Vector<string>& nblist,
              const Vector<Vector<Score> >& scoreCompsVec,
              const Vector<Score>& currWeightsVec,
              Vector<Score>& newWeightsVec);

      // Compute new weights for a closed corpus
  void updateClosedCorpus(const Vector<std::string>& reference,
                          const Vector<Vector<string> >& nblist,
                          const Vector<Vector<Score> >& scoreCompsVec,
                          const Vector<Score>& currWeightsVec,
                          Vector<Score>& newWeightsVec);
};

#endif
