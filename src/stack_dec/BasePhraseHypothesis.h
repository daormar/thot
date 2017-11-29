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
/* Module: BasePhraseHypothesis                                     */
/*                                                                  */
/* Prototypes file: BasePhraseHypothesis.h                          */
/*                                                                  */
/* Description: Declares the BasePhraseHypothesis abstract template */
/*              class this class is a base class for implementing   */
/*              different kinds of phrase-based hypotheses to be    */
/*              used in stack decoders.                             */
/*                                                                  */
/********************************************************************/

/**
 * @file BasePhraseHypothesis.h
 * 
 * @brief Declares the BasePhraseHypothesis abstract template class,
 * this class is a base class for implementing different kinds of
 * phrase-based hypotheses to be used in stack decoders.
 */

#ifndef _BasePhraseHypothesis_h
#define _BasePhraseHypothesis_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseHypothesis.h"
#include "SourceSegmentation.h"
#include "WordIndex.h"

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- BasePhraseHypothesis template class

/**
 * @brief The BasePhraseHypothesis abstract template class is a base
 * class for implementing different kinds of phrase-based hypotheses to
 * be used in stack decoders.
 */

template<class SCORE_INFO,class DATA_TYPE,class EQCLASS_FUNC>
class BasePhraseHypothesis: public BaseHypothesis<SCORE_INFO,DATA_TYPE,EQCLASS_FUNC>
{
 public:

  typedef typename BaseHypothesis<SCORE_INFO,DATA_TYPE,EQCLASS_FUNC>::ScoreInfo ScoreInfo;
  typedef typename BaseHypothesis<SCORE_INFO,DATA_TYPE,EQCLASS_FUNC>::DataType DataType;
  typedef typename BaseHypothesis<SCORE_INFO,DATA_TYPE,EQCLASS_FUNC>::EqClassFunc EqClassFunc;

      // Specific functions
  virtual bool isAligned(PositionIndex srcPos)const=0;
  virtual bool areAligned(PositionIndex srcPos,PositionIndex trgPos)const=0;
  virtual void getPhraseAlign(SourceSegmentation& sourceSegmentation,
                              std::vector<PositionIndex>& targetSegmentCuts)const=0;
  virtual Bitset<MAX_SENTENCE_LENGTH_ALLOWED> getKey(void)const=0;
  virtual std::vector<WordIndex> getPartialTrans(void)const=0;
  virtual unsigned int partialTransLength(void)const=0;

      // Destructor
  virtual ~BasePhraseHypothesis(){};
};

#endif
