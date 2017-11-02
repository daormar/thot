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
/* Module: BaseLogLinWeightUpdater                                  */
/*                                                                  */
/* Prototypes file: BaseLogLinWeightUpdater.h                       */
/*                                                                  */
/* Description: Declares the BaseLogLinWeightUpdater abstract       */
/*              template class, this class is a base class for      */
/*              implementing different kinds of algorithms to       */
/*              update log-linear model weights.                    */
/*                                                                  */
/********************************************************************/

/**
 * @file BaseLogLinWeightUpdater.h
 *
 * @brief Declares the BaseWeightUpdater abstract template class, this
 * class is a base class for implementing different kinds of algorithms
 * to update log-linear model weights.
 */

#ifndef _BaseLogLinWeightUpdater_h
#define _BaseLogLinWeightUpdater_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseScorer.h"
#include <string>
#include <vector>
#include <utility>

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- BaseLogLinWeightUpdater class

/**
 * @brief Base abstract class that defines the interface that a
 * log-linear weight updater algorithm should offer to a statistical
 * machine translation model.
 */

class BaseLogLinWeightUpdater
{
 public:

      // Declarations related to dynamic class loading
  typedef BaseLogLinWeightUpdater* create_t(const char*);
  typedef const char* type_id_t(void);

      // Function to link scorer
  virtual bool link_scorer(BaseScorer* baseScorerPtr)=0;
  
      // Function to compute new weights
  virtual void update(const std::string& reference,
                      const std::vector<std::string>& nblist,
                      const std::vector<std::vector<double> >& scoreCompsVec,
                      const std::vector<double>& currWeightsVec,
                      std::vector<double>& newWeightsVec)=0;

      // Compute new weights for a closed corpus
  virtual void updateClosedCorpus(const std::vector<std::string>& reference,
                                  const std::vector<std::vector<std::string> >& nblist,
                                  const std::vector<std::vector<std::vector<double> > >& scoreCompsVec,
                                  const std::vector<double>& currWeightsVec,
                                  std::vector<double>& newWeightsVec)=0;

      // Destructor
  virtual ~BaseLogLinWeightUpdater(){};
};

#endif
