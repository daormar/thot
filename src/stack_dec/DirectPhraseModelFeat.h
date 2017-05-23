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
/* Module: DirectPhraseModelFeat                                    */
/*                                                                  */
/* Prototypes file: DirectPhraseModelFeat.h                         */
/*                                                                  */
/* Description: Declares the DirectPhraseModelFeat template         */
/*              class. This class implements a direct phrase model  */
/*              feature.                                            */
/*                                                                  */
/********************************************************************/

/**
 * @file DirectPhraseModelFeat.h
 * 
 * @brief Declares the DirectPhraseModelFeat template class. This class
 * implements a direct phrase model feature.
 */

#ifndef _DirectPhraseModelFeat_h
#define _DirectPhraseModelFeat_h

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

#define DIRECT_PM_FEAT_DEFAULT_LAMBDA 0.01

//--------------- Classes --------------------------------------------

//--------------- DirectPhraseModelFeat class

/**
 * @brief The DirectPhraseModelFeat template class is a base class for
 * implementing a direct phrase model feature.
 */

template<class SCORE_INFO>
class DirectPhraseModelFeat: public BasePbTransModelFeature<SCORE_INFO>
{
 public:

  typedef typename BasePbTransModelFeature<SCORE_INFO>::HypScoreInfo HypScoreInfo;

      // Constructor
  DirectPhraseModelFeat();

      // Feature information
  std::string getFeatType(void);

      // Scoring functions
  HypScoreInfo extensionScore(const Vector<std::string>& srcSent,
                              const HypScoreInfo& predHypScrInf,
                              const PhrHypDataStr& predHypDataStr,
                              const PhrHypDataStr& newHypDataStr,
                              Score& unweightedScore);

      // Functions to obtain translation options
  void obtainTransOptions(const Vector<std::string>& wordVec,
                          Vector<Vector<std::string> >& transOptVec);

      // Functions related to model pointers
  void link_pm(BasePhraseModel* _invPbModelPtr);
  BasePhraseModel* get_pmptr(void);
  void link_swm(BaseSwAligModel<PpInfo>* _swAligModelPtr);
  BaseSwAligModel<PpInfo>* get_swmptr(void);

      // Functions related to lambda parameter
  void set_lambda(float _lambda);
  float get_lambda(void);
  
 protected:

  BasePhraseModel* invPbModelPtr;
  BaseSwAligModel<PpInfo>* swAligModelPtr;
  float lambda;
  
  Score directPhraseTransScore(const Vector<WordIndex>& srcPhrase,
                               const Vector<WordIndex>& trgPhrase);
  Score swLgProb(const Vector<WordIndex>& srcPhraseWidx,
                 const Vector<WordIndex>& trgPhraseWidx);
  WordIndex stringToSrcWordindex(std::string word);
  std::string wordindexToSrcString(WordIndex wordIdx);
  WordIndex stringToTrgWordindex(std::string word);
  std::string wordindexToTrgString(WordIndex wordIdx);
};

//--------------- WordPenaltyFeat class functions
//

template<class SCORE_INFO>
DirectPhraseModelFeat<SCORE_INFO>::DirectPhraseModelFeat()
{
  this->weight=1.0;
  this->lambda=DIRECT_PM_FEAT_DEFAULT_LAMBDA;
}

//---------------------------------
template<class SCORE_INFO>
std::string DirectPhraseModelFeat<SCORE_INFO>::getFeatType(void)
{
  return "DirectPhraseModelFeat";
}

//---------------------------------
template<class SCORE_INFO>
void DirectPhraseModelFeat<SCORE_INFO>::obtainTransOptions(const Vector<std::string>& wordVec,
                                                           Vector<Vector<std::string> >& transOptVec)
{
      // Obtain vector of word indices
  Vector<WordIndex> wordIdxVec;
  for(unsigned int i=0;i<wordVec.size();++i)
    wordIdxVec.push_back(this->stringToSrcWordindex(wordVec[i]));

      // Obtain translation options
  BasePhraseModel::SrcTableNode srctn;
  this->invPbModelPtr->getTransFor_t_(wordIdxVec,srctn);

      // Put options in vector
  transOptVec.clear();
  for(BasePhraseModel::SrcTableNode::iterator iter=srctn.begin(); iter!=srctn.end(); ++iter)
  {
        // Convert option to string vector
    Vector<std::string> transOpt;
    for(unsigned int i=0;i<iter->first.size();++i)
      transOpt.push_back(this->wordindexToTrgString(iter->first[i]));
    
        // Add new entry
    transOptVec.push_back(transOpt);
  }
}

//---------------------------------
template<class SCORE_INFO>
void DirectPhraseModelFeat<SCORE_INFO>::link_pm(BasePhraseModel* _invPbModelPtr)
{
  invPbModelPtr=_invPbModelPtr;
}

//---------------------------------
template<class SCORE_INFO>
BasePhraseModel* DirectPhraseModelFeat<SCORE_INFO>::get_pmptr(void)
{
  return invPbModelPtr;
}

//---------------------------------
template<class SCORE_INFO>
void DirectPhraseModelFeat<SCORE_INFO>::link_swm(BaseSwAligModel<PpInfo>* _swAligModelPtr)
{
  swAligModelPtr=_swAligModelPtr;
}

//---------------------------------
template<class SCORE_INFO>
BaseSwAligModel<PpInfo>* DirectPhraseModelFeat<SCORE_INFO>::get_swmptr(void)
{
  return swAligModelPtr;
}

//---------------------------------
template<class SCORE_INFO>
void DirectPhraseModelFeat<SCORE_INFO>::set_lambda(float _lambda)
{
  lambda=_lambda;
}

//---------------------------------
template<class SCORE_INFO>
float DirectPhraseModelFeat<SCORE_INFO>::get_lambda(void)
{
  return lambda;
}

//---------------------------------
template<class SCORE_INFO>
Score DirectPhraseModelFeat<SCORE_INFO>::directPhraseTransScore(const Vector<WordIndex>& srcPhrase,
                                                                const Vector<WordIndex>& trgPhrase)
{
  if(lambda==1.0)
  {
    return (double)invPbModelPtr->logps_t_(trgPhrase,srcPhrase);
  }
  else
  {
    float sum1=log(lambda)+(float)invPbModelPtr->logps_t_(trgPhrase,srcPhrase);
    if(sum1<=log(PHRASE_PROB_SMOOTH))
      sum1=FEAT_LGPROB_SMOOTH;
    float sum2=log(1.0-lambda)+(float)swLgProb(srcPhrase,trgPhrase);
    float interp=MathFuncs::lns_sumlog(sum1,sum2);
    return (double)interp;
  }
}

//---------------------------------
template<class SCORE_INFO>
Score DirectPhraseModelFeat<SCORE_INFO>::swLgProb(const Vector<WordIndex>& srcPhrase,
                                                  const Vector<WordIndex>& trgPhrase)
{
  return swAligModelPtr->calcLgProbPhr(srcPhrase,trgPhrase);
}

//---------------------------------
template<class SCORE_INFO>
WordIndex DirectPhraseModelFeat<SCORE_INFO>::stringToSrcWordindex(std::string word)
{
  return invPbModelPtr->stringToTrgWordIndex(word);
}

//---------------------------------
template<class SCORE_INFO>
std::string DirectPhraseModelFeat<SCORE_INFO>::wordindexToSrcString(WordIndex wordIdx)
{
  return invPbModelPtr->wordIndexToTrgString(wordIdx);
}

//---------------------------------
template<class SCORE_INFO>
WordIndex DirectPhraseModelFeat<SCORE_INFO>::stringToTrgWordindex(std::string word)
{
  return invPbModelPtr->stringToSrcWordIndex(word);  
}

//---------------------------------
template<class SCORE_INFO>
std::string DirectPhraseModelFeat<SCORE_INFO>::wordindexToTrgString(WordIndex wordIdx)
{
  return invPbModelPtr->wordIndexToSrcString(wordIdx);  
}

#endif
