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
/* Module: PbTransModel                                             */
/*                                                                  */
/* Prototypes file: PbTransModel.h                                  */
/*                                                                  */
/* Description: Declares the PbTransModel class.                    */
/*              This class is a succesor of the BasePbTransModel    */
/*              class.                                              */
/*                                                                  */
/********************************************************************/

/**
 * @file PbTransModel.h
 * 
 * @brief Declares the PbTransModel class.  This class is a
 * succesor of the _pbTransModel class.
 */

#ifndef _PbTransModel_h
#define _PbTransModel_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "DirectPhraseModelFeat.h"
#include "WordPenaltyFeat.h"
#include "_pbTransModel.h"

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- PbTransModel class

/**
 * @brief The PbTransModel class is a predecessor of the
 * _pbTransModel class.
 */

template<class EQCLASS_FUNC>
class PbTransModel: public _pbTransModel<PhraseBasedTmHypRec<EQCLASS_FUNC> >
{
 public:

  typedef typename _pbTransModel<PhraseBasedTmHypRec<EQCLASS_FUNC> >::Hypothesis Hypothesis;
  typedef typename _pbTransModel<PhraseBasedTmHypRec<EQCLASS_FUNC> >::HypScoreInfo HypScoreInfo;
  typedef typename _pbTransModel<PhraseBasedTmHypRec<EQCLASS_FUNC> >::HypDataType HypDataType;

      // Constructor
  PbTransModel();

      // Virtual object copy
  BaseSmtModel<PhraseBasedTmHypRec<EQCLASS_FUNC> >* clone(void);

      // Misc. operations with hypotheses
  Hypothesis nullHypothesis(void);
  HypDataType nullHypothesisHypData(void);
  bool obtainPredecessorHypData(HypDataType& hypd);
  bool isCompleteHypData(const HypDataType& hypd)const;
        
      // Specific phrase-based functions
  void extendHypData(PositionIndex srcLeft,
                     PositionIndex srcRight,
                     const Vector<std::string>& trgPhrase,
                     HypDataType& hypd);

      // Destructor
  ~PbTransModel();

 protected:
  
      // Scoring functions
  Score incrScore(const Hypothesis& prev_hyp,
                  const HypDataType& new_hypd,
                  Hypothesis& new_hyp,
                  Vector<Score>& scoreComponents);

      // Misc. operations with hypothesis
  unsigned int numberOfUncoveredSrcWordsHypData(const HypDataType& hypd)const;

};

//--------------- PbTransModel class functions
//

template<class EQCLASS_FUNC>
PbTransModel<EQCLASS_FUNC>::PbTransModel(void):_pbTransModel<PhraseBasedTmHypRec<EQCLASS_FUNC> >()
{
}

//---------------------------------
template<class EQCLASS_FUNC>
BaseSmtModel<PhraseBasedTmHypRec<EQCLASS_FUNC> >* PbTransModel<EQCLASS_FUNC>::clone(void)
{
  return new PbTransModel<EQCLASS_FUNC>(*this);
}

//---------------------------------
template<class EQCLASS_FUNC>
typename PbTransModel<EQCLASS_FUNC>::Hypothesis
PbTransModel<EQCLASS_FUNC>::nullHypothesis(void)
{
  
}

//---------------------------------
template<class EQCLASS_FUNC>
typename PbTransModel<EQCLASS_FUNC>::HypDataType
PbTransModel<EQCLASS_FUNC>::nullHypothesisHypData(void)
{
  
}

//---------------------------------
template<class EQCLASS_FUNC>
bool PbTransModel<EQCLASS_FUNC>::obtainPredecessorHypData(HypDataType& hypd)
{
  
}

//---------------------------------
template<class EQCLASS_FUNC>
bool PbTransModel<EQCLASS_FUNC>::isCompleteHypData(const HypDataType& hypd)const
{
  
}


//---------------------------------
template<class EQCLASS_FUNC>
void PbTransModel<EQCLASS_FUNC>::extendHypData(PositionIndex srcLeft,
                                               PositionIndex srcRight,
                                               const Vector<std::string>& trgPhrase,
                                               HypDataType& hypd)
{
      // TO-BE-DONE
}

//---------------------------------
template<class EQCLASS_FUNC>
Score PbTransModel<EQCLASS_FUNC>::incrScore(const Hypothesis& prev_hyp,
                                            const HypDataType& new_hypd,
                                            Hypothesis& new_hyp,
                                            Vector<Score>& scoreComponents)
{
  
}

//---------------------------------
template<class EQCLASS_FUNC>
unsigned int PbTransModel<EQCLASS_FUNC>::numberOfUncoveredSrcWordsHypData(const HypDataType& hypd)const
{
  
}

//---------------------------------
template<class EQCLASS_FUNC>
PbTransModel<EQCLASS_FUNC>::~PbTransModel()
{
  
}

//-------------------------

#endif
