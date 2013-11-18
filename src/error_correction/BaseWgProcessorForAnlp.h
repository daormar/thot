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
/* Module: BaseWgProcessorForAnlp                                   */
/*                                                                  */
/* Prototypes file: BaseWgProcessorForAnlp.h                        */
/*                                                                  */
/* Description: Declares the BaseWgProcessorForAnlp class. This     */
/*              class is a base class for implementing word-graph   */
/*              processors for assisted natural language            */
/*              processing.                                         */
/*                                                                  */
/********************************************************************/

/**
 * @file BaseWgProcessorForAnlp.h
 *
 * @brief Declares the BaseWgProcessorForAnlp function, this class is a
 * base class for implementing word-graph processors for assisted
 * natural language processing.
 */

#ifndef _BaseWgProcessorForAnlp_h
#define _BaseWgProcessorForAnlp_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <NbestCorrections.h>
#include <WordGraph.h>
#include "RejectedWordsSet.h"

//--------------- Constants ------------------------------------------


//--------------- Functions ------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- BaseWgProcessorForAnlp template class

/**
 * @brief The BaseWgProcessorForAnlp class is a base class for
 * implementing word-graph processors for assisted natural language
 * processing.
 */

template<class ECM_FOR_WG>
class BaseWgProcessorForAnlp
{
 public:
  
      // Link word-graph with word-graph processor
  virtual void link_wg(const WordGraph* _wg_ptr)=0;
  
      // Link error correcting model for word-graph with the word-graph
      // processor
  virtual void link_ecm_wg(ECM_FOR_WG* _ecm_wg_ptr)=0;

  virtual void set_wgw(float _wgWeight)=0;
      // Set word-graph weight

  virtual void set_ecmw(float _ecmWeight)=0;
      // Set error correcting model weight

  virtual NbestCorrections correct(std::string prefix,
                                   unsigned int n,
                                   const RejectedWordsSet& rejectedWords=RejectedWordsSet(),
                                   unsigned int verbose=0)=0;
      // Given a prefix and a pair of weights, obtains a list of the
      // n-best corrected translations.
      // IMPORTANT: the prefix should not be empty

      // clear() function
  virtual void clear(void)=0;

      // clearTempVars() function
  virtual void clearTempVars(void)=0;

      // print() function
  virtual bool print(const char* filename)const=0;

        // Destructor
  virtual ~BaseWgProcessorForAnlp(){};
};

#endif
