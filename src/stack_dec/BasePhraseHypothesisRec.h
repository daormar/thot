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
/* Module: BasePhraseHypothesisRec                                  */
/*                                                                  */
/* Prototypes file: BasePhraseHypothesisRec.h                       */
/*                                                                  */
/* Description: Declares the BasePhraseHypothesisRec abstract       */
/*              template class, this class is a base class for      */
/*              implementing different kinds of phrase-based        */
/*              hypotheses to be used in stack decoders. "Rec"      */
/*              stands for recombination.                           */
/*                                                                  */
/********************************************************************/

/**
 * @file BasePhraseHypothesisRec.h
 * 
 * @brief Declares the BasePhraseHypothesisRec abstract template class,
 * this class is a base class for implementing different kinds of
 * phrase-based hypotheses to be used in stack decoders. "Rec" stands
 * for recombination.
 */

#ifndef _BasePhraseHypothesisRec_h
#define _BasePhraseHypothesisRec_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseHypothesisRec.h"
#include "WordIndex.h"

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- BasePhraseHypothesisRec template class

/**
 * @brief The BasePhraseHypothesisRec abstract template class is a base
 * class for implementing different kinds of phrase-based hypotheses to
 * be used in stack decoders. "Rec" stands for recombination.
 */

template<class SCORE_INFO,class DATA_TYPE,class EQCLASS_FUNC,class HYPSTATE>
class BasePhraseHypothesisRec: public BaseHypothesisRec<SCORE_INFO,DATA_TYPE,EQCLASS_FUNC,HYPSTATE>
{
 public:

  typedef typename BaseHypothesisRec<SCORE_INFO,DATA_TYPE,EQCLASS_FUNC,HYPSTATE>::ScoreInfo ScoreInfo;  
  typedef typename BaseHypothesisRec<SCORE_INFO,DATA_TYPE,EQCLASS_FUNC,HYPSTATE>::DataType DataType;
  typedef typename BaseHypothesisRec<SCORE_INFO,DATA_TYPE,EQCLASS_FUNC,HYPSTATE>::EqClassFunc EqClassFunc;
  typedef typename BaseHypothesisRec<SCORE_INFO,DATA_TYPE,EQCLASS_FUNC,HYPSTATE>::HypState HypState;

      // Specific functions
  virtual bool isAligned(PositionIndex i)const=0;
  virtual bool areAligned(PositionIndex i,PositionIndex j)const=0;
  virtual Bitset<MAX_SENTENCE_LENGTH_ALLOWED> getKey(void)const=0;
  virtual Vector<WordIndex> getPartialTrans(void)const=0;
  virtual unsigned int partialTransLength(void)const=0;

      // Destructor
  virtual ~BasePhraseHypothesisRec(){};
};

#endif
