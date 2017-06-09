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
/* Module: _pbTransModel                                            */
/*                                                                  */
/* Prototypes file: _pbTransModel.h                                 */
/*                                                                  */
/* Description: Declares the _pbTransModel class.                   */
/*              This class is a succesor of the BasePbTransModel    */
/*              class.                                              */
/*                                                                  */
/********************************************************************/

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
#include "StatModelDefs.h"
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
  void addSentenceToWordPred(Vector<std::string> strVec,
                             int verbose=0);
  pair<Count,std::string> getBestSuffix(std::string input);
  pair<Count,std::string> getBestSuffixGivenHist(Vector<std::string> hist,
                                                 std::string input);

  ////// Hypothesis-related functions

      // Misc. operations with hypothesis
  Hypothesis nullHypothesis(void);

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
  void setWeights(Vector<float> wVec);
  void getWeights(Vector<pair<std::string,float> >& compWeights);
  void printWeights(ostream &outS);
  unsigned int getNumWeights(void);
  Vector<Score> scoreCompsForHyp(const Hypothesis& hyp);

      // Specific phrase-based functions
  void extendHypData(PositionIndex srcLeft,
                     PositionIndex srcRight,
                     const Vector<std::string>& trgPhrase,
                     HypDataType& hypd);

      // Destructor
  ~_pbTransModel();

 protected:
    
      // Variable to store state of the translation model
  unsigned int state;

      // Heuristic function to be used
  unsigned int heuristicId;

      // Feature vector information
  FeaturesInfo<HypScoreInfo>* featuresInfoPtr;

      // Vocabulary handler
  SingleWordVocab singleWordVocab;

      // Data structure to store input variables
  PbTransModelInputVars pbtmInputVars;

      // Set of unseen words
  std::set<std::string> unseenWordsSet;

  ////// Hypotheses-related functions

      // Expansion-related functions
  void extract_gaps(const Hypothesis& hyp,
                    Vector<pair<PositionIndex,PositionIndex> >& gaps);
  void extract_gaps(const Bitset<MAX_SENTENCE_LENGTH_ALLOWED>& hypKey,
                    Vector<pair<PositionIndex,PositionIndex> >& gaps);
  unsigned int get_num_gaps(const Bitset<MAX_SENTENCE_LENGTH_ALLOWED>& hypKey);

      // Misc. operations with hypothesis
  virtual Score nullHypothesisScrComps(Hypothesis& nullHyp,
                                       Vector<Score>& scoreComponents)=0;

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
  virtual bool getTransForHypUncovGap(const Hypothesis& hyp,
                                      PositionIndex srcLeft,
                                      PositionIndex srcRight,
                                      NbestTableNode<PhraseTransTableNodeData>& nbt,
                                      float N);
      // Get N-best translations for a subphrase of the source sentence
      // to be translated .  If N is between 0 and 1 then N represents a
      // threshold.  The result of the search is cached in the data
      // member cPhrNbestTransTable
  virtual bool getNbestTransForSrcPhrase(Vector<WordIndex> srcPhrase,
                                         NbestTableNode<PhraseTransTableNodeData>& nbt,
                                         float N);
      // Get N-best translations for a given source phrase s_.
      // If N is between 0 and 1 then N represents a threshold
      // Functions to generate translation lists
  bool getTransForSrcPhrase(const Vector<WordIndex>& srcPhrase,
                            std::set<Vector<WordIndex> >& transSet);

      // Functions to score n-best translations lists
  Score nbestTransScore(const Vector<WordIndex>& srcPhrase,
                        const Vector<WordIndex>& trgPhrase);

      // Functions related to getTransInPlainTextVec
  Vector<std::string> getTransInPlainTextVecTs(const Hypothesis& hyp)const;

      // Heuristic related functions
  virtual Score calcHeuristicScore(const _pbTransModel::Hypothesis& hyp);

      // Functions related to pre_trans_actions
  virtual void clearTempVars(void);
  void verifyDictCoverageForSentence(Vector<std::string>& sentenceVec,
                                     int maxSrcPhraseLength=MAX_SENTENCE_LENGTH_ALLOWED);
  void manageUnseenSrcWord(std::string srcw);
  bool unseenSrcWord(std::string srcw);
  bool unseenSrcWordGivenPosition(unsigned int srcPos);
  Score unkWordScoreHeur(void);
  void initHeuristic(unsigned int maxSrcPhraseLength);

      // Vocabulary functions
  WordIndex stringToSrcWordIndex(std::string s);
  std::string wordIndexToSrcString(WordIndex w)const;
  Vector<std::string> srcIndexVectorToStrVector(Vector<WordIndex> srcidxVec)const;
  Vector<WordIndex> strVectorToSrcIndexVector(Vector<std::string> srcStrVec);
  WordIndex stringToTrgWordIndex(std::string s);
  std::string wordIndexToTrgString(WordIndex w)const;
  Vector<std::string> trgIndexVectorToStrVector(Vector<WordIndex> trgidxVec)const;
  Vector<WordIndex> strVectorToTrgIndexVector(Vector<std::string> trgStrVec);
};

//--------------- _pbTransModel class functions
//

template<class HYPOTHESIS>
_pbTransModel<HYPOTHESIS>::_pbTransModel(void):BasePbTransModel<HYPOTHESIS>()
{
      // Set state info
  state=MODEL_IDLE_STATE;

      // Initialize feature information pointer
  featuresInfoPtr=NULL;
  
      // Initially, no heuristic is used
  heuristicId=NO_HEURISTIC;
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::link_feats_info(FeaturesInfo<HypScoreInfo>* _featuresInfoPtr)
{
  featuresInfoPtr=_featuresInfoPtr;
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::clear(void)
{
      // Set state info
  state=MODEL_IDLE_STATE;

      // Initialize feature information pointer
  featuresInfoPtr=NULL;

      // Initially, no heuristic is used
  heuristicId=NO_HEURISTIC;

  singleWordVocab.clear();

      // Clear input variables
  pbtmInputVars.clear();
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::pre_trans_actions(std::string srcsent)
{
      // Clear temporary variables
  this->clearTempVars();

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
void _pbTransModel<HYPOTHESIS>::pre_trans_actions_ref(std::string srcsent,
                                                      std::string refsent)
{
      // TO-BE-DONE
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::pre_trans_actions_ver(std::string srcsent,
                                                      std::string refsent)
{
      // TO-BE-DONE
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::pre_trans_actions_prefix(std::string srcsent,
                                                         std::string prefix)
{
      // TO-BE-DONE
}

//---------------------------------
template<class HYPOTHESIS>
std::string _pbTransModel<HYPOTHESIS>::getCurrentSrcSent(void)
{
  return StrProcUtils::stringVectorToString(pbtmInputVars.srcSentVec);
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::addSentenceToWordPred(Vector<std::string> /*strVec*/,
                                                      int /*verbose=0*/)
{
  cerr<<"Warning, addSentenceToWordPred function intentionally not implemented for this class"<<endl;
}

//---------------------------------
template<class HYPOTHESIS>
pair<Count,std::string>
  _pbTransModel<HYPOTHESIS>::getBestSuffix(std::string /*input*/)
{
  cerr<<"Warning, getBestSuffix function intentionally not implemented for this class"<<endl;
  pair<Count,std::string> result;
  return result;
}

//---------------------------------
template<class HYPOTHESIS>
pair<Count,std::string>
  _pbTransModel<HYPOTHESIS>::getBestSuffixGivenHist(Vector<std::string> /*hist*/,
                                                    std::string /*input*/)
{
  cerr<<"Warning, getBestSuffixGivenHist function intentionally not implemented for this class"<<endl;
  pair<Count,std::string> result;
  return result;
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::extract_gaps(const Hypothesis& hyp,
                                             Vector<pair<PositionIndex,PositionIndex> >& gaps)
{
  extract_gaps(hyp.getKey(),gaps);
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::extract_gaps(const Bitset<MAX_SENTENCE_LENGTH_ALLOWED>& hypKey,
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
  Vector<Score> scrComp;
  Hypothesis nullHyp;
  nullHypothesisScrComps(nullHyp,scrComp);
  return nullHyp;
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::expand(const Hypothesis& hyp,
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
          }
        }
      }
    }
  }
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::expand_ref(const Hypothesis& hyp,
                                           Vector<Hypothesis>& hypVec,
                                           Vector<Vector<Score> >& scrCompVec)
{
      // TO-BE-DONE
  
/*   Vector<pair<PositionIndex,PositionIndex> > gaps; */
/*   Vector<WordIndex> s_; */
/*   Hypothesis extHyp; */
/*   Vector<HypDataType> hypDataVec; */
/*   Vector<Score> scoreComponents; */

/*   hypVec.clear(); */
/*   scrCompVec.clear(); */
  
/*       // Extract gaps	 */
/*   extract_gaps(hyp,gaps); */
   
/*       // Generate new hypotheses completing the gaps */
/*   for(unsigned int k=0;k<gaps.size();++k) */
/*   { */
/*     unsigned int gap_length=gaps[k].second-gaps[k].first+1; */
/*     for(unsigned int x=0;x<gap_length;++x) */
/*     { */
/*       s_.clear(); */
/*       if(x<=this->pbTransModelPars.U) // x should be lower than U, which is the maximum */
/*                      // number of words that can be jUmped */
/*       { */
/*         for(unsigned int y=x;y<gap_length;++y) */
/*         { */
/*           unsigned int segmRightMostj=gaps[k].first+y; */
/*           unsigned int segmLeftMostj=gaps[k].first+x; */
/*               // Verify that the source phrase length does not exceed */
/*               // the limit */
/*           if((segmRightMostj-segmLeftMostj)+1 > this->pbTransModelPars.A)  */
/*             break; */
/*               // Obtain hypothesis data vector */
/*           getHypDataVecForGapRef(hyp,segmLeftMostj,segmRightMostj,hypDataVec,this->pbTransModelPars.W); */
/*           if(hypDataVec.size()!=0) */
/*           { */
/*             for(unsigned int i=0;i<hypDataVec.size();++i) */
/*             { */
/*               this->incrScore(hyp,hypDataVec[i],extHyp,scoreComponents); */
/*               hypVec.push_back(extHyp); */
/*               scrCompVec.push_back(scoreComponents); */
/*             } */
/* #           ifdef THOT_STATS     */
/*             ++this->basePbTmStats.getTransCalls; */
/*             this->basePbTmStats.transOptions+=hypDataVec.size(); */
/* #           endif     */
/*           } */
/*         } */
/*       } */
/*     } */
/*   }      */
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::expand_ver(const Hypothesis& hyp,
                                           Vector<Hypothesis>& hypVec,
                                           Vector<Vector<Score> >& scrCompVec)
{
      // TO-BE-DONE
  
/*   Vector<pair<PositionIndex,PositionIndex> > gaps; */
/*   Vector<WordIndex> s_; */
/*   Hypothesis extHyp; */
/*   Vector<HypDataType> hypDataVec; */
/*   Vector<Score> scoreComponents; */

/*   hypVec.clear(); */
/*   scrCompVec.clear(); */
  
/*       // Extract gaps	 */
/*   extract_gaps(hyp,gaps); */
   
/*       // Generate new hypotheses completing the gaps */
/*   for(unsigned int k=0;k<gaps.size();++k) */
/*   { */
/*     unsigned int gap_length=gaps[k].second-gaps[k].first+1; */
/*     for(unsigned int x=0;x<gap_length;++x) */
/*     { */
/*       s_.clear(); */
/*       if(x<=this->pbTransModelPars.U) // x should be lower than U, which is the maximum */
/*                // number of words that can be jUmped */
/*       { */
/*         for(unsigned int y=x;y<gap_length;++y) */
/*         { */
/*           unsigned int segmRightMostj=gaps[k].first+y; */
/*           unsigned int segmLeftMostj=gaps[k].first+x; */
/*               // Verify that the source phrase length does not exceed */
/*               // the limit */
/*           if((segmRightMostj-segmLeftMostj)+1 > this->pbTransModelPars.A)  */
/*             break; */
/*               // Obtain hypothesis data vector */
/*           getHypDataVecForGapVer(hyp,segmLeftMostj,segmRightMostj,hypDataVec,this->pbTransModelPars.W); */
/*           if(hypDataVec.size()!=0) */
/*           { */
/*             for(unsigned int i=0;i<hypDataVec.size();++i) */
/*             { */
/*               this->incrScore(hyp,hypDataVec[i],extHyp,scoreComponents); */
/*               hypVec.push_back(extHyp); */
/*               scrCompVec.push_back(scoreComponents); */
/*             } */
/* #           ifdef THOT_STATS     */
/*             ++this->basePbTmStats.getTransCalls; */
/*             this->basePbTmStats.transOptions+=hypDataVec.size(); */
/* #           endif     */
/*           } */
/*         } */
/*       } */
/*     } */
/*   }        */
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::expand_prefix(const Hypothesis& hyp,
                                              Vector<Hypothesis>& hypVec,
                                              Vector<Vector<Score> >& scrCompVec)
{
      // TO-BE-DONE

/*   Vector<pair<PositionIndex,PositionIndex> > gaps; */
/*   Vector<WordIndex> s_; */
/*   Hypothesis extHyp; */
/*   Vector<HypDataType> hypDataVec; */
/*   Vector<Score> scoreComponents; */

/*   hypVec.clear(); */
/*   scrCompVec.clear(); */

/*       // Extract gaps	 */
/*   extract_gaps(hyp,gaps); */
   
/*       // Generate new hypotheses completing the gaps */
/*   for(unsigned int k=0;k<gaps.size();++k) */
/*   { */
/*     unsigned int gap_length=gaps[k].second-gaps[k].first+1; */
/*     for(unsigned int x=0;x<gap_length;++x) */
/*     { */
/*       s_.clear(); */
/*       if(x<=this->pbTransModelPars.U) // x should be lower than U, which is the maximum */
/*                // number of words that can be jUmped */
/*       { */
/*         for(unsigned int y=x;y<gap_length;++y) */
/*         { */
/*           unsigned int segmRightMostj=gaps[k].first+y; */
/*           unsigned int segmLeftMostj=gaps[k].first+x; */
/*               // Verify that the source phrase length does not exceed */
/*               // the limit */
/*           if((segmRightMostj-segmLeftMostj)+1 > this->pbTransModelPars.A)  */
/*             break; */
/*               // Obtain hypothesis data vector */
/*           getHypDataVecForGapPref(hyp,segmLeftMostj,segmRightMostj,hypDataVec,this->pbTransModelPars.W); */
/*           if(hypDataVec.size()!=0) */
/*           { */
/*             for(unsigned int i=0;i<hypDataVec.size();++i) */
/*             { */
/*               this->incrScore(hyp,hypDataVec[i],extHyp,scoreComponents); */
/*               hypVec.push_back(extHyp); */
/*               scrCompVec.push_back(scoreComponents); */
/*             } */
/* #           ifdef THOT_STATS     */
/*             ++this->basePbTmStats.getTransCalls; */
/*             this->basePbTmStats.transOptions+=hypDataVec.size(); */
/* #           endif     */
/*           } */
/*         } */
/*       } */
/*     } */
/*   }        */
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
}

//---------------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::verifyDictCoverageForSentence(Vector<std::string>& sentenceVec,
                                                              int /*maxSrcPhraseLength*/)
{
      // Manage source words without translation options
  for(unsigned int j=0;j<sentenceVec.size();++j)
  {
    NbestTableNode<PhraseTransTableNodeData> ttNode;
    std::string s=sentenceVec[j];
    Vector<WordIndex> srcPhrase;
    srcPhrase.push_back(stringToSrcWordIndex(s));
    std::set<Vector<WordIndex> > transSet;
    getTransForSrcPhrase(srcPhrase,transSet);
    if(transSet.size()==0)
    {
      manageUnseenSrcWord(s);
    }
  }
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::manageUnseenSrcWord(std::string srcw)
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
      // TO-BE-DONE
  return 0;
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::initHeuristic(unsigned int maxSrcPhraseLength)
{
      // TO-BE-DONE
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
      // TO-BE-DONE
  Score score=0;
  return score;
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::printHyp(const Hypothesis& hyp,
                                        ostream &outS,
                                        int verbose)
{
      // Obtain target string vector
  Vector<std::string> trgStrVec=trgIndexVectorToStrVector(hyp.getPartialTrans());

      // Print score
  outS <<"Score: "<<hyp.getScore()<<" ; ";
      // Print weights
  this->printWeights(outS);
  outS <<" ; ";

      // Obtain score components
  Hypothesis auxHyp;
  Vector<Score> scoreComponents;
  HypDataType hypDataType=hyp.getData();
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
  SourceSegmentation sourceSegmentation;
  Vector<PositionIndex> targetSegmentCuts;
  Vector<pair<PositionIndex,PositionIndex> > amatrix;
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
}

//---------------------------------
template<class HYPOTHESIS>
Vector<std::string> _pbTransModel<HYPOTHESIS>::getTransInPlainTextVec(const _pbTransModel::Hypothesis& hyp)const
{
      // TO-BE-DONE
  switch(state)
  {
    case MODEL_TRANS_STATE: return getTransInPlainTextVecTs(hyp);
    /* case MODEL_TRANSPREFIX_STATE: return getTransInPlainTextVecTps(hyp); */
    /* case MODEL_TRANSREF_STATE: return getTransInPlainTextVecTrs(hyp); */
    /* case MODEL_TRANSVER_STATE: return getTransInPlainTextVecTvs(hyp); */
    default: Vector<std::string> strVec;
      return strVec;
  }
}

//---------------------------------
template<class HYPOTHESIS>
Vector<std::string> _pbTransModel<HYPOTHESIS>::getTransInPlainTextVecTs(const _pbTransModel<HYPOTHESIS>::Hypothesis& hyp)const
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
void _pbTransModel<HYPOTHESIS>::setWeights(Vector<float> wVec)
{
  for(unsigned int i=0;i<featuresInfoPtr->featPtrVec.size();++i)
  {
    if(i<wVec.size())
      featuresInfoPtr->featPtrVec[i]->setWeight(wVec[i]);
  }
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::getWeights(Vector<pair<std::string,float> >& compWeights)
{
  compWeights.clear();
  for(unsigned int i=0;i<featuresInfoPtr->featPtrVec.size();++i)
  {
    pair<std::string,float> str_float;
    std::string weightName=featuresInfoPtr->featPtrVec[i]->getFeatName();
    weightName+="w";
    str_float.first=weightName;
    str_float.second=featuresInfoPtr->featPtrVec[i]->getWeight();
    compWeights.push_back(str_float);
  }
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::printWeights(ostream &outS)
{
  Vector<pair<std::string,float> > compWeights;
  getWeights(compWeights);
  for(unsigned int i=0;i<compWeights.size();++i)
  {
    cerr<<compWeights[i].first<<": "<<compWeights[i].second;
    if(i!=compWeights.size()-1)
      cerr<<" , ";
  }
}

//---------------------------------
template<class HYPOTHESIS>
unsigned int _pbTransModel<HYPOTHESIS>::getNumWeights(void)
{
  return featuresInfoPtr->featPtrVec.size();
}

//---------------------------------
template<class HYPOTHESIS>
void _pbTransModel<HYPOTHESIS>::extendHypData(PositionIndex srcLeft,
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
bool _pbTransModel<HYPOTHESIS>::getHypDataVecForGap(const Hypothesis& hyp,
                                                    PositionIndex srcLeft,
                                                    PositionIndex srcRight,
                                                    Vector<HypDataType>& hypDataTypeVec,
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
    cerr<<"  trying to cover from src. pos. "<<srcLeft<<" to "<<srcRight<<"; ";
    cerr<<"Filtered "<<ttNode.size()<<" translations"<<endl;
  }

      // Generate hypothesis data for translations
  NbestTableNode<PhraseTransTableNodeData>::iterator ttNodeIter;
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
bool _pbTransModel<HYPOTHESIS>::getTransForSrcPhrase(const Vector<WordIndex>& srcPhrase,
                                                     std::set<Vector<WordIndex> >& transSet)
{
      // Clear data structures
  transSet.clear();
  
      // Obtain string vector
  Vector<std::string> srcPhraseStr=srcIndexVectorToStrVector(srcPhrase);
  
      // Obtain translation options for each feature
  for(unsigned int i=0;i<this->featuresInfoPtr->featPtrVec.size();++i)
  {
        // Obtain options
    Vector<Vector<std::string> > transOptVec;
    this->featuresInfoPtr->featPtrVec[i]->obtainTransOptions(srcPhraseStr,transOptVec);

        // Add options to set
    for(unsigned int j=0;j<transOptVec.size();++j)
      transSet.insert(strVectorToTrgIndexVector(transOptVec[j]));
  }

  if(transSet.empty())
    return false;
  else
    return true;
}

//---------------------------------
template<class HYPOTHESIS>
Score _pbTransModel<HYPOTHESIS>::nbestTransScore(const Vector<WordIndex>& srcPhrase,
                                                 const Vector<WordIndex>& trgPhrase)
{
  Score result=0;
  
      // Obtain string vectors
  Vector<std::string> srcPhraseStr=srcIndexVectorToStrVector(srcPhrase);
  Vector<std::string> trgPhraseStr=trgIndexVectorToStrVector(trgPhrase);
  
      // Obtain translation options for each feature
  for(unsigned int i=0;i<this->featuresInfoPtr->featPtrVec.size();++i)
  {
    result+=this->featuresInfoPtr->featPtrVec[i]->scorePhrasePair(srcPhraseStr,trgPhraseStr);
  }

  return result;
}

//---------------------------------
template<class HYPOTHESIS>
bool _pbTransModel<HYPOTHESIS>::getNbestTransForSrcPhrase(Vector<WordIndex> srcPhrase,
                                                          NbestTableNode<PhraseTransTableNodeData>& nbt,
                                                          float N)
{
  BasePhraseModel::SrcTableNode srctn;
  BasePhraseModel::SrcTableNode::iterator srctnIter;
  bool ret;

      // Obtain the whole list of translations
  nbt.clear();
  std::set<Vector<WordIndex> > transSet;
  ret=getTransForSrcPhrase(srcPhrase,transSet);
  if(!ret) return false;
  else
  {
    Score scr;

        // This loop may become a bottleneck if the number of translation
        // options is high
    for(std::set<Vector<WordIndex> >::iterator transSetIter=transSet.begin();transSetIter!=transSet.end();++transSetIter)
    {
      scr=nbestTransScore(srcPhrase,*transSetIter);
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
bool _pbTransModel<HYPOTHESIS>::getTransForHypUncovGap(const Hypothesis& hyp,
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
          // search translations for source phrase in translation table
      NbestTableNode<PhraseTransTableNodeData> *transTableNodePtr;
      Vector<WordIndex> srcPhrase;
    
      for(unsigned int i=srcLeft;i<=srcRight;++i)
      {
        srcPhrase.push_back(pbtmInputVars.nsrcSentIdVec[i]);
      }
    
      getNbestTransForSrcPhrase(srcPhrase,nbt,N);
      if(nbt.size()==0) return false;
      else return true;
    }
  }
}

//---------------------------------
template<class HYPOTHESIS>
Vector<Score>
_pbTransModel<HYPOTHESIS>::scoreCompsForHyp(const _pbTransModel::Hypothesis& hyp)
{
      // Obtain null hypothesis score components
  Hypothesis nullHyp;
  Vector<Score> nullHypScoreComponents;
  nullHypothesisScrComps(nullHyp,nullHypScoreComponents);
  
      // Obtain score components
  Hypothesis auxHyp;
  Vector<Score> scoreComponents;
  HypDataType hypDataType=hyp.getData();
  this->incrScore(nullHyp,hypDataType,auxHyp,scoreComponents);

      // Accumulate score component values
  Vector<Score> result;
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
Vector<std::string> _pbTransModel<HYPOTHESIS>::srcIndexVectorToStrVector(Vector<WordIndex> srcidxVec)const
{
  Vector<std::string> srcwordVec;
  for(unsigned int i=0;i<srcidxVec.size();++i)
  {
    srcwordVec.push_back(wordIndexToSrcString(srcidxVec[i]));
  }
  return srcwordVec;
}

//--------------------------------- 
template<class HYPOTHESIS>
Vector<WordIndex> _pbTransModel<HYPOTHESIS>::strVectorToSrcIndexVector(Vector<std::string> srcStrVec)
{
  Vector<WordIndex> srcidxVec;
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
Vector<std::string> _pbTransModel<HYPOTHESIS>::trgIndexVectorToStrVector(Vector<WordIndex> trgidxVec)const
{
  Vector<std::string> trgwordVec;
  for(unsigned int i=0;i<trgidxVec.size();++i)
  {
    trgwordVec.push_back(wordIndexToTrgString(trgidxVec[i]));
  }
  return trgwordVec;
}

//--------------------------------- 
template<class HYPOTHESIS>
Vector<WordIndex> _pbTransModel<HYPOTHESIS>::strVectorToTrgIndexVector(Vector<std::string> trgStrVec)
{
  Vector<WordIndex> trgidxVec;
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
