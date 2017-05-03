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
 * succesor of the BasePbTransModel class.
 */

#ifndef _PbTransModel_h
#define _PbTransModel_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "DirectPhraseModelFeat.h"
#include "WordPenaltyFeat.h"
#include "BaseSmtModelFeature.h"
#include "BasePbTransModel.h"
#include "SourceSegmentation.h"
#include "PbTransModelInputVars.h"
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

//--------------- PbTransModel class

/**
 * @brief The PbTransModel class is a predecessor of the
 * BasePbTransModel class.
 */

template<class HYPOTHESIS>
class PbTransModel: public BasePbTransModel<HYPOTHESIS>
{
 public:

  typedef typename BasePbTransModel<HYPOTHESIS>::Hypothesis Hypothesis;
  typedef typename BasePbTransModel<HYPOTHESIS>::HypScoreInfo HypScoreInfo;
  typedef typename BasePbTransModel<HYPOTHESIS>::HypDataType HypDataType;

      // Constructor
  PbTransModel();

      // Virtual object copy
  BaseSmtModel<HYPOTHESIS>* clone(void);

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

  ////// Hypothesis-related functions

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

      // Misc. operations with hypotheses
  Hypothesis nullHypothesis(void);
  HypDataType nullHypothesisHypData(void);
  bool obtainPredecessorHypData(HypDataType& hypd);
  bool isCompleteHypData(const HypDataType& hypd)const;

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

      // Functions for performing on-line training
  int onlineTrainSentPair(const char *srcSent,
                          const char *refSent,
                          const char *sysSent,
                          int verbose=0);

      // Destructor
  ~PbTransModel();

 protected:

      // Variable to store state of the translation model
  unsigned int state;

      // Heuristic function to be used
  unsigned int heuristicId;

      // Feature vector
  Vector<BaseSmtModelFeature<HYPOTHESIS>* > featVec;
  
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

template<class HYPOTHESIS>
PbTransModel<HYPOTHESIS>::PbTransModel(void):BasePbTransModel<HYPOTHESIS>()
{
      // Set state info
  state=MODEL_IDLE_STATE;

      // Initially, no heuristic is used
  heuristicId=NO_HEURISTIC;
}

//---------------------------------
template<class HYPOTHESIS>
BaseSmtModel<HYPOTHESIS>* PbTransModel<HYPOTHESIS>::clone(void)
{
  return new PbTransModel<HYPOTHESIS>(*this);
}

//---------------------------------
template<class HYPOTHESIS>
bool PbTransModel<HYPOTHESIS>::loadLangModel(const char* prefixFileName)
{
      // TO-BE-DONE
  return OK;
}

//---------------------------------
template<class HYPOTHESIS>
bool PbTransModel<HYPOTHESIS>::loadAligModel(const char* prefixFileName)
{
      // TO-BE-DONE
  return OK;
}

//---------------------------------
template<class HYPOTHESIS>
bool PbTransModel<HYPOTHESIS>::printLangModel(std::string printPrefix)
{
      // TO-BE-DONE
  return OK;
}

//---------------------------------
template<class HYPOTHESIS>
bool PbTransModel<HYPOTHESIS>::printAligModel(std::string printPrefix)
{
      // TO-BE-DONE
  return OK;
}

//---------------------------------
template<class HYPOTHESIS>
void PbTransModel<HYPOTHESIS>::clear(void)
{
      // TO-BE-DONE
      
      // Set state info
  state=MODEL_IDLE_STATE;
}

//---------------------------------
template<class HYPOTHESIS>
void PbTransModel<HYPOTHESIS>::pre_trans_actions(std::string srcsent)
{
      // TO-BE-DONE
}

//---------------------------------
template<class HYPOTHESIS>
void PbTransModel<HYPOTHESIS>::pre_trans_actions_ref(std::string srcsent,
                                                     std::string refsent)
{
      // TO-BE-DONE
}

//---------------------------------
template<class HYPOTHESIS>
void PbTransModel<HYPOTHESIS>::pre_trans_actions_ver(std::string srcsent,
                                                     std::string refsent)
{
      // TO-BE-DONE
}

//---------------------------------
template<class HYPOTHESIS>
void PbTransModel<HYPOTHESIS>::pre_trans_actions_prefix(std::string srcsent,
                                                        std::string prefix)
{
      // TO-BE-DONE
}

//---------------------------------
template<class HYPOTHESIS>
std::string PbTransModel<HYPOTHESIS>::getCurrentSrcSent(void)
{
      // TO-BE-DONE
  /* return StrProcUtils::stringVectorToString(pbtmInputVars.srcSentVec); */
}

//---------------------------------
template<class HYPOTHESIS>
void PbTransModel<HYPOTHESIS>::addSentenceToWordPred(Vector<std::string> strVec,
                                                     int verbose/*=0*/)
{
      // TO-BE-DONE
}

//---------------------------------
template<class HYPOTHESIS>
pair<Count,std::string>
PbTransModel<HYPOTHESIS>::getBestSuffix(std::string input)
{
      // TO-BE-DONE
}

//---------------------------------
template<class HYPOTHESIS>
pair<Count,std::string>
PbTransModel<HYPOTHESIS>::getBestSuffixGivenHist(Vector<std::string> hist,
                                                 std::string input)
{
      // TO-BE-DONE
}

//---------------------------------
template<class HYPOTHESIS>
void PbTransModel<HYPOTHESIS>::expand(const Hypothesis& hyp,
                                      Vector<Hypothesis>& hypVec,
                                      Vector<Vector<Score> >& scrCompVec)
{
/*   Vector<pair<PositionIndex,PositionIndex> > gaps; */
/*   Vector<WordIndex> s_; */
/*   Hypothesis extHyp; */
/*   Vector<HypDataType> hypDataVec; */
/*   Vector<Score> scoreComponents; */
  
/*   hypVec.clear(); */
/*   scrCompVec.clear(); */
  
/*       // Extract gaps	 */
/*   extract_gaps(hyp,gaps); */
/*   if(this->verbosity>=2) */
/*   { */
/*     cerr<<"  gaps: "<<gaps.size()<<endl; */
/*   } */
   
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
/*           bool srcPhraseIsAffectedByConstraint=this->trConstraintsPtr->srcPhrAffectedByConstraint(make_pair(segmLeftMostj,segmRightMostj)); */
/*               // Verify that the source phrase length does not exceed */
/*               // the limit. The limit can be exceeded when the source */
/*               // phrase is affected by a translation constraint */
/*           if((segmRightMostj-segmLeftMostj)+1 > this->pbTransModelPars.A && !srcPhraseIsAffectedByConstraint) */
/*             break; */
/*               // Obtain hypothesis data vector */
/*           getHypDataVecForGap(hyp,segmLeftMostj,segmRightMostj,hypDataVec,this->pbTransModelPars.W); */
/*           if(hypDataVec.size()!=0) */
/*           { */
/*             for(unsigned int i=0;i<hypDataVec.size();++i) */
/*             { */
/*                   // Create hypothesis extension */
/*               this->incrScore(hyp,hypDataVec[i],extHyp,scoreComponents); */
/*                   // Obtain information about hypothesis extension */
/*               Vector<std::string> targetWordVec=this->getTransInPlainTextVec(extHyp); */
/*               Vector<pair<PositionIndex,PositionIndex> > aligPos; */
/*               this->aligMatrix(extHyp,aligPos); */
/*                   // Check if translation constraints are satisfied */
/*               if(this->trConstraintsPtr->translationSatisfiesConstraints(targetWordVec,aligPos)) */
/*               { */
/*                 hypVec.push_back(extHyp); */
/*                 scrCompVec.push_back(scoreComponents); */
/*               } */
/*             } */
/* #           ifdef THOT_STATS     */
/*             this->basePbTmStats.transOptions+=hypDataVec.size(); */
/*             ++this->basePbTmStats.getTransCalls; */
/* #           endif     */
/*           } */
/*         } */
/*       } */
/*     } */
/*   }    */
}

//---------------------------------
template<class HYPOTHESIS>
void PbTransModel<HYPOTHESIS>::expand_ref(const Hypothesis& hyp,
                                          Vector<Hypothesis>& hypVec,
                                          Vector<Vector<Score> >& scrCompVec)
{
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
void PbTransModel<HYPOTHESIS>::expand_ver(const Hypothesis& hyp,
                                          Vector<Hypothesis>& hypVec,
                                          Vector<Vector<Score> >& scrCompVec)
{
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
void PbTransModel<HYPOTHESIS>::expand_prefix(const Hypothesis& hyp,
                                             Vector<Hypothesis>& hypVec,
                                             Vector<Vector<Score> >& scrCompVec)
{
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
typename PbTransModel<HYPOTHESIS>::Hypothesis
PbTransModel<HYPOTHESIS>::nullHypothesis(void)
{
  
}

//---------------------------------
template<class HYPOTHESIS>
typename PbTransModel<HYPOTHESIS>::HypDataType
PbTransModel<HYPOTHESIS>::nullHypothesisHypData(void)
{
  
}

//---------------------------------
template<class HYPOTHESIS>
bool PbTransModel<HYPOTHESIS>::obtainPredecessorHypData(HypDataType& hypd)
{
  
}

//---------------------------------
template<class HYPOTHESIS>
bool PbTransModel<HYPOTHESIS>::isCompleteHypData(const HypDataType& hypd)const
{
  
}

//---------------------------------
template<class HYPOTHESIS>
void PbTransModel<HYPOTHESIS>::addHeuristicToHyp(Hypothesis& hyp)
{
//  hyp.addHeuristic(calcHeuristicScore(hyp));
}

//---------------------------------
template<class HYPOTHESIS>
void PbTransModel<HYPOTHESIS>::subtractHeuristicToHyp(Hypothesis& hyp)
{
//  hyp.subtractHeuristic(calcHeuristicScore(hyp));
}

//---------------------------------
template<class HYPOTHESIS>
void PbTransModel<HYPOTHESIS>::setHeuristic(unsigned int _heuristicId)
{
  heuristicId=_heuristicId;
}

//---------------------------------
template<class HYPOTHESIS>
void PbTransModel<HYPOTHESIS>::printHyp(const Hypothesis& hyp,
                                        ostream &outS,
                                        int verbose)
{
/*   Vector<std::string> trgStrVec; */
/*   Vector<WordIndex> trans=hyp.getPartialTrans(); */
/*   SourceSegmentation sourceSegmentation; */
/*   Vector<PositionIndex> targetSegmentCuts; */
/*   Vector<pair<PositionIndex,PositionIndex> > amatrix; */
/*   HypDataType hypDataType; */
/*   Hypothesis auxHyp; */
/*   Vector<Score> scoreComponents; */
  
/*       // Obtain target string vector */
/*   trgStrVec=trgIndexVectorToStrVector(hyp.getPartialTrans()); */

/*       // Print score */
/*   outS <<"Score: "<<hyp.getScore()<<" ; "; */
/*       // Print weights */
/*   this->printWeights(outS); */
/*   outS <<" ; "; */
/*       // Obtain score components */
/*   hypDataType=hyp.getData(); */
/*   this->incrScore(this->nullHypothesis(),hypDataType,auxHyp,scoreComponents); */
/*       // Print score components */
/*   for(unsigned int i=0;i<scoreComponents.size();++i) */
/*     outS<<scoreComponents[i]<<" "; */

/*       // Print score + heuristic */
/*   addHeuristicToHyp(auxHyp); */
/*   outS <<"; Score+heur: "<<auxHyp.getScore()<<" "; */
    
/*       // Print warning if the alignment is not complete */
/*   if(!this->isComplete(hyp)) outS<< "; Incomplete_alignment!"; */

/*       // Obtain phrase alignment */
/*   this->aligMatrix(hyp,amatrix); */
/*   this->getPhraseAlignment(amatrix,sourceSegmentation,targetSegmentCuts); */

/*       // Print alignment information */
/*   outS<<" | "; */
/*   for(unsigned int i=1;i<trgStrVec.size();++i) */
/*     outS<<trgStrVec[i]<<" "; */
/*   outS << "| "; */
/*   for(unsigned int k=0;k<sourceSegmentation.size();k++) */
/*  	outS<<"( "<<sourceSegmentation[k].first<<" , "<<sourceSegmentation[k].second<<" ) ";  */
/*   outS<< "| ";  */
/*   for (unsigned int j=0; j<targetSegmentCuts.size(); j++) */
/*     outS << targetSegmentCuts[j] << " "; */
  
/*       // Print hypothesis key */
/*   outS<<"| hypkey: "<<hyp.getKey()<<" "; */

/*       // Print hypothesis equivalence class */
/*   outS<<"| hypEqClass: "<<hyp.getEqClass()<<endl; */

/*   if(verbose) */
/*   { */
/*     unsigned int numSteps=sourceSegmentation.size()-1; */
/*     outS<<"----------------------------------------------"<<endl; */
/*     outS<<"Score components for previous expansion steps:"<<endl; */
/*     auxHyp=hyp; */
/*     while(this->obtainPredecessor(auxHyp)) */
/*     { */
/*       scoreComponents=scoreCompsForHyp(auxHyp); */
/*       outS<<"Step "<<numSteps<<" : "; */
/*       for(unsigned int i=0;i<scoreComponents.size();++i) */
/*       { */
/*         outS<<scoreComponents[i]<<" "; */
/*       } */
/*       outS<<endl; */
/*       --numSteps; */
/*     } */
/*     outS<<"----------------------------------------------"<<endl; */
/*   } */
/* #ifdef THOT_DEBUG */
/*       // Print debug information */
/*   for(unsigned int i=0;i<hyp.hDebug.size();++i) */
/*   { */
/*     hyp.hDebug[i].print(outS); */
/*   } */
/* #endif  */
}

//---------------------------------
template<class HYPOTHESIS>
Vector<std::string> PbTransModel<HYPOTHESIS>::getTransInPlainTextVec(const PbTransModel::Hypothesis& hyp)const
{
      // TO-BE-DONE
}

//---------------------------------
template<class HYPOTHESIS>
void PbTransModel<HYPOTHESIS>::setWeights(Vector<float> wVec)
{
      // TO-BE-DONE
}

//---------------------------------
template<class HYPOTHESIS>
void PbTransModel<HYPOTHESIS>::getWeights(Vector<pair<std::string,float> >& compWeights)
{
      // TO-BE-DONE
}

//---------------------------------
template<class HYPOTHESIS>
void PbTransModel<HYPOTHESIS>::printWeights(ostream &outS)
{
      // TO-BE-DONE
}

//---------------------------------
template<class HYPOTHESIS>
unsigned int PbTransModel<HYPOTHESIS>::getNumWeights(void)
{
      // TO-BE-DONE
}

//---------------------------------
template<class HYPOTHESIS>
Vector<Score>
PbTransModel<HYPOTHESIS>::scoreCompsForHyp(const PbTransModel::Hypothesis& hyp)
{
      // TO-BE-DONE
}

//---------------------------------
template<class HYPOTHESIS>
void PbTransModel<HYPOTHESIS>::extendHypData(PositionIndex srcLeft,
                                             PositionIndex srcRight,
                                             const Vector<std::string>& trgPhrase,
                                             HypDataType& hypd)
{
      // TO-BE-DONE
}

//---------------------------------
template<class HYPOTHESIS>
int PbTransModel<HYPOTHESIS>::onlineTrainSentPair(const char *srcSent,
                                                  const char *refSent,
                                                  const char *sysSent,
                                                  int verbose/*=0*/)
{
      // TO-BE-DONE  
}

//---------------------------------
template<class HYPOTHESIS>
Score PbTransModel<HYPOTHESIS>::incrScore(const Hypothesis& prev_hyp,
                                          const HypDataType& new_hypd,
                                          Hypothesis& new_hyp,
                                          Vector<Score>& scoreComponents)
{
  
}

//---------------------------------
template<class HYPOTHESIS>
unsigned int PbTransModel<HYPOTHESIS>::numberOfUncoveredSrcWordsHypData(const HypDataType& hypd)const
{
  
}

//---------------------------------
template<class HYPOTHESIS>
PbTransModel<HYPOTHESIS>::~PbTransModel()
{
  
}

//-------------------------

#endif
