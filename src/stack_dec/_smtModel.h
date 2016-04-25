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
/* Module: _smtModel                                                */
/*                                                                  */
/* Prototypes file: _smtModel.h                                     */
/*                                                                  */
/* Description: Declares the _smtModel abstract template            */
/*              class, this class is a predecessor class for        */
/*              implementing different kinds of statistical machine */
/*              translation models.                                 */
/*                                                                  */
/********************************************************************/

/**
 * @file _smtModel.h
 *
 * @brief Declares the _smtModel abstract template class, this class is
 * a predecessor class for implementing different kinds of statistical
 * machine translation models.
 */

#ifndef __smtModel_h
#define __smtModel_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseSmtModel.h"
#include "BaseLogLinWeightUpdater.h"

//--------------- Constants ------------------------------------------

#define NBEST_LIST_SIZE_FOR_LLWEIGHT_UPDATE 1000
#define SMALL_LLWEIGHT 1e-6

//--------------- Classes --------------------------------------------

//--------------- _smtModel template class

/**
 * @brief Base predecessor class that starts the development of the
 * interface defined in the BaseSmtModel abstract class.
 */

template<class HYPOTHESIS>
class _smtModel: public BaseSmtModel<HYPOTHESIS>
{
 public:

  typedef typename BaseSmtModel<HYPOTHESIS>::Hypothesis Hypothesis;
  typedef typename BaseSmtModel<HYPOTHESIS>::HypScoreInfo HypScoreInfo;
  typedef typename BaseSmtModel<HYPOTHESIS>::HypDataType HypDataType;

      // Constructor
  _smtModel(void);

      // Link log-linear weight updater with model
  void link_ll_weight_upd(BaseLogLinWeightUpdater* _llWeightUpdaterPtr);

      // Actions to be executed before the translation and before using
      // hypotheses-related functions
  virtual void pre_trans_actions(std::string srcsent)=0;
  virtual void pre_trans_actions_ref(std::string srcsent,
                                     std::string refsent)=0;
  virtual void pre_trans_actions_ver(std::string srcsent,
                                     std::string refsent)=0;
  virtual void pre_trans_actions_prefix(std::string srcsent,
                                        std::string prefix)=0;
      
  ////// Hypotheses-related functions

        // Expansion-related functions
  virtual void expand(const Hypothesis& hyp,
                      Vector<Hypothesis>& hypVec,
                      Vector<Vector<Score> >& scrCompVec)=0;
  virtual void expand_ref(const Hypothesis& hyp,
                          Vector<Hypothesis>& hypVec,
                          Vector<Vector<Score> >& scrCompVec)=0;
  virtual void expand_ver(const Hypothesis& hyp,
                          Vector<Hypothesis>& hypVec,
                          Vector<Vector<Score> >& scrCompVec)=0;
  virtual void expand_prefix(const Hypothesis& hyp,
                             Vector<Hypothesis>& hypVec,
                             Vector<Vector<Score> >& scrCompVec)=0;

      // Functions for performing on-line training
  void setOnlineTrainingPars(OnlineTrainingPars _onlineTrainingPars,
                             int verbose);
  void updateLogLinearWeights(std::string refSent,
                              WordGraph* wgPtr,
                              int verbose=0);

      // Misc. operations with hypothesis
  virtual void obtainHypFromHypData(const HypDataType& hypDataType,
                                    Hypothesis& hyp);
  virtual bool obtainPredecessor(Hypothesis& hyp);
  virtual unsigned int distToNullHyp(const Hypothesis& hyp)=0;
  virtual void printHyp(const Hypothesis& hyp,
                        ostream &outS,
                        int verbose=false)=0; 
  virtual void diffScoreCompsForHyps(const Hypothesis& pred_hyp,
                                     const Hypothesis& succ_hyp,
                                     Vector<Score>& scoreComponents);
     
      // IMPORTANT NOTE: Before using the hypothesis-related functions
      // it is mandatory the previous invocation of one of the
      // pre_trans_actionsXXXX functions

      // Destructor
  virtual ~_smtModel(){};

# ifdef THOT_STATS
  virtual ostream & printStats(ostream &outS)=0;
  virtual void clearStats(void)=0;
# endif

 protected:

  OnlineTrainingPars onlineTrainingPars;

  BaseLogLinWeightUpdater* llWeightUpdaterPtr;
    
      // Scoring functions
  virtual Score incrScore(const Hypothesis& prev_hyp,
                          const HypDataType& new_hypd,
                          Hypothesis& new_hyp,
                          Vector<Score>& scoreComponents)=0;

      // Helper functions
  float smoothLlWeight(float weight);
};

//--------------- Template method definitions

//--------------- _smtModel template class method definitions

//---------------------------------
template<class HYPOTHESIS>
_smtModel<HYPOTHESIS>::_smtModel(void)
{
}

//---------------------------------
template<class HYPOTHESIS>
void _smtModel<HYPOTHESIS>::link_ll_weight_upd(BaseLogLinWeightUpdater* _llWeightUpdaterPtr)
{
  llWeightUpdaterPtr=_llWeightUpdaterPtr;
}

//---------------------------------
template<class HYPOTHESIS>
void _smtModel<HYPOTHESIS>::setOnlineTrainingPars(OnlineTrainingPars _onlineTrainingPars,
                                                  int /*verbose*/)
{
  onlineTrainingPars=_onlineTrainingPars;
}

//---------------------------------
template<class HYPOTHESIS>
bool _smtModel<HYPOTHESIS>::obtainPredecessor(Hypothesis& hyp)
{
  typename Hypothesis::DataType predData;
  Vector<Score> scoreComponents;

  predData=hyp.getData();
  if(!this->obtainPredecessorHypData(predData)) return false;
  {
    Hypothesis null_hyp=this->nullHypothesis();

    incrScore(null_hyp,predData,hyp,scoreComponents);
    
    return true;
  }
}

//---------------------------------
template<class HYPOTHESIS>
void _smtModel<HYPOTHESIS>::obtainHypFromHypData(const HypDataType& hypDataType,
                                                 Hypothesis& hyp)
{
  Vector<Score> scoreComponents;
  
  incrScore(this->nullHypothesis(),hypDataType,hyp,scoreComponents);
}

//---------------------------------
template<class HYPOTHESIS>
void _smtModel<HYPOTHESIS>::diffScoreCompsForHyps(const Hypothesis& pred_hyp,
                                                  const Hypothesis& succ_hyp,
                                                  Vector<Score>& scoreComponents)
{
  typename Hypothesis::DataType succ_hypd=succ_hyp.getData();
  Hypothesis aux;
  incrScore(pred_hyp,succ_hypd,aux,scoreComponents);
}

//--------------------------
template<class HYPOTHESIS>
void _smtModel<HYPOTHESIS>::updateLogLinearWeights(std::string refSent,
                                                   WordGraph* wgPtr,
                                                   int verbose/*=0*/)
{
      // Obtain n-best list
  unsigned int len=NBEST_LIST_SIZE_FOR_LLWEIGHT_UPDATE;
  Vector<pair<Score,std::string> > nblist;
  Vector<Vector<double> > scoreCompsVec;
  wgPtr->obtainNbestList(len,nblist,scoreCompsVec);

      // Obtain current weights
  Vector<pair<std::string,float> > compWeights;
  this->getWeights(compWeights);
  vector<double> currentWeights;
  for(unsigned int i=0;i<compWeights.size();++i)
    currentWeights.push_back(compWeights[i].second);
  
      // Print verbose information
  if(verbose)
  {
    cerr<<"Training log linear combination weights (n-best list size= "<<nblist.size()<<")..."<<endl;
  }
  
      // Obtain new weights
  vector<double> newWeights;
      // Check if n-best list is empty 
  if(nblist.empty())
    newWeights=currentWeights;
  else
  {    
        // Invoke weight update engine
    std::string reference=refSent;
    vector<string> nblistWithNoScr;
    for(unsigned int i=0;i<nblist.size();++i) nblistWithNoScr.push_back(nblist[i].second);
    llWeightUpdaterPtr->update(reference,
                               nblistWithNoScr,
                               scoreCompsVec,
                               currentWeights,
                               newWeights);
  }
      // Set new weights
  Vector<float> tmwVec;
  for(unsigned int i=0;i<newWeights.size();++i) tmwVec.push_back(newWeights[i]);
  this->setWeights(tmwVec);
  
  if(verbose)
  {
    cerr<<"The weights of the loglinear combination have been trained:"<<endl;
    cerr<<" - Previous weights:";
    for(unsigned int i=0;i<currentWeights.size();++i) cerr<<" "<<currentWeights[i];
    cerr<<endl;
    cerr<<" - New weights     :";
    for(unsigned int i=0;i<newWeights.size();++i) cerr<<" "<<newWeights[i];
    cerr<<endl;
  }
}

//--------------------------
template<class HYPOTHESIS>
float _smtModel<HYPOTHESIS>::smoothLlWeight(float weight)
{
  if(weight<=SMALL_LLWEIGHT && weight>=0)
    return SMALL_LLWEIGHT;
  else
  {
    if(weight>=-SMALL_LLWEIGHT && weight<0)
      return -SMALL_LLWEIGHT;
    else
      return weight;
  }
}

#endif
