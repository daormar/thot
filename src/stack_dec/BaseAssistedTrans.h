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
/* Module: BaseAssistedTrans                                        */
/*                                                                  */
/* Prototypes file: BaseAssistedTrans.h                             */
/*                                                                  */
/* Description: Declares the BaseAssistedTrans abstract             */
/*              template class, this is a base class useful to      */
/*              derive new classes that implement assisted          */
/*              translators.                                        */
/*                                                                  */
/********************************************************************/

/**
 * @file BaseAssistedTrans.h
 *
 * @brief Declares the BaseAssistedTrans abstract template class, this
 * is a base class useful to derive new classes that implement assisted
 * translators.
 */

#ifndef _BaseAssistedTrans_h
#define _BaseAssistedTrans_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <ErrorDefs.h>
#include "BaseStackDecoder.h"
#include <set>
#include <RejectedWordsSet.h>

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------


//--------------- BaseAssistedTrans template class: interface for
//                assisted translators

/**
 * @brief The BaseAssistedTrans abstract template class is a base class
 * useful to derive new classes that implement assisted translators.
 */

template<class SMT_MODEL>
class BaseAssistedTrans
{
 public:

      // Declarations related to dynamic class loading
  typedef BaseAssistedTrans* create_t(std::string);
  typedef std::string type_id_t(void);

      // Link stack decoder with the assisted translator
  virtual int link_stack_trans(BaseStackDecoder<SMT_MODEL>* _sd_ptr)=0;

      // Basic services
  virtual std::string translateWithPrefix(std::string s,
                                          std::string pref,
                                          const RejectedWordsSet& rejectedWords=RejectedWordsSet(),
                                          unsigned int verbose=0)=0;
      // Translates std::string s using pref as prefix
  
  virtual std::string addStrToPrefix(std::string s,
                                     const RejectedWordsSet& rejectedWords=RejectedWordsSet(),
                                     unsigned int verbose=0)=0;
      // Adds the string 's' to the user prefix
  
  virtual void resetPrefix(void)=0;
      // Resets the prefix

      // Model weights functions
  virtual void setWeights(std::vector<float> wVec)=0;
  virtual unsigned int getNumWeights(void)=0;
  virtual void printWeights(std::ostream &outS)=0;

      // clear() function
  virtual void clear(void)=0;
      // Remove all data structures used by the assisted translator

      // Destructor
  virtual ~BaseAssistedTrans(){};
};

//--------------- BaseAssistedTrans template class method definitions


#endif
