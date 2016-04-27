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
/* Module: _phrSwTransModel                                         */
/*                                                                  */
/* Prototypes file: _phrSwTransModel.h                              */
/*                                                                  */
/* Description: Declares the _phrSwTransModel class.                */
/*              This class is a base class for deriving translation */
/*              models that combine phrase based models and sw      */
/*              models.                                             */
/*                                                                  */
/********************************************************************/

/**
 * @file _phrSwTransModel.h
 *
 * @brief Declares the _phrSwTransModel class.  This class is a base
 * class for deriving translation models that combine phrase based
 * models and single word models.
 */

#ifndef __phrSwTransModel_h
#define __phrSwTransModel_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "_phraseBasedTransModel.h"
#include "SwModelInfo.h"

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------

typedef pair<unsigned int,unsigned int> uint_pair;

//--------------- Classes --------------------------------------------

//--------------- _phrSwTransModel class

/**
 * @brief The _phrSwTransModel class is a base class for deriving
 * translation models that combine phrase based models and single-word
 * models.
 */

template<class HYPOTHESIS>
class _phrSwTransModel: public _phraseBasedTransModel<HYPOTHESIS>
{
 public:

  typedef typename _phraseBasedTransModel<HYPOTHESIS>::Hypothesis Hypothesis;
  typedef typename _phraseBasedTransModel<HYPOTHESIS>::HypScoreInfo HypScoreInfo;
  typedef typename _phraseBasedTransModel<HYPOTHESIS>::HypDataType HypDataType;

  // class functions
  
      // Constructor
  _phrSwTransModel(void);

      // Link sw model information
  void link_swm_info(SwModelInfo* _swModelInfoPtr);

      // Init alignment model
  bool loadAligModel(const char* prefixFileName);

      // Print models
  bool printAligModel(std::string printPrefix);

  void clear(void);

      // Actions to be executed before the translation
  void pre_trans_actions(std::string srcsent);
  void pre_trans_actions_ref(std::string srcsent,
                             std::string refsent);
  void pre_trans_actions_ver(std::string srcsent,
                             std::string refsent);
  void pre_trans_actions_prefix(std::string srcsent,
                                std::string prefix);

  ////// Hypotheses-related functions

      // Destructor
  ~_phrSwTransModel();

 protected:

      // SwModelInfo pointer
  SwModelInfo* swModelInfoPtr;

      // Precalculated lgProbs
  Vector<Vector<Prob> > sumSentLenProbVec;
      // sumSentLenProbVec[slen][tlen] stores p(sl=slen|tl<=tlen)
  Vector<Vector<uint_pair> > lenRangeForGaps; 

      // Cached scores
  PhrasePairCacheTable cSwmScore;
  PhrasePairCacheTable cInvSwmScore;
  
  Score invSwScore(const Vector<WordIndex>& s_,
                   const Vector<WordIndex>& t_);
  Score swScore(const Vector<WordIndex>& s_,
                const Vector<WordIndex>& t_);
  LgProb swLgProb(const Vector<WordIndex>& s_,
                  const Vector<WordIndex>& t_);
  LgProb invSwLgProb(const Vector<WordIndex>& s_,
                     const Vector<WordIndex>& t_);
  
      // Sentence length scoring functions
  Score sentLenScore(unsigned int slen,unsigned int tlen);
  Score sentLenScoreForPartialHyp(Bitset<MAX_SENTENCE_LENGTH_ALLOWED> key,
                                  unsigned int curr_tlen);
  Prob sumSentLenProb(unsigned int slen,unsigned int tlen);
      // Returns p(sl=slen|tl<=tlen)
  Score sumSentLenScoreRange(unsigned int slen,
                             uint_pair range);
      // Returns p(sl=slen|tl\in range)
  uint_pair obtainLengthRangeForGaps(const Bitset<MAX_SENTENCE_LENGTH_ALLOWED>& hypKey);
  void initLenRangeForGapsVec(int maxSrcPhraseLength);
  
      // Functions related to pre_trans_actions
  void clearTempVars(void);

      // Vocabulary-related functions
  WordIndex addSrcSymbolToAligModels(std::string s);
  WordIndex addTrgSymbolToAligModels(std::string t);
  void updateAligModelsSrcVoc(const Vector<std::string>& sStrVec);
  void updateAligModelsTrgVoc(const Vector<std::string>& tStrVec);
};

//--------------- _phrSwTransModel class functions
//
template<class HYPOTHESIS>
_phrSwTransModel<HYPOTHESIS>::_phrSwTransModel(void):_phraseBasedTransModel<HYPOTHESIS>()
{
}

//---------------------------------
template<class HYPOTHESIS>
void _phrSwTransModel<HYPOTHESIS>::link_swm_info(SwModelInfo* _swModelInfoPtr)
{
  swModelInfoPtr=_swModelInfoPtr;
}

//---------------------------------
template<class HYPOTHESIS>
bool _phrSwTransModel<HYPOTHESIS>::loadAligModel(const char* prefixFileName)
{
  unsigned int ret;
  
  // Phrase Model

  // Load phrase model vocabularies 
  this->phrModelInfoPtr->phraseModelPars.srcTrainVocabFileName=prefixFileName;
  this->phrModelInfoPtr->phraseModelPars.srcTrainVocabFileName+="_swm.svcb";
  this->phrModelInfoPtr->phraseModelPars.trgTrainVocabFileName=prefixFileName;
  this->phrModelInfoPtr->phraseModelPars.trgTrainVocabFileName+="_swm.tvcb";

  ret=this->phrModelInfoPtr->invPbModelPtr->loadSrcVocab(this->phrModelInfoPtr->phraseModelPars.srcTrainVocabFileName.c_str());
  if(ret==ERROR) return ERROR;

  ret=this->phrModelInfoPtr->invPbModelPtr->loadTrgVocab(this->phrModelInfoPtr->phraseModelPars.trgTrainVocabFileName.c_str());
  if(ret==ERROR) return ERROR;

  // Load phrase model
  this->phrModelInfoPtr->phraseModelPars.readTablePrefix=prefixFileName;
  if(this->phrModelInfoPtr->invPbModelPtr->load(prefixFileName)!=0)
  {
    cerr<<"Error while reading phrase model file\n";
    return ERROR;
  }  
  
  // sw model (The direct model is the one with the prefix _invswm)
  swModelInfoPtr->swModelPars.readTablePrefix=prefixFileName;
  swModelInfoPtr->swModelPars.readTablePrefix=swModelInfoPtr->swModelPars.readTablePrefix+"_invswm";
  ret=swModelInfoPtr->swAligModelPtr->load(swModelInfoPtr->swModelPars.readTablePrefix.c_str());
  if(ret==ERROR) return ERROR;
  
  // Inverse sw model
  swModelInfoPtr->invSwModelPars.readTablePrefix=prefixFileName;
  swModelInfoPtr->invSwModelPars.readTablePrefix=swModelInfoPtr->invSwModelPars.readTablePrefix+"_swm";
  ret=swModelInfoPtr->invSwAligModelPtr->load(swModelInfoPtr->invSwModelPars.readTablePrefix.c_str());
  if(ret==ERROR) return ERROR;

  return OK;
}

//---------------------------------
template<class HYPOTHESIS>
bool _phrSwTransModel<HYPOTHESIS>::printAligModel(std::string printPrefix)
{
      // Print phrase model
  bool ret=_phraseBasedTransModel<HYPOTHESIS>::printAligModel(printPrefix);
  if(ret==ERROR) return ERROR;
  
      // Print inverse sw model
  std::string invSwModelPrefix=printPrefix+"_swm";
  ret=swModelInfoPtr->invSwAligModelPtr->print(invSwModelPrefix.c_str());
  if(ret==ERROR) return ERROR;

      // Print direct sw model
  std::string swModelPrefix=printPrefix+"_invswm";
  ret=swModelInfoPtr->swAligModelPtr->print(swModelPrefix.c_str());
  if(ret==ERROR) return ERROR;

  return OK;
}

//---------------------------------
template<class HYPOTHESIS>
void _phrSwTransModel<HYPOTHESIS>::clear(void)
{
  _phraseBasedTransModel<HYPOTHESIS>::clear();
  swModelInfoPtr->swAligModelPtr->clear();
  swModelInfoPtr->invSwAligModelPtr->clear();
  sumSentLenProbVec.clear();
}

//---------------------------------
template<class HYPOTHESIS>
Score _phrSwTransModel<HYPOTHESIS>::invSwScore(const Vector<WordIndex>& s_,
                                               const Vector<WordIndex>& t_)
{
  return swModelInfoPtr->invSwModelPars.swWeight*(double)invSwLgProb(s_,t_);
}

//---------------------------------
template<class HYPOTHESIS>
Score _phrSwTransModel<HYPOTHESIS>::swScore(const Vector<WordIndex>& s_,
                                            const Vector<WordIndex>& t_)
{
  return swModelInfoPtr->swModelPars.swWeight*(double)swLgProb(s_,t_);
}

//---------------------------------
template<class HYPOTHESIS>
LgProb _phrSwTransModel<HYPOTHESIS>::swLgProb(const Vector<WordIndex>& s_,
                                              const Vector<WordIndex>& t_)
{
  PhrasePairCacheTable::iterator ppctIter;
  ppctIter=cSwmScore.find(make_pair(s_,t_));
  if(ppctIter!=cSwmScore.end())
  {
        // Score was previously stored in the cache table
    return ppctIter->second;
  }
  else
  {
        // Score is not stored in the cache table
    LgProb lp=swModelInfoPtr->swAligModelPtr->calcLgProbPhr(s_,t_);
    cSwmScore[make_pair(s_,t_)]=lp;
    return lp;
  }
}

//---------------------------------
template<class HYPOTHESIS>
LgProb _phrSwTransModel<HYPOTHESIS>::invSwLgProb(const Vector<WordIndex>& s_,
                                                 const Vector<WordIndex>& t_)
{
  PhrasePairCacheTable::iterator ppctIter;
  ppctIter=cInvSwmScore.find(make_pair(s_,t_));
  if(ppctIter!=cInvSwmScore.end())
  {
        // Score was previously stored in the cache table
    return ppctIter->second;
  }
  else
  {
        // Score is not stored in the cache table
    LgProb lp=swModelInfoPtr->invSwAligModelPtr->calcLgProbPhr(t_,s_);
    cInvSwmScore[make_pair(s_,t_)]=lp;
    return lp;
  }
}

//---------------------------------
template<class HYPOTHESIS>
Score _phrSwTransModel<HYPOTHESIS>::sentLenScore(unsigned int slen,
                                                 unsigned int tlen)
{
  return swModelInfoPtr->invSwModelPars.lenWeight*(double)swModelInfoPtr->invSwAligModelPtr->sentLenLgProb(tlen,slen);
}

//---------------------------------
template<class HYPOTHESIS>
Score _phrSwTransModel<HYPOTHESIS>::sentLenScoreForPartialHyp(Bitset<MAX_SENTENCE_LENGTH_ALLOWED> key,
                                                              unsigned int curr_tlen)
{
  if(this->state==MODEL_TRANS_STATE)
  {
        // The model is being used for translate a sentence
    uint_pair range=obtainLengthRangeForGaps(key);
    range.first+=curr_tlen;
    range.second+=curr_tlen;
    return sumSentLenScoreRange(this->pbtmInputVars.srcSentVec.size(),range);
  }
  else
  {
    if(this->state==MODEL_TRANSREF_STATE)
    {
          // The model is being used for align a pair of sentences
      uint_pair range;
      range.first=this->pbtmInputVars.refSentVec.size();
      range.second=this->pbtmInputVars.refSentVec.size();      
      return sumSentLenScoreRange(this->pbtmInputVars.srcSentVec.size(),range);
    }
    else
    {
          // The model is being used for translate a sentence given a
          // prefix
      if(curr_tlen>=this->pbtmInputVars.prefSentVec.size())
      {
            // The prefix has been generated
        uint_pair range=obtainLengthRangeForGaps(key);
        range.first+=curr_tlen;
        range.second+=curr_tlen;
        return sumSentLenScoreRange(this->pbtmInputVars.srcSentVec.size(),range);
      }
      else
      {
            // The prefix has not been generated yet.  The predicted
            // sentence range is (length(prefix),MAX_SENTENCE_LENGTH_ALLOWED),
            // the prediction can be improved but the required code
            // could be complex.
        uint_pair range;
        range.first=this->pbtmInputVars.prefSentVec.size();
        range.second=MAX_SENTENCE_LENGTH_ALLOWED;      
        return sumSentLenScoreRange(this->pbtmInputVars.srcSentVec.size(),range);
      }
    }
  }
}

//---------------------------------
template<class HYPOTHESIS>
Prob _phrSwTransModel<HYPOTHESIS>::sumSentLenProb(unsigned int slen,
                                                  unsigned int tlen)
{
      // Reserve memory if necesary
  while(sumSentLenProbVec.size()<=slen)
  {
    Vector<Prob> vp;
    sumSentLenProbVec.push_back(vp);
  }
  while(sumSentLenProbVec[slen].size()<=tlen)
    sumSentLenProbVec[slen].push_back(-1.0);

      // Check if the probability is already stored
  if((double)sumSentLenProbVec[slen][tlen]>=0.0)
  {
    return sumSentLenProbVec[slen][tlen];
  }
  else
  {
        // The probability has to be calculated
    Prob result;
    if(tlen==0)
    {
      result=swModelInfoPtr->invSwAligModelPtr->sentLenProb(tlen,slen);
    }
    else
    {
      result=sumSentLenProb(slen,tlen-1)+swModelInfoPtr->invSwAligModelPtr->sentLenProb(tlen,slen);
    }
    sumSentLenProbVec[slen][tlen]=result;
    return result;    
  }
}

//---------------------------------
template<class HYPOTHESIS>
Score _phrSwTransModel<HYPOTHESIS>::sumSentLenScoreRange(unsigned int slen,
                                                         uint_pair range)
{
  if(range.first!=0)
    return swModelInfoPtr->invSwModelPars.lenWeight*log((double)(sumSentLenProb(slen,range.second)-sumSentLenProb(slen,range.first-1)));
  else
    return swModelInfoPtr->invSwModelPars.lenWeight*log((double) sumSentLenProb(slen,range.second));
}

//---------------------------------
template<class HYPOTHESIS>
uint_pair _phrSwTransModel<HYPOTHESIS>::obtainLengthRangeForGaps(const Bitset<MAX_SENTENCE_LENGTH_ALLOWED>& hypKey)
{
  unsigned int J;
  Vector<pair<PositionIndex,PositionIndex> > gaps;
  uint_pair result;
  
  J=this->pbtmInputVars.srcSentVec.size();
  this->extract_gaps(hypKey,gaps);
  for(unsigned int i=0;i<gaps.size();++i)
  {
    uint_pair rangeForGap;
    
    rangeForGap=lenRangeForGaps[gaps[i].second-1][J-gaps[i].first];
    result.first+=rangeForGap.first;
    result.second+=rangeForGap.second;
  }
  return result;
}

//---------------------------------
template<class HYPOTHESIS>
void _phrSwTransModel<HYPOTHESIS>::initLenRangeForGapsVec(int maxSrcPhraseLength)
{
  unsigned int J,segmRightMostj,segmLeftMostj;
  Vector<uint_pair> row;
  NbestTableNode<PhraseTransTableNodeData> ttNode;
  NbestTableNode<PhraseTransTableNodeData>::iterator ttNodeIter;
  uint_pair target_uip;
  Vector<WordIndex> s_;
    
  J=this->pbtmInputVars.nsrcSentIdVec.size()-1;
  lenRangeForGaps.clear();
      // Initialize row vector    
  for(unsigned int j=0;j<J;++j) row.push_back(make_pair(0,0));
      // Insert rows into lenRangeForGaps     
  for(unsigned int j=0;j<J;++j) lenRangeForGaps.push_back(row);
     
      // Fill the length range table
  for(unsigned int y=0;y<J;++y)
  {
    for(unsigned int x=J-y-1;x<J;++x)
    {// obtain phrase
      segmRightMostj=y;
      segmLeftMostj=J-x-1; 
      s_.clear();

      target_uip.first=MAX_SENTENCE_LENGTH_ALLOWED*10;
      target_uip.second=0;
      if((segmRightMostj-segmLeftMostj)+1>(unsigned int)maxSrcPhraseLength)
      {
        ttNode.clear();
      }
      else
      {
        for(unsigned int j=segmLeftMostj;j<=segmRightMostj;++j)
          s_.push_back(this->pbtmInputVars.nsrcSentIdVec[j+1]);
  
            // obtain translations for s_
        this->getNbestTransFor_s_(s_,ttNode,this->W);
        
        if(ttNode.size()!=0) // Obtain best p(s_|t_)
        {
          for(ttNodeIter=ttNode.begin();ttNodeIter!=ttNode.end();++ttNodeIter)
          {
                // Update range
            if(target_uip.first>ttNodeIter->second.size())
              target_uip.first=ttNodeIter->second.size();
            if(target_uip.second<ttNodeIter->second.size())
              target_uip.second=ttNodeIter->second.size();
          }
        }
        else
        {
              // Check if source word has been marked as unseen
          if(s_.size()==1 && this->unseenSrcWord(this->pbtmInputVars.srcSentVec[segmLeftMostj]))
          {
            target_uip.first=1;
            target_uip.second=1;
          }
        }
      }
      
          //if phrase has only one word    
      if(x==J-y-1)
      {
        lenRangeForGaps[y][x]=target_uip;
      }
      else
      {
            // phrase has more than one word
        lenRangeForGaps[y][x]=target_uip;
        for(unsigned int z=J-x-1;z<y;++z) 
        {
          uint_pair composition_uip;
          composition_uip.first=lenRangeForGaps[z][x].first+lenRangeForGaps[y][J-2-z].first;
          composition_uip.second=lenRangeForGaps[z][x].second+lenRangeForGaps[y][J-2-z].second;
          if(lenRangeForGaps[y][x].first>composition_uip.first)
            lenRangeForGaps[y][x].first=composition_uip.first;
          if(lenRangeForGaps[y][x].second<composition_uip.second)
            lenRangeForGaps[y][x].second=composition_uip.second;
        }
      }     
    }
  }

      // Correct invalid values due to coverage problems
  for(unsigned int y=0;y<J;++y)
  {
    for(unsigned int x=J-y-1;x<J;++x)
    {
      if(lenRangeForGaps[y][x].first>lenRangeForGaps[y][x].second)
      {
        lenRangeForGaps[y][x].first=0;
        lenRangeForGaps[y][x].second=0;
      }
    }
  }
      // Print verbose mode information
  if(this->verbosity>=1)
  {
    cerr<<"Table with length range for gaps: "<<endl;
    for(unsigned int y=0;y<J;++y)
    {
      for(unsigned int x=0;x<J;++x)
      {
        fprintf(stderr,"(%3d,%3d)",lenRangeForGaps[y][x].first,lenRangeForGaps[y][x].second);
      }
      cerr<<endl;
    }
  }
}

//---------------------------------
template<class HYPOTHESIS>
void _phrSwTransModel<HYPOTHESIS>::clearTempVars(void)
{
  _phraseBasedTransModel<HYPOTHESIS>::clearTempVars();
  swModelInfoPtr->swAligModelPtr->clearTempVars();
  swModelInfoPtr->invSwAligModelPtr->clearTempVars();
  cSwmScore.clear();
  cInvSwmScore.clear();
}

//---------------------------------
template<class HYPOTHESIS>
WordIndex _phrSwTransModel<HYPOTHESIS>::addSrcSymbolToAligModels(std::string s)
{
  WordIndex windex_ipbm=this->phrModelInfoPtr->invPbModelPtr->addTrgSymbol(s,0);
  WordIndex windex_lex=swModelInfoPtr->swAligModelPtr->addSrcSymbol(s,0);
  WordIndex windex_ilex=swModelInfoPtr->invSwAligModelPtr->addTrgSymbol(s,0);
  if(windex_ipbm!=windex_lex || windex_ipbm!=windex_ilex)
  {
    cerr<<"Warning! phrase-based model vocabularies are now different from lexical model vocabularies."<<endl;
  }
  
  return windex_ipbm;
}

//---------------------------------
template<class HYPOTHESIS>
WordIndex _phrSwTransModel<HYPOTHESIS>::addTrgSymbolToAligModels(std::string t)
{
  WordIndex windex_ipbm=this->phrModelInfoPtr->invPbModelPtr->addSrcSymbol(t,0);
  WordIndex windex_lex=swModelInfoPtr->swAligModelPtr->addTrgSymbol(t,0);
  WordIndex windex_ilex=swModelInfoPtr->invSwAligModelPtr->addSrcSymbol(t,0);
  if(windex_ipbm!=windex_lex || windex_ipbm!=windex_ilex)
  {
    cerr<<"Warning! phrase-based model vocabularies are now different from lexical model vocabularies."<<endl;
  }
  
  return windex_ipbm;
}

//---------------------------------
template<class HYPOTHESIS>
void _phrSwTransModel<HYPOTHESIS>::updateAligModelsSrcVoc(const Vector<std::string>& sStrVec)
{
  for(unsigned int i=0;i<sStrVec.size();++i)
  {
    addSrcSymbolToAligModels(sStrVec[i]);
  }
}

//---------------------------------
template<class HYPOTHESIS>
void _phrSwTransModel<HYPOTHESIS>::updateAligModelsTrgVoc(const Vector<std::string>& tStrVec)
{
  for(unsigned int i=0;i<tStrVec.size();++i)
  {
    addTrgSymbolToAligModels(tStrVec[i]);
  }
}

//---------------------------------
template<class HYPOTHESIS>
void _phrSwTransModel<HYPOTHESIS>::pre_trans_actions(std::string srcsent)
{
  _phraseBasedTransModel<HYPOTHESIS>::pre_trans_actions(srcsent);
  initLenRangeForGapsVec(this->A);
}

//---------------------------------
template<class HYPOTHESIS>
void _phrSwTransModel<HYPOTHESIS>::pre_trans_actions_ref(std::string srcsent,
                                                         std::string refsent)
{
  _phraseBasedTransModel<HYPOTHESIS>::pre_trans_actions_ref(srcsent,refsent);
  initLenRangeForGapsVec(this->A);  
}

//---------------------------------
template<class HYPOTHESIS>
void _phrSwTransModel<HYPOTHESIS>::pre_trans_actions_ver(std::string srcsent,
                                                         std::string refsent)
{
  _phraseBasedTransModel<HYPOTHESIS>::pre_trans_actions_ver(srcsent,refsent);
  initLenRangeForGapsVec(this->A);  
}

//---------------------------------
template<class HYPOTHESIS>
void _phrSwTransModel<HYPOTHESIS>::pre_trans_actions_prefix(std::string srcsent,
                                                            std::string prefix)
{
  _phraseBasedTransModel<HYPOTHESIS>::pre_trans_actions_prefix(srcsent,prefix);
  initLenRangeForGapsVec(this->A);
}

//---------------------------------
template<class HYPOTHESIS>
_phrSwTransModel<HYPOTHESIS>::~_phrSwTransModel()
{
}

#endif
