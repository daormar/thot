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
        
      // Destructor
  ~PbTransModel();

 protected:

      // Misc. operations with hypothesis
  unsigned int numberOfUncoveredSrcWordsHypData(const HypDataType& hypd)const;

      // Scoring functions
  Score incrScore(const Hypothesis& prev_hyp,
                  const HypDataType& new_hypd,
                  Hypothesis& new_hyp,
                  Vector<Score>& scoreComponents);

      // Specific phrase-based functions
  void extendHypDataIdx(PositionIndex srcLeft,
                        PositionIndex srcRight,
                        const Vector<WordIndex>& trgPhraseIdx,
                        HypDataType& hypd);

};

//--------------- PbTransModel class functions
//

template<class EQCLASS_FUNC>
PbTransModel<EQCLASS_FUNC>::PbTransModel(void):_pbTransModel<PhraseBasedTmHypRec<EQCLASS_FUNC> >()
{
  this->featVec.push_back(new WordPenaltyFeat<EQCLASS_FUNC>);
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
  HypDataType dataType;

  dataType.ntarget.clear();  
  dataType.ntarget.push_back(NULL_WORD);
  dataType.sourceSegmentation.clear();  
  dataType.targetSegmentCuts.clear();

  return dataType;
}

//---------------------------------
template<class EQCLASS_FUNC>
bool PbTransModel<EQCLASS_FUNC>::obtainPredecessorHypData(HypDataType& hypd)
{
  HypDataType predData;

  predData=hypd;
      // verify if hyp has a predecessor
  if(predData.ntarget.size()<=1) return false;
  else
  {
    unsigned int i;
    unsigned int cuts;

    if(predData.targetSegmentCuts.size()==0)
    {
      cerr<<"Warning: hypothesis data corrupted"<<endl;
      return false;
    }

        // get previous ntarget
    cuts=predData.targetSegmentCuts.size();
    if(cuts==1)
    {
      i=predData.targetSegmentCuts[0];
    }
    else
    {
      i=predData.targetSegmentCuts[cuts-1]-predData.targetSegmentCuts[cuts-2];
    }
    while(i>0)
    {
      predData.ntarget.pop_back();
      --i;
    }
        // get previous sourceSegmentation
    predData.sourceSegmentation.pop_back();
        // get previous targetSegmentCuts
    predData.targetSegmentCuts.pop_back();
        // set data
    hypd=predData;
    
    return true;
  }  
}

//---------------------------------
template<class EQCLASS_FUNC>
bool PbTransModel<EQCLASS_FUNC>::isCompleteHypData(const HypDataType& hypd)const
{
  if(numberOfUncoveredSrcWordsHypData(hypd)==0) return true;
  else return false;  
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
  unsigned int n=0;
  for(unsigned int k=0;k<hypd.sourceSegmentation.size();k++)
	n+=hypd.sourceSegmentation[k].second-hypd.sourceSegmentation[k].first+1; 

  return (this->pbtmInputVars.srcSentVec.size()-n);    
}

//---------------------------------
template<class EQCLASS_FUNC>
void PbTransModel<EQCLASS_FUNC>::extendHypDataIdx(PositionIndex srcLeft,
                                                  PositionIndex srcRight,
                                                  const Vector<WordIndex>& trgPhraseIdx,
                                                  HypDataType& hypd)
{
  pair<PositionIndex,PositionIndex> sourceSegm;
  
        // Add trgPhraseIdx to the target vector
  for(unsigned int i=0;i<trgPhraseIdx.size();++i)
  {
    hypd.ntarget.push_back(trgPhraseIdx[i]);
  }
  
      // Add source segment and target cut
  sourceSegm.first=srcLeft;
  sourceSegm.second=srcRight;
  hypd.sourceSegmentation.push_back(sourceSegm);
  
  hypd.targetSegmentCuts.push_back(hypd.ntarget.size()-1);  
}

//---------------------------------
template<class EQCLASS_FUNC>
PbTransModel<EQCLASS_FUNC>::~PbTransModel()
{
  for(unsigned int i=0;i<this->featVec.size();++i)
    delete this->featVec[i];
}

//-------------------------

#endif
