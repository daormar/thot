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
/* Module: LangModelFeat                                            */
/*                                                                  */
/* Prototypes file: LangModelFeat.h                                 */
/*                                                                  */
/* Description: Declares the LangModelFeat template                 */
/*              class. This class implements a language model       */
/*              feature.                                            */
/*                                                                  */
/********************************************************************/

/**
 * @file LangModelFeat.h
 * 
 * @brief Declares the LangModelFeat template class. This class
 * implements a language model feature.
 */

#ifndef _LangModelFeat_h
#define _LangModelFeat_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include THOT_LM_STATE_H // Define LM_State type. It is set in
                         // configure by checking LM_STATE_H
                         // variable (default value: LM_State.h)
#include "BaseNgramLM.h"
#include "PhraseBasedTmHypRec.h"
#include "BasePbTransModelFeature.h"

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- LangModelFeat class

/**
 * @brief The LangModelFeat template class is a base class for
 * implementing a language model feature.
 */

template<class SCORE_INFO>
class LangModelFeat: public BasePbTransModelFeature<SCORE_INFO>
{
 public:

  typedef typename BasePbTransModelFeature<SCORE_INFO>::HypScoreInfo HypScoreInfo;

      // Constructor
  LangModelFeat();
  
      // Feature information
  std::string getFeatType(void);

      // Scoring functions
  HypScoreInfo extensionScore(const Vector<std::string>& srcSent,
                              const HypScoreInfo& predHypScrInf,
                              const PhrHypDataStr& predHypDataStr,
                              const PhrHypDataStr& newHypDataStr,
                              Score& unweightedScore);

      // Link pointer
  void link_lm(BaseNgramLM<LM_State>* _lModelPtr);

 protected:

  BaseNgramLM<LM_State>* lModelPtr;

  Score getEosScoreGivenState(LM_State& lmHist);
  Score getNgramScoreGivenState(Vector<std::string> trgphrase,
                                LM_State& lmHist);

};

//--------------- WordPenaltyFeat class functions
//

template<class SCORE_INFO>
LangModelFeat<SCORE_INFO>::LangModelFeat()
{
  this->weight=1.0;
}

//---------------------------------
template<class SCORE_INFO>
std::string LangModelFeat<SCORE_INFO>::getFeatType(void)
{
  return "LangModelFeat";
}

//---------------------------------
template<class SCORE_INFO>
void LangModelFeat<SCORE_INFO>::link_lm(BaseNgramLM<LM_State>* _lModelPtr)
{
  lModelPtr=_lModelPtr;
}

//---------------------------------
template<class SCORE_INFO>
Score LangModelFeat<SCORE_INFO>::getEosScoreGivenState(LM_State& lmHist)
{
#ifdef WORK_WITH_ZERO_GRAM_PROB
  return this->lModelPtr->getZeroGramProb());
#else
  return this->lModelPtr->getLgProbEndGivenState(lmHist);
#endif
}

//---------------------------------
template<class SCORE_INFO>
Score LangModelFeat<SCORE_INFO>::getNgramScoreGivenState(Vector<std::string> trgphrase,
                                                         LM_State& lmHist)
{
        // Score not present in cache table
  Vector<WordIndex> trgPhraseIdx;
  Score result=0;

      // trgPhraseIdx stores the target sentence using indices of the language model
  for(unsigned int i=0;i<trgphrase.size();++i)
  {
    trgPhraseIdx.push_back(this->lModelPtr->stringToWordIndex(trgphrase[i]));
  }
      
  for(unsigned int i=0;i<trgPhraseIdx.size();++i)
  {
#ifdef WORK_WITH_ZERO_GRAM_PROB
      Score scr=this->lModelPtr->getZeroGramProb();
#else
      Score scr=this->lModelPtr->getNgramLgProbGivenState(trgPhraseIdx[i],lmHist);
#endif
          // Increase score
      result+=scr;
  }
      // Return result
  return result;
}

#endif
