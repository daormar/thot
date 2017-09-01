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
/* Module: BaseStackDecoder                                         */
/*                                                                  */
/* Prototypes file: BaseStackDecoder.h                              */
/*                                                                  */
/* Description: Declares the BaseStackDecoder abstract template     */
/*              class, this class is a base class for implementing  */
/*              different kinds of stack based decoders.            */    
/*                                                                  */
/********************************************************************/

/**
 * @file BaseStackDecoder.h
 *
 * @brief Declares the BaseStackDecoder abstract template class, this
 * class is a base class for implementing different kinds of stack based
 * decoders.
 */

#ifndef _BaseStackDecoder_h
#define _BaseStackDecoder_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseSmtModel.h"
#include "ErrorDefs.h"
#include <Score.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <utility>

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- BaseStackDecoder template class

/**
 * @brief Base abstract class that defines the interface offered by a
 * stack-based decoder.
 */

template<class SMT_MODEL>
class BaseStackDecoder
{
 public:

  typedef typename SMT_MODEL::Hypothesis Hypothesis;

      // Declarations related to dynamic class loading
  typedef BaseStackDecoder* create_t(std::string);
  typedef std::string type_id_t(void);

      // Link statistical translation model with the decoder
  virtual bool link_smt_model(BaseSmtModel<Hypothesis>* _smtm_ptr)=0;
      // Get pointer to the statistical translation model
  virtual SMT_MODEL* get_smt_model_ptr(void)=0;

      // Functions for setting the decoder parameters
  virtual void set_S_par(unsigned int S_par)=0;
  virtual void set_I_par(unsigned int I_par)=0;
  virtual void set_G_par(unsigned int G_par);
  virtual void set_breadthFirst(bool b)=0;

      // Basic services
  virtual Hypothesis translate(std::string s)=0; 
      // Translates the sentence 's' using the model fixed previously
      // with 'setModel'
  virtual Hypothesis getNextTrans(void)=0;
      // Obtains the next hypothesis that the algorithm yields
  virtual Hypothesis translateWithRef(std::string s,
                                      std::string ref)=0;
      // Obtains the best alignment for the source and ref sentence pair
  virtual Hypothesis verifyCoverageForRef(std::string s,
                                          std::string ref)=0;
      // Verifies coverage of the translation model given a source
      // sentence s and the desired output ref. For this purpose, the
      // decoder filters those translations of s that are compatible
      // with ref. The resulting hypothesis won't be complete if the
      // model can't generate the reference
  virtual Hypothesis translateWithSuggestion(string s,
                                             typename Hypothesis::DataType sug)=0;
      // Translates string s using hypothesis sug as suggestion instead
      // of using the null hypothesis
  virtual Hypothesis translateWithPrefix(std::string s,
                                         std::string pref)=0;  
      // Translates std::string s using pref as prefix

  virtual void clear(void)=0;
      // Remove all partial hypotheses contained in the stack/s
  
  virtual void useBestScorePruning(bool b)=0;
  virtual void setWorstScoreAllowed(Score _worstScoreAllowed)=0;

      // Functions to report information about the search
  virtual bool printSearchGraph(const char* filename);
  virtual void printSearchGraphStream(std::ostream &outS)=0;
  virtual void printGraphForHyp(const Hypothesis& hyp,std::ostream &outS)=0;

      // Set verbosity level
  virtual void setVerbosity(int _verbosity)=0;

# ifdef THOT_STATS
  virtual void printStats(void);
# endif

      // Destructor
  virtual ~BaseStackDecoder(){};
};

//--------------- Template method definitions

//--------------- BaseStackDecoder template class method definitions

//---------------------------------------
template<class SMT_MODEL>
bool BaseStackDecoder<SMT_MODEL>::printSearchGraph(const char* filename)
{
  ofstream outS;

  outS.open(filename,ios::out);
  if(!outS)
  {
    std::cerr<<"Error while printing search graph to file."<<std::endl;
    return THOT_ERROR;
  }
  else
  {
    printSearchGraphStream(outS);
    outS.close();	
    return THOT_OK;
  }
}

//---------------------------------------
template<class SMT_MODEL>
void BaseStackDecoder<SMT_MODEL>::set_G_par(unsigned int /*G_par*/)
{
//  std::cerr<<"Warning: granularity parameter not available"<<std::endl;
}

//---------------------------------------
# ifdef THOT_STATS
template<class SMT_MODEL>
void BaseStackDecoder<SMT_MODEL>::printStats(void)
{
}
# endif

#endif
