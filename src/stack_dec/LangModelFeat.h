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
#include "WordPredictor.h"
#include "PhrScoreInfo.h"
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
  HypScoreInfo nullHypScore(const HypScoreInfo& predHypScrInf,
                            Score& unweightedScore);
  HypScoreInfo extensionScore(const std::vector<std::string>& srcSent,
                              const HypScoreInfo& predHypScrInf,
                              const PhrHypDataStr& predHypDataStr,
                              const PhrHypDataStr& newHypDataStr,
                              Score& unweightedScore);
  Score scorePhrasePairUnweighted(const std::vector<std::string>& srcPhrase,
                                  const std::vector<std::string>& trgPhrase);
  Score scoreTrgSentence(const std::vector<std::string>& trgSent,
                         std::vector<Score>& cumulativeScoreVec);

      // Word predictor related functions
  std::pair<Count,std::string> getBestSuffix(std::string input);
  std::pair<Count,std::string> getBestSuffixGivenHist(std::vector<std::string> hist,
                                                 std::string input);
  
      // Link pointers
  void link_lm(BaseNgramLM<LM_State>* _lModelPtr);
  BaseNgramLM<LM_State>* get_lmptr(void);
  void link_wp(WordPredictor* _wordPredPtr);
  
 protected:

  BaseNgramLM<LM_State>* lModelPtr;
  WordPredictor* wordPredPtr;
  
      // Functions to access language model parameters
  Score getEosScoreGivenState(LM_State& lmHist);
  Score getNgramScoreGivenState(std::vector<std::string> trgphrase,
                                LM_State& lmHist);
  void addWordSeqToStateStr(const std::vector<std::string>& trgPhrase,
                            LM_State& state);
  void addNextWordToStateStr(std::string word,
                             LM_State& state);

      // Auxiliary functions
  void obtainCurrPartialTrans(const PhrHypDataStr& predHypDataStr,
                              std::vector<std::string>& currPartialTrans);
  WordIndex stringToWordIndex(std::string str);
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
Score LangModelFeat<SCORE_INFO>::scorePhrasePairUnweighted(const std::vector<std::string>& /*srcPhrase*/,
                                                           const std::vector<std::string>& trgPhrase)
{
  std::vector<WordIndex> hist;
  LM_State state;    
  lModelPtr->getStateForWordSeq(hist,state);
  return getNgramScoreGivenState(trgPhrase,state);
}

//---------------------------------
template<class SCORE_INFO>
Score LangModelFeat<SCORE_INFO>::scoreTrgSentence(const std::vector<std::string>& trgSent,
                                                  std::vector<Score>& cumulativeScoreVec)
{
      // Initialize state
  LM_State state;
  lModelPtr->getStateForBeginOfSentence(state);

      // Obtain scores for words
  Score finalScr=0;
  cumulativeScoreVec.clear();
  for(unsigned int i=0;i<trgSent.size();++i)
  {
    std::vector<std::string> wordVec;
    wordVec.push_back(trgSent[i]);
    Score scr=this->weight*getNgramScoreGivenState(wordVec,state);
    finalScr+=scr;
    cumulativeScoreVec.push_back(finalScr);
  }

      // Obtain score for end of sentence
  Score scr=getEosScoreGivenState(state);
  finalScr+=scr;
  cumulativeScoreVec.push_back(finalScr);
  
  return finalScr;
}

//---------------------------------
template<class SCORE_INFO>
std::pair<Count,std::string>
LangModelFeat<SCORE_INFO>::getBestSuffix(std::string input)
{
  return wordPredPtr->getBestSuffix(input);
}

//---------------------------------
template<class SCORE_INFO>
std::pair<Count,std::string>
LangModelFeat<SCORE_INFO>::getBestSuffixGivenHist(std::vector<std::string> hist,
                                                  std::string input)
{
  WordPredictor::SuffixList suffixList;
  WordPredictor::SuffixList::iterator suffixListIter;
  LgProb lp;
  LgProb maxlp=-FLT_MAX;
  std::pair<Count,std::string> bestCountSuffix;

      // Get suffix list for input
  wordPredPtr->getSuffixList(input,suffixList);
  if(suffixList.size()==0)
  {
        // There are not any suffix
    return std::make_pair(0,"");
  }
  else
  {
        // There are one or more suffixes
    LM_State lmState;
    LM_State aux;

        // Initialize language model state given history
    lModelPtr->getStateForBeginOfSentence(lmState);
    for(unsigned int i=0;i<hist.size();++i)
    {
      lModelPtr->getNgramLgProbGivenState(lModelPtr->stringToWordIndex(hist[i]),lmState);
    }

        // Obtain probability for each suffix given history
    for(suffixListIter=suffixList.begin();suffixListIter!=suffixList.end();++suffixListIter)
    {
      std::string lastw;
      
      aux=lmState;
      lastw=input+suffixListIter->second;
      lp=lModelPtr->getNgramLgProbGivenState(lModelPtr->stringToWordIndex(lastw),aux);
      if(maxlp<lp)
      {
        bestCountSuffix.first=suffixListIter->first;
        bestCountSuffix.second=suffixListIter->second;
        maxlp=lp;
      }
    }
        // Return best suffix
    return bestCountSuffix;
  }
}

//---------------------------------
template<class SCORE_INFO>
void LangModelFeat<SCORE_INFO>::link_lm(BaseNgramLM<LM_State>* _lModelPtr)
{
  lModelPtr=_lModelPtr;
}

//---------------------------------
template<class SCORE_INFO>
BaseNgramLM<LM_State>* LangModelFeat<SCORE_INFO>::get_lmptr(void)
{
  return lModelPtr;
}

//---------------------------------
template<class SCORE_INFO>
void LangModelFeat<SCORE_INFO>::link_wp(WordPredictor* _wordPredPtr)
{
  wordPredPtr=_wordPredPtr;
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
Score LangModelFeat<SCORE_INFO>::getNgramScoreGivenState(std::vector<std::string> trgphrase,
                                                         LM_State& lmHist)
{
        // Score not present in cache table
  std::vector<WordIndex> trgPhraseIdx;
  Score result=0;

      // trgPhraseIdx stores the target sentence using indices of the language model
  for(unsigned int i=0;i<trgphrase.size();++i)
  {
    trgPhraseIdx.push_back(this->stringToWordIndex(trgphrase[i]));
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

//---------------------------------
template<class SCORE_INFO>
void LangModelFeat<SCORE_INFO>::obtainCurrPartialTrans(const PhrHypDataStr& predHypDataStr,
                                                       std::vector<std::string>& currPartialTrans)
{
      // Add current partial translation words
  currPartialTrans.clear();
  for(unsigned int i=1;i<predHypDataStr.ntarget.size();++i)
    currPartialTrans.push_back(predHypDataStr.ntarget[i]);
}

//---------------------------------
template<class SCORE_INFO>
void LangModelFeat<SCORE_INFO>::addWordSeqToStateStr(const std::vector<std::string>& trgPhrase,
                                                     LM_State& state)
{
  for(unsigned int i=0;i<trgPhrase.size();++i)
    addNextWordToStateStr(trgPhrase[i],state);
}

//---------------------------------
template<class SCORE_INFO>
void LangModelFeat<SCORE_INFO>::addNextWordToStateStr(std::string word,
                                                      LM_State& state)
{
  WordIndex wordIdx=this->stringToWordIndex(word);
  this->lModelPtr->addNextWordToState(wordIdx,state);
}

//---------------------------------
template<class SCORE_INFO>
WordIndex LangModelFeat<SCORE_INFO>::stringToWordIndex(std::string str)
{
  if(this->lModelPtr->existSymbol(str))
    return this->lModelPtr->stringToWordIndex(str);
  else
    return UNK_SYMBOL;
}

#endif
