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
/* Module: _phraseBasedTransModel                                   */
/*                                                                  */
/* Prototypes file: _phraseBasedTransModel.h                        */
/*                                                                  */
/* Description: Declares the _phraseBasedTransModel class.          */
/*              This class is a succesor of the BasePbTransModel    */
/*              class.                                              */
/*                                                                  */
/********************************************************************/

/**
 * @file _phraseBasedTransModel.h
 *
 * @brief Declares the _phraseBasedTransModel class.  This class is a
 * succesor of the BasePbTransModel class.
 */

#ifndef __phraseBasedTransModel_h
#define __phraseBasedTransModel_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BasePbTransModel.h"
#include "_incrMuxPhraseModel.h"
#include "PhraseModelInfo.h"
#include "NgramCacheTable.h"
#include "LangModelInfo.h"
#include "SourceSegmentation.h"
#include "NbestTransCacheData.h"
#include "PbTransModelInputVars.h"
#include "PhrasePairCacheTable.h"
#include "ScoreCompDefs.h"
#include "Prob.h"
#include <math.h>
#include <set>

//--------------- Constants ------------------------------------------

#define NO_HEURISTIC            0
#define LOCAL_T_HEURISTIC       4
#define LOCAL_TD_HEURISTIC      6
#define MODEL_IDLE_STATE        1
#define MODEL_TRANS_STATE       2
#define MODEL_TRANSREF_STATE    3
#define MODEL_TRANSVER_STATE    4
#define MODEL_TRANSPREFIX_STATE 5

//--------------- Classes --------------------------------------------

//--------------- _phraseBasedTransModel class

/**
 * @brief The _phraseBasedTransModel class is a predecessor of the
 * BasePbTransModel class.
 */

template<class HYPOTHESIS>
class _phraseBasedTransModel: public BasePbTransModel<HYPOTHESIS>
{
 public:

  typedef typename BasePbTransModel<HYPOTHESIS>::Hypothesis Hypothesis;
  typedef typename BasePbTransModel<HYPOTHESIS>::HypScoreInfo HypScoreInfo;
  typedef typename BasePbTransModel<HYPOTHESIS>::HypDataType HypDataType;

  // class functions

  // Constructor
  _phraseBasedTransModel();

      // Link language model information
  void link_lm_info(LangModelInfo* _langModelInfoPtr);
  
      // Link phrase model information
  void link_pm_info(PhraseModelInfo* _phrModelInfoPtr);

  // class methods

      // Init language and alignment models
  bool loadLangModel(const char* prefixFileName);
  bool loadAligModel(const char* prefixFileName);

      // Print models
  bool printLangModel(std::string printPrefix);
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

      // Function to obtain current source sentence (it may differ from
      // that provided when calling pre_trans_actions since information
      // about translation constraints is removed)
  std::string getCurrentSrcSent(void);

      // Word prediction functions
  void addSentenceToWordPred(Vector<std::string> strVec,
                             int verbose=0);
  pair<Count,std::string> getBestSuffix(std::string input);
  pair<Count,std::string> getBestSuffixGivenHist(Vector<std::string> hist,
                                                 std::string input);

  ////// Hypotheses-related functions

      // Expansion-related functions
  void expand(const Hypothesis& hyp,
              Vector<Hypothesis>& hypVec,
              Vector<Vector<Score> >& scrCompVec);
  void expand_ref(const Hypothesis& hyp,
                  Vector<Hypothesis>& hypVec,
                  Vector<Vector<Score> >& scrCompVec);
  void expand_ver(const Hypothesis& hyp,
                  Vector<Hypothesis>& hypVec,
                  Vector<Vector<Score> >& scrCompVec);
  void expand_prefix(const Hypothesis& hyp,
                     Vector<Hypothesis>& hypVec,
                     Vector<Vector<Score> >& scrCompVec);

      // Heuristic-related functions
  void setHeuristic(unsigned int _heuristicId);
  void addHeuristicToHyp(Hypothesis& hyp);
  void subtractHeuristicToHyp(Hypothesis& hyp);

      // Printing functions and data conversion
  void printHyp(const Hypothesis& hyp,
                ostream &outS,
                int verbose=false);
  Vector<std::string> getTransInPlainTextVec(const Hypothesis& hyp)const;
      
      // Model weights functions
  Vector<Score> scoreCompsForHyp(const Hypothesis& hyp);
  
        // Specific phrase-based functions
  void extendHypData(PositionIndex srcLeft,
                     PositionIndex srcRight,
                     const Vector<std::string>& trgPhrase,
                     HypDataType& hypd);

      // Destructor
  ~_phraseBasedTransModel();

 protected:

  typedef std::map<pair<Vector<WordIndex>,Vector<WordIndex> >,Vector<Score> > PhrasePairVecScore;

      // Data structure to store input variables
  PbTransModelInputVars pbtmInputVars;

      // Language model members
  LangModelInfo* langModelInfoPtr;
  NgramCacheTable cachedNgramScores;
  
      // Phrase model members
  PhraseModelInfo* phrModelInfoPtr;
      // Auxiliary variable to handle phrase model multiplexers
  _incrMuxPhraseModel* incrInvMuxPmPtr;
      // Members useful for caching data
  PhrasePairVecScore cachedDirectPhrScoreVecs;
  PhrasePairVecScore cachedInversePhrScoreVecs;

      // Data used to cache n-best translation scores
  NbestTransCacheData nbTransCacheData;
  
      // Set of unseen words
  std::set<std::string> unseenWordsSet;
      
      // Mapping between phrase and language model vocabularies
  map<WordIndex,WordIndex> tmToLmVocMap;

      // Heuristic function to be used
  unsigned int heuristicId;
      // Heuristic probability vector
  Vector<Vector<Score> > heuristicScoreVec; 
      // Additional data structures to store information about heuristics
  Vector<LgProb> refHeurLmLgProb;
  Vector<LgProb> prefHeurLmLgProb;

      // Variable to store state of the translation model
  unsigned int state;
      
      // Training-related data members
  Vector<Vector<std::string> > wordPredSentVec;

      // Protected functions

      // Functions related to class initialization
  void instantiateWeightVectors(void);
  
      // Word prediction functions
  void incrAddSentenceToWordPred(Vector<std::string> strVec,
                                 int verbose=0);
  void minibatchAddSentenceToWordPred(Vector<std::string> strVec,
                                      int verbose=0);
  void batchAddSentenceToWordPred(Vector<std::string> strVec,
                                  int verbose=0);

  ////// Hypotheses-related functions

      // Expansion-related functions
  void extract_gaps(const Hypothesis& hyp,
                    Vector<pair<PositionIndex,PositionIndex> >& gaps);
  void extract_gaps(const Bitset<MAX_SENTENCE_LENGTH_ALLOWED>& hypKey,
                    Vector<pair<PositionIndex,PositionIndex> >& gaps);
  unsigned int get_num_gaps(const Bitset<MAX_SENTENCE_LENGTH_ALLOWED>& hypKey);

      // Specific phrase-based functions
  virtual void extendHypDataIdx(PositionIndex srcLeft,
                                PositionIndex srcRight,
                                const Vector<WordIndex>& trgPhraseIdx,
                                HypDataType& hypd)=0;

  virtual bool getHypDataVecForGap(const Hypothesis& hyp,
                                   PositionIndex srcLeft,
                                   PositionIndex srcRight,
                                   Vector<HypDataType>& hypDataTypeVec,
                                   float N);
      // Get N-best translations for a subphrase of the source sentence
      // to be translated .  If N is between 0 and 1 then N represents a
      // threshold. 
  virtual bool getHypDataVecForGapRef(const Hypothesis& hyp,
                                      PositionIndex srcLeft,
                                      PositionIndex srcRight,
                                      Vector<HypDataType>& hypDataTypeVec,
                                      float N);
      // This function is identical to the previous function but is to
      // be used when the translation process is conducted by a given
      // reference sentence
  virtual bool getHypDataVecForGapVer(const Hypothesis& hyp,
                                      PositionIndex srcLeft,
                                      PositionIndex srcRight,
                                      Vector<HypDataType>& hypDataTypeVec,
                                      float N);
      // This function is identical to the previous function but is to
      // be used when the translation process is performed to verify the
      // coverage of the model given a reference sentence
  virtual bool getHypDataVecForGapPref(const Hypothesis& hyp,
                                       PositionIndex srcLeft,
                                       PositionIndex srcRight,
                                       Vector<HypDataType>& hypDataTypeVec,
                                       float N);
      // This function is identical to the previous function but is to
      // be used when the translation process is conducted by a given
      // prefix

  virtual bool getTransForHypUncovGap(const Hypothesis& hyp,
                                      PositionIndex srcLeft,
                                      PositionIndex srcRight,
                                      NbestTableNode<PhraseTransTableNodeData>& nbt,
                                      float N);
      // Get N-best translations for a subphrase of the source sentence
      // to be translated .  If N is between 0 and 1 then N represents a
      // threshold.  The result of the search is cached in the data
      // member cPhrNbestTransTable
  virtual bool getTransForHypUncovGapRef(const Hypothesis& hyp,
                                         PositionIndex srcLeft,
                                         PositionIndex srcRight,
                                         NbestTableNode<PhraseTransTableNodeData>& nbt,
                                         float N);
      // This function is identical to the previous function but is to
      // be used when the translation process is conducted by a given
      // reference sentence
  virtual bool getTransForHypUncovGapVer(const Hypothesis& hyp,
                                         PositionIndex srcLeft,
                                         PositionIndex srcRight,
                                         NbestTableNode<PhraseTransTableNodeData>& nbt,
                                         float N);
      // This function is identical to the previous function but is to
      // be used when the translation process is performed to verify the
      // coverage of the model given a reference sentence
  virtual bool getTransForHypUncovGapPref(const Hypothesis& hyp,
                                          PositionIndex srcLeft,
                                          PositionIndex srcRight,
                                          NbestTableNode<PhraseTransTableNodeData>& nbt,
                                          float N);
      // This function is identical to the previous function but is to
      // be used when the translation process is conducted by a given
      // prefix

      // Functions for translating with references or prefixes
  virtual bool hypDataTransIsPrefixOfTargetRef(const HypDataType& hypd,
                                               bool& equal)const=0;
  void transUncovGapPrefNoGen(const Hypothesis& hyp,
                              PositionIndex srcLeft,
                              PositionIndex srcRight,
                              NbestTableNode<PhraseTransTableNodeData>& nbt);
  void genListOfTransLongerThanPref(Vector<WordIndex> s_,
                                    unsigned int ntrgSize,
                                    NbestTableNode<PhraseTransTableNodeData>& nbt);
  bool trgWordVecIsPrefix(const Vector<WordIndex>& wiVec1,
                          bool lastWiVec1WordIsComplete,
                          const std::string& lastWiVec1Word,
                          const Vector<WordIndex>& wiVec2,
                          bool& equal);
      // returns true if target word vector wiVec1 is a prefix of wiVec2
  bool isPrefix(std::string str1,std::string str2);
      // returns true if string str1 is a prefix of string str2
  
  PositionIndex getLastSrcPosCovered(const Hypothesis& hyp);
      // Get the index of last source position which was covered
  virtual PositionIndex getLastSrcPosCoveredHypData(const HypDataType& hypd)=0;
      // The same as the previous function, but given an object of
      // HypDataType

      // Language model scoring functions
  Score wordPenaltyScore(unsigned int tlen);
  Score sumWordPenaltyScore(unsigned int tlen);
  Score nbestLmScoringFunc(const Vector<WordIndex>& target);
  Score getNgramScoreGivenState(const Vector<WordIndex>& target,
                                LM_State &state);
  Score getScoreEndGivenState(LM_State &state);
  LgProb getSentenceLgProb(const Vector<WordIndex>& target,
                           int verbose=0);

      // Phrase model scoring functions
  Score phrScore_s_t_(const Vector<WordIndex>& s_,
                      const Vector<WordIndex>& t_);
      // obtains the logarithm of pstWeight*ps_t_ 
  Vector<Score> phrScoreVec_s_t_(const Vector<WordIndex>& s_,
                                 const Vector<WordIndex>& t_);
      // the same as phrScore_s_t_ but returns a score vector for each model
  Score phrScore_t_s_(const Vector<WordIndex>& s_,
                      const Vector<WordIndex>& t_);
      // obtains the logarithm of ptsWeight*pt_s_
  Vector<Score> phrScoreVec_t_s_(const Vector<WordIndex>& s_,
                                 const Vector<WordIndex>& t_);
      // the same as phrScore_t_s_ but returns a score vector for each model
  Score srcJumpScore(unsigned int offset);
      // obtains score for source jump
  Score srcSegmLenScore(unsigned int k,
                        const SourceSegmentation& srcSegm,
                        unsigned int srcLen,
                        unsigned int lastTrgSegmLen);
      // obtains the log-probability for the length of the k'th source
      // segment
  Score trgSegmLenScore(unsigned int x_k,
                        unsigned int x_km1,
                        unsigned int trgLen);
      // obtains the log-probability for the length of a target segment
  
      // Functions to generate translation lists
  bool getTransForInvPbModel(const Vector<WordIndex>& s_,
                             std::set<Vector<WordIndex> >& transSet);
  virtual bool getNbestTransFor_s_(Vector<WordIndex> s_,
                                   NbestTableNode<PhraseTransTableNodeData>& nbt,
                                   float N);
      // Get N-best translations for a given source phrase s_.
      // If N is between 0 and 1 then N represents a threshold
      
      // Functions to score n-best translations lists
  virtual Score nbestTransScore(const Vector<WordIndex>& s_,
                                const Vector<WordIndex>& t_)=0;
  virtual Score nbestTransScoreLast(const Vector<WordIndex>& s_,
                                    const Vector<WordIndex>& t_)=0;
      // Cached functions to score n-best translations lists
  Score nbestTransScoreCached(const Vector<WordIndex>& s_,
                              const Vector<WordIndex>& t_);
  Score nbestTransScoreLastCached(const Vector<WordIndex>& s_,
                                  const Vector<WordIndex>& t_);

      // Functions related to pre_trans_actions
  virtual void clearTempVars(void);
  bool lastCharIsBlank(std::string str);
  void verifyDictCoverageForSentence(Vector<std::string>& sentenceVec,
                                     int maxSrcPhraseLength=MAX_SENTENCE_LENGTH_ALLOWED);
      // Verifies dictionary coverage for the sentence to translate.  It
      // is possible to impose an additional constraint consisting of a
      // maximum length for the source phrases.
  void manageUnseenSrcWord(std::string srcw);
  bool unseenSrcWord(std::string srcw);
  bool unseenSrcWordGivenPosition(unsigned int srcPos);
  Score unkWordScoreHeur(void);
  void initHeuristic(unsigned int maxSrcPhraseLength);
      // Initialize heuristic for the sentence to be translated

      // Functions related to getTransInPlainTextVec
  Vector<std::string> getTransInPlainTextVecTs(const Hypothesis& hyp)const;
  Vector<std::string> getTransInPlainTextVecTps(const Hypothesis& hyp)const;
  Vector<std::string> getTransInPlainTextVecTrs(const Hypothesis& hyp)const;
  Vector<std::string> getTransInPlainTextVecTvs(const Hypothesis& hyp)const;

      // Heuristic related functions
  virtual Score calcHeuristicScore(const _phraseBasedTransModel::Hypothesis& hyp);
  void initHeuristicLocalt(int maxSrcPhraseLength);
  Score heurLmScoreLt(Vector<WordIndex>& t_);
  Score heurLmScoreLtNoAdmiss(Vector<WordIndex>& t_);
  Score calcRefLmHeurScore(const _phraseBasedTransModel::Hypothesis& hyp);
  Score calcPrefLmHeurScore(const _phraseBasedTransModel::Hypothesis& hyp);
  Score heuristicLocalt(const Hypothesis& hyp);
  void initHeuristicLocaltd(int maxSrcPhraseLength);
  Score heuristicLocaltd(const Hypothesis& hyp);
  Vector<unsigned int> min_jumps(const Vector<pair<PositionIndex,PositionIndex> >& gaps,
                                 PositionIndex lastSrcPosCovered)const;

      // Vocabulary functions
  WordIndex stringToSrcWordIndex(std::string s)const;
  std::string wordIndexToSrcString(WordIndex w)const;
  Vector<std::string> srcIndexVectorToStrVector(Vector<WordIndex> srcidxVec)const;
  Vector<WordIndex> strVectorToSrcIndexVector(Vector<std::string> srcStrVec)const;
  WordIndex stringToTrgWordIndex(std::string s)const;
  std::string wordIndexToTrgString(WordIndex w)const;
  Vector<std::string> trgIndexVectorToStrVector(Vector<WordIndex> trgidxVec)const;
  Vector<WordIndex> strVectorToTrgIndexVector(Vector<std::string> trgStrVec)const;
  std::string phraseToStr(const Vector<WordIndex>& phr)const;
  Vector<std::string> phraseToStrVec(const Vector<WordIndex>& phr)const;
  WordIndex tmVocabToLmVocab(WordIndex w);
  void initTmToLmVocabMap(void);
};

//--------------- _phraseBasedTransModel class functions
//

template<class HYPOTHESIS>
_phraseBasedTransModel<HYPOTHESIS>::_phraseBasedTransModel(void):BasePbTransModel<HYPOTHESIS>()
{
      // Set state info
  state=MODEL_IDLE_STATE;

      // Initially, no heuristic is used
  heuristicId=NO_HEURISTIC;
}

//---------------------------------
template<class HYPOTHESIS>
void _phraseBasedTransModel<HYPOTHESIS>::link_lm_info(LangModelInfo* _langModelInfoPtr)
{
  langModelInfoPtr=_langModelInfoPtr;
  
      // Initialize tm to lm vocab map
  initTmToLmVocabMap();
}

//---------------------------------
template<class HYPOTHESIS>
void _phraseBasedTransModel<HYPOTHESIS>::instantiateWeightVectors(void)
{
  if(incrInvMuxPmPtr)
  {
    phrModelInfoPtr->phraseModelPars.ptsWeightVec.clear();
    phrModelInfoPtr->phraseModelPars.pstWeightVec.clear();
    
    unsigned int nmodels=incrInvMuxPmPtr->getNumModels();
    for(unsigned int i=0;i<nmodels;++i)
    {
      phrModelInfoPtr->phraseModelPars.ptsWeightVec.push_back(DEFAULT_PTS_WEIGHT);
      phrModelInfoPtr->phraseModelPars.pstWeightVec.push_back(DEFAULT_PST_WEIGHT);
    }
  }
  else
  {
    phrModelInfoPtr->phraseModelPars.ptsWeightVec.clear();
    phrModelInfoPtr->phraseModelPars.pstWeightVec.clear();

    phrModelInfoPtr->phraseModelPars.ptsWeightVec.push_back(DEFAULT_PTS_WEIGHT);
    phrModelInfoPtr->phraseModelPars.pstWeightVec.push_back(DEFAULT_PST_WEIGHT);
  }
}

//---------------------------------
template<class HYPOTHESIS>
void _phraseBasedTransModel<HYPOTHESIS>::link_pm_info(PhraseModelInfo* _phrModelInfoPtr)
{
  phrModelInfoPtr=_phrModelInfoPtr;
      // Obtain pointer to multiplexer phrase model if appliable
  incrInvMuxPmPtr=dynamic_cast<_incrMuxPhraseModel* > (_phrModelInfoPtr->invPbModelPtr);
}

//---------------------------------
template<class HYPOTHESIS>
bool _phraseBasedTransModel<HYPOTHESIS>::loadLangModel(const char* prefixFileName)
{
  std::string penFile;
  std::string predFile;
  int err;
  
  langModelInfoPtr->langModelPars.languageModelFileName=prefixFileName;
  
      // Initializes language model
  if(langModelInfoPtr->lModelPtr->load(prefixFileName)==ERROR)
    return ERROR;
    
      // load WordPredictor info
  predFile=prefixFileName;
  predFile=predFile+".wp";
  err=langModelInfoPtr->wordPredictor.load(predFile.c_str());
  if(err==ERROR)
  {
    cerr<<"Warning: File for initializing the word predictor not provided!"<<endl;
  }
  return OK;
}

//---------------------------------
template<class HYPOTHESIS>
bool _phraseBasedTransModel<HYPOTHESIS>::loadAligModel(const char* prefixFileName)
{
      // Save parameters
  phrModelInfoPtr->phraseModelPars.srcTrainVocabFileName="";
  phrModelInfoPtr->phraseModelPars.trgTrainVocabFileName="";
  phrModelInfoPtr->phraseModelPars.readTablePrefix=prefixFileName;
  
      // Load phrase model
  if(this->phrModelInfoPtr->invPbModelPtr->load(prefixFileName)!=0)
  {
    cerr<<"Error while reading phrase model file\n";
    return ERROR;
  }  

      // Instantiate weight vectors for phrase model
  instantiateWeightVectors();

  return OK;
}

//---------------------------------
template<class HYPOTHESIS>
bool _phraseBasedTransModel<HYPOTHESIS>::printLangModel(std::string printPrefix)
{
  bool retVal=langModelInfoPtr->lModelPtr->print(printPrefix.c_str());
  if(retVal==ERROR) return ERROR;

  return OK;
}

//---------------------------------
template<class HYPOTHESIS>
bool _phraseBasedTransModel<HYPOTHESIS>::printAligModel(std::string printPrefix)
{
  bool retVal=this->phrModelInfoPtr->invPbModelPtr->print(printPrefix.c_str());
  if(retVal==ERROR) return ERROR;

  return OK;
}

//---------------------------------
template<class HYPOTHESIS>
void _phraseBasedTransModel<HYPOTHESIS>::clear(void)
{
  this->phrModelInfoPtr->invPbModelPtr->clear();
  langModelInfoPtr->lModelPtr->clear();
  langModelInfoPtr->wpModelPtr->clear();
  langModelInfoPtr->wordPredictor.clear();
      // Set state info
  state=MODEL_IDLE_STATE;
}

//---------------------------------
template<class HYPOTHESIS>
PositionIndex _phraseBasedTransModel<HYPOTHESIS>::getLastSrcPosCovered(const Hypothesis& hyp)
{
  return getLastSrcPosCoveredHypData(hyp.getData());
}

//---------------------------------------
template<class HYPOTHESIS>
Score _phraseBasedTransModel<HYPOTHESIS>::wordPenaltyScore(unsigned int tlen)
{
  return langModelInfoPtr->langModelPars.wpScaleFactor*(double)langModelInfoPtr->wpModelPtr->wordPenaltyScore(tlen);
}

//---------------------------------------
template<class HYPOTHESIS>
Score _phraseBasedTransModel<HYPOTHESIS>::sumWordPenaltyScore(unsigned int tlen)
{
  return langModelInfoPtr->langModelPars.wpScaleFactor*(double)langModelInfoPtr->wpModelPtr->sumWordPenaltyScore(tlen);
}

//---------------------------------------
template<class HYPOTHESIS>
Score _phraseBasedTransModel<HYPOTHESIS>::nbestLmScoringFunc(const Vector<WordIndex>& target)
{
      // Warning: this function may become a bottleneck when the list of
      // translation options is large
  
  PhraseCacheTable::iterator pctIter;
  pctIter=nbTransCacheData.cnbLmScores.find(target);
  if(pctIter!=nbTransCacheData.cnbLmScores.end())
  {
        // Score was previously stored in the cache table
    return pctIter->second;
  }
  else
  {
        // Score is not stored in the cache table
    Vector<WordIndex> hist;
    LM_State state;    
    langModelInfoPtr->lModelPtr->getStateForWordSeq(hist,state);
    Score scr=getNgramScoreGivenState(target,state);
    nbTransCacheData.cnbLmScores[target]=scr;
    return scr;
  }
}

//---------------------------------------
template<class HYPOTHESIS>
Score _phraseBasedTransModel<HYPOTHESIS>::getNgramScoreGivenState(const Vector<WordIndex>& target,
                                                                  LM_State &state)
{
      // Score not present in cache table
  Vector<WordIndex> target_lm;
  Score unweighted_result=0;

      // target_lm stores the target sentence using indices of the language model
  for(unsigned int i=0;i<target.size();++i)
  {
    target_lm.push_back(tmVocabToLmVocab(target[i]));
  }
      
  for(unsigned int i=0;i<target_lm.size();++i)
  {
        // Try to find score in cache table
    NgramCacheTable::iterator nctIter=cachedNgramScores.find(make_pair(target_lm[i],state));
    if(nctIter!=cachedNgramScores.end())
    {
      unweighted_result+=nctIter->second;
    }
    else
    {
#ifdef WORK_WITH_ZERO_GRAM_PROB
      Score scr=log((double)langModelInfoPtr->lModelPtr->getZeroGramProb());
#else
      Score scr=(double)langModelInfoPtr->lModelPtr->getNgramLgProbGivenState(target_lm[i],state);
#endif
          // Increase score
      unweighted_result+=scr;
          // Update cache table
      cachedNgramScores[make_pair(target_lm[i],state)]=scr;
    }
  }
      // Return result
  return langModelInfoPtr->langModelPars.lmScaleFactor*unweighted_result;
}

//---------------------------------------
template<class HYPOTHESIS>
Score _phraseBasedTransModel<HYPOTHESIS>::getScoreEndGivenState(LM_State &state)
{
      // Try to find score in cache table
  NgramCacheTable::iterator nctIter=cachedNgramScores.find(make_pair(S_END,state));
  if(nctIter!=cachedNgramScores.end())
  {
    return nctIter->second;
  }
  else
  {
#ifdef WORK_WITH_ZERO_GRAM_PROB
    Score result=langModelInfoPtr->langModelPars.lmScaleFactor*log((double)langModelInfoPtr->lModelPtr->getZeroGramProb());
#else
    Score result=langModelInfoPtr->langModelPars.lmScaleFactor*(double)langModelInfoPtr->lModelPtr->getLgProbEndGivenState(state);	
#endif
    cachedNgramScores[make_pair(S_END,state)]=result;
    return result;
  }
}

//---------------------------------------
template<class HYPOTHESIS>
LgProb _phraseBasedTransModel<HYPOTHESIS>::getSentenceLgProb(const Vector<WordIndex>& target,
                                                             int verbose)
{
  LgProb lmLgProb=0;
 	 
  Vector<WordIndex> s;
  unsigned int i;
  for(i=0;i<target.size();++i)
    s.push_back(tmVocabToLmVocab(target[i]));
  lmLgProb=(double)langModelInfoPtr->lModelPtr->getSentenceLog10Prob(s,verbose)*M_LN10; 

  return lmLgProb;  
}

//---------------------------------------
template<class HYPOTHESIS>
Score _phraseBasedTransModel<HYPOTHESIS>::phrScore_s_t_(const Vector<WordIndex>& s_,
                                                        const Vector<WordIndex>& t_)
{
  Vector<Score> scoreVec=phrScoreVec_s_t_(s_,t_);
  Score sum=0;
  for(unsigned int i=0;i<scoreVec.size();++i)
    sum+=scoreVec[i];
  return sum;
}

//---------------------------------------
template<class HYPOTHESIS>
Vector<Score> _phraseBasedTransModel<HYPOTHESIS>::phrScoreVec_s_t_(const Vector<WordIndex>& s_,
                                                                   const Vector<WordIndex>& t_)
{
      // Check if score of phrase pair is stored in cache table
  PhrasePairVecScore::iterator ppctIter=cachedInversePhrScoreVecs.find(make_pair(s_,t_));
  if(ppctIter!=cachedInversePhrScoreVecs.end()) return ppctIter->second;
  else
  {
        // Score has not been cached previously
        // Check whether a mux phrase model is being used
    if(incrInvMuxPmPtr)
    {
      Vector<Score> scoreVec;
      unsigned int nmodels=incrInvMuxPmPtr->getNumModels();
      for(unsigned int i=0;i<nmodels;++i)
      {
        Score score=this->phrModelInfoPtr->phraseModelPars.pstWeightVec[i] * (double)incrInvMuxPmPtr->idxLogpt_s_(i,t_,s_);
        scoreVec.push_back(score);
      }
      cachedInversePhrScoreVecs[make_pair(s_,t_)]=scoreVec;
      return scoreVec;
    }
    else
    {
      Vector<Score> scoreVec;
      Score score=this->phrModelInfoPtr->phraseModelPars.pstWeightVec[0] * (double)this->phrModelInfoPtr->invPbModelPtr->logpt_s_(t_,s_);
      scoreVec.push_back(score);
      cachedInversePhrScoreVecs[make_pair(s_,t_)]=scoreVec;
      return scoreVec;
    }
  }
}

//---------------------------------------
template<class HYPOTHESIS>
Score _phraseBasedTransModel<HYPOTHESIS>::phrScore_t_s_(const Vector<WordIndex>& s_,
                                                        const Vector<WordIndex>& t_)
{
  Vector<Score> scoreVec=phrScoreVec_t_s_(s_,t_);
  Score sum=0;
  for(unsigned int i=0;i<scoreVec.size();++i)
    sum+=scoreVec[i];
  return sum;
}

//---------------------------------------
template<class HYPOTHESIS>
Vector<Score> _phraseBasedTransModel<HYPOTHESIS>::phrScoreVec_t_s_(const Vector<WordIndex>& s_,
                                                                   const Vector<WordIndex>& t_)
{
      // Check if score of phrase pair is stored in cache table
  PhrasePairVecScore::iterator ppctIter=cachedDirectPhrScoreVecs.find(make_pair(s_,t_));
  if(ppctIter!=cachedDirectPhrScoreVecs.end()) return ppctIter->second;
  else
  {
        // Score has not been cached previously
        // Check whether a mux phrase model is being used
    if(incrInvMuxPmPtr)
    {
      Vector<Score> scoreVec;
      unsigned int nmodels=incrInvMuxPmPtr->getNumModels();
      for(unsigned int i=0;i<nmodels;++i)
      {
        Score score=this->phrModelInfoPtr->phraseModelPars.ptsWeightVec[i] * (double)incrInvMuxPmPtr->idxLogps_t_(i,t_,s_);
        scoreVec.push_back(score);
      }
      cachedDirectPhrScoreVecs[make_pair(s_,t_)]=scoreVec;
      return scoreVec;
    }
    else
    {
      Vector<Score> scoreVec;
      Score score=this->phrModelInfoPtr->phraseModelPars.ptsWeightVec[0] * (double)this->phrModelInfoPtr->invPbModelPtr->logps_t_(t_,s_);
      scoreVec.push_back(score);
      cachedDirectPhrScoreVecs[make_pair(s_,t_)]=scoreVec;
      return scoreVec;
    }
  }
}

//---------------------------------------
template<class HYPOTHESIS>
Score _phraseBasedTransModel<HYPOTHESIS>::srcJumpScore(unsigned int offset)
{
      // Check whether a mux phrase model is being used
    if(incrInvMuxPmPtr)
    {
      return this->phrModelInfoPtr->phraseModelPars.srcJumpWeight * (double)incrInvMuxPmPtr->idxTrgCutsLgProb(MAIN_MUX_PMODEL_INDEX,offset);
    }
    else
      return this->phrModelInfoPtr->phraseModelPars.srcJumpWeight * (double)this->phrModelInfoPtr->invPbModelPtr->trgCutsLgProb(offset);
}

//---------------------------------------
template<class HYPOTHESIS>
Score _phraseBasedTransModel<HYPOTHESIS>::srcSegmLenScore(unsigned int k,
                                                          const SourceSegmentation& srcSegm,
                                                          unsigned int srcLen,
                                                          unsigned int lastTrgSegmLen)
{
      // Check whether a mux phrase model is being used
  if(incrInvMuxPmPtr)
  {
    return this->phrModelInfoPtr->phraseModelPars.srcSegmLenWeight * (double)incrInvMuxPmPtr->idxTrgSegmLenLgProb(MAIN_MUX_PMODEL_INDEX,k,srcSegm,srcLen,lastTrgSegmLen);
  }
  else
    return this->phrModelInfoPtr->phraseModelPars.srcSegmLenWeight * (double)this->phrModelInfoPtr->invPbModelPtr->trgSegmLenLgProb(k,srcSegm,srcLen,lastTrgSegmLen);
}

//---------------------------------
template<class HYPOTHESIS>
Score _phraseBasedTransModel<HYPOTHESIS>::trgSegmLenScore(unsigned int x_k,
                                                          unsigned int x_km1,
                                                          unsigned int trgLen)
{
      // Check whether a mux phrase model is being used
  if(incrInvMuxPmPtr)
  {
    return this->phrModelInfoPtr->phraseModelPars.trgSegmLenWeight * (double)incrInvMuxPmPtr->idxSrcSegmLenLgProb(MAIN_MUX_PMODEL_INDEX,x_k,x_km1,trgLen);
  }
  else
    return this->phrModelInfoPtr->phraseModelPars.trgSegmLenWeight * (double)this->phrModelInfoPtr->invPbModelPtr->srcSegmLenLgProb(x_k,x_km1,trgLen);
}

//---------------------------------
template<class HYPOTHESIS>
void _phraseBasedTransModel<HYPOTHESIS>::clearTempVars(void)
{
  // Clear input information
  pbtmInputVars.clear();

  // Clear set of unseen words
  unseenWordsSet.clear();
  
  // Clear data structures that are used 
  // for fast access.

      // Clear language models data members
  cachedNgramScores.clear();
  
      // Clear phrase model caching data members
  cachedDirectPhrScoreVecs.clear();
  cachedInversePhrScoreVecs.clear();

      // Clear n-best translation cache data
  nbTransCacheData.clear();

      // Init the map between TM and LM vocabularies
  initTmToLmVocabMap();

      // Clear information of the heuristic used in the translation
  heuristicScoreVec.clear();

      // Clear additional heuristic information
  refHeurLmLgProb.clear();
  prefHeurLmLgProb.clear();

      // Clear temporary variables of the language model
  langModelInfoPtr->lModelPtr->clearTempVars();

      // Clear temporary variables of the phrase model
  this->phrModelInfoPtr->invPbModelPtr->clearTempVars();
}

//---------------------------------------
template<class HYPOTHESIS>
bool _phraseBasedTransModel<HYPOTHESIS>::lastCharIsBlank(std::string str)
{
  if(str.size()==0) return false;
  else
  {
    if(str[str.size()-1]==' ') return true;
    else return false;
  }
}

//---------------------------------------
template<class HYPOTHESIS>
void _phraseBasedTransModel<HYPOTHESIS>::verifyDictCoverageForSentence(Vector<std::string>& sentenceVec,
                                                                       int /*maxSrcPhraseLength*/)
{
      // Manage source words without translation options
  for(unsigned int j=0;j<sentenceVec.size();++j)
  {
    NbestTableNode<PhraseTransTableNodeData> ttNode;
    std::string s=sentenceVec[j];
    Vector<WordIndex> s_;
    s_.push_back(stringToSrcWordIndex(s));
    std::set<Vector<WordIndex> > transSet;
    getTransForInvPbModel(s_,transSet);
    if(transSet.size()==0)
    {
      manageUnseenSrcWord(s);
    }
  }
      // Clear temporary variables of the phrase model
  this->phrModelInfoPtr->invPbModelPtr->clearTempVars();
}

//---------------------------------
template<class HYPOTHESIS>
void _phraseBasedTransModel<HYPOTHESIS>::manageUnseenSrcWord(std::string srcw)
{
      // Visualize warning depending on the verbosity level
  if(this->verbosity>0)
  {
    cerr<<"Warning! word "<<srcw<<" has been marked as unseen."<<endl;
  }
      // Add word to the set of unseen words
  unseenWordsSet.insert(srcw);
}

//---------------------------------
template<class HYPOTHESIS>
bool _phraseBasedTransModel<HYPOTHESIS>::unseenSrcWord(std::string srcw)
{
  std::set<std::string>::iterator setIter;

  setIter=unseenWordsSet.find(srcw);
  if(setIter!=unseenWordsSet.end())
    return true;
  else
    return false;
}

//---------------------------------
template<class HYPOTHESIS>
bool _phraseBasedTransModel<HYPOTHESIS>::unseenSrcWordGivenPosition(unsigned int srcPos)
{
  return unseenSrcWord(pbtmInputVars.srcSentVec[srcPos-1]);
}

//---------------------------------------
template<class HYPOTHESIS>
Score _phraseBasedTransModel<HYPOTHESIS>::unkWordScoreHeur(void)
{
  Score result=0;

      // Obtain phrase scores
  Vector<WordIndex> s_;
  Vector<WordIndex> t_;

      // Init s_ and t_
  s_.push_back(UNK_WORD);
  t_.push_back(UNK_WORD);
  
      // p(t_|s_) phrase score
  result+=this->phrScore_t_s_(s_,t_); 
  
      // p(s_|t_) phrase score
  result+=this->phrScore_s_t_(s_,t_); 

      // Obtain lm scores
  Vector<WordIndex> hist;
  LM_State state;    
  langModelInfoPtr->lModelPtr->getStateForWordSeq(hist,state);
  t_.clear();
  t_.push_back(UNK_WORD);
  result+=getNgramScoreGivenState(t_,state);

      // Return result
  return result;
}

//---------------------------------
template<class HYPOTHESIS>
void _phraseBasedTransModel<HYPOTHESIS>::initHeuristic(unsigned int maxSrcPhraseLength)
{
  switch(heuristicId)
  {
    case LOCAL_T_HEURISTIC:
      initHeuristicLocalt(maxSrcPhraseLength);
      break;
    case LOCAL_TD_HEURISTIC:
      initHeuristicLocaltd(maxSrcPhraseLength);
      break;
  }
}

//---------------------------------
template<class HYPOTHESIS>
void _phraseBasedTransModel<HYPOTHESIS>::initHeuristicLocalt(int maxSrcPhraseLength)
{
  Vector<Score> row;
  NbestTableNode<PhraseTransTableNodeData> ttNode;
  NbestTableNode<PhraseTransTableNodeData>::iterator ttNodeIter;
  Score compositionProduct;
  Score bestScore_ts=0;
  Score score_ts;
  Vector<WordIndex> s_;
    
  unsigned int J=pbtmInputVars.nsrcSentIdVec.size()-1;
  heuristicScoreVec.clear();
      // Initialize row vector    
  for(unsigned int j=0;j<J;++j) row.push_back(-FLT_MAX);
      // Insert rows into t-heuristic table     
  for(unsigned int j=0;j<J;++j) heuristicScoreVec.push_back(row);
     
      // Fill the t-heuristic table
  for(unsigned int y=0;y<J;++y)
  {
    for(unsigned int x=J-y-1;x<J;++x)
    {
          // obtain source phrase
      unsigned int segmRightMostj=y;
      unsigned int segmLeftMostj=J-x-1; 
      s_.clear();

          // obtain score for best translation
      if((segmRightMostj-segmLeftMostj)+1>(unsigned int)maxSrcPhraseLength)
      {
        ttNode.clear();
      }
      else
      {
        for(unsigned int j=segmLeftMostj;j<=segmRightMostj;++j)
          s_.push_back(pbtmInputVars.nsrcSentIdVec[j+1]);
  
            // obtain translations for s_
        getNbestTransFor_s_(s_,ttNode,this->pbTransModelPars.W);
        if(ttNode.size()!=0) // Obtain best p(s_|t_)
        {
          bestScore_ts=-FLT_MAX;
          for(ttNodeIter=ttNode.begin();ttNodeIter!=ttNode.end();++ttNodeIter)
          {
                // Obtain phrase to phrase translation probability
            score_ts=phrScore_s_t_(s_,ttNodeIter->second)+phrScore_t_s_(s_,ttNodeIter->second);
                // Obtain language model heuristic estimation
//            score_ts+=heurLmScoreLt(ttNodeIter->second);
            score_ts+=heurLmScoreLtNoAdmiss(ttNodeIter->second);
            
            if(bestScore_ts<score_ts) bestScore_ts=score_ts;
          }
        }
      }
      
          // Check source phrase length     
      if(x==J-y-1)
      {
            // source phrase has only one word
        if(ttNode.size()!=0)
        {
          heuristicScoreVec[y][x]=bestScore_ts;
        }
        else
        {
          heuristicScoreVec[y][x]=unkWordScoreHeur();
        }
      }
      else
      {
            // source phrase has more than one word
        if(ttNode.size()!=0)
        {
          heuristicScoreVec[y][x]=bestScore_ts;
        }
        else
        {
          heuristicScoreVec[y][x]=-FLT_MAX;
        }
        for(unsigned int z=J-x-1;z<y;++z) 
        {
          compositionProduct=heuristicScoreVec[z][x]+heuristicScoreVec[y][J-2-z];
          if(heuristicScoreVec[y][x]<compositionProduct)
          {
            heuristicScoreVec[y][x]=compositionProduct; 
          }
        }   
      }       
    } 
  }
}

//---------------------------------
template<class HYPOTHESIS>
Score _phraseBasedTransModel<HYPOTHESIS>::heurLmScoreLt(Vector<WordIndex>& t_)
{
  Vector<WordIndex> lmHist;
  unsigned int i;
  LM_State lmState;
  LgProb lp=0;
  
  if(t_.size()>2)
  {
    langModelInfoPtr->lModelPtr->getStateForBeginOfSentence(lmState);
    langModelInfoPtr->lModelPtr->getNgramLgProbGivenState(tmVocabToLmVocab(t_[0]),lmState);
    langModelInfoPtr->lModelPtr->getNgramLgProbGivenState(tmVocabToLmVocab(t_[1]),lmState);
  }
  for(i=2;i<t_.size();++i)
  {
    lp=lp+(double)langModelInfoPtr->lModelPtr->getNgramLgProbGivenState(tmVocabToLmVocab(t_[i]),lmState);
  }
  return lp*(double)langModelInfoPtr->langModelPars.lmScaleFactor;
}

//---------------------------------
template<class HYPOTHESIS>
Score _phraseBasedTransModel<HYPOTHESIS>::heurLmScoreLtNoAdmiss(Vector<WordIndex>& t_)
{
  Vector<WordIndex> hist;
  LM_State state;    
  langModelInfoPtr->lModelPtr->getStateForWordSeq(hist,state);
  Score scr=getNgramScoreGivenState(t_,state);
  return scr;
}

//---------------------------------
template<class HYPOTHESIS>
Score _phraseBasedTransModel<HYPOTHESIS>::calcRefLmHeurScore(const _phraseBasedTransModel::Hypothesis& hyp)
{
  if(refHeurLmLgProb.empty())
  {
        // Fill vector with lm components for the reference sentence
    LgProb lp=0;
    LM_State lmState;
    langModelInfoPtr->lModelPtr->getStateForBeginOfSentence(lmState);
    
    refHeurLmLgProb.push_back(NULL_WORD);
    for(unsigned int i=1;i<pbtmInputVars.nrefSentIdVec.size();++i)
    {
      lp+=langModelInfoPtr->lModelPtr->getNgramLgProbGivenState(tmVocabToLmVocab(pbtmInputVars.nrefSentIdVec[i]),lmState);
      refHeurLmLgProb.push_back(lp);
    }
  }
      // Return heuristic value
  unsigned int len=hyp.partialTransLength();
  LgProb lp=refHeurLmLgProb.back()-refHeurLmLgProb[len];

  return (double)langModelInfoPtr->langModelPars.lmScaleFactor*(double)lp;
}

//---------------------------------
template<class HYPOTHESIS>
Score _phraseBasedTransModel<HYPOTHESIS>::calcPrefLmHeurScore(const _phraseBasedTransModel::Hypothesis& hyp)
{
  if(prefHeurLmLgProb.empty())
  {
        // Fill vector with lm components for the reference sentence
    LgProb lp=0;
    LM_State lmState;
    langModelInfoPtr->lModelPtr->getStateForBeginOfSentence(lmState);
    
    prefHeurLmLgProb.push_back(0);
    for(unsigned int i=1;i<pbtmInputVars.nprefSentIdVec.size();++i)
    {
      lp+=langModelInfoPtr->lModelPtr->getNgramLgProbGivenState(tmVocabToLmVocab(pbtmInputVars.nprefSentIdVec[i]),lmState);
      prefHeurLmLgProb.push_back(lp);
    }
  }
      // Return heuristic value
  LgProb lp;
  unsigned int len=hyp.partialTransLength();
  if(len>=pbtmInputVars.nprefSentIdVec.size()-1)
    lp=0;
  else
  {
    lp=prefHeurLmLgProb.back()-prefHeurLmLgProb[len];
  }
  return (double)langModelInfoPtr->langModelPars.lmScaleFactor*(double)lp;
}

//---------------------------------
template<class HYPOTHESIS>
Score _phraseBasedTransModel<HYPOTHESIS>::heuristicLocalt(const Hypothesis& hyp)
{
  if(state==MODEL_TRANS_STATE)
  {
    LgProb result=0;
    unsigned int J;
    Vector<pair<PositionIndex,PositionIndex> > gaps;
    
    J=pbtmInputVars.srcSentVec.size();
    this->extract_gaps(hyp,gaps);
    for(unsigned int i=0;i<gaps.size();++i)
    {
      result+=heuristicScoreVec[gaps[i].second-1][J-gaps[i].first];	
    }
    return result;
  }
  else
  {
        // TO-DO
    return 0;
  }
}

//---------------------------------
template<class HYPOTHESIS>
void _phraseBasedTransModel<HYPOTHESIS>::initHeuristicLocaltd(int maxSrcPhraseLength)
{
  initHeuristicLocalt(maxSrcPhraseLength);
}

//---------------------------------
template<class HYPOTHESIS>
Score _phraseBasedTransModel<HYPOTHESIS>::heuristicLocaltd(const Hypothesis& hyp)
{

  if(state==MODEL_TRANS_STATE)
  {
    Score result=0;
    
        // Get local t heuristic information
    unsigned int J=pbtmInputVars.srcSentVec.size();
    Vector<pair<PositionIndex,PositionIndex> > gaps;
    this->extract_gaps(hyp,gaps);
    for(unsigned int i=0;i<gaps.size();++i)
    {
      result+=heuristicScoreVec[gaps[i].second-1][J-gaps[i].first];	
    }

        // Distortion heuristic information
    PositionIndex lastSrcPosCovered=getLastSrcPosCovered(hyp);
    Vector<unsigned int> jumps=min_jumps(gaps,lastSrcPosCovered);
    for(unsigned int k=0;k<jumps.size();++k)
      result+=srcJumpScore(jumps[k]);

    return result;
  }
  else
  {
        // TO-DO
    return 0;
  }
}

//---------------------------------
template<class HYPOTHESIS>
Vector<unsigned int> _phraseBasedTransModel<HYPOTHESIS>::min_jumps(const Vector<pair<PositionIndex,PositionIndex> >& gaps,
                                                                   PositionIndex lastSrcPosCovered)const
{
  Vector<unsigned int> result;
  PositionIndex j=lastSrcPosCovered;
  for(unsigned int k=0;k<gaps.size();++k)
  {
    if(j>gaps[k].first)
      result.push_back(j-gaps[k].first);
    else
      result.push_back(gaps[k].first-j);

    j=gaps[k].second;
  }
  
  return result;
}

//---------------------------------
template<class HYPOTHESIS>
WordIndex _phraseBasedTransModel<HYPOTHESIS>::stringToSrcWordIndex(std::string s)const
{
  return this->phrModelInfoPtr->invPbModelPtr->stringToTrgWordIndex(s);    
}

//---------------------------------
template<class HYPOTHESIS>
std::string _phraseBasedTransModel<HYPOTHESIS>::wordIndexToSrcString(WordIndex w)const
{
  return this->phrModelInfoPtr->invPbModelPtr->wordIndexToTrgString(w);      
}

//--------------------------------- 
template<class HYPOTHESIS>
Vector<std::string> _phraseBasedTransModel<HYPOTHESIS>::srcIndexVectorToStrVector(Vector<WordIndex> srcidxVec)const
{
  Vector<std::string> vStr;
  unsigned int i;

  for(i=0;i<srcidxVec.size();++i)
    vStr.push_back(wordIndexToSrcString(srcidxVec[i])); 	 
	
  return vStr;
}

//--------------------------------- 
template<class HYPOTHESIS>
Vector<WordIndex> _phraseBasedTransModel<HYPOTHESIS>::strVectorToSrcIndexVector(Vector<std::string> srcStrVec)const
{
  Vector<WordIndex> widxVec;
  unsigned int i;

  for(i=0;i<srcStrVec.size();++i)
    widxVec.push_back(stringToSrcWordIndex(srcStrVec[i])); 	 
	
  return widxVec;
}

//--------------------------------- 
template<class HYPOTHESIS>
WordIndex _phraseBasedTransModel<HYPOTHESIS>::stringToTrgWordIndex(std::string s)const
{
  return this->phrModelInfoPtr->invPbModelPtr->stringToSrcWordIndex(s);
}

//--------------------------------- 
template<class HYPOTHESIS>
std::string _phraseBasedTransModel<HYPOTHESIS>::wordIndexToTrgString(WordIndex w)const
{
  return this->phrModelInfoPtr->invPbModelPtr->wordIndexToSrcString(w);  
}

//--------------------------------- 
template<class HYPOTHESIS>
Vector<std::string> _phraseBasedTransModel<HYPOTHESIS>::trgIndexVectorToStrVector(Vector<WordIndex> trgidxVec)const
{
  Vector<std::string> vStr;
  unsigned int i;

  for(i=0;i<trgidxVec.size();++i)
    vStr.push_back(wordIndexToTrgString(trgidxVec[i])); 	 
	
  return vStr;
}

//--------------------------------- 
template<class HYPOTHESIS>
Vector<WordIndex> _phraseBasedTransModel<HYPOTHESIS>::strVectorToTrgIndexVector(Vector<std::string> trgStrVec)const
{
  Vector<WordIndex> widxVec;
  unsigned int i;

  for(i=0;i<trgStrVec.size();++i)
    widxVec.push_back(stringToTrgWordIndex(trgStrVec[i])); 	 
	
  return widxVec;
}

//---------------------------------
template<class HYPOTHESIS>
std::string _phraseBasedTransModel<HYPOTHESIS>::phraseToStr(const Vector<WordIndex>& phr)const
{
  std::string s;
  Vector<std::string> svec;

  svec=phraseToStrVec(phr);
  for(unsigned int i=0;i<svec.size();++i)
   {
    if(i==0) s=svec[0];
    else s=s+" "+svec[i];
  }
  return s;  
}

//---------------------------------
template<class HYPOTHESIS>
Vector<std::string> _phraseBasedTransModel<HYPOTHESIS>::phraseToStrVec(const Vector<WordIndex>& phr)const
{
  return trgIndexVectorToStrVector(phr);
}

//---------------------------------
template<class HYPOTHESIS>
WordIndex _phraseBasedTransModel<HYPOTHESIS>::tmVocabToLmVocab(WordIndex w)
{
  std::map<WordIndex,WordIndex>::const_iterator mapIter;
  
  mapIter=tmToLmVocMap.find(w);
  if(mapIter==tmToLmVocMap.end())
  {
        // w not found
        // Obtain string from index
    std::string s=wordIndexToTrgString(w);
        // Add string to the lm vocabulary if necessary
    if(!langModelInfoPtr->lModelPtr->existSymbol(s))
    {
      WordIndex nw=langModelInfoPtr->lModelPtr->stringToWordIndex(UNK_SYMBOL_STR);
      tmToLmVocMap[w]=nw;
      return nw;
    }
    else
    {
          // Map tm word to lm word
      WordIndex nw=langModelInfoPtr->lModelPtr->stringToWordIndex(s);
      tmToLmVocMap[w]=nw;
      return nw;
    } 
  }
  else
  {
        // w found
    return mapIter->second;
  }  
}

//---------------------------------
template<class HYPOTHESIS>
void _phraseBasedTransModel<HYPOTHESIS>::initTmToLmVocabMap(void)
{
  tmToLmVocMap.clear();
  tmToLmVocMap[UNK_WORD]=langModelInfoPtr->lModelPtr->stringToWordIndex(UNK_SYMBOL_STR);
}

//--------------- _phraseBasedTransModel class methods
//

//---------------------------------
template<class HYPOTHESIS>
void _phraseBasedTransModel<HYPOTHESIS>::pre_trans_actions(std::string srcsent)
{
      // Clear temporary variables
  clearTempVars();

      // Set state info
  state=MODEL_TRANS_STATE;
  
      // Store source sentence to be translated
  this->trConstraintsPtr->obtainTransConstraints(srcsent,this->verbosity);
  pbtmInputVars.srcSentVec=this->trConstraintsPtr->getSrcSentVec();
  
      // Verify coverage for source
  if(this->verbosity>0)
    cerr<<"Verify model coverage for source sentence..."<<endl; 
  verifyDictCoverageForSentence(pbtmInputVars.srcSentVec,this->pbTransModelPars.A);

      // Store source sentence as an array of WordIndex.
      // Note: this must be done after verifying the coverage for the
      // source sentence since it may contain unknown words

      // Init source sentence index vector after the coverage has been
      // verified
  pbtmInputVars.srcSentIdVec.clear();
  pbtmInputVars.nsrcSentIdVec.clear();
  pbtmInputVars.nsrcSentIdVec.push_back(NULL_WORD);
  for(unsigned int i=0;i<pbtmInputVars.srcSentVec.size();++i)
  {
    WordIndex w=stringToSrcWordIndex(pbtmInputVars.srcSentVec[i]);
    pbtmInputVars.srcSentIdVec.push_back(w);
    pbtmInputVars.nsrcSentIdVec.push_back(w);
  }

      // Initialize heuristic (the source sentence must be previously
      // stored)
  if(this->verbosity>0)
    cerr<<"Initializing information about search heuristic..."<<endl; 
  initHeuristic(this->pbTransModelPars.A);
}

//---------------------------------
template<class HYPOTHESIS>
void _phraseBasedTransModel<HYPOTHESIS>::pre_trans_actions_ref(std::string srcsent,
                                                               std::string refsent)
{
      // Clear temporary variables
  clearTempVars();

        // Set state info
  state=MODEL_TRANSREF_STATE;

      // Store source sentence to be translated
  pbtmInputVars.srcSentVec=StrProcUtils::stringToStringVector(srcsent);

      // Verify coverage for source
  if(this->verbosity>0)
    cerr<<"Verify model coverage for source sentence..."<<endl; 
  verifyDictCoverageForSentence(pbtmInputVars.srcSentVec,this->pbTransModelPars.A);

      // Init source sentence index vector after the coverage has been
      // verified
  pbtmInputVars.srcSentIdVec.clear();
  pbtmInputVars.nsrcSentIdVec.clear();
  pbtmInputVars.nsrcSentIdVec.push_back(NULL_WORD);
  for(unsigned int i=0;i<pbtmInputVars.srcSentVec.size();++i)
  {
    WordIndex w=stringToSrcWordIndex(pbtmInputVars.srcSentVec[i]);
    pbtmInputVars.srcSentIdVec.push_back(w);
    pbtmInputVars.nsrcSentIdVec.push_back(w);
  }

      // Store reference sentence
  pbtmInputVars.refSentVec=StrProcUtils::stringToStringVector(refsent);

  pbtmInputVars.nrefSentIdVec.clear();
  pbtmInputVars.nrefSentIdVec.push_back(NULL_WORD);
  for(unsigned int i=0;i<pbtmInputVars.refSentVec.size();++i)
  {
    WordIndex w=stringToTrgWordIndex(pbtmInputVars.refSentVec[i]);
    if(w==UNK_WORD && this->verbosity>0)
      cerr<<"Warning: word "<<pbtmInputVars.refSentVec[i]<<" is not contained in the phrase model vocabulary, ensure that your language model contains the unknown-word token."<<endl;
    pbtmInputVars.nrefSentIdVec.push_back(w);
  }

      // Initialize heuristic (the source sentence must be previously
      // stored)
  if(this->verbosity>0)
    cerr<<"Initializing information about search heuristic..."<<endl; 
  initHeuristic(this->pbTransModelPars.A);
}

//---------------------------------
template<class HYPOTHESIS>
void _phraseBasedTransModel<HYPOTHESIS>::pre_trans_actions_ver(std::string srcsent,
                                                               std::string refsent)
{
      // Clear temporary variables
  clearTempVars();

        // Set state info
  state=MODEL_TRANSVER_STATE;

      // Store source sentence to be translated
  pbtmInputVars.srcSentVec=StrProcUtils::stringToStringVector(srcsent);

      // Verify coverage for source
  if(this->verbosity>0)
    cerr<<"Verify model coverage for source sentence..."<<endl; 
  verifyDictCoverageForSentence(pbtmInputVars.srcSentVec,this->pbTransModelPars.A);

      // Init source sentence index vector after the coverage has been
      // verified
  pbtmInputVars.srcSentIdVec.clear();
  pbtmInputVars.nsrcSentIdVec.clear();
  pbtmInputVars.nsrcSentIdVec.push_back(NULL_WORD);
  for(unsigned int i=0;i<pbtmInputVars.srcSentVec.size();++i)
  {
    WordIndex w=stringToSrcWordIndex(pbtmInputVars.srcSentVec[i]);
    pbtmInputVars.srcSentIdVec.push_back(w);
    pbtmInputVars.nsrcSentIdVec.push_back(w);
  }

      // Store reference sentence
  pbtmInputVars.refSentVec=StrProcUtils::stringToStringVector(refsent);

  pbtmInputVars.nrefSentIdVec.clear();
  pbtmInputVars.nrefSentIdVec.push_back(NULL_WORD);
  for(unsigned int i=0;i<pbtmInputVars.refSentVec.size();++i)
  {
    WordIndex w=stringToTrgWordIndex(pbtmInputVars.refSentVec[i]);
    if(w==UNK_WORD && this->verbosity>0)
      cerr<<"Warning: word "<<pbtmInputVars.refSentVec[i]<<" is not contained in the phrase model vocabulary, ensure that your language model contains the unknown-word token."<<endl;
    pbtmInputVars.nrefSentIdVec.push_back(w);
  }

      // Initialize heuristic (the source sentence must be previously
      // stored)
  if(this->verbosity>0)
    cerr<<"Initializing information about search heuristic..."<<endl; 
  initHeuristic(this->pbTransModelPars.A);
}

//---------------------------------
template<class HYPOTHESIS>
void _phraseBasedTransModel<HYPOTHESIS>::pre_trans_actions_prefix(std::string srcsent,
                                                                  std::string prefix)
{
      // Clear temporary variables
  clearTempVars();

      // Set state info
  state=MODEL_TRANSPREFIX_STATE;

      // Store source sentence to be translated
  pbtmInputVars.srcSentVec=StrProcUtils::stringToStringVector(srcsent);

      // Verify coverage for source
  if(this->verbosity>0)
    cerr<<"Verify model coverage for source sentence..."<<endl; 
  verifyDictCoverageForSentence(pbtmInputVars.srcSentVec,this->pbTransModelPars.A);

      // Init source sentence index vector after the coverage has been
      // verified
  pbtmInputVars.srcSentIdVec.clear();
  pbtmInputVars.nsrcSentIdVec.clear();
  pbtmInputVars.nsrcSentIdVec.push_back(NULL_WORD);
  for(unsigned int i=0;i<pbtmInputVars.srcSentVec.size();++i)
  {
    WordIndex w=stringToSrcWordIndex(pbtmInputVars.srcSentVec[i]);
    pbtmInputVars.srcSentIdVec.push_back(w);
    pbtmInputVars.nsrcSentIdVec.push_back(w);
  }

      // Store prefix sentence
  if(lastCharIsBlank(prefix)) pbtmInputVars.lastCharOfPrefIsBlank=true;
  else pbtmInputVars.lastCharOfPrefIsBlank=false;
  pbtmInputVars.prefSentVec=StrProcUtils::stringToStringVector(prefix);

  pbtmInputVars.nprefSentIdVec.clear();
  pbtmInputVars.nprefSentIdVec.push_back(NULL_WORD);
  for(unsigned int i=0;i<pbtmInputVars.prefSentVec.size();++i)
  {
    WordIndex w=stringToTrgWordIndex(pbtmInputVars.prefSentVec[i]);
    if(w==UNK_WORD && this->verbosity>0)
      cerr<<"Warning: word "<<pbtmInputVars.prefSentVec[i]<<" is not contained in the phrase model vocabulary, ensure that your language model contains the unknown-word token."<<endl;
    pbtmInputVars.nprefSentIdVec.push_back(w);
  }

      // Initialize heuristic (the source sentence must be previously
      // stored)
  if(this->verbosity>0)
    cerr<<"Initializing information about search heuristic..."<<endl; 
  initHeuristic(this->pbTransModelPars.A);
}

//---------------------------------
template<class HYPOTHESIS>
std::string _phraseBasedTransModel<HYPOTHESIS>::getCurrentSrcSent(void)
{
  return StrProcUtils::stringVectorToString(pbtmInputVars.srcSentVec);
}

//---------------------------------
template<class HYPOTHESIS>
void _phraseBasedTransModel<HYPOTHESIS>::addSentenceToWordPred(Vector<std::string> strVec,
                                                               int verbose/*=0*/)
{
  switch(this->onlineTrainingPars.onlineLearningAlgorithm)
  {
    case BASIC_INCR_TRAINING:
      incrAddSentenceToWordPred(strVec,verbose);
      break;
    case MINIBATCH_TRAINING:
      minibatchAddSentenceToWordPred(strVec,verbose);
      break;
    case BATCH_RETRAINING:
      batchAddSentenceToWordPred(strVec,verbose);
      break;
    default:
      cerr<<"Warning: requested online update of word predictor with id="<<this->onlineTrainingPars.onlineLearningAlgorithm<<" is not implemented."<<endl;
      break;
  }
}

//---------------------------------
template<class HYPOTHESIS>
void _phraseBasedTransModel<HYPOTHESIS>::incrAddSentenceToWordPred(Vector<std::string> strVec,
                                                                   int verbose/*=0*/)
{
  if(verbose)
    cerr<<"Adding a new sentence to word predictor..."<<endl;
  langModelInfoPtr->wordPredictor.addSentence(strVec);
}

//---------------------------------
template<class HYPOTHESIS>
void _phraseBasedTransModel<HYPOTHESIS>::minibatchAddSentenceToWordPred(Vector<std::string> strVec,
                                                                        int verbose/*=0*/)
{
      // Store sentence
  wordPredSentVec.push_back(strVec);
  
      // Check if a mini-batch has to be processed
      // (onlineTrainingPars.learnStepSize determines the size of the
      // mini-batch)
  unsigned int batchSize=(unsigned int)this->onlineTrainingPars.learnStepSize;
  if(!wordPredSentVec.empty() &&
     (wordPredSentVec.size()%batchSize)==0)
  {
    if(verbose)
      cerr<<"Adding "<<batchSize<<" sentences to word predictor..."<<endl;
    
    for(unsigned int i=0;i<wordPredSentVec.size();++i)
      langModelInfoPtr->wordPredictor.addSentence(wordPredSentVec[i]);
    wordPredSentVec.clear();
  }
}

//---------------------------------
template<class HYPOTHESIS>
void _phraseBasedTransModel<HYPOTHESIS>::batchAddSentenceToWordPred(Vector<std::string> strVec,
                                                                    int verbose/*=0*/)
{
      // Store sentence
  wordPredSentVec.push_back(strVec);
  
      // Check if a mini-batch has to be processed
      // (onlineTrainingPars.learnStepSize determines the size of the
      // mini-batch)
  unsigned int batchSize=(unsigned int)this->onlineTrainingPars.learnStepSize;
  if(!wordPredSentVec.empty() &&
     (wordPredSentVec.size()%batchSize)==0)
  {
    if(verbose)
      cerr<<"Adding "<<batchSize<<" sentences to word predictor..."<<endl;
    
    for(unsigned int i=0;i<wordPredSentVec.size();++i)
      langModelInfoPtr->wordPredictor.addSentence(wordPredSentVec[i]);
    wordPredSentVec.clear();
  }
}

//---------------------------------
template<class HYPOTHESIS>
pair<Count,std::string>
_phraseBasedTransModel<HYPOTHESIS>::getBestSuffix(std::string input)
{
  return langModelInfoPtr->wordPredictor.getBestSuffix(input);
}

//---------------------------------
template<class HYPOTHESIS>
pair<Count,std::string>
_phraseBasedTransModel<HYPOTHESIS>::getBestSuffixGivenHist(Vector<std::string> hist,
                                                           std::string input)
{
  WordPredictor::SuffixList suffixList;
  WordPredictor::SuffixList::iterator suffixListIter;
  LgProb lp;
  LgProb maxlp=-FLT_MAX;
  pair<Count,std::string> bestCountSuffix;

      // Get suffix list for input
  langModelInfoPtr->wordPredictor.getSuffixList(input,suffixList);
  if(suffixList.size()==0)
  {
        // There are not any suffix
    return make_pair(0,"");
  }
  else
  {
        // There are one or more suffixes
    LM_State lmState;
    LM_State aux;

        // Initialize language model state given history
    langModelInfoPtr->lModelPtr->getStateForBeginOfSentence(lmState);
    for(unsigned int i=0;i<hist.size();++i)
    {
      langModelInfoPtr->lModelPtr->getNgramLgProbGivenState(langModelInfoPtr->lModelPtr->stringToWordIndex(hist[i]),lmState);
    }

        // Obtain probability for each suffix given history
    for(suffixListIter=suffixList.begin();suffixListIter!=suffixList.end();++suffixListIter)
    {
      std::string lastw;
      
      aux=lmState;
      lastw=input+suffixListIter->second;
      lp=langModelInfoPtr->lModelPtr->getNgramLgProbGivenState(langModelInfoPtr->lModelPtr->stringToWordIndex(lastw),aux);
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
template<class HYPOTHESIS>
void _phraseBasedTransModel<HYPOTHESIS>::expand(const Hypothesis& hyp,
                                                Vector<Hypothesis>& hypVec,
                                                Vector<Vector<Score> >& scrCompVec)
{
  Vector<pair<PositionIndex,PositionIndex> > gaps;
  Vector<WordIndex> s_;
  Hypothesis extHyp;
  Vector<HypDataType> hypDataVec;
  Vector<Score> scoreComponents;
  
  hypVec.clear();
  scrCompVec.clear();
  
      // Extract gaps	
  extract_gaps(hyp,gaps);
  if(this->verbosity>=2)
  {
    cerr<<"  gaps: "<<gaps.size()<<endl;
  }
   
      // Generate new hypotheses completing the gaps
  for(unsigned int k=0;k<gaps.size();++k)
  {
    unsigned int gap_length=gaps[k].second-gaps[k].first+1;
    for(unsigned int x=0;x<gap_length;++x)
    {
      s_.clear();
      if(x<=this->pbTransModelPars.U) // x should be lower than U, which is the maximum
               // number of words that can be jUmped
      {
        for(unsigned int y=x;y<gap_length;++y)
        {
          unsigned int segmRightMostj=gaps[k].first+y;
          unsigned int segmLeftMostj=gaps[k].first+x;
          bool srcPhraseIsAffectedByConstraint=this->trConstraintsPtr->srcPhrAffectedByConstraint(make_pair(segmLeftMostj,segmRightMostj));
              // Verify that the source phrase length does not exceed
              // the limit. The limit can be exceeded when the source
              // phrase is affected by a translation constraint
          if((segmRightMostj-segmLeftMostj)+1 > this->pbTransModelPars.A && !srcPhraseIsAffectedByConstraint)
            break;
              // Obtain hypothesis data vector
          getHypDataVecForGap(hyp,segmLeftMostj,segmRightMostj,hypDataVec,this->pbTransModelPars.W);
          if(hypDataVec.size()!=0)
          {
            for(unsigned int i=0;i<hypDataVec.size();++i)
            {
                  // Create hypothesis extension
              this->incrScore(hyp,hypDataVec[i],extHyp,scoreComponents);
                  // Obtain information about hypothesis extension
              Vector<std::string> targetWordVec=this->getTransInPlainTextVec(extHyp);
              Vector<pair<PositionIndex,PositionIndex> > aligPos;
              this->aligMatrix(extHyp,aligPos);
                  // Check if translation constraints are satisfied
              if(this->trConstraintsPtr->translationSatisfiesConstraints(targetWordVec,aligPos))
              {
                hypVec.push_back(extHyp);
                scrCompVec.push_back(scoreComponents);
              }
            }
#           ifdef THOT_STATS    
            this->basePbTmStats.transOptions+=hypDataVec.size();
            ++this->basePbTmStats.getTransCalls;
#           endif    
          }
        }
      }
    }
  }   
}

//---------------------------------
template<class HYPOTHESIS>
void _phraseBasedTransModel<HYPOTHESIS>::expand_ref(const Hypothesis& hyp,
                                                    Vector<Hypothesis>& hypVec,
                                                    Vector<Vector<Score> >& scrCompVec)
{
  Vector<pair<PositionIndex,PositionIndex> > gaps;
  Vector<WordIndex> s_;
  Hypothesis extHyp;
  Vector<HypDataType> hypDataVec;
  Vector<Score> scoreComponents;

  hypVec.clear();
  scrCompVec.clear();
  
      // Extract gaps	
  extract_gaps(hyp,gaps);
   
      // Generate new hypotheses completing the gaps
  for(unsigned int k=0;k<gaps.size();++k)
  {
    unsigned int gap_length=gaps[k].second-gaps[k].first+1;
    for(unsigned int x=0;x<gap_length;++x)
    {
      s_.clear();
      if(x<=this->pbTransModelPars.U) // x should be lower than U, which is the maximum
                     // number of words that can be jUmped
      {
        for(unsigned int y=x;y<gap_length;++y)
        {
          unsigned int segmRightMostj=gaps[k].first+y;
          unsigned int segmLeftMostj=gaps[k].first+x;
              // Verify that the source phrase length does not exceed
              // the limit
          if((segmRightMostj-segmLeftMostj)+1 > this->pbTransModelPars.A) 
            break;
              // Obtain hypothesis data vector
          getHypDataVecForGapRef(hyp,segmLeftMostj,segmRightMostj,hypDataVec,this->pbTransModelPars.W);
          if(hypDataVec.size()!=0)
          {
            for(unsigned int i=0;i<hypDataVec.size();++i)
            {
              this->incrScore(hyp,hypDataVec[i],extHyp,scoreComponents);
              hypVec.push_back(extHyp);
              scrCompVec.push_back(scoreComponents);
            }
#           ifdef THOT_STATS    
            ++this->basePbTmStats.getTransCalls;
            this->basePbTmStats.transOptions+=hypDataVec.size();
#           endif    
          }
        }
      }
    }
  }     
}

//---------------------------------
template<class HYPOTHESIS>
void _phraseBasedTransModel<HYPOTHESIS>::expand_ver(const Hypothesis& hyp,
                                                    Vector<Hypothesis>& hypVec,
                                                    Vector<Vector<Score> >& scrCompVec)
{
  Vector<pair<PositionIndex,PositionIndex> > gaps;
  Vector<WordIndex> s_;
  Hypothesis extHyp;
  Vector<HypDataType> hypDataVec;
  Vector<Score> scoreComponents;

  hypVec.clear();
  scrCompVec.clear();
  
      // Extract gaps	
  extract_gaps(hyp,gaps);
   
      // Generate new hypotheses completing the gaps
  for(unsigned int k=0;k<gaps.size();++k)
  {
    unsigned int gap_length=gaps[k].second-gaps[k].first+1;
    for(unsigned int x=0;x<gap_length;++x)
    {
      s_.clear();
      if(x<=this->pbTransModelPars.U) // x should be lower than U, which is the maximum
               // number of words that can be jUmped
      {
        for(unsigned int y=x;y<gap_length;++y)
        {
          unsigned int segmRightMostj=gaps[k].first+y;
          unsigned int segmLeftMostj=gaps[k].first+x;
              // Verify that the source phrase length does not exceed
              // the limit
          if((segmRightMostj-segmLeftMostj)+1 > this->pbTransModelPars.A) 
            break;
              // Obtain hypothesis data vector
          getHypDataVecForGapVer(hyp,segmLeftMostj,segmRightMostj,hypDataVec,this->pbTransModelPars.W);
          if(hypDataVec.size()!=0)
          {
            for(unsigned int i=0;i<hypDataVec.size();++i)
            {
              this->incrScore(hyp,hypDataVec[i],extHyp,scoreComponents);
              hypVec.push_back(extHyp);
              scrCompVec.push_back(scoreComponents);
            }
#           ifdef THOT_STATS    
            ++this->basePbTmStats.getTransCalls;
            this->basePbTmStats.transOptions+=hypDataVec.size();
#           endif    
          }
        }
      }
    }
  }       
}

//---------------------------------
template<class HYPOTHESIS>
void _phraseBasedTransModel<HYPOTHESIS>::expand_prefix(const Hypothesis& hyp,
                                                       Vector<Hypothesis>& hypVec,
                                                       Vector<Vector<Score> >& scrCompVec)
{
  Vector<pair<PositionIndex,PositionIndex> > gaps;
  Vector<WordIndex> s_;
  Hypothesis extHyp;
  Vector<HypDataType> hypDataVec;
  Vector<Score> scoreComponents;

  hypVec.clear();
  scrCompVec.clear();

      // Extract gaps	
  extract_gaps(hyp,gaps);
   
      // Generate new hypotheses completing the gaps
  for(unsigned int k=0;k<gaps.size();++k)
  {
    unsigned int gap_length=gaps[k].second-gaps[k].first+1;
    for(unsigned int x=0;x<gap_length;++x)
    {
      s_.clear();
      if(x<=this->pbTransModelPars.U) // x should be lower than U, which is the maximum
               // number of words that can be jUmped
      {
        for(unsigned int y=x;y<gap_length;++y)
        {
          unsigned int segmRightMostj=gaps[k].first+y;
          unsigned int segmLeftMostj=gaps[k].first+x;
              // Verify that the source phrase length does not exceed
              // the limit
          if((segmRightMostj-segmLeftMostj)+1 > this->pbTransModelPars.A) 
            break;
              // Obtain hypothesis data vector
          getHypDataVecForGapPref(hyp,segmLeftMostj,segmRightMostj,hypDataVec,this->pbTransModelPars.W);
          if(hypDataVec.size()!=0)
          {
            for(unsigned int i=0;i<hypDataVec.size();++i)
            {
              this->incrScore(hyp,hypDataVec[i],extHyp,scoreComponents);
              hypVec.push_back(extHyp);
              scrCompVec.push_back(scoreComponents);
            }
#           ifdef THOT_STATS    
            ++this->basePbTmStats.getTransCalls;
            this->basePbTmStats.transOptions+=hypDataVec.size();
#           endif    
          }
        }
      }
    }
  }       
}

//---------------------------------
template<class HYPOTHESIS>
void _phraseBasedTransModel<HYPOTHESIS>::extract_gaps(const Hypothesis& hyp,
                                                      Vector<pair<PositionIndex,PositionIndex> >& gaps)
{
  extract_gaps(hyp.getKey(),gaps);
}

//---------------------------------
template<class HYPOTHESIS>
void _phraseBasedTransModel<HYPOTHESIS>::extract_gaps(const Bitset<MAX_SENTENCE_LENGTH_ALLOWED>& hypKey,
                                                      Vector<pair<PositionIndex,PositionIndex> >& gaps)
{
      // Extract all uncovered gaps
  pair<PositionIndex,PositionIndex> gap;
  unsigned int srcSentLen=this->numberOfUncoveredSrcWordsHypData(this->nullHypothesisHypData());
  unsigned int j;
  
      // Extract gaps
  gaps.clear();
  bool crossing_a_gap=false;	
	
  for(j=1;j<=srcSentLen;++j)
  {
    if(crossing_a_gap==false && hypKey.test(j)==0)
    {
      crossing_a_gap=true;
      gap.first=j;
    }
	
    if(crossing_a_gap==true && hypKey.test(j)==1)
    {
      crossing_a_gap=false;
      gap.second=j-1;
      gaps.push_back(gap);	
    }	
  }
  if(crossing_a_gap==true)
  {
    gap.second=j-1;
    gaps.push_back(gap);
  }
}

//---------------------------------
template<class HYPOTHESIS>
unsigned int
_phraseBasedTransModel<HYPOTHESIS>::get_num_gaps(const Bitset<MAX_SENTENCE_LENGTH_ALLOWED>& hypKey)
{
      // Count all uncovered gaps
  unsigned int result=0;
  unsigned int j;
  bool crossing_a_gap;
  unsigned int srcSentLen;
  HypDataType nullHypData=this->nullHypothesisHypData();

  srcSentLen=this->numberOfUncoveredSrcWordsHypData(nullHypData);
  
      // count gaps	
  crossing_a_gap=false;	
	
  for(j=1;j<=srcSentLen;++j)
  {
    if(crossing_a_gap==false && hypKey.test(j)==0)
    {
      crossing_a_gap=true;
    }
	
    if(crossing_a_gap==true && hypKey.test(j)==1)
    {
      crossing_a_gap=false;
      ++result;	
    }	
  }
  if(crossing_a_gap==true)
  {
    ++result;
  }
  return result;
}

//---------------------------------
template<class HYPOTHESIS>
bool _phraseBasedTransModel<HYPOTHESIS>::getHypDataVecForGap(const Hypothesis& hyp,
                                                             PositionIndex srcLeft,
                                                             PositionIndex srcRight,
                                                             Vector<HypDataType>& hypDataTypeVec,
                                                             float N)
{
  NbestTableNode<PhraseTransTableNodeData> ttNode;
  NbestTableNode<PhraseTransTableNodeData>::iterator ttNodeIter;
  HypDataType hypData=hyp.getData();
  HypDataType newHypData;

  hypDataTypeVec.clear();

      // Obtain translations for gap
  getTransForHypUncovGap(hyp,srcLeft,srcRight,ttNode,N);

  if(this->verbosity>=2)
  {
    cerr<<"  trying to cover from src. pos. "<<srcLeft<<" to "<<srcRight<<"; ";
    cerr<<"Filtered "<<ttNode.size()<<" translations"<<endl;
  }

      // Generate hypothesis data for translations
  for(ttNodeIter=ttNode.begin();ttNodeIter!=ttNode.end();++ttNodeIter)
  {
    if(this->verbosity>=3)
    {
      cerr<<"   ";
      for(unsigned int i=srcLeft;i<=srcRight;++i) cerr<<this->pbtmInputVars.srcSentVec[i-1]<<" ";
      cerr<<"||| ";
      for(unsigned int i=0;i<ttNodeIter->second.size();++i)
        cerr<<this->wordIndexToTrgString(ttNodeIter->second[i])<<" ";
      cerr<<"||| "<<ttNodeIter->first<<endl;
    }

    newHypData=hypData;
    extendHypDataIdx(srcLeft,srcRight,ttNodeIter->second,newHypData);
    hypDataTypeVec.push_back(newHypData);
  }

      // Return boolean value
  if(hypDataTypeVec.empty()) return false;
  else return true;
}

//---------------------------------
template<class HYPOTHESIS>
bool _phraseBasedTransModel<HYPOTHESIS>::getHypDataVecForGapRef(const Hypothesis& hyp,
                                                                PositionIndex srcLeft,
                                                                PositionIndex srcRight,
                                                                Vector<HypDataType>& hypDataTypeVec,
                                                                float N)
{
  NbestTableNode<PhraseTransTableNodeData> ttNode;
  NbestTableNode<PhraseTransTableNodeData>::iterator ttNodeIter;
  HypDataType hypData=hyp.getData();
  HypDataType newHypData;

  hypDataTypeVec.clear();
  
  getTransForHypUncovGapRef(hyp,srcLeft,srcRight,ttNode,N);

  if(this->verbosity>=2)
  {
    cerr<<"  trying to cover from src. pos. "<<srcLeft<<" to "<<srcRight<<"; ";
    cerr<<"Filtered "<<ttNode.size()<<" translations"<<endl;
  }

  for(ttNodeIter=ttNode.begin();ttNodeIter!=ttNode.end();++ttNodeIter)
  {
    if(this->verbosity>=3)
    {
      cerr<<"   ";
      for(unsigned int i=srcLeft;i<=srcRight;++i) cerr<<this->pbtmInputVars.srcSentVec[i-1]<<" ";
      cerr<<"||| ";
      for(unsigned int i=0;i<ttNodeIter->second.size();++i)
        cerr<<this->wordIndexToTrgString(ttNodeIter->second[i])<<" ";
      cerr<<"||| "<<ttNodeIter->first<<endl;
    }

    newHypData=hypData;
    extendHypDataIdx(srcLeft,srcRight,ttNodeIter->second,newHypData);
    bool equal;
    if(hypDataTransIsPrefixOfTargetRef(newHypData,equal))
    {
      if((this->isCompleteHypData(newHypData) && equal) || !this->isCompleteHypData(newHypData))
        hypDataTypeVec.push_back(newHypData);
    }
  }
  if(hypDataTypeVec.empty()) return false;
  else return true;  
}

//---------------------------------
template<class HYPOTHESIS>
bool _phraseBasedTransModel<HYPOTHESIS>::getHypDataVecForGapVer(const Hypothesis& hyp,
                                                                PositionIndex srcLeft,
                                                                PositionIndex srcRight,
                                                                Vector<HypDataType>& hypDataTypeVec,
                                                                float N)
{
  NbestTableNode<PhraseTransTableNodeData> ttNode;
  NbestTableNode<PhraseTransTableNodeData>::iterator ttNodeIter;
  HypDataType hypData=hyp.getData();
  HypDataType newHypData;

  hypDataTypeVec.clear();
  
  getTransForHypUncovGapVer(hyp,srcLeft,srcRight,ttNode,N);

  if(this->verbosity>=2)
  {
    cerr<<"  trying to cover from src. pos. "<<srcLeft<<" to "<<srcRight<<"; ";
    cerr<<"Filtered "<<ttNode.size()<<" translations"<<endl;
  }

  for(ttNodeIter=ttNode.begin();ttNodeIter!=ttNode.end();++ttNodeIter)
  {
    if(this->verbosity>=3)
    {
      cerr<<"   ";
      for(unsigned int i=srcLeft;i<=srcRight;++i) cerr<<this->pbtmInputVars.srcSentVec[i-1]<<" ";
      cerr<<"||| ";
      for(unsigned int i=0;i<ttNodeIter->second.size();++i)
        cerr<<this->wordIndexToTrgString(ttNodeIter->second[i])<<" ";
      cerr<<"||| "<<ttNodeIter->first<<endl;
    }

    newHypData=hypData;
    extendHypDataIdx(srcLeft,srcRight,ttNodeIter->second,newHypData);
    bool equal;
    if(hypDataTransIsPrefixOfTargetRef(newHypData,equal))
    {
      if((this->isCompleteHypData(newHypData) && equal) || !this->isCompleteHypData(newHypData))
        hypDataTypeVec.push_back(newHypData);
    }
  }
  if(hypDataTypeVec.empty()) return false;
  else return true;  
}

//---------------------------------
template<class HYPOTHESIS>
bool _phraseBasedTransModel<HYPOTHESIS>::getHypDataVecForGapPref(const Hypothesis& hyp,
                                                                 PositionIndex srcLeft,
                                                                 PositionIndex srcRight,
                                                                 Vector<HypDataType>& hypDataTypeVec,
                                                                 float N)
{
  NbestTableNode<PhraseTransTableNodeData> ttNode;
  NbestTableNode<PhraseTransTableNodeData>::iterator ttNodeIter;
  HypDataType hypData=hyp.getData();
  HypDataType newHypData;

  hypDataTypeVec.clear();
  
  getTransForHypUncovGapPref(hyp,srcLeft,srcRight,ttNode,N);

  if(this->verbosity>=2)
  {
    cerr<<"  trying to cover from src. pos. "<<srcLeft<<" to "<<srcRight<<"; ";
    cerr<<"Filtered "<<ttNode.size()<<" translations"<<endl;
  }

  for(ttNodeIter=ttNode.begin();ttNodeIter!=ttNode.end();++ttNodeIter)
  {
    if(this->verbosity>=3)
    {
      cerr<<"   ";
      for(unsigned int i=srcLeft;i<=srcRight;++i) cerr<<this->pbtmInputVars.srcSentVec[i-1]<<" ";
      cerr<<"||| ";
      for(unsigned int i=0;i<ttNodeIter->second.size();++i)
        cerr<<this->wordIndexToTrgString(ttNodeIter->second[i])<<" ";
      cerr<<"||| "<<ttNodeIter->first<<endl;
    }

    newHypData=hypData;
    extendHypDataIdx(srcLeft,srcRight,ttNodeIter->second,newHypData);
    hypDataTypeVec.push_back(newHypData);
  }
  if(hypDataTypeVec.empty()) return false;
  else return true;
}

//---------------------------------
template<class HYPOTHESIS>
bool _phraseBasedTransModel<HYPOTHESIS>::getTransForHypUncovGap(const Hypothesis& /*hyp*/,
                                                                PositionIndex srcLeft,
                                                                PositionIndex srcRight,
                                                                NbestTableNode<PhraseTransTableNodeData>& nbt,
                                                                float N)
{
      // Check if gap is affected by translation constraints
  if(this->trConstraintsPtr->srcPhrAffectedByConstraint(make_pair(srcLeft,srcRight)))
  {
        // Obtain constrained target translation for gap (if any)
    Vector<std::string> trgWordVec=this->trConstraintsPtr->getTransForSrcPhr(make_pair(srcLeft,srcRight));
    if(trgWordVec.size()>0)
    {
          // Convert string vector to WordIndex vector
      Vector<WordIndex> trgWiVec;
      for(unsigned int i=0;i<trgWordVec.size();++i)
      {
        WordIndex w=stringToTrgWordIndex(trgWordVec[i]);
        trgWiVec.push_back(w);
      }
      
          // Insert translation into n-best table
      nbt.clear();      
      nbt.insert(0,trgWiVec);      
      return true;
    }
    else
    {
          // No constrained target translation was found
      nbt.clear();
      return false;
    } 
  }
  else
  {
        // The gap to be covered is not affected by translation constraints
       
        // Check if source phrase has only one word and this word has
        // been marked as an unseen word
    if(srcLeft==srcRight && unseenSrcWord(pbtmInputVars.srcSentVec[srcLeft-1]))
    {
      Vector<WordIndex> unkWordVec;
      unkWordVec.push_back(UNK_WORD);
      nbt.clear();
      nbt.insert(0,unkWordVec);
      return false;
    }
    else
    {
          // search translations for s in translation table
      NbestTableNode<PhraseTransTableNodeData> *transTableNodePtr;
      Vector<WordIndex> s_;
    
      for(unsigned int i=srcLeft;i<=srcRight;++i)
      {
        s_.push_back(pbtmInputVars.nsrcSentIdVec[i]);
      }
    
      transTableNodePtr=nbTransCacheData.cPhrNbestTransTable.getTranslationsForKey(make_pair(srcLeft,srcRight));
      if(transTableNodePtr!=NULL)
      {
            // translation present in the cache translation table
        nbt=*transTableNodePtr;
        if(nbt.size()==0) return false;
        else return true;
      }
      else
      {   
        getNbestTransFor_s_(s_,nbt,N);
        nbTransCacheData.cPhrNbestTransTable.insertEntry(make_pair(srcLeft,srcRight),nbt);
        if(nbt.size()==0) return false;
        else return true;
      }
    }
  }
}

//---------------------------------
template<class HYPOTHESIS>
bool _phraseBasedTransModel<HYPOTHESIS>::getTransForHypUncovGapRef(const Hypothesis& hyp,
                                                                   PositionIndex srcLeft,
                                                                   PositionIndex srcRight,
                                                                   NbestTableNode<PhraseTransTableNodeData>& nbt,
                                                                   float N)
{
  Vector<WordIndex> s_;
  Vector<WordIndex> t_;
  Vector<WordIndex> ntarget;

      // Obtain source phrase
  for(unsigned int i=srcLeft;i<=srcRight;++i)
  {
    s_.push_back(pbtmInputVars.nsrcSentIdVec[i]);
  }

      // Obtain length limits for target phrase
  unsigned int minTrgSize=0;
  if(s_.size()>this->pbTransModelPars.E) minTrgSize=s_.size()-this->pbTransModelPars.E;
  unsigned int maxTrgSize=s_.size()+this->pbTransModelPars.E;

  ntarget=hyp.getPartialTrans();	

  nbt.clear();
  if(ntarget.size()>pbtmInputVars.nrefSentIdVec.size()) return false;
  if(this->numberOfUncoveredSrcWords(hyp)-(srcRight-srcLeft+1)>0)
  {
        // This is not the last gap to be covered
    NbestTableNode<PhraseTransTableNodeData> *transTableNodePtr;
    PhrNbestTransTableRefKey pNbtRefKey;

    pNbtRefKey.srcLeft=srcLeft;
    pNbtRefKey.srcRight=srcRight;
    pNbtRefKey.ntrgSize=ntarget.size();
        // The number of gaps to be covered AFTER covering
        // s_{srcLeft}...s_{srcRight} is obtained to ensure that the
        // resulting hypotheses have at least as many gaps as reference
        // words to add
    if(this->pbTransModelPars.U==0)
      pNbtRefKey.numGaps=1;
    else
    {
      Bitset<MAX_SENTENCE_LENGTH_ALLOWED> key=hyp.getKey();
      for(unsigned int i=srcLeft;i<=srcRight;++i) key.set(i);
      pNbtRefKey.numGaps=this->get_num_gaps(key);
    }
     
        // Search the required translations in the cache translation
        // table
    
    transTableNodePtr=nbTransCacheData.cPhrNbestTransTableRef.getTranslationsForKey(pNbtRefKey);
    if(transTableNodePtr!=NULL)
    {// translations present in the cache translation table
      nbt=*transTableNodePtr;
    }
    else
    {// translations not present in the cache translation table
      for(PositionIndex i=ntarget.size();i<pbtmInputVars.nrefSentIdVec.size()-pNbtRefKey.numGaps;++i)
      {
        t_.push_back(pbtmInputVars.nrefSentIdVec[i]);
        if(t_.size()>=minTrgSize && t_.size()<=maxTrgSize)
        {
          Score scr=nbestTransScoreCached(s_,t_);
          nbt.insert(scr,t_);
        }
      }
          // Prune the list
      if(N>=1)
        while(nbt.size()>(unsigned int) N) nbt.removeLastElement();
      else
      {
        Score bscr=nbt.getScoreOfBestElem();
        nbt.pruneGivenThreshold(bscr+(double)log(N));
      }
          // Store the list in cPhrNbestTransTableRef
      nbTransCacheData.cPhrNbestTransTableRef.insertEntry(pNbtRefKey,nbt);
    }
  }
  else
  {
        // The last gap will be covered
    for(PositionIndex i=ntarget.size();i<pbtmInputVars.nrefSentIdVec.size();++i)
      t_.push_back(pbtmInputVars.nrefSentIdVec[i]);
    if(t_.size()>=minTrgSize && t_.size()<=maxTrgSize)
    {
      Score scr=nbestTransScoreCached(s_,t_);
      nbt.insert(scr,t_);
    }
  }
  if(nbt.size()==0) return false;
  else return true;
}

//---------------------------------
template<class HYPOTHESIS>
bool _phraseBasedTransModel<HYPOTHESIS>::getTransForHypUncovGapVer(const Hypothesis& hyp,
                                                                   PositionIndex srcLeft,
                                                                   PositionIndex srcRight,
                                                                   NbestTableNode<PhraseTransTableNodeData>& nbt,
                                                                   float N)
{
  return getTransForHypUncovGap(hyp,srcLeft,srcRight,nbt,N);
}

//---------------------------------
template<class HYPOTHESIS>
bool _phraseBasedTransModel<HYPOTHESIS>::getTransForHypUncovGapPref(const Hypothesis& hyp,
                                                                    PositionIndex srcLeft,
                                                                    PositionIndex srcRight,
                                                                    NbestTableNode<PhraseTransTableNodeData>& nbt,
                                                                    float N)
{
  unsigned int ntrgSize=hyp.getPartialTrans().size();
      // Check if the prefix has been generated
  if(ntrgSize<pbtmInputVars.nprefSentIdVec.size())
  {
        // The prefix has not been generated
    NbestTableNode<PhraseTransTableNodeData> *transTableNodePtr;
    PhrNbestTransTablePrefKey pNbtPrefKey;

    pNbtPrefKey.srcLeft=srcLeft;
    pNbtPrefKey.srcRight=srcRight;
    pNbtPrefKey.ntrgSize=ntrgSize;
    if(this->numberOfUncoveredSrcWords(hyp)-(srcRight-srcLeft+1)>0)
      pNbtPrefKey.lastGap=false;
    else pNbtPrefKey.lastGap=true;
    
        // Search the required translations in the cache translation
        // table
    transTableNodePtr=nbTransCacheData.cPhrNbestTransTablePref.getTranslationsForKey(pNbtPrefKey);
    if(transTableNodePtr!=NULL)
    {// translations present in the cache translation table
      nbt=*transTableNodePtr;
    }
    else
    {
          // Obtain list
      transUncovGapPrefNoGen(hyp,srcLeft,srcRight,nbt);
      
          // Prune the list
      if(N>=1)
        while(nbt.size()>(unsigned int) N) nbt.removeLastElement();
      else
      {
        Score bscr=nbt.getScoreOfBestElem();
        nbt.pruneGivenThreshold(bscr+(double)log(N));
      }
          // Store the list in cPhrNbestTransTablePref
      nbTransCacheData.cPhrNbestTransTablePref.insertEntry(pNbtPrefKey,nbt);
    }
    if(nbt.size()==0) return false;
    else return true;
  }
  else
  {
        // The prefix has been completely generated, the nbest list
        // is obtained as if no prefix was given
    return getTransForHypUncovGap(hyp,srcLeft,srcRight,nbt,N);
  }
}

//---------------------------------
template<class HYPOTHESIS>
void _phraseBasedTransModel<HYPOTHESIS>::transUncovGapPrefNoGen(const Hypothesis& hyp,
                                                                PositionIndex srcLeft,
                                                                PositionIndex srcRight,
                                                                NbestTableNode<PhraseTransTableNodeData>& nbt)
{
  Vector<WordIndex> s_;
    
      // Obtain source phrase
  nbt.clear();
  for(unsigned int i=srcLeft;i<=srcRight;++i)
  {
    s_.push_back(pbtmInputVars.nsrcSentIdVec[i]);
  }
      // Obtain length limits for target phrase
  unsigned int minTrgSize=0;
  if(s_.size()>this->pbTransModelPars.E) minTrgSize=s_.size()-this->pbTransModelPars.E;
  unsigned int maxTrgSize=s_.size()+this->pbTransModelPars.E;
    
  unsigned int ntrgSize=hyp.getPartialTrans().size();

      // Check if we are covering the last gap of the hypothesis
  if(this->numberOfUncoveredSrcWords(hyp)-(srcRight-srcLeft+1)>0)
  {
        // This is not the last gap to be covered.

        // Add translations with length in characters greater than the
        // prefix length.
    genListOfTransLongerThanPref(s_,ntrgSize,nbt);

        // Add translations with length lower than the prefix length.
    Vector<WordIndex> t_;
    if(pbtmInputVars.nprefSentIdVec.size()>1)
    {
      for(PositionIndex i=ntrgSize;i<pbtmInputVars.nprefSentIdVec.size()-1;++i)
      {
        t_.push_back(pbtmInputVars.nprefSentIdVec[i]);
        if(t_.size()>=minTrgSize && t_.size()<=maxTrgSize)
        {
          Score scr=nbestTransScoreCached(s_,t_);
          nbt.insert(scr,t_);
        }
      }
    }
  }
  else
  {
        // This is the last gap to be covered.

        // Add translations with length in characters greater than the
        // prefix length.
    genListOfTransLongerThanPref(s_,ntrgSize,nbt);
  }
  
      // Insert the remaining prefix itself in nbt
  Vector<WordIndex> remainingPref;
  for(unsigned int i=ntrgSize;i<pbtmInputVars.nprefSentIdVec.size();++i)
    remainingPref.push_back(pbtmInputVars.nprefSentIdVec[i]);
  nbt.insert(nbestTransScoreLastCached(s_,remainingPref),remainingPref);
}

//---------------------------------
template<class HYPOTHESIS>
bool _phraseBasedTransModel<HYPOTHESIS>::getTransForInvPbModel(const Vector<WordIndex>& s_,
                                                               std::set<Vector<WordIndex> >& transSet)
{
  if(incrInvMuxPmPtr)
  {
        // Obtain translation options vector for multiplexed models
    Vector<BasePhraseModel::SrcTableNode> srctnVec;
    bool ret=incrInvMuxPmPtr->getTransVecFor_t_(s_,srctnVec);

        // Create translation options data structure
    transSet.clear();
    for(unsigned int i=0;i<srctnVec.size();++i)
    {
      for(BasePhraseModel::SrcTableNode::iterator iter=srctnVec[i].begin(); iter!=srctnVec[i].end(); ++iter)
      {
            // Add new entry
        transSet.insert(iter->first);
      }
    }
    return ret;
  }
  else
  {
        // Obtain translation options vector for model
    BasePhraseModel::SrcTableNode srctn;
    bool ret=this->phrModelInfoPtr->invPbModelPtr->getTransFor_t_(s_,srctn);
    
        // Create translation options data structure
    transSet.clear();
    for(BasePhraseModel::SrcTableNode::iterator iter=srctn.begin(); iter!=srctn.end(); ++iter)
    {
          // Add new entry
      transSet.insert(iter->first);
    }
    return ret;
  }
}

//---------------------------------
template<class HYPOTHESIS>
void _phraseBasedTransModel<HYPOTHESIS>::genListOfTransLongerThanPref(Vector<WordIndex> s_,
                                                                      unsigned int ntrgSize,
                                                                      NbestTableNode<PhraseTransTableNodeData>& nbt)
{
  Vector<WordIndex> remainingPref;

      // clear nbt
  nbt.clear();
  
      // Store the remaining prefix to be generated in remainingPref
  for(unsigned int i=ntrgSize;i<pbtmInputVars.nprefSentIdVec.size();++i)
    remainingPref.push_back(pbtmInputVars.nprefSentIdVec[i]);

      // Obtain translations for source segment s_
  std::set<Vector<WordIndex> > transSet;
  getTransForInvPbModel(s_,transSet);
  for(std::set<Vector<WordIndex> >::iterator transSetIter=transSet.begin();transSetIter!=transSet.end();++transSetIter)
  {
        // Filter those translations whose length in words is
        // greater or equal than the remaining prefix length
    if(transSetIter->size()>=remainingPref.size())
    {
          // Filter those translations having "remainingPref"
          // as prefix
      bool equal;
      if(trgWordVecIsPrefix(remainingPref,
                            pbtmInputVars.lastCharOfPrefIsBlank,
                            pbtmInputVars.prefSentVec.back(),
                            *transSetIter,
                            equal))
      {
            // Filter translations not exactly equal to "remainingPref"
        if(!equal)
        {
          Score scr=nbestTransScoreLastCached(s_,*transSetIter);
          nbt.insert(scr,*transSetIter);
        }
      }
    }
  }
}

//---------------------------------
template<class HYPOTHESIS>
bool _phraseBasedTransModel<HYPOTHESIS>::trgWordVecIsPrefix(const Vector<WordIndex>& wiVec1,
                                                            bool lastWiVec1WordIsComplete,
                                                            const std::string& lastWiVec1Word,
                                                            const Vector<WordIndex>& wiVec2,
                                                            bool& equal)
{
  equal=false;
  
      // returns true if target word vector wiVec1 is a prefix of wiVec2
  if(wiVec1.size()>wiVec2.size()) return false;

  for(unsigned int i=0;i<wiVec1.size();++i)
  {
    if(wiVec1[i]!=wiVec2[i])
    {
      if(i==wiVec1.size()-1 && !lastWiVec1WordIsComplete)
      {
        if(!StrProcUtils::isPrefix(lastWiVec1Word,wordIndexToTrgString(wiVec2[i])))
          return false;
      }
      else return false;
    }
  }

  if(wiVec1.size()==wiVec2.size() &&
     lastWiVec1Word==wordIndexToTrgString(wiVec2.back()))
  {
    equal=true;
  }
  
  return true;
}

//---------------------------------
template<class HYPOTHESIS>
bool _phraseBasedTransModel<HYPOTHESIS>::getNbestTransFor_s_(Vector<WordIndex> s_,
                                                             NbestTableNode<PhraseTransTableNodeData>& nbt,
                                                             float N)
{
  BasePhraseModel::SrcTableNode srctn;
  BasePhraseModel::SrcTableNode::iterator srctnIter;
  bool ret;

      // Obtain the whole list of translations
  nbt.clear();
  std::set<Vector<WordIndex> > transSet;
  ret=getTransForInvPbModel(s_,transSet);
  if(!ret) return false;
  else
  {
    Score scr;

        // This loop may become a bottleneck if the number of translation
        // options is high
    for(std::set<Vector<WordIndex> >::iterator transSetIter=transSet.begin();transSetIter!=transSet.end();++transSetIter)
    {
      scr=nbestTransScoreCached(s_,*transSetIter);
      nbt.insert(scr,*transSetIter);
    }
  }
      // Prune the list depending on the value of N
      // retrieve translations from table
  if(N>=1)
    while(nbt.size()>(unsigned int) N) nbt.removeLastElement();
  else
  {
    Score bscr=nbt.getScoreOfBestElem();    
    nbt.pruneGivenThreshold(bscr+(double)log(N));
  }
  return true;
}

//---------------------------------
template<class HYPOTHESIS>
Score _phraseBasedTransModel<HYPOTHESIS>::nbestTransScoreCached(const Vector<WordIndex>& s_,
                                                                const Vector<WordIndex>& t_)
{
  PhrasePairCacheTable::iterator ppctIter;
  ppctIter=nbTransCacheData.cnbestTransScore.find(make_pair(s_,t_));
  if(ppctIter!=nbTransCacheData.cnbestTransScore.end())
  {
        // Score was previously stored in the cache table
    return ppctIter->second;
  }
  else
  {
        // Score is not stored in the cache table
    Score scr=nbestTransScore(s_,t_);
    nbTransCacheData.cnbestTransScore[make_pair(s_,t_)]=scr;
    return scr;
  }
}

//---------------------------------
template<class HYPOTHESIS>
Score _phraseBasedTransModel<HYPOTHESIS>::nbestTransScoreLastCached(const Vector<WordIndex>& s_,
                                                                    const Vector<WordIndex>& t_)
{
  PhrasePairCacheTable::iterator ppctIter;
  ppctIter=nbTransCacheData.cnbestTransScoreLast.find(make_pair(s_,t_));
  if(ppctIter!=nbTransCacheData.cnbestTransScoreLast.end())
  {
        // Score was previously stored in the cache table
    return ppctIter->second;
  }
  else
  {
        // Score is not stored in the cache table
    Score scr=nbestTransScoreLast(s_,t_);
    nbTransCacheData.cnbestTransScoreLast[make_pair(s_,t_)]=scr;
    return scr;
  }
}

//---------------------------------
template<class HYPOTHESIS>
void _phraseBasedTransModel<HYPOTHESIS>::addHeuristicToHyp(Hypothesis& hyp)
{
  hyp.addHeuristic(calcHeuristicScore(hyp));
}

//---------------------------------
template<class HYPOTHESIS>
void _phraseBasedTransModel<HYPOTHESIS>::subtractHeuristicToHyp(Hypothesis& hyp)
{
  hyp.subtractHeuristic(calcHeuristicScore(hyp));
}

//---------------------------------
template<class HYPOTHESIS>
void _phraseBasedTransModel<HYPOTHESIS>::setHeuristic(unsigned int _heuristicId)
{
  heuristicId=_heuristicId;
}

//---------------------------------
template<class HYPOTHESIS>
Score _phraseBasedTransModel<HYPOTHESIS>::calcHeuristicScore(const _phraseBasedTransModel::Hypothesis& hyp)
{
  Score score=0;

  if(state==MODEL_TRANSREF_STATE)
  {
        // translation with reference
    score+=calcRefLmHeurScore(hyp);
  }
  if(state==MODEL_TRANSPREFIX_STATE)
  {
        // translation with prefix
    score+=calcPrefLmHeurScore(hyp);
  }

  switch(heuristicId)
  {
    case NO_HEURISTIC:
      break;
    case LOCAL_T_HEURISTIC:
      score+=heuristicLocalt(hyp);
      break;
    case LOCAL_TD_HEURISTIC:
      score+=heuristicLocaltd(hyp);
      break;
  }
  return score;
}

//---------------------------------
template<class HYPOTHESIS>
void _phraseBasedTransModel<HYPOTHESIS>::printHyp(const Hypothesis& hyp,
                                                  ostream &outS,
                                                  int verbose)
{
  Vector<std::string> trgStrVec;
  Vector<WordIndex> trans=hyp.getPartialTrans();
  SourceSegmentation sourceSegmentation;
  Vector<PositionIndex> targetSegmentCuts;
  Vector<pair<PositionIndex,PositionIndex> > amatrix;
  HypDataType hypDataType;
  Hypothesis auxHyp;
  Vector<Score> scoreComponents;
  
      // Obtain target string vector
  trgStrVec=trgIndexVectorToStrVector(hyp.getPartialTrans());

      // Print score
  outS <<"Score: "<<hyp.getScore()<<" ; ";
      // Print weights
  this->printWeights(outS);
  outS <<" ; ";
      // Obtain score components
  hypDataType=hyp.getData();
  this->incrScore(this->nullHypothesis(),hypDataType,auxHyp,scoreComponents);
      // Print score components
  for(unsigned int i=0;i<scoreComponents.size();++i)
    outS<<scoreComponents[i]<<" ";

      // Print score + heuristic
  addHeuristicToHyp(auxHyp);
  outS <<"; Score+heur: "<<auxHyp.getScore()<<" ";
    
      // Print warning if the alignment is not complete
  if(!this->isComplete(hyp)) outS<< "; Incomplete_alignment!";

      // Obtain phrase alignment
  this->aligMatrix(hyp,amatrix);
  this->getPhraseAlignment(amatrix,sourceSegmentation,targetSegmentCuts);

      // Print alignment information
  outS<<" | ";
  for(unsigned int i=1;i<trgStrVec.size();++i)
    outS<<trgStrVec[i]<<" ";
  outS << "| ";
  for(unsigned int k=0;k<sourceSegmentation.size();k++)
 	outS<<"( "<<sourceSegmentation[k].first<<" , "<<sourceSegmentation[k].second<<" ) "; 
  outS<< "| "; 
  for (unsigned int j=0; j<targetSegmentCuts.size(); j++)
    outS << targetSegmentCuts[j] << " ";
  
      // Print hypothesis key
  outS<<"| hypkey: "<<hyp.getKey()<<" ";

      // Print hypothesis equivalence class
  outS<<"| hypEqClass: "<<hyp.getEqClass()<<endl;

  if(verbose)
  {
    unsigned int numSteps=sourceSegmentation.size()-1;
    outS<<"----------------------------------------------"<<endl;
    outS<<"Score components for previous expansion steps:"<<endl;
    auxHyp=hyp;
    while(this->obtainPredecessor(auxHyp))
    {
      scoreComponents=scoreCompsForHyp(auxHyp);
      outS<<"Step "<<numSteps<<" : ";
      for(unsigned int i=0;i<scoreComponents.size();++i)
      {
        outS<<scoreComponents[i]<<" ";
      }
      outS<<endl;
      --numSteps;
    }
    outS<<"----------------------------------------------"<<endl;
  }
#ifdef THOT_DEBUG
      // Print debug information
  for(unsigned int i=0;i<hyp.hDebug.size();++i)
  {
    hyp.hDebug[i].print(outS);
  }
#endif 
}

//---------------------------------
template<class HYPOTHESIS>
void _phraseBasedTransModel<HYPOTHESIS>::extendHypData(PositionIndex srcLeft,
                                                       PositionIndex srcRight,
                                                       const Vector<std::string>& trgPhrase,
                                                       HypDataType& hypd)
{
  Vector<WordIndex> trgPhraseIdx;
  
  for(unsigned int i=0;i<trgPhrase.size();++i)
    trgPhraseIdx.push_back(stringToTrgWordIndex(trgPhrase[i]));
  extendHypDataIdx(srcLeft,srcRight,trgPhraseIdx,hypd);
}

//---------------------------------
template<class HYPOTHESIS>
Vector<std::string> _phraseBasedTransModel<HYPOTHESIS>::getTransInPlainTextVec(const _phraseBasedTransModel::Hypothesis& hyp)const
{
  switch(state)
  {
    case MODEL_TRANS_STATE: return getTransInPlainTextVecTs(hyp);
    case MODEL_TRANSPREFIX_STATE: return getTransInPlainTextVecTps(hyp);
    case MODEL_TRANSREF_STATE: return getTransInPlainTextVecTrs(hyp);
    case MODEL_TRANSVER_STATE: return getTransInPlainTextVecTvs(hyp);
    default: Vector<std::string> strVec;
      return strVec;
  }
}

//---------------------------------
template<class HYPOTHESIS>
Vector<std::string> _phraseBasedTransModel<HYPOTHESIS>::getTransInPlainTextVecTs(const _phraseBasedTransModel::Hypothesis& hyp)const
{
  Vector<WordIndex> nvwi;
  Vector<WordIndex> vwi;

      // Obtain vector of WordIndex
  nvwi=hyp.getPartialTrans();
  for(unsigned int i=1;i<nvwi.size();++i)
  {
    vwi.push_back(nvwi[i]);
  }
      // Obtain vector of strings
  Vector<std::string> trgVecStr=trgIndexVectorToStrVector(vwi);

      // Treat unknown words contained in trgVecStr. Model is being used
      // to translate a sentence
    
      // Replace unknown words affected by constraints

      // Iterate over constraints
  std::set<pair<PositionIndex,PositionIndex> > srcPhrSet=this->trConstraintsPtr->getConstrainedSrcPhrases();
  std::set<pair<PositionIndex,PositionIndex> >::const_iterator const_iter;
  for(const_iter=srcPhrSet.begin();const_iter!=srcPhrSet.end();++const_iter)
  {
        // Obtain target translation for constraint
    Vector<std::string> trgPhr=this->trConstraintsPtr->getTransForSrcPhr(*const_iter);
    
        // Find first aligned target word
    for(unsigned int i=0;i<trgVecStr.size();++i)
    {
      if(hyp.areAligned(const_iter->first,i+1))
      {
        for(unsigned int k=0;k<trgPhr.size();++k)
        {
          if(trgVecStr[i+k]==UNK_WORD_STR)
            trgVecStr[i+k]=trgPhr[k];                      
        }
            // Replace unknown words and finish
        break;
      }
    }
  }
  
      // Replace unknown words not affected by constraints
  for(unsigned int i=0;i<trgVecStr.size();++i)
  {
    if(trgVecStr[i]==UNK_WORD_STR)
    {
          // Find source word aligned with unknown word
      for(unsigned int j=0;j<pbtmInputVars.srcSentVec.size();++j)
      {
        if(hyp.areAligned(j+1,i+1))
        {
          trgVecStr[i]=pbtmInputVars.srcSentVec[j];
          break;
        }
      }
    }
  }
  return trgVecStr;
}

//---------------------------------
template<class HYPOTHESIS>
Vector<std::string> _phraseBasedTransModel<HYPOTHESIS>::getTransInPlainTextVecTps(const _phraseBasedTransModel::Hypothesis& hyp)const
{
  Vector<WordIndex> nvwi;
  Vector<WordIndex> vwi;

      // Obtain vector of WordIndex
  nvwi=hyp.getPartialTrans();
  for(unsigned int i=1;i<nvwi.size();++i)
  {
    vwi.push_back(nvwi[i]);
  }
      // Obtain vector of strings
  Vector<std::string> trgVecStr=trgIndexVectorToStrVector(vwi);

      // Treat unknown words contained in trgVecStr. Model is being used
      // to translate a sentence given a prefix
        
      // Replace unknown words from trgVecStr
  for(unsigned int i=0;i<trgVecStr.size();++i)
  {
    if(trgVecStr[i]==UNK_WORD_STR)
    {
      if(i<pbtmInputVars.prefSentVec.size())
      {
            // Unknown word must be replaced by a prefix word
        trgVecStr[i]=pbtmInputVars.prefSentVec[i];
      }
      else
      {
            // Find source word aligned with unknown word
        for(unsigned int j=0;j<pbtmInputVars.srcSentVec.size();++j)
        {
          if(hyp.areAligned(j+1,i+1))
          {
            trgVecStr[i]=pbtmInputVars.srcSentVec[j];
            break;
          }
        }
      }
    }
  }
  return trgVecStr;
}

//---------------------------------
template<class HYPOTHESIS>
Vector<std::string> _phraseBasedTransModel<HYPOTHESIS>::getTransInPlainTextVecTrs(const _phraseBasedTransModel::Hypothesis& hyp)const
{
    Vector<WordIndex> nvwi;
  Vector<WordIndex> vwi;

      // Obtain vector of WordIndex
  nvwi=hyp.getPartialTrans();
  for(unsigned int i=1;i<nvwi.size();++i)
  {
    vwi.push_back(nvwi[i]);
  }
      // Obtain vector of strings
  Vector<std::string> trgVecStr=trgIndexVectorToStrVector(vwi);

      // Treat unknown words contained in trgVecStr. Model is being used
      // to generate a reference.
  for(unsigned int i=0;i<trgVecStr.size();++i)
  {
    if(i<pbtmInputVars.refSentVec.size())
      trgVecStr[i]=pbtmInputVars.refSentVec[i];
  }
  return trgVecStr;
}

//---------------------------------
template<class HYPOTHESIS>
Vector<std::string> _phraseBasedTransModel<HYPOTHESIS>::getTransInPlainTextVecTvs(const _phraseBasedTransModel::Hypothesis& hyp)const
{
  Vector<WordIndex> nvwi;
  Vector<WordIndex> vwi;

      // Obtain vector of WordIndex
  nvwi=hyp.getPartialTrans();
  for(unsigned int i=1;i<nvwi.size();++i)
  {
    vwi.push_back(nvwi[i]);
  }
      // Obtain vector of strings
  Vector<std::string> trgVecStr=trgIndexVectorToStrVector(vwi);

      // Treat unknown words contained in trgVecStr. Model is being used
      // to verify model coverage
  for(unsigned int i=0;i<trgVecStr.size();++i)
  {
    if(i<pbtmInputVars.refSentVec.size())
      trgVecStr[i]=pbtmInputVars.refSentVec[i];
  }
  return trgVecStr;
}

//---------------------------------
template<class HYPOTHESIS>
Vector<Score>
_phraseBasedTransModel<HYPOTHESIS>::scoreCompsForHyp(const _phraseBasedTransModel::Hypothesis& hyp)
{
  HypDataType hypDataType;
  Hypothesis auxHyp;
  Vector<Score> scoreComponents;
  
      // Obtain score components
  hypDataType=hyp.getData();
  this->incrScore(this->nullHypothesis(),hypDataType,auxHyp,scoreComponents);

  return scoreComponents;
}

//---------------------------------
template<class HYPOTHESIS>
_phraseBasedTransModel<HYPOTHESIS>::~_phraseBasedTransModel()
{
}

//-------------------------

#endif
