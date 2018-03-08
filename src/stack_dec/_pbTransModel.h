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
 
/**
 * @file _pbTransModel.h
 *
 * @brief Declares the _pbTransModel class.  This class is a
 * succesor of the BasePbTransModel class.
 */

#ifndef __pbTransModel_h
#define __pbTransModel_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "OnTheFlyDictFeat.h"
#include "DirectPhraseModelFeat.h"
#include "FeaturesInfo.h"
#include "BasePbTransModel.h"
#include "PhraseTransTableNodeData.h"
#include "NbestTableNode.h"
#include "NbestTransTable.h"
#include "SingleWordVocab.h"
#include "SourceSegmentation.h"
#include "WordPredictor.h"
#include "PbTransModelInputVars.h"
#include "NbestTransCacheData.h"
#include "StatModelDefs.h"
#include "Prob.h"
#include <math.h>
#include <set>
#include "StrProcUtils.h"

//--------------- Constants ------------------------------------------

#define NO_HEURISTIC            0
#define LOCAL_T_HEURISTIC       4
#define LOCAL_TD_HEURISTIC      6
#define MODEL_IDLE_STATE        1
#define MODEL_TRANS_STATE       2
#define MODEL_TRANSREF_STATE    3
#define MODEL_TRANSVER_STATE    4
#define MODEL_TRANSPREFIX_STATE 5
#define DEFAULT_LOGLIN_WEIGHT   1

//--------------- Classes --------------------------------------------

//--------------- _pbTransModel class

/**
 * @brief The _pbTransModel class is a predecessor of the
 * Base_pbTransModel class.
 */

template<class HYPOTHESIS>
class _pbTransModel: public BasePbTransModel<HYPOTHESIS>
{
 public:

  typedef typename BasePbTransModel<HYPOTHESIS>::Hypothesis Hypothesis;
  typedef typename BasePbTransModel<HYPOTHESIS>::HypScoreInfo HypScoreInfo;
  typedef typename BasePbTransModel<HYPOTHESIS>::HypDataType HypDataType;

      // Constructor
  _pbTransModel();

      // Link translation constraints with model
  void link_trans_metadata(BaseTranslationMetadata<HypScoreInfo> * _trMetadataPtr);

      // Link features information
  void link_feats_info(FeaturesInfo<HypScoreInfo>* _featuresInfoPtr);

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
  void addSentenceToWordPred(std::vector<std::string> strVec,
                             int verbose=0);
  std::pair<Count,std::string> getBestSuffix(std::string input);
  std::pair<Count,std::string> getBestSuffixGivenHist(std::vector<std::string> hist,
                                                 std::string input);

  ////// Hypothesis-related functions

      // Misc. operations with hypothesis
  Hypothesis nullHypothesis(void);

      // Expansion-related functions
  void expand(const Hypothesis& hyp,
              std::vector<Hypothesis>& hypVec,
              std::vector<std::vector<Score> >& scrCompVec);
  void expand_ref(const Hypothesis& hyp,
                  std::vector<Hypothesis>& hypVec,
                  std::vector<std::vector<Score> >& scrCompVec);
  void expand_ver(const Hypothesis& hyp,
                  std::vector<Hypothesis>& hypVec,
                  std::vector<std::vector<Score> >& scrCompVec);
  void expand_prefix(const Hypothesis& hyp,
                     std::vector<Hypothesis>& hypVec,
                     std::vector<std::vector<Score> >& scrCompVec);

      // Heuristic-related functions
  void setHeuristic(unsigned int _heuristicId);
  void addHeuristicToHyp(Hypothesis& hyp);
  void subtractHeuristicToHyp(Hypothesis& hyp);

      // Printing functions and data conversion
  void printHyp(const Hypothesis& hyp,
                std::ostream &outS,
                int verbose=false);
  std::vector<std::string> getTransInPlainTextVec(const Hypothesis& hyp)const;
      
      // Model weights functions
  void setWeights(std::vector<float> wVec);
  void getWeights(std::vector<std::pair<std::string,float> >& compWeights);
  void printWeights(std::ostream &outS);
  unsigned int getNumWeights(void);
  void getUnweightedComps(const std::vector<Score>& scrComps,
                          std::vector<Score>& unweightedScrComps);
  std::vector<Score> scoreCompsForHyp(const Hypothesis& hyp);

      // Specific phrase-based functions
  void extendHypData(PositionIndex srcLeft,
                     PositionIndex srcRight,
                     const std::vector<std::string>& trgPhrase,
                     HypDataType& hypd);

      // Destructor
  ~_pbTransModel();

 protected:
  
      // Variable to store state of the translation model
  unsigned int state;

      // Feature vector information
  std::vector<float> defaultFeatWeights;
  
  FeaturesInfo<HypScoreInfo>* standardFeaturesInfoPtr;
  std::vector<float> standardFeaturesWeights;
  
  FeaturesInfo<HypScoreInfo> onTheFlyFeaturesInfo;
  std::vector<float> onTheFlyFeaturesWeights;
  
      // Vocabulary handler
  SingleWordVocab singleWordVocab;

      // Heuristic function to be used
  unsigned int heuristicId;

      // Heuristic probability vector
  std::vector<std::vector<Score> > heuristicScoreVec; 

      // Additional data structures to store information about heuristics
  std::vector<LgProb> refHeurLmLgProb;
  std::vector<LgProb> prefHeurLmLgProb;

      // Data structure to store input variables
  PbTransModelInputVars pbtmInputVars;

      // Set of unseen words
  std::set<std::string> unseenWordsSet;

      // Data used to cache n-best translation data
  NbestTransCacheData nbTransCacheData;

  ////// Weight-related functions
  void initFeatWeights(std::vector<float> wVec);
  float getStdFeatWeight(unsigned int i);
  float getOnTheFlyFeatWeight(unsigned int i);
  
  ////// Hypotheses-related functions

      // Expansion-related functions
  void extract_gaps(const Hypothesis& hyp,
                    std::vector<std::pair<PositionIndex,PositionIndex> >& gaps);
  void extract_gaps(const Bitset<MAX_SENTENCE_LENGTH_ALLOWED>& hypKey,
                    std::vector<std::pair<PositionIndex,PositionIndex> >& gaps);
  unsigned int get_num_gaps(const Bitset<MAX_SENTENCE_LENGTH_ALLOWED>& hypKey);

      // Misc. operations with hypothesis
  virtual Score nullHypothesisScrComps(Hypothesis& nullHyp,
                                       std::vector<Score>& scoreComponents)=0;

      // Specific phrase-based functions
  virtual void extendHypDataIdx(PositionIndex srcLeft,
                                PositionIndex srcRight,
                                const std::vector<WordIndex>& trgPhraseIdx,
                                HypDataType& hypd)=0;
  virtual bool getHypDataVecForGap(const Hypothesis& hyp,
                                   PositionIndex srcLeft,
                                   PositionIndex srcRight,
                                   std::vector<HypDataType>& hypDataTypeVec,
                                   float N);
      // Get N-best translations for a subphrase of the source sentence
      // to be translated .  If N is between 0 and 1 then N represents a
      // threshold. 
  virtual bool getHypDataVecForGapRef(const Hypothesis& hyp,
                                      PositionIndex srcLeft,
                                      PositionIndex srcRight,
                                      std::vector<HypDataType>& hypDataTypeVec,
                                      float N);
      // This function is identical to the previous function but is to
      // be used when the translation process is conducted by a given
      // reference sentence
  virtual bool getHypDataVecForGapVer(const Hypothesis& hyp,
                                      PositionIndex srcLeft,
                                      PositionIndex srcRight,
                                      std::vector<HypDataType>& hypDataTypeVec,
                                      float N);
      // This function is identical to the previous function but is to
      // be used when the translation process is performed to verify the
      // coverage of the model given a reference sentence
  virtual bool getHypDataVecForGapPref(const Hypothesis& hyp,
                                       PositionIndex srcLeft,
                                       PositionIndex srcRight,
                                       std::vector<HypDataType>& hypDataTypeVec,
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
  void transUncovGapRefNoLastGapCached(const Hypothesis& hyp,
                                       PositionIndex srcLeft,
                                       PositionIndex srcRight,
                                       NbestTableNode<PhraseTransTableNodeData>& nbt,
                                       float N);
  void transUncovGapRefLastGapCached(const Hypothesis& hyp,
                                     PositionIndex srcLeft,
                                     PositionIndex srcRight,
                                     NbestTableNode<PhraseTransTableNodeData>& nbt);
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
  void transUncovGapPrefNoGenCached(const Hypothesis& hyp,
                                    PositionIndex srcLeft,
                                    PositionIndex srcRight,
                                    NbestTableNode<PhraseTransTableNodeData>& nbt,
                                    float N);
  void transUncovGapPrefNoGen(const Hypothesis& hyp,
                              PositionIndex srcLeft,
                              PositionIndex srcRight,
                              NbestTableNode<PhraseTransTableNodeData>& nbt);
  void genListOfTransLongerThanPref(std::vector<WordIndex> srcPhrase,
                                    unsigned int ntrgSize,
                                    NbestTableNode<PhraseTransTableNodeData>& nbt);
  bool trgWordVecIsPrefix(const std::vector<WordIndex>& wiVec1,
                          bool lastWiVec1WordIsComplete,
                          const std::string& lastWiVec1Word,
                          const std::vector<WordIndex>& wiVec2,
                          bool& equal);
      // returns true if target word vector wiVec1 is a prefix of wiVec2

      // Functions to generate translation lists
  bool getNbestTransForSrcPhraseCached(PositionIndex srcLeft,
                                       PositionIndex srcRight,
                                       NbestTableNode<PhraseTransTableNodeData>& nbt,
                                       float N);
  virtual bool getNbestTransForSrcPhrase(std::vector<WordIndex> srcPhrase,
                                         NbestTableNode<PhraseTransTableNodeData>& nbt,
                                         float N);
      // Get N-best translations for a given source phrase srcPhrase.
      // If N is between 0 and 1 then N represents a threshold
      // Functions to generate translation lists
  bool getTransForSrcPhrase(const std::vector<WordIndex>& srcPhrase,
                            std::set<std::vector<WordIndex> >& transSet);
  bool getTransForSrcPhraseStr(const std::vector<std::string>& srcPhrase,
                               std::set<std::vector<std::string> >& transSet);
  std::string getLogLinFeatNamesForPhrTransStr(std::pair<PositionIndex,PositionIndex> pidxPair,
                                               std::vector<std::string> trgPhr);
  std::vector<std::string> getLogLinFeatNamesForPhrTrans(std::pair<PositionIndex,PositionIndex> pidxPair,
                                                         std::vector<std::string> trgPhr);

      // Functions to score n-best translations lists
  Score nbestTransScore(const std::vector<WordIndex>& srcPhrase,
                        const std::vector<WordIndex>& trgPhrase);
  Score nbestTransScoreLast(const std::vector<WordIndex>& srcPhrase,
                            const std::vector<WordIndex>& t_);
      // Cached functions to score n-best translations lists
  Score nbestTransScoreCached(const std::vector<WordIndex>& srcPhrase,
                              const std::vector<WordIndex>& t_);
  Score nbestTransScoreLastCached(const std::vector<WordIndex>& srcPhrase,
                                  const std::vector<WordIndex>& t_);

      // Functions related to getTransInPlainTextVec
  std::vector<std::string> getTransInPlainTextVecTs(const Hypothesis& hyp)const;
  std::vector<std::string> getTransInPlainTextVecTps(const Hypothesis& hyp)const;
  std::vector<std::string> getTransInPlainTextVecTrs(const Hypothesis& hyp)const;
  std::vector<std::string> getTransInPlainTextVecTvs(const Hypothesis& hyp)const;

      // Heuristic related functions
  void initHeuristic(unsigned int maxSrcPhraseLength);
  Score heurDirectPmScoreLt(const std::vector<WordIndex>& srcPhrase,
                            const std::vector<WordIndex>& trgPhrase);
  Score heurInversePmScoreLt(const std::vector<WordIndex>& srcPhrase,
                             const std::vector<WordIndex>& trgPhrase);
  Score heurLmScoreLtNoAdmiss(const std::vector<WordIndex>& trgPhrase);
  void initHeuristicLocalt(int maxSrcPhraseLength);
  void initHeuristicLocaltd(int maxSrcPhraseLength);
  virtual Score calcHeuristicScore(const _pbTransModel::Hypothesis& hyp);
  Score calcRefLmHeurScore(const _pbTransModel::Hypothesis& hyp);
  Score calcPrefLmHeurScore(const _pbTransModel::Hypothesis& hyp);
  Score heuristicLocalt(const Hypothesis& hyp);
  Score heuristicLocaltd(const Hypothesis& hyp);
  Score getLocalTmHeurScore(const Hypothesis& hyp);
  Score getDistortionHeurScore(const Hypothesis& hyp);
  PositionIndex getLastSrcPosCovered(const Hypothesis& hyp);
      // Get the index of last source position which was covered
  virtual PositionIndex getLastSrcPosCoveredHypData(const HypDataType& hypd)=0;
      // The same as the previous function, but given an object of
      // HypDataType

      // Functions related to pre_trans_actions
  virtual void clearTempVars(void);
  void verifyDictCoverageForSentence(const std::vector<std::string>& sentenceVec,
                                     int maxSrcPhraseLength=MAX_SENTENCE_LENGTH_ALLOWED);
  bool srcPhrHasAtLeastOneValidTranslation(const std::vector<std::string> srcPhraseStr,
                                           const std::set<std::vector<std::string> >& transSetStr);
  void manageUnseenSrcWord(std::string srcw);
  bool unseenSrcWord(std::string srcw);
  bool unseenSrcWordGivenPosition(unsigned int srcPos);
  Score unkWordScoreHeur(void);

      // Vocabulary functions
  WordIndex stringToSrcWordIndex(std::string s);
  std::string wordIndexToSrcString(WordIndex w)const;
  std::vector<std::string> srcIndexVectorToStrVector(std::vector<WordIndex> srcidxVec)const;
  std::vector<WordIndex> strVectorToSrcIndexVector(std::vector<std::string> srcStrVec);
  WordIndex stringToTrgWordIndex(std::string s);
  std::string wordIndexToTrgString(WordIndex w)const;
  std::vector<std::string> trgIndexVectorToStrVector(std::vector<WordIndex> trgidxVec)const;
  std::vector<WordIndex> strVectorToTrgIndexVector(std::vector<std::string> trgStrVec);
};

//--------------- _pbTransModel class functions
//

template<class HYPOTHESIS>
_pbTransModel<HYPOTHESIS>::_pbTransModel(void):BasePbTransModel<HYPOTHESIS>()
{
      // Set state info
  state=MODEL_IDLE_STATE;

      // Initialize feature information pointer
  standardFeaturesInfoPtr=NULL;
  
      // Initially, no heuristic is used
  heuristicId=NO_HEURISTIC;
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::link_trans_metadata(BaseTranslationMetadata<HypScoreInfo> * _trMetadataPtr)
{
      // Link pointer
  this->trMetadataPtr=_trMetadataPtr;
  
      // On-the-fly features are initialized at this point so as to be
      // able to correctly know the features that are available to the
      // decoder from the very beginning
  onTheFlyFeaturesInfo.featPtrVec=this->trMetadataPtr->getOnTheFlyModelFeatures();
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::link_feats_info(FeaturesInfo<HypScoreInfo>* _standardFeaturesInfoPtr)
{
  standardFeaturesInfoPtr=_standardFeaturesInfoPtr;
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::clear(void)
{
      // Set state info
  state=MODEL_IDLE_STATE;

      // Initialize feature information pointer
  standardFeaturesInfoPtr=NULL;

      // Clear weight information for features
  defaultFeatWeights.clear();
  standardFeaturesWeights.clear();
  onTheFlyFeaturesWeights.clear();
  
      // Initially, no heuristic is used
  heuristicId=NO_HEURISTIC;

      // Clear vocabulary
  singleWordVocab.clear();

      // Clear input variables
  pbtmInputVars.clear();
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::pre_trans_actions(std::string srcsent)
{
      // Clear temporary variables
  clearTempVars();

      // Set state info
  state=MODEL_TRANS_STATE;
  
      // Store source sentence to be translated
  this->trMetadataPtr->obtainTransConstraints(srcsent,this->verbosity);
  pbtmInputVars.srcSentVec=this->trMetadataPtr->getSrcSentVec();

      // Initialize on-the-fly features
  onTheFlyFeaturesInfo.featPtrVec=this->trMetadataPtr->getOnTheFlyModelFeatures();

      // Set log-linear weights for sentence if available
  std::vector<float> wVec=this->trMetadataPtr->getLogLinearModelWeightsForSrcSent();
  if(wVec.empty())
    initFeatWeights(defaultFeatWeights);
  else
    initFeatWeights(wVec);
  
      // Verify coverage for source
  if(this->verbosity>0)
    std::cerr<<"Verify model coverage for source sentence..."<<std::endl;
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
    std::cerr<<"Initializing information about search heuristic..."<<std::endl;
  initHeuristic(this->pbTransModelPars.A);
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::pre_trans_actions_ref(std::string srcsent,
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
    std::cerr<<"Verify model coverage for source sentence..."<<std::endl; 
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
      std::cerr<<"Warning: word "<<pbtmInputVars.refSentVec[i]<<" is not contained in the phrase model vocabulary, ensure that your language model contains the unknown-word token."<<std::endl;
    pbtmInputVars.nrefSentIdVec.push_back(w);
  }

      // Initialize heuristic (the source sentence must be previously
      // stored)
  if(this->verbosity>0)
    std::cerr<<"Initializing information about search heuristic..."<<std::endl; 
  initHeuristic(this->pbTransModelPars.A);
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::pre_trans_actions_ver(std::string srcsent,
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
    std::cerr<<"Verify model coverage for source sentence..."<<std::endl; 
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
      std::cerr<<"Warning: word "<<pbtmInputVars.refSentVec[i]<<" is not contained in the phrase model vocabulary, ensure that your language model contains the unknown-word token."<<std::endl;
    pbtmInputVars.nrefSentIdVec.push_back(w);
  }

      // Initialize heuristic (the source sentence must be previously
      // stored)
  if(this->verbosity>0)
    std::cerr<<"Initializing information about search heuristic..."<<std::endl; 
  initHeuristic(this->pbTransModelPars.A);
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::pre_trans_actions_prefix(std::string srcsent,
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
    std::cerr<<"Verify model coverage for source sentence..."<<std::endl; 
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
  if(StrProcUtils::lastCharIsBlank(prefix)) pbtmInputVars.lastCharOfPrefIsBlank=true;
  else pbtmInputVars.lastCharOfPrefIsBlank=false;
  pbtmInputVars.prefSentVec=StrProcUtils::stringToStringVector(prefix);

  pbtmInputVars.nprefSentIdVec.clear();
  pbtmInputVars.nprefSentIdVec.push_back(NULL_WORD);
  for(unsigned int i=0;i<pbtmInputVars.prefSentVec.size();++i)
  {
    WordIndex w=stringToTrgWordIndex(pbtmInputVars.prefSentVec[i]);
    if(w==UNK_WORD && this->verbosity>0)
      std::cerr<<"Warning: word "<<pbtmInputVars.prefSentVec[i]<<" is not contained in the phrase model vocabulary, ensure that your language model contains the unknown-word token."<<std::endl;
    pbtmInputVars.nprefSentIdVec.push_back(w);
  }

      // Initialize heuristic (the source sentence must be previously
      // stored)
  if(this->verbosity>0)
    std::cerr<<"Initializing information about search heuristic..."<<std::endl; 
  initHeuristic(this->pbTransModelPars.A);
}

//---------------------------------
template<class HYPOTHESIS>
std::string _pbTransModel<HYPOTHESIS>::getCurrentSrcSent(void)
{
  return StrProcUtils::stringVectorToString(pbtmInputVars.srcSentVec);
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::addSentenceToWordPred(std::vector<std::string> /*strVec*/,
                                                      int /*verbose=0*/)
{
  std::cerr<<"Warning, addSentenceToWordPred function intentionally not implemented for this class"<<std::endl;
}

//---------------------------------
template<class HYPOTHESIS>
std::pair<Count,std::string>
  _pbTransModel<HYPOTHESIS>::getBestSuffix(std::string /*input*/)
{
  std::cerr<<"Warning, getBestSuffix function intentionally not implemented for this class"<<std::endl;
  std::pair<Count,std::string> result;
  return result;
}

//---------------------------------
template<class HYPOTHESIS>
std::pair<Count,std::string>
  _pbTransModel<HYPOTHESIS>::getBestSuffixGivenHist(std::vector<std::string> /*hist*/,
                                                    std::string /*input*/)
{
  std::cerr<<"Warning, getBestSuffixGivenHist function intentionally not implemented for this class"<<std::endl;
  std::pair<Count,std::string> result;
  return result;
}

//---------------------------------
template<class HYPOTHESIS>
float _pbTransModel<HYPOTHESIS>::getStdFeatWeight(unsigned int i)
{
  if(i<standardFeaturesWeights.size())
    return standardFeaturesWeights[i];
  else
    return DEFAULT_LOGLIN_WEIGHT;
}

//---------------------------------
template<class HYPOTHESIS>
float _pbTransModel<HYPOTHESIS>::getOnTheFlyFeatWeight(unsigned int i)
{
  if(i<onTheFlyFeaturesWeights.size())
    return onTheFlyFeaturesWeights[i];
  else
    return DEFAULT_LOGLIN_WEIGHT;  
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::extract_gaps(const Hypothesis& hyp,
                                             std::vector<std::pair<PositionIndex,PositionIndex> >& gaps)
{
  extract_gaps(hyp.getKey(),gaps);
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::extract_gaps(const Bitset<MAX_SENTENCE_LENGTH_ALLOWED>& hypKey,
                                             std::vector<std::pair<PositionIndex,PositionIndex> >& gaps)
{
      // Extract all uncovered gaps
  std::pair<PositionIndex,PositionIndex> gap;
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
unsigned int _pbTransModel<HYPOTHESIS>::get_num_gaps(const Bitset<MAX_SENTENCE_LENGTH_ALLOWED>& hypKey)
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
typename _pbTransModel<HYPOTHESIS>::Hypothesis
_pbTransModel<HYPOTHESIS>::nullHypothesis(void)
{
  std::vector<Score> scrComp;
  Hypothesis nullHyp;
  nullHypothesisScrComps(nullHyp,scrComp);
  return nullHyp;
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::expand(const Hypothesis& hyp,
                                       std::vector<Hypothesis>& hypVec,
                                       std::vector<std::vector<Score> >& scrCompVec)
{
  std::vector<std::pair<PositionIndex,PositionIndex> > gaps;
  std::vector<WordIndex> srcPhrase;
  Hypothesis extHyp;
  std::vector<HypDataType> hypDataVec;
  std::vector<Score> scoreComponents;
  
  hypVec.clear();
  scrCompVec.clear();
  
      // Extract gaps
  extract_gaps(hyp,gaps);
  if(this->verbosity>=2)
  {
    std::cerr<<"  gaps: "<<gaps.size()<<std::endl;
  }
   
      // Generate new hypotheses completing the gaps
  for(unsigned int k=0;k<gaps.size();++k)
  {
    unsigned int gap_length=gaps[k].second-gaps[k].first+1;
    for(unsigned int x=0;x<gap_length;++x)
    {
      srcPhrase.clear();
      if(x<=this->pbTransModelPars.U) // x should be lower than U, which is the maximum
               // number of words that can be jUmped
      {
        for(unsigned int y=x;y<gap_length;++y)
        {
          unsigned int segmRightMostj=gaps[k].first+y;
          unsigned int segmLeftMostj=gaps[k].first+x;
          bool srcPhraseIsAffectedByConstraint=this->trMetadataPtr->srcPhrAffectedByConstraint(std::make_pair(segmLeftMostj,segmRightMostj));
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
              SourceSegmentation srcSegm;
              std::vector<PositionIndex> trgSegmCuts;
              extHyp.getPhraseAlign(srcSegm,trgSegmCuts);
              std::vector<std::string> targetWordVec=this->getTransInPlainTextVec(extHyp);
              if(this->trMetadataPtr->translationSatisfiesConstraints(srcSegm,trgSegmCuts,targetWordVec))
              {
                hypVec.push_back(extHyp);
                scrCompVec.push_back(scoreComponents);
              }
            }
          }
        }
      }
    }
  }
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::expand_ref(const Hypothesis& hyp,
                                           std::vector<Hypothesis>& hypVec,
                                           std::vector<std::vector<Score> >& scrCompVec)
{
  std::vector<std::pair<PositionIndex,PositionIndex> > gaps;
  std::vector<WordIndex> srcPhrase;
  Hypothesis extHyp;
  std::vector<HypDataType> hypDataVec;
  std::vector<Score> scoreComponents;

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
      srcPhrase.clear();
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
          }
        }
      }
    }
  }
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::expand_ver(const Hypothesis& hyp,
                                           std::vector<Hypothesis>& hypVec,
                                           std::vector<std::vector<Score> >& scrCompVec)
{
  std::vector<std::pair<PositionIndex,PositionIndex> > gaps;
  std::vector<WordIndex> srcPhrase;
  Hypothesis extHyp;
  std::vector<HypDataType> hypDataVec;
  std::vector<Score> scoreComponents;

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
      srcPhrase.clear();
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
          }
        }
      }
    }
  }
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::expand_prefix(const Hypothesis& hyp,
                                              std::vector<Hypothesis>& hypVec,
                                              std::vector<std::vector<Score> >& scrCompVec)
{
  std::vector<std::pair<PositionIndex,PositionIndex> > gaps;
  std::vector<WordIndex> srcPhrase;
  Hypothesis extHyp;
  std::vector<HypDataType> hypDataVec;
  std::vector<Score> scoreComponents;

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
      srcPhrase.clear();
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
          }
        }
      }
    }
  }
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::addHeuristicToHyp(Hypothesis& hyp)
{
  hyp.addHeuristic(calcHeuristicScore(hyp));
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::subtractHeuristicToHyp(Hypothesis& hyp)
{
  hyp.subtractHeuristic(calcHeuristicScore(hyp));
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::clearTempVars(void)
{
      // Clear input information
  pbtmInputVars.clear();

      // Clear set of unseen words
  unseenWordsSet.clear();

      // Clear information of the heuristic used in the translation
  heuristicScoreVec.clear();

      // Clear additional heuristic information
  refHeurLmLgProb.clear();
  prefHeurLmLgProb.clear();

      // Clear n-best translation cache data
  nbTransCacheData.clear();
}

//---------------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::verifyDictCoverageForSentence(const std::vector<std::string>& sentenceVec,
                                                              int /*maxSrcPhraseLength*/)
{
      // Manage source words without translation options
  for(unsigned int j=0;j<sentenceVec.size();++j)
  {
        // Obtain translation options
    NbestTableNode<PhraseTransTableNodeData> ttNode;
    std::string s=sentenceVec[j];
    std::vector<std::string> srcPhraseStr;
    srcPhraseStr.push_back(s);
    std::set<std::vector<std::string> > transSetStr;
    getTransForSrcPhraseStr(srcPhraseStr,transSetStr);

        // Check if word has at least one valid translation
    if(!srcPhrHasAtLeastOneValidTranslation(srcPhraseStr,transSetStr))
    {
      manageUnseenSrcWord(s);
    }
  }
}

//---------------------------------
template<class HYPOTHESIS>
bool _pbTransModel<HYPOTHESIS>::srcPhrHasAtLeastOneValidTranslation(const std::vector<std::string> srcPhraseStr,
                                                                    const std::set<std::vector<std::string> >& transSetStr)
{
      // Iterate over set of translations
  std::set<std::vector<std::string> >::const_iterator iter;
  for(iter=transSetStr.begin();iter!=transSetStr.end();++iter)
  {
    if(this->trMetadataPtr->phraseTranslationIsValid(srcPhraseStr,*iter))
      return true;
  }
  return false;
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::manageUnseenSrcWord(std::string srcw)
{
      // Visualize warning depending on the verbosity level
  if(this->verbosity>0)
  {
    std::cerr<<"Warning! word "<<srcw<<" has been marked as unseen."<<std::endl;
  }
      // Add word to the set of unseen words
  unseenWordsSet.insert(srcw);
}

//---------------------------------
template<class HYPOTHESIS>
bool _pbTransModel<HYPOTHESIS>::unseenSrcWord(std::string srcw)
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
bool _pbTransModel<HYPOTHESIS>::unseenSrcWordGivenPosition(unsigned int srcPos)
{
  return unseenSrcWord(pbtmInputVars.srcSentVec[srcPos-1]);
}

//---------------------------------
template<class HYPOTHESIS>
Score _pbTransModel<HYPOTHESIS>::unkWordScoreHeur(void)
{
      // Init srcPhrase and trgPhrase
  std::vector<WordIndex> srcPhrase;
  std::vector<WordIndex> trgPhrase;
  srcPhrase.push_back(UNK_WORD);
  trgPhrase.push_back(UNK_WORD);

  return nbestTransScoreCached(srcPhrase,trgPhrase);
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::initHeuristic(unsigned int maxSrcPhraseLength)
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
void _pbTransModel<HYPOTHESIS>::initHeuristicLocalt(int maxSrcPhraseLength)
{
  std::vector<Score> row;
  NbestTableNode<PhraseTransTableNodeData> ttNode;
  NbestTableNode<PhraseTransTableNodeData>::iterator ttNodeIter;
  Score compositionProduct;
  Score bestScore_ts=0;
  Score score_ts;
    
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
      unsigned int segmRightMostj=y+1;
      unsigned int segmLeftMostj=J-x; 

          // obtain score for best translation
      if((segmRightMostj-segmLeftMostj)+1>(unsigned int)maxSrcPhraseLength)
      {
        ttNode.clear();
      }
      else
      {
        std::vector<WordIndex> srcPhrase;
        for(unsigned int j=segmLeftMostj;j<=segmRightMostj;++j)
          srcPhrase.push_back(pbtmInputVars.nsrcSentIdVec[j]);
  
            // Obtain translations for srcPhrase
        getNbestTransForSrcPhraseCached(segmLeftMostj,segmRightMostj,ttNode,this->pbTransModelPars.W);
        if(ttNode.size()!=0) // Obtain best p(srcPhrase|t_)
        {
          bestScore_ts=-FLT_MAX;
          for(ttNodeIter=ttNode.begin();ttNodeIter!=ttNode.end();++ttNodeIter)
          {
                // Obtain phrase to phrase translation probability
            score_ts=heurDirectPmScoreLt(srcPhrase,ttNodeIter->second)+heurInversePmScoreLt(srcPhrase,ttNodeIter->second);

                // Obtain language model heuristic estimation
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
Score _pbTransModel<HYPOTHESIS>::heurDirectPmScoreLt(const std::vector<WordIndex>& srcPhrase,
                                                     const std::vector<WordIndex>& trgPhrase)
{
      // Obtain string vector
  std::vector<std::string> srcPhraseStr=srcIndexVectorToStrVector(srcPhrase);
  std::vector<std::string> trgPhraseStr=trgIndexVectorToStrVector(trgPhrase);
  
      // Obtain direct phrase model feature pointers 
  Score scr=0;
  std::vector<unsigned int> featIndexVec;
  std::vector<DirectPhraseModelFeat<HypScoreInfo>* > directPhraseModelFeatPtrs=standardFeaturesInfoPtr->getDirectPhraseModelFeatPtrs(featIndexVec);
  for(unsigned int i=0;i<directPhraseModelFeatPtrs.size();++i)
  {
    scr+=getStdFeatWeight(featIndexVec[i]) * directPhraseModelFeatPtrs[i]->scorePhrasePairUnweighted(srcPhraseStr,trgPhraseStr);
  }
  return scr;
}

//---------------------------------
template<class HYPOTHESIS>
Score _pbTransModel<HYPOTHESIS>::heurInversePmScoreLt(const std::vector<WordIndex>& srcPhrase,
                                                      const std::vector<WordIndex>& trgPhrase)
{
      // Obtain string vector
  std::vector<std::string> srcPhraseStr=srcIndexVectorToStrVector(srcPhrase);
  std::vector<std::string> trgPhraseStr=trgIndexVectorToStrVector(trgPhrase);

      // Obtain inverse phrase model feature pointers 
  Score scr=0;
  std::vector<unsigned int> featIndexVec;
  std::vector<InversePhraseModelFeat<HypScoreInfo>* > inversePhraseModelFeatPtrs=standardFeaturesInfoPtr->getInversePhraseModelFeatPtrs(featIndexVec);
  for(unsigned int i=0;i<inversePhraseModelFeatPtrs.size();++i)
  {
    scr+=getStdFeatWeight(featIndexVec[i]) * inversePhraseModelFeatPtrs[i]->scorePhrasePairUnweighted(srcPhraseStr,trgPhraseStr);
  }
  return scr;
}

//---------------------------------
template<class HYPOTHESIS>
Score _pbTransModel<HYPOTHESIS>::heurLmScoreLtNoAdmiss(const std::vector<WordIndex>& trgPhrase)
{
      // Obtain string vector
  std::vector<std::string> trgPhraseStr=trgIndexVectorToStrVector(trgPhrase);

      // Obtain language model feature pointers 
  Score scr=0;
  std::vector<unsigned int> featIndexVec;
  std::vector<LangModelFeat<HypScoreInfo>* > langModelFeatPtrs=standardFeaturesInfoPtr->getLangModelFeatPtrs(featIndexVec);
  for(unsigned int i=0;i<langModelFeatPtrs.size();++i)
  {
    std::vector<std::string> emptyPhraseStr;
    scr+=getStdFeatWeight(featIndexVec[i]) * langModelFeatPtrs[i]->scorePhrasePairUnweighted(emptyPhraseStr,trgPhraseStr);
  }
  return scr;
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::initHeuristicLocaltd(int maxSrcPhraseLength)
{
  initHeuristicLocalt(maxSrcPhraseLength);
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::setHeuristic(unsigned int _heuristicId)
{
  heuristicId=_heuristicId;
}

//---------------------------------
template<class HYPOTHESIS>
Score _pbTransModel<HYPOTHESIS>::calcHeuristicScore(const _pbTransModel::Hypothesis& hyp)
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
Score _pbTransModel<HYPOTHESIS>::calcRefLmHeurScore(const _pbTransModel::Hypothesis& hyp)
{
      // TO-BE-DONE
  
  if(refHeurLmLgProb.empty())
  {
    std::vector<unsigned int> featIndexVec;
    std::vector<LangModelFeat<HypScoreInfo>* > langModelFeatPtrs=standardFeaturesInfoPtr->getLangModelFeatPtrs(featIndexVec);
    for(unsigned int i=0;i<langModelFeatPtrs.size();++i)
    {
          // Obtain scores
      std::vector<Score> cumulativeScoreVec;
      langModelFeatPtrs[i]->scoreTrgSentence(pbtmInputVars.refSentVec,getStdFeatWeight(featIndexVec[i]),cumulativeScoreVec);
      
          // Update refHeurLmLgProb
      for(unsigned int j=0;j<cumulativeScoreVec.size();++j)
      {
        if(j==refHeurLmLgProb.size())
          refHeurLmLgProb.push_back(cumulativeScoreVec[j]);
        else
          refHeurLmLgProb[j]+=cumulativeScoreVec[j];
      }
    }
  }
      // Return heuristic value
  unsigned int len=hyp.partialTransLength();
  if(len==0)
    return refHeurLmLgProb.back();
  else
    return refHeurLmLgProb.back()-refHeurLmLgProb[len-1];
}

//---------------------------------
template<class HYPOTHESIS>
Score _pbTransModel<HYPOTHESIS>::calcPrefLmHeurScore(const _pbTransModel::Hypothesis& hyp)
{
  if(prefHeurLmLgProb.empty())
  {
    std::vector<unsigned int> featIndexVec;
    std::vector<LangModelFeat<HypScoreInfo>* > langModelFeatPtrs=standardFeaturesInfoPtr->getLangModelFeatPtrs(featIndexVec);
    for(unsigned int i=0;i<langModelFeatPtrs.size();++i)
    {
          // Obtain scores
      std::vector<Score> cumulativeScoreVec;
      langModelFeatPtrs[i]->scoreTrgSentence(pbtmInputVars.prefSentVec,getStdFeatWeight(featIndexVec[i]),cumulativeScoreVec);

          // Update prefHeurLmLgProb
      for(unsigned int j=0;j<cumulativeScoreVec.size();++j)
      {
        if(j==prefHeurLmLgProb.size())
          prefHeurLmLgProb.push_back(cumulativeScoreVec[j]);
        else
          prefHeurLmLgProb[j]+=cumulativeScoreVec[j];
      }
    }
  }
      // Return heuristic value
  unsigned int len=hyp.partialTransLength();
  if(len>=pbtmInputVars.nprefSentIdVec.size()-1)
    return 0;
  else
  {
    if(len==0)
      return prefHeurLmLgProb.back();
    else
      return prefHeurLmLgProb.back()-prefHeurLmLgProb[len-1];
  }
}

//---------------------------------
template<class HYPOTHESIS>
Score _pbTransModel<HYPOTHESIS>::heuristicLocalt(const Hypothesis& hyp)
{
  if(state==MODEL_TRANS_STATE)
  {
    return getLocalTmHeurScore(hyp);
  }
  else
  {
        // TO-DO
    return 0;
  }  
}

//---------------------------------
template<class HYPOTHESIS>
Score _pbTransModel<HYPOTHESIS>::heuristicLocaltd(const Hypothesis& hyp)
{
  if(state==MODEL_TRANS_STATE)
  {
    Score result=getLocalTmHeurScore(hyp);
    result+=getDistortionHeurScore(hyp);
    
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
Score _pbTransModel<HYPOTHESIS>::getLocalTmHeurScore(const Hypothesis& hyp)
{
  Score result=0;  
  unsigned int J=pbtmInputVars.srcSentVec.size();
  std::vector<std::pair<PositionIndex,PositionIndex> > gaps;
  this->extract_gaps(hyp,gaps);
  for(unsigned int i=0;i<gaps.size();++i)
  {
    result+=heuristicScoreVec[gaps[i].second-1][J-gaps[i].first];	
  }
  
  return result;
}

//---------------------------------
template<class HYPOTHESIS>
Score _pbTransModel<HYPOTHESIS>::getDistortionHeurScore(const Hypothesis& hyp)
{
      // Initialize variables
  Score result=0;  
  std::vector<std::pair<PositionIndex,PositionIndex> > gaps;
  this->extract_gaps(hyp,gaps);
  PositionIndex lastSrcPosCovered=getLastSrcPosCovered(hyp);

      // Obtain score
  std::vector<unsigned int> featIndexVec;
  std::vector<SrcPosJumpFeat<HypScoreInfo>* > srcPosJumpFeatPtrs=standardFeaturesInfoPtr->getSrcPosJumpFeatPtrs(featIndexVec);
  for(unsigned int i=0;i<srcPosJumpFeatPtrs.size();++i)
  {
    result+=srcPosJumpFeatPtrs[i]->calcHeurScore(gaps,lastSrcPosCovered);
  }

  return result;
}

//---------------------------------
template<class HYPOTHESIS>
PositionIndex _pbTransModel<HYPOTHESIS>::getLastSrcPosCovered(const Hypothesis& hyp)
{
  return getLastSrcPosCoveredHypData(hyp.getData());
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::printHyp(const Hypothesis& hyp,
                                         std::ostream &outS,
                                         int verbose)
{
      // Obtain target string vector
  std::vector<std::string> trgStrVec=trgIndexVectorToStrVector(hyp.getPartialTrans());

      // Print score
  outS <<"Score: "<<hyp.getScore()<<" ; ";

      // Print weights
  this->printWeights(outS);
  outS <<" ; ";

      // Obtain null hypothesis score components
  Hypothesis nullHyp;
  std::vector<Score> nullHypScoreComponents;
  this->nullHypothesisScrComps(nullHyp,nullHypScoreComponents);
  
      // Obtain extension score components
  Hypothesis auxHyp;
  std::vector<Score> extScoreComponents;
  HypDataType hypDataType=hyp.getData();
  this->incrScore(nullHyp,hypDataType,auxHyp,extScoreComponents);

      // Print score components
  for(unsigned int i=0;i<extScoreComponents.size();++i)
    outS<<nullHypScoreComponents[i]+extScoreComponents[i]<<" ";

      // Print score + heuristic
  addHeuristicToHyp(auxHyp);
  outS <<"; Score+heur: "<<auxHyp.getScore()<<" ";
    
      // Print warning if the alignment is not complete
  if(!this->isComplete(hyp)) outS<< "; Incomplete_alignment!";

      // Obtain phrase alignment
  SourceSegmentation sourceSegmentation;
  std::vector<PositionIndex> targetSegmentCuts;
  std::vector<std::pair<PositionIndex,PositionIndex> > amatrix;
  this->aligMatrix(hyp,amatrix);
  this->getPhraseAlignment(amatrix,sourceSegmentation,targetSegmentCuts);

      // Print target translation
  outS<<" | ";
  for(unsigned int i=1;i<trgStrVec.size();++i)
    outS<<trgStrVec[i]<<" ";

      // Print source segmentation
  outS << "| Source Segmentation: ";
  for(unsigned int k=0;k<sourceSegmentation.size();k++)
  {
    outS<<"( "<<sourceSegmentation[k].first<<" , "<<sourceSegmentation[k].second<<" ; type: ";
        // Print translation type for source segment
    if(this->trMetadataPtr->srcPhrAffectedByConstraint(sourceSegmentation[k]))
    {
      std::string constrType=this->trMetadataPtr->getConstraintTypeForSrcPhr(sourceSegmentation[k]);
      if(constrType.empty())
        outS<<"Constraint";
      else
        outS<<constrType;
    }
    else
    {
      std::pair<PositionIndex,PositionIndex> pidxPair;
      pidxPair.first=sourceSegmentation[k].first;
      pidxPair.second=sourceSegmentation[k].second;
      std::vector<WordIndex> trgPhr;
      hyp.getTrgTransForSrcPhr(pidxPair,trgPhr);
      outS<<getLogLinFeatNamesForPhrTransStr(pidxPair,trgIndexVectorToStrVector(trgPhr));
    }
    outS<<" ) ";
  }

      // Print target segmentation
  outS<< "| Target Segmentation: ";
  for (unsigned int j=0; j<targetSegmentCuts.size(); j++)
    outS << targetSegmentCuts[j] << " ";
  
      // Print hypothesis key
  outS<<"| hypkey: "<<hyp.getKey()<<" ";

      // Print hypothesis equivalence class
  outS<<"| hypEqClass: "<<hyp.getEqClass()<<std::endl;

  if(verbose)
  {
    unsigned int numSteps=sourceSegmentation.size()-1;
    outS<<"----------------------------------------------"<<std::endl;
    outS<<"Score components for previous expansion steps:"<<std::endl;
    auxHyp=hyp;
    while(this->obtainPredecessor(auxHyp))
    {
      std::vector<Score> scoreComponents=scoreCompsForHyp(auxHyp);
      outS<<"Step "<<numSteps<<" : ";
      for(unsigned int i=0;i<scoreComponents.size();++i)
      {
        outS<<scoreComponents[i]<<" ";
      }
      outS<<std::endl;
      --numSteps;
    }
    outS<<"----------------------------------------------"<<std::endl;
  }
}

//---------------------------------
template<class HYPOTHESIS>
std::vector<std::string> _pbTransModel<HYPOTHESIS>::getTransInPlainTextVec(const _pbTransModel::Hypothesis& hyp)const
{
  switch(state)
  {
    case MODEL_TRANS_STATE: return getTransInPlainTextVecTs(hyp);
    case MODEL_TRANSPREFIX_STATE: return getTransInPlainTextVecTps(hyp);
    case MODEL_TRANSREF_STATE: return getTransInPlainTextVecTrs(hyp);
    case MODEL_TRANSVER_STATE: return getTransInPlainTextVecTvs(hyp);
    default: std::vector<std::string> strVec;
      return strVec;
  }
}

//---------------------------------
template<class HYPOTHESIS>
std::vector<std::string> _pbTransModel<HYPOTHESIS>::getTransInPlainTextVecTs(const _pbTransModel<HYPOTHESIS>::Hypothesis& hyp)const
{
  std::vector<WordIndex> nvwi;
  std::vector<WordIndex> vwi;

      // Obtain vector of WordIndex
  nvwi=hyp.getPartialTrans();
  for(unsigned int i=1;i<nvwi.size();++i)
  {
    vwi.push_back(nvwi[i]);
  }
      // Obtain vector of strings
  std::vector<std::string> trgVecStr=trgIndexVectorToStrVector(vwi);

      // Treat unknown words contained in trgVecStr. Model is being used
      // to translate a sentence
    
      // Replace unknown words affected by constraints

      // Iterate over constraints
  std::set<std::pair<PositionIndex,PositionIndex> > srcPhrSet=this->trMetadataPtr->getConstrainedSrcPhrases();
  std::set<std::pair<PositionIndex,PositionIndex> >::const_iterator const_iter;
  for(const_iter=srcPhrSet.begin();const_iter!=srcPhrSet.end();++const_iter)
  {
        // Obtain target translation for constraint
    std::vector<std::string> trgPhr=this->trMetadataPtr->getTransForSrcPhr(*const_iter);
    
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
std::vector<std::string> _pbTransModel<HYPOTHESIS>::getTransInPlainTextVecTps(const _pbTransModel::Hypothesis& hyp)const
{
  std::vector<WordIndex> nvwi;
  std::vector<WordIndex> vwi;

      // Obtain vector of WordIndex
  nvwi=hyp.getPartialTrans();
  for(unsigned int i=1;i<nvwi.size();++i)
  {
    vwi.push_back(nvwi[i]);
  }
      // Obtain vector of strings
  std::vector<std::string> trgVecStr=trgIndexVectorToStrVector(vwi);

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
std::vector<std::string> _pbTransModel<HYPOTHESIS>::getTransInPlainTextVecTrs(const _pbTransModel::Hypothesis& hyp)const
{
  std::vector<WordIndex> nvwi;
  std::vector<WordIndex> vwi;

      // Obtain vector of WordIndex
  nvwi=hyp.getPartialTrans();
  for(unsigned int i=1;i<nvwi.size();++i)
  {
    vwi.push_back(nvwi[i]);
  }
      // Obtain vector of strings
  std::vector<std::string> trgVecStr=trgIndexVectorToStrVector(vwi);

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
std::vector<std::string> _pbTransModel<HYPOTHESIS>::getTransInPlainTextVecTvs(const _pbTransModel::Hypothesis& hyp)const
{
  std::vector<WordIndex> nvwi;
  std::vector<WordIndex> vwi;

      // Obtain vector of WordIndex
  nvwi=hyp.getPartialTrans();
  for(unsigned int i=1;i<nvwi.size();++i)
  {
    vwi.push_back(nvwi[i]);
  }
      // Obtain vector of strings
  std::vector<std::string> trgVecStr=trgIndexVectorToStrVector(vwi);

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
void _pbTransModel<HYPOTHESIS>::setWeights(std::vector<float> wVec)
{
      // Initialize variables
  defaultFeatWeights=wVec;

      // Initialize weights
  initFeatWeights(wVec);
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::initFeatWeights(std::vector<float> wVec)
{
      // Clear weight vectors
  standardFeaturesWeights.clear();
  onTheFlyFeaturesWeights.clear();
  
      // Set weights of standard features
  for(unsigned int i=0;i<standardFeaturesInfoPtr->featPtrVec.size();++i)
  {
    if(i<wVec.size())
      standardFeaturesWeights.push_back(wVec[i]);
    else
      standardFeaturesWeights.push_back(DEFAULT_LOGLIN_WEIGHT);
  }

      // Set weights of on-the-fly features
  int numStdWeights=standardFeaturesInfoPtr->featPtrVec.size();
  for(unsigned int i=0;i<onTheFlyFeaturesInfo.featPtrVec.size();++i)
  {
    if(numStdWeights+i<wVec.size())
      onTheFlyFeaturesWeights.push_back(wVec[numStdWeights+i]);
    else
      onTheFlyFeaturesWeights.push_back(DEFAULT_LOGLIN_WEIGHT);
  }
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::getWeights(std::vector<std::pair<std::string,float> >& compWeights)
{
  compWeights.clear();
      // Obtain weight info for standard features
  for(unsigned int i=0;i<standardFeaturesInfoPtr->featPtrVec.size();++i)
  {
    std::pair<std::string,float> str_float;
    std::string weightName=standardFeaturesInfoPtr->featPtrVec[i]->getFeatName();
    weightName+="w";
    str_float.first=weightName;
    if(i<standardFeaturesWeights.size())
      str_float.second=standardFeaturesWeights[i];
    else
      str_float.second=DEFAULT_LOGLIN_WEIGHT;
    compWeights.push_back(str_float);
  }
  
      // Obtain weight info for on-the-fly features
  for(unsigned int i=0;i<onTheFlyFeaturesInfo.featPtrVec.size();++i)
  {
    std::pair<std::string,float> str_float;
    std::string weightName=onTheFlyFeaturesInfo.featPtrVec[i]->getFeatName();
    weightName+="w";
    str_float.first=weightName;
    if(i<onTheFlyFeaturesWeights.size())
      str_float.second=onTheFlyFeaturesWeights[i];
    else
      str_float.second=DEFAULT_LOGLIN_WEIGHT;
    compWeights.push_back(str_float);
  }
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::printWeights(std::ostream &outS)
{
  std::vector<std::pair<std::string,float> > compWeights;
  getWeights(compWeights);
  for(unsigned int i=0;i<compWeights.size();++i)
  {
    outS<<compWeights[i].first<<": "<<compWeights[i].second;
    if(i!=compWeights.size()-1)
      outS<<" , ";
  }
}

//---------------------------------
template<class HYPOTHESIS>
unsigned int _pbTransModel<HYPOTHESIS>::getNumWeights(void)
{
  return standardFeaturesInfoPtr->featPtrVec.size();
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::getUnweightedComps(const std::vector<Score>& scrComps,
                                                   std::vector<Score>& unweightedScrComps)
{
  unweightedScrComps=scrComps;
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::extendHypData(PositionIndex srcLeft,
                                              PositionIndex srcRight,
                                              const std::vector<std::string>& trgPhrase,
                                              HypDataType& hypd)
{
  std::vector<WordIndex> trgPhraseIdx;
  
  for(unsigned int i=0;i<trgPhrase.size();++i)
    trgPhraseIdx.push_back(stringToTrgWordIndex(trgPhrase[i]));
  extendHypDataIdx(srcLeft,srcRight,trgPhraseIdx,hypd);
}

//---------------------------------
template<class HYPOTHESIS>
bool _pbTransModel<HYPOTHESIS>::getHypDataVecForGap(const Hypothesis& hyp,
                                                    PositionIndex srcLeft,
                                                    PositionIndex srcRight,
                                                    std::vector<HypDataType>& hypDataTypeVec,
                                                    float N)
{
  HypDataType hypData=hyp.getData();
  HypDataType newHypData;

  hypDataTypeVec.clear();

      // Obtain translations for gap
  NbestTableNode<PhraseTransTableNodeData> ttNode;
  getTransForHypUncovGap(hyp,srcLeft,srcRight,ttNode,N);

  if(this->verbosity>=2)
  {
    std::cerr<<"  trying to cover from src. pos. "<<srcLeft<<" to "<<srcRight<<"; ";
    std::cerr<<"Filtered "<<ttNode.size()<<" translations"<<std::endl;
  }

      // Generate hypothesis data for translations
  NbestTableNode<PhraseTransTableNodeData>::iterator ttNodeIter;
  for(ttNodeIter=ttNode.begin();ttNodeIter!=ttNode.end();++ttNodeIter)
  {
    if(this->verbosity>=3)
    {
      std::cerr<<"   ";
      for(unsigned int i=srcLeft;i<=srcRight;++i) std::cerr<<this->pbtmInputVars.srcSentVec[i-1]<<" ";
      std::cerr<<"||| ";
      for(unsigned int i=0;i<ttNodeIter->second.size();++i)
        std::cerr<<this->wordIndexToTrgString(ttNodeIter->second[i])<<" ";
      std::cerr<<"||| "<<ttNodeIter->first<<std::endl;
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
bool _pbTransModel<HYPOTHESIS>::getHypDataVecForGapRef(const Hypothesis& hyp,
                                                       PositionIndex srcLeft,
                                                       PositionIndex srcRight,
                                                       std::vector<HypDataType>& hypDataTypeVec,
                                                       float N)
{
  HypDataType hypData=hyp.getData();
  HypDataType newHypData;

  hypDataTypeVec.clear();

      // Obtain translation for gap
  NbestTableNode<PhraseTransTableNodeData> ttNode;
  getTransForHypUncovGapRef(hyp,srcLeft,srcRight,ttNode,N);

  if(this->verbosity>=2)
  {
    std::cerr<<"  trying to cover from src. pos. "<<srcLeft<<" to "<<srcRight<<"; ";
    std::cerr<<"Filtered "<<ttNode.size()<<" translations"<<std::endl;
  }

      // Generate hypothesis data for translations
  NbestTableNode<PhraseTransTableNodeData>::iterator ttNodeIter;
  for(ttNodeIter=ttNode.begin();ttNodeIter!=ttNode.end();++ttNodeIter)
  {
    if(this->verbosity>=3)
    {
      std::cerr<<"   ";
      for(unsigned int i=srcLeft;i<=srcRight;++i) std::cerr<<this->pbtmInputVars.srcSentVec[i-1]<<" ";
      std::cerr<<"||| ";
      for(unsigned int i=0;i<ttNodeIter->second.size();++i)
        std::cerr<<this->wordIndexToTrgString(ttNodeIter->second[i])<<" ";
      std::cerr<<"||| "<<ttNodeIter->first<<std::endl;
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

      // Return boolean value
  if(hypDataTypeVec.empty()) return false;
  else return true;  
}

//---------------------------------
template<class HYPOTHESIS>
bool _pbTransModel<HYPOTHESIS>::getHypDataVecForGapVer(const Hypothesis& hyp,
                                                       PositionIndex srcLeft,
                                                       PositionIndex srcRight,
                                                       std::vector<HypDataType>& hypDataTypeVec,
                                                       float N)
{
  HypDataType hypData=hyp.getData();
  HypDataType newHypData;

  hypDataTypeVec.clear();
  
      // Obtain translation for gap
  NbestTableNode<PhraseTransTableNodeData> ttNode;
  getTransForHypUncovGapVer(hyp,srcLeft,srcRight,ttNode,N);

  if(this->verbosity>=2)
  {
    std::cerr<<"  trying to cover from src. pos. "<<srcLeft<<" to "<<srcRight<<"; ";
    std::cerr<<"Filtered "<<ttNode.size()<<" translations"<<std::endl;
  }

      // Generate hypothesis data for translations
  NbestTableNode<PhraseTransTableNodeData>::iterator ttNodeIter;
  for(ttNodeIter=ttNode.begin();ttNodeIter!=ttNode.end();++ttNodeIter)
  {
    if(this->verbosity>=3)
    {
      std::cerr<<"   ";
      for(unsigned int i=srcLeft;i<=srcRight;++i) std::cerr<<this->pbtmInputVars.srcSentVec[i-1]<<" ";
      std::cerr<<"||| ";
      for(unsigned int i=0;i<ttNodeIter->second.size();++i)
        std::cerr<<this->wordIndexToTrgString(ttNodeIter->second[i])<<" ";
      std::cerr<<"||| "<<ttNodeIter->first<<std::endl;
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

      // Return boolean value
  if(hypDataTypeVec.empty()) return false;
  else return true;  
}

//---------------------------------
template<class HYPOTHESIS>
bool _pbTransModel<HYPOTHESIS>::getHypDataVecForGapPref(const Hypothesis& hyp,
                                                        PositionIndex srcLeft,
                                                        PositionIndex srcRight,
                                                        std::vector<HypDataType>& hypDataTypeVec,
                                                        float N)
{
  HypDataType hypData=hyp.getData();
  HypDataType newHypData;

  hypDataTypeVec.clear();
  
      // Obtain translation for gap
  NbestTableNode<PhraseTransTableNodeData> ttNode;
  getTransForHypUncovGapPref(hyp,srcLeft,srcRight,ttNode,N);

  if(this->verbosity>=2)
  {
    std::cerr<<"  trying to cover from src. pos. "<<srcLeft<<" to "<<srcRight<<"; ";
    std::cerr<<"Filtered "<<ttNode.size()<<" translations"<<std::endl;
  }

      // Generate hypothesis data for translations
  NbestTableNode<PhraseTransTableNodeData>::iterator ttNodeIter;
  for(ttNodeIter=ttNode.begin();ttNodeIter!=ttNode.end();++ttNodeIter)
  {
    if(this->verbosity>=3)
    {
      std::cerr<<"   ";
      for(unsigned int i=srcLeft;i<=srcRight;++i) std::cerr<<this->pbtmInputVars.srcSentVec[i-1]<<" ";
      std::cerr<<"||| ";
      for(unsigned int i=0;i<ttNodeIter->second.size();++i)
        std::cerr<<this->wordIndexToTrgString(ttNodeIter->second[i])<<" ";
      std::cerr<<"||| "<<ttNodeIter->first<<std::endl;
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
bool _pbTransModel<HYPOTHESIS>::getTransForSrcPhrase(const std::vector<WordIndex>& srcPhrase,
                                                     std::set<std::vector<WordIndex> >& transSet)
{
      // Clear data structures
  transSet.clear();
  
      // Obtain string vector
  std::vector<std::string> srcPhraseStr=srcIndexVectorToStrVector(srcPhrase);
  std::set<std::vector<std::string> > transSetStr;
  
      // Obtain translation options for each feature
  getTransForSrcPhraseStr(srcPhraseStr,transSetStr);

      // Convert strings to words
  std::set<std::vector<std::string> >::const_iterator iter;
  for(iter=transSetStr.begin();iter!=transSetStr.end();++iter)
    transSet.insert(strVectorToTrgIndexVector(*iter));

  if(transSet.empty())
    return false;
  else
    return true;
}

//---------------------------------
template<class HYPOTHESIS>
bool _pbTransModel<HYPOTHESIS>::getTransForSrcPhraseStr(const std::vector<std::string>& srcPhraseStr,
                                                        std::set<std::vector<std::string> >& transSetStr)
{
      // Clear data structures
  transSetStr.clear();
    
      // Obtain translation options for each standard feature
  for(unsigned int i=0;i<this->standardFeaturesInfoPtr->featPtrVec.size();++i)
  {
        // Obtain options
    std::vector<std::vector<std::string> > transOptVec;
    this->standardFeaturesInfoPtr->featPtrVec[i]->obtainTransOptions(srcPhraseStr,transOptVec);

        // Add options to set
    for(unsigned int j=0;j<transOptVec.size();++j)
      transSetStr.insert(transOptVec[j]);
  }

      // Obtain translation options for each on-the-fly feature
  for(unsigned int i=0;i<this->onTheFlyFeaturesInfo.featPtrVec.size();++i)
  {
        // Obtain options
    std::vector<std::vector<std::string> > transOptVec;
    this->onTheFlyFeaturesInfo.featPtrVec[i]->obtainTransOptions(srcPhraseStr,transOptVec);

        // Add options to set
    for(unsigned int j=0;j<transOptVec.size();++j)
      transSetStr.insert(transOptVec[j]);
  }

  if(transSetStr.empty())
    return false;
  else
    return true;  
}

//---------------------------------
template<class HYPOTHESIS>
std::string _pbTransModel<HYPOTHESIS>::getLogLinFeatNamesForPhrTransStr(std::pair<PositionIndex,PositionIndex> pidxPair,
                                                                        std::vector<std::string> trgPhr)
{
      // Obtain vector of log linear feature names
  std::vector<std::string> strVec=getLogLinFeatNamesForPhrTrans(pidxPair,trgPhr);

      // Compose output string
  if(strVec.empty())
    return UNK_WORD_STR;
  else
  {
    std::string result;
    for(unsigned int i=0;i<strVec.size();++i)
    {
      result+=strVec[i];
      if(i<strVec.size()-1)
        result+=",";
    }
    
    return result;
  }
}

//---------------------------------
template<class HYPOTHESIS>
std::vector<std::string> _pbTransModel<HYPOTHESIS>::getLogLinFeatNamesForPhrTrans(std::pair<PositionIndex,PositionIndex> pidxPair,
                                                                                  std::vector<std::string> trgPhr)
{
  std::vector<std::string> featNames;

      // Obtain source phrase
  std::vector<std::string> srcPhraseStr;
  for(unsigned int j=pidxPair.first;j<=pidxPair.second;++j)
    srcPhraseStr.push_back(pbtmInputVars.srcSentVec[j-1]);

      // Obtain translation options for each standard feature
  for(unsigned int i=0;i<this->standardFeaturesInfoPtr->featPtrVec.size();++i)
  {
        // Obtain options
    std::vector<std::vector<std::string> > transOptVec;
    this->standardFeaturesInfoPtr->featPtrVec[i]->obtainTransOptions(srcPhraseStr,transOptVec);

        // Add featName to result if appropriate
    std::vector<std::vector<std::string> >::const_iterator iter;
    iter=find(transOptVec.begin(),transOptVec.end(),trgPhr);
    if(iter!=transOptVec.end())
    {
      featNames.push_back(this->standardFeaturesInfoPtr->featPtrVec[i]->getFeatName());
    }
  }

      // Obtain translation options for each on-the-fly feature
  for(unsigned int i=0;i<this->onTheFlyFeaturesInfo.featPtrVec.size();++i)
  {
        // Obtain pointer to OnTheFlyDictFeat object
    OnTheFlyDictFeat<HypScoreInfo>* onTheFlyFeatPtr=dynamic_cast<OnTheFlyDictFeat<HypScoreInfo>* >(this->onTheFlyFeaturesInfo.featPtrVec[i]);
        // Obtain options
    std::vector<std::vector<std::string> > transOptVec;
    this->onTheFlyFeaturesInfo.featPtrVec[i]->obtainTransOptions(srcPhraseStr,transOptVec);

        // Add featName to result if appropriate
    std::vector<std::vector<std::string> >::const_iterator iter;
    iter=find(transOptVec.begin(),transOptVec.end(),trgPhr);
    if(iter!=transOptVec.end())
    {
      std::string featName=this->onTheFlyFeaturesInfo.featPtrVec[i]->getFeatName();
      if(onTheFlyFeatPtr)
      {
        bool found;
        featName=featName + ":" + onTheFlyFeatPtr->getTransOptInfo(srcPhraseStr,trgPhr,found);
      }
      featNames.push_back(featName);
    }
  }

  return featNames;
}

//---------------------------------
template<class HYPOTHESIS>
Score _pbTransModel<HYPOTHESIS>::nbestTransScore(const std::vector<WordIndex>& srcPhrase,
                                                 const std::vector<WordIndex>& trgPhrase)
{
  Score result=0;
  
      // Obtain string vectors
  std::vector<std::string> srcPhraseStr=srcIndexVectorToStrVector(srcPhrase);
  std::vector<std::string> trgPhraseStr=trgIndexVectorToStrVector(trgPhrase);
  
      // Obtain score for each standard feature
  for(unsigned int i=0;i<this->standardFeaturesInfoPtr->featPtrVec.size();++i)
  {
    result+=getStdFeatWeight(i) * this->standardFeaturesInfoPtr->featPtrVec[i]->scorePhrasePairUnweighted(srcPhraseStr,trgPhraseStr);
  }

      // Obtain score for each on-the-fly feature
  for(unsigned int i=0;i<this->onTheFlyFeaturesInfo.featPtrVec.size();++i)
  {
    result+=getOnTheFlyFeatWeight(i) * this->onTheFlyFeaturesInfo.featPtrVec[i]->scorePhrasePairUnweighted(srcPhraseStr,trgPhraseStr);
  }

  return result;
}

//---------------------------------------
template<class EQCLASS_FUNC>
Score _pbTransModel<EQCLASS_FUNC>::nbestTransScoreLast(const std::vector<WordIndex>& srcPhrase,
                                                       const std::vector<WordIndex>& trgPhrase)
{
  return nbestTransScore(srcPhrase,trgPhrase);
}

//---------------------------------
template<class HYPOTHESIS>
bool _pbTransModel<HYPOTHESIS>::getNbestTransForSrcPhraseCached(PositionIndex srcLeft,
                                                                PositionIndex srcRight,
                                                                NbestTableNode<PhraseTransTableNodeData>& nbt,
                                                                float N)
{
  NbestTableNode<PhraseTransTableNodeData>* transTableNodePtr=nbTransCacheData.cPhrNbestTransTable.getTranslationsForKey(std::make_pair(srcLeft,srcRight));
  if(transTableNodePtr!=NULL)
  {
        // translation present in the cache translation table
    nbt=*transTableNodePtr;
    if(nbt.size()==0) return false;
    else return true;
  }
  else
  {
        // translation not present in the cache translation table
    std::vector<WordIndex> srcPhrase;
    for(unsigned int i=srcLeft;i<=srcRight;++i)
    {
      srcPhrase.push_back(pbtmInputVars.nsrcSentIdVec[i]);
    }
    getNbestTransForSrcPhrase(srcPhrase,nbt,N);
    nbTransCacheData.cPhrNbestTransTable.insertEntry(std::make_pair(srcLeft,srcRight),nbt);
    if(nbt.size()==0) return false;
    else return true;
  }
}

//---------------------------------
template<class HYPOTHESIS>
bool _pbTransModel<HYPOTHESIS>::getNbestTransForSrcPhrase(std::vector<WordIndex> srcPhrase,
                                                          NbestTableNode<PhraseTransTableNodeData>& nbt,
                                                          float N)
{
  BasePhraseModel::SrcTableNode srctn;
  BasePhraseModel::SrcTableNode::iterator srctnIter;
  bool ret;

      // Obtain the whole list of translations
  nbt.clear();
  std::set<std::vector<WordIndex> > transSet;
  ret=getTransForSrcPhrase(srcPhrase,transSet);
  if(!ret) return false;
  else
  {
    Score scr;

        // This loop may become a bottleneck if the number of translation
        // options is high
    for(std::set<std::vector<WordIndex> >::iterator transSetIter=transSet.begin();transSetIter!=transSet.end();++transSetIter)
    {
      scr=nbestTransScoreCached(srcPhrase,*transSetIter);
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
bool _pbTransModel<HYPOTHESIS>::getTransForHypUncovGap(const Hypothesis& /*hyp*/,
                                                       PositionIndex srcLeft,
                                                       PositionIndex srcRight,
                                                       NbestTableNode<PhraseTransTableNodeData>& nbt,
                                                       float N)
{
        // Check if gap is affected by translation constraints
  if(this->trMetadataPtr->srcPhrAffectedByConstraint(std::make_pair(srcLeft,srcRight)))
  {
        // Obtain constrained target translation for gap (if any)
    std::vector<std::string> trgWordVec=this->trMetadataPtr->getTransForSrcPhr(std::make_pair(srcLeft,srcRight));
    if(trgWordVec.size()>0)
    {
          // Convert string vector to WordIndex vector
      std::vector<WordIndex> trgWiVec;
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
      std::vector<WordIndex> unkWordVec;
      unkWordVec.push_back(UNK_WORD);
      nbt.clear();
      nbt.insert(0,unkWordVec);
      return false;
    }
    else
    {
          // search translations for source phrase in translation table
      return getNbestTransForSrcPhraseCached(srcLeft,srcRight,nbt,N);
    }
  }
}

//---------------------------------
template<class HYPOTHESIS>
bool _pbTransModel<HYPOTHESIS>::getTransForHypUncovGapRef(const Hypothesis& hyp,
                                                          PositionIndex srcLeft,
                                                          PositionIndex srcRight,
                                                          NbestTableNode<PhraseTransTableNodeData>& nbt,
                                                          float N)
{  
  if(hyp.getPartialTrans().size()>pbtmInputVars.nrefSentIdVec.size())
    return false;
  
  if(this->numberOfUncoveredSrcWords(hyp)-(srcRight-srcLeft+1)>0)
  {
        // This is not the last gap to be covered
    transUncovGapRefNoLastGapCached(hyp,srcLeft,srcRight,nbt,N);
    if(nbt.size()==0) return false;
    else return true;
  }
  else
  {
        // The last gap will be covered
    transUncovGapRefLastGapCached(hyp,srcLeft,srcRight,nbt);
    if(nbt.size()==0) return false;
    else return true;
  }
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::transUncovGapRefNoLastGapCached(const Hypothesis& hyp,
                                                                PositionIndex srcLeft,
                                                                PositionIndex srcRight,
                                                                NbestTableNode<PhraseTransTableNodeData>& nbt,
                                                                float N)
{
  nbt.clear();
  std::vector<WordIndex> srcPhrase;
  std::vector<WordIndex> trgPhrase;
  std::vector<WordIndex> ntarget=hyp.getPartialTrans();

      // Obtain source phrase
  for(unsigned int i=srcLeft;i<=srcRight;++i)
  {
    srcPhrase.push_back(pbtmInputVars.nsrcSentIdVec[i]);
  }

      // Obtain length limits for target phrase
  unsigned int minTrgSize=0;
  if(srcPhrase.size()>this->pbTransModelPars.E) minTrgSize=srcPhrase.size()-this->pbTransModelPars.E;
  unsigned int maxTrgSize=srcPhrase.size()+this->pbTransModelPars.E;
    
  PhrNbestTransTableRefKey pNbtRefKey;
  pNbtRefKey.srcLeft=srcLeft;
  pNbtRefKey.srcRight=srcRight;
  pNbtRefKey.ntrgSize=ntarget.size();
      // The number of gaps to be covered AFTER covering
      // srcPhrase{srcLeft}...srcPhrase{srcRight} is obtained to ensure that the
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
  NbestTableNode<PhraseTransTableNodeData>* transTableNodePtr=nbTransCacheData.cPhrNbestTransTableRef.getTranslationsForKey(pNbtRefKey);
  if(transTableNodePtr!=NULL)
  {
        // translations present in the cache translation table
    nbt=*transTableNodePtr;
  }
  else
  {
        // translations not present in the cache translation table
    for(PositionIndex i=ntarget.size();i<pbtmInputVars.nrefSentIdVec.size()-pNbtRefKey.numGaps;++i)
    {
      trgPhrase.push_back(pbtmInputVars.nrefSentIdVec[i]);
      if(trgPhrase.size()>=minTrgSize && trgPhrase.size()<=maxTrgSize)
      {
        Score scr=nbestTransScoreCached(srcPhrase,trgPhrase);
        nbt.insert(scr,trgPhrase);
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

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::transUncovGapRefLastGapCached(const Hypothesis& hyp,
                                                              PositionIndex srcLeft,
                                                              PositionIndex srcRight,
                                                              NbestTableNode<PhraseTransTableNodeData>& nbt)
{
  nbt.clear();
  std::vector<WordIndex> srcPhrase;
  std::vector<WordIndex> trgPhrase;
  std::vector<WordIndex> ntarget=hyp.getPartialTrans();

      // Obtain source phrase
  for(unsigned int i=srcLeft;i<=srcRight;++i)
  {
    srcPhrase.push_back(pbtmInputVars.nsrcSentIdVec[i]);
  }

      // Obtain length limits for target phrase
  unsigned int minTrgSize=0;
  if(srcPhrase.size()>this->pbTransModelPars.E) minTrgSize=srcPhrase.size()-this->pbTransModelPars.E;
  unsigned int maxTrgSize=srcPhrase.size()+this->pbTransModelPars.E;
  
      // Obtain target phrase
  for(PositionIndex i=ntarget.size();i<pbtmInputVars.nrefSentIdVec.size();++i)
    trgPhrase.push_back(pbtmInputVars.nrefSentIdVec[i]);

      // Obtain translations
  if(trgPhrase.size()>=minTrgSize && trgPhrase.size()<=maxTrgSize)
  {
    Score scr=nbestTransScoreCached(srcPhrase,trgPhrase);
    nbt.insert(scr,trgPhrase);
  }
}

//---------------------------------
template<class HYPOTHESIS>
bool _pbTransModel<HYPOTHESIS>::getTransForHypUncovGapVer(const Hypothesis& hyp,
                                                          PositionIndex srcLeft,
                                                          PositionIndex srcRight,
                                                          NbestTableNode<PhraseTransTableNodeData>& nbt,
                                                          float N)
{
  return getTransForHypUncovGap(hyp,srcLeft,srcRight,nbt,N);
}

//---------------------------------
template<class HYPOTHESIS>
bool _pbTransModel<HYPOTHESIS>::getTransForHypUncovGapPref(const Hypothesis& hyp,
                                                           PositionIndex srcLeft,
                                                           PositionIndex srcRight,
                                                           NbestTableNode<PhraseTransTableNodeData>& nbt,
                                                           float N)
{
  unsigned int ntrgSize=hyp.getPartialTrans().size();
      // Check if the prefix has been generated
  if(ntrgSize<pbtmInputVars.nprefSentIdVec.size())
  {
    transUncovGapPrefNoGenCached(hyp,srcLeft,srcRight,nbt,N);
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
void _pbTransModel<HYPOTHESIS>::transUncovGapPrefNoGenCached(const Hypothesis& hyp,
                                                             PositionIndex srcLeft,
                                                             PositionIndex srcRight,
                                                             NbestTableNode<PhraseTransTableNodeData>& nbt,
                                                             float N)
{
      // Initialize variables
  PhrNbestTransTablePrefKey pNbtPrefKey;
  pNbtPrefKey.srcLeft=srcLeft;
  pNbtPrefKey.srcRight=srcRight;
  pNbtPrefKey.ntrgSize=hyp.getPartialTrans().size();
  if(this->numberOfUncoveredSrcWords(hyp)-(srcRight-srcLeft+1)>0)
    pNbtPrefKey.lastGap=false;
  else pNbtPrefKey.lastGap=true;
  
      // Search the required translations in the cache translation
      // table
  NbestTableNode<PhraseTransTableNodeData>* transTableNodePtr=nbTransCacheData.cPhrNbestTransTablePref.getTranslationsForKey(pNbtPrefKey);
  if(transTableNodePtr!=NULL)
  {
        // translations present in the cache translation table
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
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::transUncovGapPrefNoGen(const Hypothesis& hyp,
                                                       PositionIndex srcLeft,
                                                       PositionIndex srcRight,
                                                       NbestTableNode<PhraseTransTableNodeData>& nbt)
{
  std::vector<WordIndex> srcPhrase;
    
      // Obtain source phrase
  nbt.clear();
  for(unsigned int i=srcLeft;i<=srcRight;++i)
  {
    srcPhrase.push_back(pbtmInputVars.nsrcSentIdVec[i]);
  }
      // Obtain length limits for target phrase
  unsigned int minTrgSize=0;
  if(srcPhrase.size()>this->pbTransModelPars.E) minTrgSize=srcPhrase.size()-this->pbTransModelPars.E;
  unsigned int maxTrgSize=srcPhrase.size()+this->pbTransModelPars.E;
    
  unsigned int ntrgSize=hyp.getPartialTrans().size();

      // Check if we are covering the last gap of the hypothesis
  if(this->numberOfUncoveredSrcWords(hyp)-(srcRight-srcLeft+1)>0)
  {
        // This is not the last gap to be covered.

        // Add translations with length in characters greater than the
        // prefix length.
    genListOfTransLongerThanPref(srcPhrase,ntrgSize,nbt);

        // Add translations with length lower than the prefix length.
    std::vector<WordIndex> trgPhrase;
    if(pbtmInputVars.nprefSentIdVec.size()>1)
    {
      for(PositionIndex i=ntrgSize;i<pbtmInputVars.nprefSentIdVec.size()-1;++i)
      {
        trgPhrase.push_back(pbtmInputVars.nprefSentIdVec[i]);
        if(trgPhrase.size()>=minTrgSize && trgPhrase.size()<=maxTrgSize)
        {
          Score scr=nbestTransScoreCached(srcPhrase,trgPhrase);
          nbt.insert(scr,trgPhrase);
        }
      }
    }
  }
  else
  {
        // This is the last gap to be covered.

        // Add translations with length in characters greater than the
        // prefix length.
    genListOfTransLongerThanPref(srcPhrase,ntrgSize,nbt);
  }
  
      // Insert the remaining prefix itself in nbt
  std::vector<WordIndex> remainingPref;
  for(unsigned int i=ntrgSize;i<pbtmInputVars.nprefSentIdVec.size();++i)
    remainingPref.push_back(pbtmInputVars.nprefSentIdVec[i]);
  nbt.insert(nbestTransScoreLastCached(srcPhrase,remainingPref),remainingPref);
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::genListOfTransLongerThanPref(std::vector<WordIndex> srcPhrase,
                                                             unsigned int ntrgSize,
                                                             NbestTableNode<PhraseTransTableNodeData>& nbt)
{
  std::vector<WordIndex> remainingPref;

      // clear nbt
  nbt.clear();
  
      // Store the remaining prefix to be generated in remainingPref
  for(unsigned int i=ntrgSize;i<pbtmInputVars.nprefSentIdVec.size();++i)
    remainingPref.push_back(pbtmInputVars.nprefSentIdVec[i]);

      // Obtain translations for source segment srcPhrase
  std::set<std::vector<WordIndex> > transSet;
  getTransForSrcPhrase(srcPhrase,transSet);
  for(std::set<std::vector<WordIndex> >::iterator transSetIter=transSet.begin();transSetIter!=transSet.end();++transSetIter)
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
          Score scr=nbestTransScoreLastCached(srcPhrase,*transSetIter);
          nbt.insert(scr,*transSetIter);
        }
      }
    }
  }
}

//---------------------------------
template<class HYPOTHESIS>
bool _pbTransModel<HYPOTHESIS>::trgWordVecIsPrefix(const std::vector<WordIndex>& wiVec1,
                                                   bool lastWiVec1WordIsComplete,
                                                   const std::string& lastWiVec1Word,
                                                   const std::vector<WordIndex>& wiVec2,
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
Score _pbTransModel<HYPOTHESIS>::nbestTransScoreCached(const std::vector<WordIndex>& srcPhrase,
                                                       const std::vector<WordIndex>& trgPhrase)
{
  PhrasePairCacheTable::iterator ppctIter;
  ppctIter=nbTransCacheData.cnbestTransScore.find(std::make_pair(srcPhrase,trgPhrase));
  if(ppctIter!=nbTransCacheData.cnbestTransScore.end())
  {
        // Score was previously stored in the cache table
    return ppctIter->second;
  }
  else
  {
        // Score is not stored in the cache table
    Score scr=nbestTransScore(srcPhrase,trgPhrase);
    nbTransCacheData.cnbestTransScore[std::make_pair(srcPhrase,trgPhrase)]=scr;
    return scr;
  }
}

//---------------------------------
template<class HYPOTHESIS>
Score _pbTransModel<HYPOTHESIS>::nbestTransScoreLastCached(const std::vector<WordIndex>& srcPhrase,
                                                           const std::vector<WordIndex>& trgPhrase)
{
  PhrasePairCacheTable::iterator ppctIter;
  ppctIter=nbTransCacheData.cnbestTransScoreLast.find(std::make_pair(srcPhrase,trgPhrase));
  if(ppctIter!=nbTransCacheData.cnbestTransScoreLast.end())
  {
        // Score was previously stored in the cache table
    return ppctIter->second;
  }
  else
  {
        // Score is not stored in the cache table
    Score scr=nbestTransScoreLast(srcPhrase,trgPhrase);
    nbTransCacheData.cnbestTransScoreLast[std::make_pair(srcPhrase,trgPhrase)]=scr;
    return scr;
  }
}

//---------------------------------
template<class HYPOTHESIS>
std::vector<Score>
_pbTransModel<HYPOTHESIS>::scoreCompsForHyp(const _pbTransModel::Hypothesis& hyp)
{
      // Obtain null hypothesis score components
  Hypothesis nullHyp;
  std::vector<Score> nullHypScoreComponents;
  nullHypothesisScrComps(nullHyp,nullHypScoreComponents);
  
      // Obtain score components
  Hypothesis auxHyp;
  std::vector<Score> scoreComponents;
  HypDataType hypDataType=hyp.getData();
  this->incrScore(nullHyp,hypDataType,auxHyp,scoreComponents);

      // Accumulate score component values
  std::vector<Score> result;
  for(unsigned int i=0;i<scoreComponents.size();++i)
  {
    result.push_back(nullHypScoreComponents[i]+scoreComponents[i]);
  }
  
  return result;
}

//---------------------------------
template<class HYPOTHESIS>
WordIndex _pbTransModel<HYPOTHESIS>::stringToSrcWordIndex(std::string s)
{
  if(singleWordVocab.existSrcSymbol(s))
    return singleWordVocab.stringToSrcWordIndex(s);
  else
    return singleWordVocab.addSrcSymbol(s);
}

//---------------------------------
template<class HYPOTHESIS>
std::string _pbTransModel<HYPOTHESIS>::wordIndexToSrcString(WordIndex w)const
{
  return singleWordVocab.wordIndexToSrcString(w);
}

//---------------------------------
template<class HYPOTHESIS>
std::vector<std::string> _pbTransModel<HYPOTHESIS>::srcIndexVectorToStrVector(std::vector<WordIndex> srcidxVec)const
{
  std::vector<std::string> srcwordVec;
  for(unsigned int i=0;i<srcidxVec.size();++i)
  {
    srcwordVec.push_back(wordIndexToSrcString(srcidxVec[i]));
  }
  return srcwordVec;
}

//--------------------------------- 
template<class HYPOTHESIS>
std::vector<WordIndex> _pbTransModel<HYPOTHESIS>::strVectorToSrcIndexVector(std::vector<std::string> srcStrVec)
{
  std::vector<WordIndex> srcidxVec;
  for(unsigned int i=0;i<srcStrVec.size();++i)
  {
    srcidxVec.push_back(stringToSrcWordIndex(srcStrVec[i]));
  }
  return srcidxVec;
}

//--------------------------------- 
template<class HYPOTHESIS>
WordIndex _pbTransModel<HYPOTHESIS>::stringToTrgWordIndex(std::string s)
{
  if(singleWordVocab.existTrgSymbol(s))
    return singleWordVocab.stringToTrgWordIndex(s);
  else
    return singleWordVocab.addTrgSymbol(s);
}

//--------------------------------- 
template<class HYPOTHESIS>
std::string _pbTransModel<HYPOTHESIS>::wordIndexToTrgString(WordIndex w)const
{
  return singleWordVocab.wordIndexToTrgString(w);
}

//---------------------------------
template<class HYPOTHESIS>
std::vector<std::string> _pbTransModel<HYPOTHESIS>::trgIndexVectorToStrVector(std::vector<WordIndex> trgidxVec)const
{
  std::vector<std::string> trgwordVec;
  for(unsigned int i=0;i<trgidxVec.size();++i)
  {
    trgwordVec.push_back(wordIndexToTrgString(trgidxVec[i]));
  }
  return trgwordVec;
}

//--------------------------------- 
template<class HYPOTHESIS>
std::vector<WordIndex> _pbTransModel<HYPOTHESIS>::strVectorToTrgIndexVector(std::vector<std::string> trgStrVec)
{
  std::vector<WordIndex> trgidxVec;
  for(unsigned int i=0;i<trgStrVec.size();++i)
  {
    trgidxVec.push_back(stringToTrgWordIndex(trgStrVec[i]));
  }
  return trgidxVec;
}

//---------------------------------
template<class HYPOTHESIS>
_pbTransModel<HYPOTHESIS>::~_pbTransModel()
{
}

//-------------------------

#endif
