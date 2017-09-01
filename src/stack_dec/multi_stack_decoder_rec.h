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
/* Module: multi_stack_decoder_rec                                  */
/*                                                                  */
/* Prototypes file: multi_stack_decoder_rec.h                       */
/*                                                                  */
/* Description: Declares the multi_stack_decoder_rec template       */
/*              class, this class is derived from the               */
/*              _stackDecoderRec class and implements a             */
/*              multiple-stack decoder with hypothesis              */
/*              recombination.                                      */
/*                                                                  */
/********************************************************************/

/**
 * @file multi_stack_decoder_rec.h
 *
 * @brief Declares the multi_stack_decoder_rec template class, this
 * class is derived from the _stackDecoderRec class and implements a
 * multiple-stack decoder with hypothesis recombination.
 */

#ifndef _multi_stack_decoder_rec_h
#define _multi_stack_decoder_rec_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "_stackDecoderRec.h"
#include "SmtMultiStackRec.h"

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------

/**
 * @brief The multi_stack_decoder_rec template class is derived from the
 * _stackDecoderRec class and implements a multiple-stack decoder with
 * hypothesis recombination.
 */

//--------------- multi_stack_decoder_rec template class

template<class SMT_MODEL>
class multi_stack_decoder_rec: public _stackDecoderRec<SMT_MODEL>
{
 public:

  typedef typename BaseStackDecoder<SMT_MODEL>::Hypothesis Hypothesis;
  
  multi_stack_decoder_rec(void); 
      // Constructor. 

      // Functions to report information about the search
  void printSearchGraphStream(std::ostream &outS);

      // Destructor
  ~multi_stack_decoder_rec();
  
 protected:

  void pre_trans_actions(void);
  void specific_pre_trans_actions(std::string srcsent);
  void specific_pre_trans_actions_ref(std::string srcsent,
                                      std::string refsent);
  void specific_pre_trans_actions_ver(std::string srcsent,
                                      std::string refsent);
  void specific_pre_trans_actions_prefix(std::string srcsent,
                                         std::string prefix);
      // Initialize additional variables for specific decoders
};

//--------------- multi_stack_decoder_rec template class function definitions


//---------------------------------------
template<class SMT_MODEL>
multi_stack_decoder_rec<SMT_MODEL>::multi_stack_decoder_rec(void):_stackDecoderRec<SMT_MODEL>()
{
  SmtMultiStackRec<Hypothesis>* smtMultiStackRecPtr;

      // Create stack container
  this->stack_ptr=new SmtMultiStackRec<Hypothesis>();
  
      // Link hypothesis state dictionary to the stack container
  smtMultiStackRecPtr=dynamic_cast<SmtMultiStackRec<Hypothesis>*>(this->stack_ptr);
  smtMultiStackRecPtr->setHypStateDictPtr(this->hypStateDictPtr);
}

//---------------------------------------
template<class SMT_MODEL>
void multi_stack_decoder_rec<SMT_MODEL>::printSearchGraphStream(std::ostream &outS)
{
  SmtMultiStackRec<Hypothesis>* smtMultiStackRecPtr;
  typename SmtMultiStackRec<Hypothesis>::iterator mStackIter;
  Hypothesis nullHyp=this->smtm_ptr->nullHypothesis();

  smtMultiStackRecPtr=dynamic_cast<SmtMultiStackRec<Hypothesis>*>(this->stack_ptr);

  outS<<"SrcLen= "<<StrProcUtils::stringToStringVector(this->srcSentence).size()<<std::endl;
  for(mStackIter=smtMultiStackRecPtr->begin();mStackIter!=smtMultiStackRecPtr->end();++mStackIter)
  {
    typename SmtStack<Hypothesis>::iterator stackIter;

    for(stackIter=mStackIter->second.begin();stackIter!=mStackIter->second.end();++stackIter)
    {
      Hypothesis hyp;

      hyp=*stackIter;
      outS<<"Stack ID. "<<mStackIter->first<<std::endl;
      this->smtm_ptr->subtractHeuristicToHyp(hyp);
      this->subtractgToHyp(hyp);
      this->printGraphForHyp(hyp,outS);
    }
  }
}

//---------------------------------------
template<class SMT_MODEL>
multi_stack_decoder_rec<SMT_MODEL>::~multi_stack_decoder_rec()
{
  delete this->stack_ptr;
}

//--------------- multi_stack_decoder_rec template class method definitions

//---------------------------------------
template<class SMT_MODEL>
void multi_stack_decoder_rec<SMT_MODEL>::specific_pre_trans_actions(std::string /*srcsent*/)
{

}

//---------------------------------------
template<class SMT_MODEL>
void multi_stack_decoder_rec<SMT_MODEL>::specific_pre_trans_actions_ref(std::string /*srcsent*/,
                                                                        std::string /*refsent*/)
{

}

//---------------------------------------
template<class SMT_MODEL>
void multi_stack_decoder_rec<SMT_MODEL>::specific_pre_trans_actions_ver(std::string /*srcsent*/,
                                                                        std::string /*refsent*/)
{

}

//---------------------------------------
template<class SMT_MODEL>
void multi_stack_decoder_rec<SMT_MODEL>::specific_pre_trans_actions_prefix(std::string /*srcsent*/,
                                                                           std::string /*prefix*/)
{

}

#endif
