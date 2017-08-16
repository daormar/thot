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
/* Module: InversePhraseModelFeat                                   */
/*                                                                  */
/* Prototypes file: InversePhraseModelFeat.h                        */
/*                                                                  */
/* Description: Declares the InversePhraseModelFeat template        */
/*              class. This class implements a inverse phrase model */
/*              feature.                                            */
/*                                                                  */
/********************************************************************/

/**
 * @file InversePhraseModelFeat.h
 * 
 * @brief Declares the InversePhraseModelFeat template class. This class
 * implements a inverse phrase model feature.
 */

#ifndef _InversePhraseModelFeat_h
#define _InversePhraseModelFeat_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include THOT_PPINFO_H // Define PpInfo type. It is set in
                       // configure by checking PPINFO_H variable
                       // (default value: PpInfo.h)
#include "BaseSwAligModel.h"
#include "BasePhraseModel.h"
#include "PhraseBasedTmHypRec.h"
#include "BasePbTransModelFeature.h"

//--------------- Constants ------------------------------------------

#define INVERSE_PM_FEAT_DEFAULT_LAMBDA 0.01

//--------------- Classes --------------------------------------------

//--------------- InversePhraseModelFeat class

/**
 * @brief The InversePhraseModelFeat template class is a base class for
 * implementing a inverse phrase model feature.
 */

template<class SCORE_INFO>
class InversePhraseModelFeat: public BasePbTransModelFeature<SCORE_INFO>
{
 public:

  typedef typename BasePbTransModelFeature<SCORE_INFO>::HypScoreInfo HypScoreInfo;

      // Constructor
  InversePhraseModelFeat();

      // Feature information
  std::string getFeatType(void);

      // Scoring functions
  HypScoreInfo extensionScore(const Vector<std::string>& srcSent,
                              const HypScoreInfo& predHypScrInf,
                              const PhrHypDataStr& predHypDataStr,
                              const PhrHypDataStr& newHypDataStr,
                              Score& unweightedScore);
  Score scorePhrasePairUnweighted(const Vector<std::string>& srcPhrase,
                        const Vector<std::string>& trgPhrase);

      // Functions to obtain translation options
  void obtainTransOptions(const Vector<std::string>& wordVec,
                          Vector<Vector<std::string> >& transOptVec);

      // Functions related to model pointers
  void link_pm(BasePhraseModel* _invPbModelPtr);
  BasePhraseModel* get_pmptr(void);
  void link_swm(BaseSwAligModel<PpInfo>* _invSwAligModelPtr);
  BaseSwAligModel<PpInfo>* get_swmptr(void);

      // Functions related to lambda parameter
  void set_lambda(float _lambda);
  float get_lambda(void);
  
 protected:

  BasePhraseModel* invPbModelPtr;
  BaseSwAligModel<PpInfo>* invSwAligModelPtr;
  float lambda;
  
  Score inversePhrTransUnweightedScore(const Vector<WordIndex>& srcPhrase,
                                       const Vector<WordIndex>& trgPhrase);
  Score invSwLgProb(const Vector<WordIndex>& srcPhraseWidx,
                    const Vector<WordIndex>& trgPhraseWidx);
  WordIndex stringToSrcWordindex(std::string word);
  std::string wordindexToSrcString(WordIndex wordIdx);
  WordIndex stringToTrgWordindex(std::string word);
  std::string wordindexToTrgString(WordIndex wordIdx);
};

//--------------- WordPenaltyFeat class functions
//

template<class SCORE_INFO>
InversePhraseModelFeat<SCORE_INFO>::InversePhraseModelFeat()
{
  this->weight=1.0;
  this->lambda=INVERSE_PM_FEAT_DEFAULT_LAMBDA;
}

//---------------------------------
template<class SCORE_INFO>
std::string InversePhraseModelFeat<SCORE_INFO>::getFeatType(void)
{
  return "InversePhraseModelFeat";
}

//---------------------------------
template<class SCORE_INFO>
Score InversePhraseModelFeat<SCORE_INFO>::scorePhrasePairUnweighted(const Vector<std::string>& srcPhrase,
                                                                    const Vector<std::string>& trgPhrase)
{
      // Obtain WordIndex vectors
  Vector<WordIndex> srcPhraseIdx;
  for(unsigned int i=0;i<srcPhrase.size();++i)
    srcPhraseIdx.push_back(this->stringToSrcWordindex(srcPhrase[i]));

  Vector<WordIndex> trgPhraseIdx;
  for(unsigned int i=0;i<trgPhrase.size();++i)
    trgPhraseIdx.push_back(this->stringToTrgWordindex(trgPhrase[i]));

  return inversePhrTransUnweightedScore(srcPhraseIdx,trgPhraseIdx);
}

//---------------------------------
template<class SCORE_INFO>
void InversePhraseModelFeat<SCORE_INFO>::obtainTransOptions(const Vector<std::string>& /*wordVec*/,
                                                           Vector<Vector<std::string> >& transOptVec)
{
  transOptVec.clear();
}

//---------------------------------
template<class SCORE_INFO>
void InversePhraseModelFeat<SCORE_INFO>::link_pm(BasePhraseModel* _invPbModelPtr)
{
  invPbModelPtr=_invPbModelPtr;
}

//---------------------------------
template<class SCORE_INFO>
BasePhraseModel* InversePhraseModelFeat<SCORE_INFO>::get_pmptr(void)
{
  return invPbModelPtr;
}

//---------------------------------
template<class SCORE_INFO>
void InversePhraseModelFeat<SCORE_INFO>::link_swm(BaseSwAligModel<PpInfo>* _invSwAligModelPtr)
{
  invSwAligModelPtr=_invSwAligModelPtr;
}

//---------------------------------
template<class SCORE_INFO>
BaseSwAligModel<PpInfo>* InversePhraseModelFeat<SCORE_INFO>::get_swmptr(void)
{
  return invSwAligModelPtr;
}

//---------------------------------
template<class SCORE_INFO>
void InversePhraseModelFeat<SCORE_INFO>::set_lambda(float _lambda)
{
  lambda=_lambda;
}

//---------------------------------
template<class SCORE_INFO>
float InversePhraseModelFeat<SCORE_INFO>::get_lambda(void)
{
  return lambda;
}

//---------------------------------
template<class SCORE_INFO>
Score InversePhraseModelFeat<SCORE_INFO>::inversePhrTransUnweightedScore(const Vector<WordIndex>& srcPhrase,
                                                                         const Vector<WordIndex>& trgPhrase)
{
  if(lambda==1.0)
  {
    return (double)invPbModelPtr->logpt_s_(trgPhrase,srcPhrase);
  }
  else
  {
    float sum1=log(lambda)+(float)invPbModelPtr->logpt_s_(trgPhrase,srcPhrase);
    if(sum1<=log(PHRASE_PROB_SMOOTH))
      sum1=FEAT_LGPROB_SMOOTH;
    float sum2=log(1.0-lambda)+(float)invSwLgProb(srcPhrase,trgPhrase);
    float interp=MathFuncs::lns_sumlog(sum1,sum2);
    return (double)interp;
  }
}

//---------------------------------
template<class SCORE_INFO>
Score InversePhraseModelFeat<SCORE_INFO>::invSwLgProb(const Vector<WordIndex>& srcPhrase,
                                                      const Vector<WordIndex>& trgPhrase)
{
  return invSwAligModelPtr->calcLgProbPhr(trgPhrase,srcPhrase);
}

//---------------------------------
template<class SCORE_INFO>
WordIndex InversePhraseModelFeat<SCORE_INFO>::stringToSrcWordindex(std::string word)
{
  return invPbModelPtr->stringToTrgWordIndex(word);
}

//---------------------------------
template<class SCORE_INFO>
std::string InversePhraseModelFeat<SCORE_INFO>::wordindexToSrcString(WordIndex wordIdx)
{
  return invPbModelPtr->wordIndexToTrgString(wordIdx);
}

//---------------------------------
template<class SCORE_INFO>
WordIndex InversePhraseModelFeat<SCORE_INFO>::stringToTrgWordindex(std::string word)
{
  return invPbModelPtr->stringToSrcWordIndex(word);  
}

//---------------------------------
template<class SCORE_INFO>
std::string InversePhraseModelFeat<SCORE_INFO>::wordindexToTrgString(WordIndex wordIdx)
{
  return invPbModelPtr->wordIndexToSrcString(wordIdx);  
}

#endif
