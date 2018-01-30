/*
stack_dec package for statistical machine translation
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
 * @file PhraseBasedTmHyp.h
 * 
 * @brief Defines the PhraseBasedTmHyp class, this class is
 * derived from the _hypothesis class.
 */

#ifndef _PhraseBasedTmHyp_h
#define _PhraseBasedTmHyp_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

//--------------- Constants ------------------------------------------

#include "_phraseHypothesis.h"
#include "PhrScoreInfo.h"
#include "PhrHypEqClassF.h"

//--------------- Classes --------------------------------------------

//--------------- PhraseBasedTmHyp template class

/**
 * @brief The PhraseBasedTmHyp class is instantiated from the
 * _phraseHypothesis class.
 */

template<class EQCLASS_FUNC=PhrHypEqClassF>
class PhraseBasedTmHyp: public _phraseHypothesis<PhrScoreInfo,EQCLASS_FUNC>
{
 public:
  typedef typename _phraseHypothesis<PhrScoreInfo,EQCLASS_FUNC>::ScoreInfo ScoreInfo;
  typedef typename _phraseHypothesis<PhrScoreInfo,EQCLASS_FUNC>::DataType DataType;
  typedef typename _phraseHypothesis<PhrScoreInfo,EQCLASS_FUNC>::EqClassFunc EqClassFunc;
};

#endif
