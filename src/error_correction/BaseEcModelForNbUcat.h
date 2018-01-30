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
#include <map>
#include <vector>
#include "BaseErrorCorrectionModel.h"
#include "NbestCorrections.h"

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
  typedef BaseEcModelForNbUcat* create_t(const char*);
  typedef const char* type_id_t(void);

      // Link error correcting model with the error correcting model for
      // uncoupled cat
  virtual void link_ecm(BaseErrorCorrectionModel* _ecm_ptr)=0;

  virtual NbestCorrections correct(const std::vector<std::string>& outputSentVec,
                                   const std::vector<unsigned int>& sourceCuts,
                                   const std::vector<std::string>& prefixVec,
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
