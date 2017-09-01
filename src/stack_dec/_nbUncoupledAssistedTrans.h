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
/* Module: _nbUncoupledAssistedTrans                                */
/*                                                                  */
/* Prototypes file: _nbUncoupledAssistedTrans.h                     */
/*                                                                  */
/* Description: Declares the _nbUncoupledAssistedTrans abstract     */
/*              template class, this is a base class useful to      */
/*              derive new classes that implement uncoupled         */
/*              assisted translators based on n-best lists.         */
/*                                                                  */
/********************************************************************/

/**
 * @file _nbUncoupledAssistedTrans.h
 *
 * @brief Declares the _nbUncoupledAssistedTrans abstract template
 * class, this is a base class useful to derive new classes that
 * implement uncoupled assisted translators based on n-best lists.
 */

#ifndef __nbUncoupledAssistedTrans_h
#define __nbUncoupledAssistedTrans_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "_assistedTrans.h"
#include "BaseEcModelForNbUcat.h"

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------


//--------------- _nbUncoupledAssistedTrans template class: interface for
//--------------- uncoupled assisted translators

/**
 * @brief The _nbUncoupledAssistedTrans abstract template class is a
 * base class useful to derive new classes that implement uncoupled
 * assisted translators based on n-best lists.
 */

template<class SMT_MODEL>
class _nbUncoupledAssistedTrans: public _assistedTrans<SMT_MODEL>
{
 public:
  
      // Link statistical translation model with the decoder
  virtual int link_stack_trans(BaseStackDecoder<SMT_MODEL>* _sd_ptr)=0;

      // Link cat error correcting model with the decoder
  virtual void link_cat_ec_model(BaseEcModelForNbUcat* _ecm_ucat_ptr)=0;

      // Basic services
  virtual std::string translateWithPrefix(std::string s,
                                          std::string pref,
                                          const RejectedWordsSet& rejectedWords=RejectedWordsSet(),
                                          unsigned int verbose=0)=0;
      // Translates std::string s using pref as prefix, uncoupled version
  virtual std::string addStrToPrefix(std::string s,
                                     const RejectedWordsSet& rejectedWords=RejectedWordsSet(),
                                     unsigned int verbose=0)=0;
      // Adds the string 's' to the user prefix
  virtual void resetPrefix(void)=0;
      // Resets the prefix

  virtual void set_n(unsigned int n)=0;
      // Sets the size of n-best translations list used in uncoupled
      // assisted translation
    
      // Model weights functions
  virtual void setWeights(std::vector<float> wVec)=0;
  virtual unsigned int getNumWeights(void)=0;
  virtual void printWeights(std::ostream &outS)=0;
  
  virtual void clear(void)=0;
      // Remove all data structures used by the assisted translator

      // Destructor
  virtual ~_nbUncoupledAssistedTrans(){};
};

#endif
