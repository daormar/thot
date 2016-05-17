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
/* Module: BaseHypothesisRec                                        */
/*                                                                  */
/* Prototypes file: BaseHypothesisRec.h                             */
/*                                                                  */
/* Description: Declares the BaseHypothesisRec abstract template    */
/*              class, this class is a base class for implementing  */
/*              different kinds of hypotheses to be used            */
/*              in stack decoders. "Rec" stands for recombination.  */
/*                                                                  */
/********************************************************************/

/**
 * @file BaseHypothesisRec.h
 * 
 * @brief Declares the BaseHypothesisRec abstract template class, this
 * class is a base class for implementing different kinds of hypotheses
 * to be used in stack decoders. "Rec" stands for recombination.
 */

#ifndef _BaseHypothesisRec_h
#define _BaseHypothesisRec_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseHypothesis.h"

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- BaseHypothesisRec template class

/**
 * @brief The BaseHypothesisRec abstract template class is a base class
 * for implementing different kinds of hypotheses to be used in stack
 * decoders. "Rec" stands for recombination.
 */

template<class SCORE_INFO,class DATA_TYPE,class EQCLASS_FUNC,class HYPSTATE>
class BaseHypothesisRec: public BaseHypothesis<SCORE_INFO,DATA_TYPE,EQCLASS_FUNC>
{
 public:

  typedef typename BaseHypothesis<SCORE_INFO,DATA_TYPE,EQCLASS_FUNC>::ScoreInfo ScoreInfo;
  typedef typename BaseHypothesis<SCORE_INFO,DATA_TYPE,EQCLASS_FUNC>::DataType DataType;
  typedef typename BaseHypothesis<SCORE_INFO,DATA_TYPE,EQCLASS_FUNC>::EqClassFunc EqClassFunc;

  typedef HYPSTATE HypState;

      // Basic functions
  virtual Score getScore(void)const=0;
  virtual void setScoreInfo(const ScoreInfo& ScInf)=0;
  virtual ScoreInfo getScoreInfo(void)const=0;
  virtual void addHeuristic(Score h)=0;
  virtual void subtractHeuristic(Score h)=0;
  virtual DATA_TYPE getData(void)const=0;
  virtual void setData(const DATA_TYPE& _data)=0;
  virtual Bitset<MAX_SENTENCE_LENGTH_ALLOWED> getKey(void)const=0;
      // Returns coverage vector for the hypothesis. This function is
      // required when using multiple stack translators with granularity

      // Hypothesis to state mapping
  virtual HypState getHypState(void)const=0;

      // Destructor
  virtual ~BaseHypothesisRec(){};
};

#endif
