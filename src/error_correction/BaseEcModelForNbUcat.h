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
/* Module: BaseEcModelForNbUcat                                     */
/*                                                                  */
/* Prototypes file: BaseEcModelForNbUcat.h                          */
/*                                                                  */
/* Description: Declares the BaseEcModelForNbUcat abstract          */
/*              template class, this is a base class useful to      */
/*              derive new classes that implement error correcting  */
/*              models for computer assisted translation based on   */
/*              n-best lists.                                       */
/*                                                                  */
/********************************************************************/

/**
 * @file BaseEcModelForNbUcat.h
 *
 * @brief Declares the BaseEcModelForNbUcat abstract template class,
 * this is a base class useful to derive new classes that implement
 * error correcting models for uncoupled computer assisted translation
 * based on n-best lists.
 */

#ifndef _BaseEcModelForNbUcat_h
#define _BaseEcModelForNbUcat_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "Prob.h"
#include "myVector.h"
#include <map>
#include "BaseErrorCorrectionModel.h"
#include "NbestCorrections.h"

using namespace std;

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------


//--------------- BaseEcModelForNbUcat template class

/**
 * @brief The BaseEcModelForNbUcat abstract template class is a base
 * class useful to derive new classes that implement error correcting
 * models for uncoupled computer assisted translation based on n-best
 * lists.
 */

class BaseEcModelForNbUcat
{
 public:

      // Declarations related to dynamic class loading
  typedef BaseEcModelForNbUcat* create_t(std::string);

      // Link error correcting model with the error correcting model for
      // uncoupled cat
  virtual void link_ecm(BaseErrorCorrectionModel* _ecm_ptr)=0;

  virtual NbestCorrections correct(const Vector<std::string>& outputSentVec,
                                   const Vector<unsigned int>& sourceCuts,
                                   const Vector<std::string>& prefixVec,
                                   unsigned int _maxMapSize,
                                   int verbose=0)=0;
      // Correct sentence given in outputSentVec using prefixVec. The
      // basic units for the corrections can be restricted by means of
      // sourceCuts, which contains a vector of cuts for
      // outputSentVec. Set one cut per word for unrestricted correction.

      // clear() function
  virtual void clear(void)=0;

  virtual void clearTempVars(void)=0;
      // Clear temporary variables

        // Destructor
  virtual ~BaseEcModelForNbUcat(){};
};

#endif
