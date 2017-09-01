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
/* Module: WgUncoupledAssistedTrans                                 */
/*                                                                  */
/* Prototypes file: WgUncoupledAssistedTrans.h                      */
/*                                                                  */
/* Description: Declares the WgUncoupledAssistedTrans template      */
/*              class, this class implements uncoupled assisted     */
/*              translators based on word-graphs.                   */
/*                                                                  */
/********************************************************************/

/**
 * @file WgUncoupledAssistedTrans.h
 *
 * @brief Declares the WgUncoupledAssistedTrans template class, this
 * class implements uncoupled assisted translators based on word-graphs.
 */

#ifndef _WgUncoupledAssistedTrans_h
#define _WgUncoupledAssistedTrans_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <StrProcUtils.h>
#include "_assistedTrans.h"
#include "BaseErrorCorrectionModel.h"
#include "_stackDecoderRec.h"
#include "NbestCorrections.h"
#include "BaseWgProcessorForAnlp.h"
#include <WgHandler.h>

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------


//--------------- WgUncoupledAssistedTrans template class

/**
 * @brief The WgUncoupledAssistedTrans template class implements
 * uncoupled assisted translators based on word-graphs.
 */

template<class SMT_MODEL>
class WgUncoupledAssistedTrans: public _assistedTrans<SMT_MODEL>
{
 public:

  typedef typename SMT_MODEL::Hypothesis Hypothesis;

  WgUncoupledAssistedTrans(void);
      // Constructor
  
      // Link statistical translation model with the decoder
  int link_stack_trans(BaseStackDecoder<SMT_MODEL>* _sd_ptr);

      // Link word-graph processor
  void link_wgp(BaseWgProcessorForAnlp* _wgp_ptr);

      // Link word-graph handler
  void link_wgh(WgHandler* _wgh_ptr);

      // Basic services
  std::string translateWithPrefix(std::string s,
                                  std::string pref,
                                  const RejectedWordsSet& rejectedWords=RejectedWordsSet(),
                                  unsigned int verbose=0);
      // Translates std::string s using pref as prefix, uncoupled version
  std::string addStrToPrefix(std::string s,
                             const RejectedWordsSet& rejectedWords=RejectedWordsSet(),
                             unsigned int verbose=0);
      // Adds the string 's' to the user prefix
  void resetPrefix(void);
      // Resets the prefix

  void set_wgp(float _wgp);
      // Sets the word-graph pruning parameter used in uncoupled
      // assisted translation

      // Model weights functions
  void setWeights(std::vector<float> wVec);
  unsigned int getNumWeights(void);
  void printWeights(std::ostream &outS);

      // clear() function
  void clear(void);
      // Remove all data structures used by the assisted translator

      // Destructor
  virtual ~WgUncoupledAssistedTrans();

 protected:

      // CAT-related data members
  _stackDecoderRec<SMT_MODEL>* sdr_ptr;         // Pointer to a stack decoder

  BaseWgProcessorForAnlp* wgp_ptr;  // Pointer to a word-graph
                                    // processor

  WordGraph* wg_ptr;  // Pointer to word-graph. It is required only when
                      // the word-graph is obtained from the word-graph
                      // handler
  
  WgHandler* wgh_ptr; // Pointer to a word-graph handler
  
  float psutw;                   // Weight for the p(s|u,t) model used
                                 // in CAT
  
  float putw;                    // Weight for the p(u|t) model used in
                                 // CAT

      // CAT-related data members
  std::string source;
  std::string catPrefix;

  float wgp; // Word-graph pruning threshold

  unsigned int number_of_results;

      // Auxiliary functions
  WordGraph* obtainWgUsingTranslator(std::string s,
                                     bool& completeHypReachable,
                                     unsigned int verbose=0);
  WordGraph* obtainWgUsingWgHandler(std::string s,
                                    bool& completeHypReachable,
                                    unsigned int verbose=0);

};

//--------------- WgUncoupledAssistedTrans template class method definitions

//---------------------------------------
template<class SMT_MODEL>
WgUncoupledAssistedTrans<SMT_MODEL>::WgUncoupledAssistedTrans(void):_assistedTrans<SMT_MODEL>()
{
  psutw=1;
  putw=1;
  wgp=UNLIMITED_DENSITY;
  number_of_results=1;
  sdr_ptr=NULL;
  wgp_ptr=NULL;
  wgh_ptr=NULL;

      // Create pointer to wordgraph
  wg_ptr=new WordGraph;
}

//---------------------------------
template<class SMT_MODEL>
int WgUncoupledAssistedTrans<SMT_MODEL>::link_stack_trans(BaseStackDecoder<SMT_MODEL>* _sd_ptr)
{
  sdr_ptr=dynamic_cast<_stackDecoderRec<SMT_MODEL>*>(_sd_ptr);
  if(sdr_ptr)
    return THOT_OK;
  else
    return THOT_ERROR;
}

//---------------------------------
template<class SMT_MODEL>
void WgUncoupledAssistedTrans<SMT_MODEL>::link_wgp(BaseWgProcessorForAnlp* _wgp_ptr)
{
  wgp_ptr=_wgp_ptr;
}

//---------------------------------
template<class SMT_MODEL>
void WgUncoupledAssistedTrans<SMT_MODEL>::link_wgh(WgHandler* _wgh_ptr)
{
  wgh_ptr=_wgh_ptr;
}

//---------------------------------
template<class SMT_MODEL>
std::string WgUncoupledAssistedTrans<SMT_MODEL>::translateWithPrefix(std::string s,
                                                                     std::string pref,
                                                                     const RejectedWordsSet& rejectedWords,
                                                                     unsigned int verbose)
{
      // Set catPrefix data member
  catPrefix=pref;

      // Try to obtain word-graph using word-graph handler
  bool completeHypReachable;
  WordGraph* wg_ptr_aux=obtainWgUsingWgHandler(s,completeHypReachable);
  
      // Otherwise, obtain word-graph using translator
  if(wg_ptr_aux==NULL)
    wg_ptr_aux=obtainWgUsingTranslator(s,completeHypReachable);
  
      // Prune word-graph
  unsigned int numPrunedArcs=wg_ptr_aux->prune(wgp);
  if(verbose)
    std::cerr<<numPrunedArcs<<" arcs were pruned"<<std::endl;

      // Remove non-useful states from word-graph
  wg_ptr_aux->obtainWgComposedOfUsefulStates();
  
      // Initialize word-graph processor with word-graph
  wgp_ptr->link_wg(wg_ptr_aux);
  if(verbose)
  {
    std::cerr<<"Linking word-graph with word-graph processor,";
    std::cerr<<" #Nodes: "<<wg_ptr_aux->numStates();
    std::cerr<<" , #Arcs: "<<wg_ptr_aux->numArcs()<<std::endl;
  }
      // Set word-graph processor weights
  wgp_ptr->set_wgw(psutw);
  wgp_ptr->set_ecmw(putw);

  if(completeHypReachable)
  {
    std::string result="";

    NbestCorrections nbestCorrections;
      
        // Obtain n-best corrected translations
    nbestCorrections=wgp_ptr->correct(catPrefix,
                                      number_of_results,
                                      rejectedWords,
                                      verbose);
        // Return the best corrected translation
    std::vector<std::string> strVec;
    if(!nbestCorrections.empty())
      strVec=nbestCorrections.begin()->second;
      
    for(unsigned int i=0;i<strVec.size();++i)
    {
      if(i==0) result=strVec[0];
      else result+=" "+strVec[i];
    }
    return result;
  }
  else
  {
        // No translations were obtained
    if(verbose) std::cerr<<"Unable to translate sentence!"<<std::endl;
    std::string nullStr="";
    return nullStr;
  }
}

//---------------------------------
template<class SMT_MODEL>
WordGraph*
WgUncoupledAssistedTrans<SMT_MODEL>::obtainWgUsingWgHandler(std::string s,
                                                            bool& completeHypReachable,
                                                            unsigned int verbose/*=0*/)
{
  completeHypReachable=false;
  bool found;
  std::vector<std::string> sentStrVec=StrProcUtils::stringToStringVector(s);
  std::string wgPathStr=wgh_ptr->pathAssociatedToSentence(sentStrVec,found);
  if(found)
  {
        // Load word graph
    wg_ptr->clear();
    wg_ptr->load(wgPathStr.c_str());

        // Obtain original word graph component weights
    std::vector<pair<std::string,float> > originalWgCompWeights;
    wg_ptr->getCompWeights(originalWgCompWeights);

        // Print component weight info to the error output
    if(verbose)
    {
      std::cerr<<"Original word graph component vector:";
      for(unsigned int i=0;i<originalWgCompWeights.size();++i)
        std::cerr<<" "<<originalWgCompWeights[i].first<<": "<<originalWgCompWeights[i].second<<";";
      std::cerr<<std::endl;
    }

        // Set current component weights (this operation causes a
        // complete re-scoring of the word graph arcs if there exist
        // score component information for them)
    std::vector<pair<std::string,float> > currCompWeights;
    SMT_MODEL* smtm_ptr=sdr_ptr->get_smt_model_ptr();
    smtm_ptr->getWeights(currCompWeights);
    wg_ptr->setCompWeights(currCompWeights);

        // Print component weight info to the error output
    if(verbose)
    {
      std::cerr<<"New word graph component vector:";
      for(unsigned int i=0;i<currCompWeights.size();++i)
        std::cerr<<" "<<currCompWeights[i].first<<": "<<currCompWeights[i].second<<";";
      std::cerr<<std::endl;
    }

        // Obtain best path
    std::set<WordGraphArcId> emptyExcludedArcsSet;
    std::vector<WordGraphArc> arcVec;
    Score score=wg_ptr->bestPathFromFinalStateToIdx(INITIAL_STATE,emptyExcludedArcsSet,arcVec);

    if(score!=SMALL_SCORE)
      completeHypReachable=true;
    return wg_ptr;
  }
  else
    return NULL;
}

//---------------------------------
template<class SMT_MODEL>
WordGraph*
WgUncoupledAssistedTrans<SMT_MODEL>::obtainWgUsingTranslator(std::string s,
                                                             bool& completeHypReachable,
                                                             unsigned int /*verbose=0*/)
{
      // Get pointer to the statistical machine translation model
  SMT_MODEL* smtm_ptr=sdr_ptr->get_smt_model_ptr();
  
      // Disable best score pruning
  sdr_ptr->useBestScorePruning(false);

      // Enable word-graph
  if(wgp!=DISABLE_WORDGRAPH)
    sdr_ptr->enableWordGraph();

      // Obtain best translation
  Hypothesis hyp=sdr_ptr->translate(s);

      // Set flag
  completeHypReachable=smtm_ptr->isComplete(hyp);
  
  return sdr_ptr->getWordGraphPtr();
}

//---------------------------------
template<class SMT_MODEL>
std::string WgUncoupledAssistedTrans<SMT_MODEL>::addStrToPrefix(std::string s,
                                                                const RejectedWordsSet& rejectedWords,
                                                                unsigned int verbose)
{
      // Set word-graph processor weights
  wgp_ptr->set_wgw(psutw);
  wgp_ptr->set_ecmw(putw);
    
      // Obtain n-best corrected translations
  catPrefix=catPrefix+s;
  NbestCorrections nbestCorrections=wgp_ptr->correct(catPrefix,
                                                     number_of_results,
                                                     rejectedWords,
                                                     verbose);
      // Return the best corrected translation
  std::vector<std::string> strVec;
  if(!nbestCorrections.empty())
    strVec=nbestCorrections.begin()->second;

  std::string result="";
  for(unsigned int i=0;i<strVec.size();++i)
  {
    if(i==0) result=strVec[0];
    else result+=" "+strVec[i];
  }
  return result;
}

//---------------------------------
template<class SMT_MODEL>
void WgUncoupledAssistedTrans<SMT_MODEL>::resetPrefix(void)
{
  catPrefix.clear();
}

//---------------------------------
template<class SMT_MODEL>
void WgUncoupledAssistedTrans<SMT_MODEL>::setWeights(std::vector<float> wVec)
{
  if(wVec.size()>=1) psutw=wVec[0];
  if(wVec.size()>=2) putw=wVec[1];
}

//---------------------------------
template<class SMT_MODEL>
unsigned int WgUncoupledAssistedTrans<SMT_MODEL>::getNumWeights(void)
{
  return 2;
}

//---------------------------------
template<class SMT_MODEL>
void WgUncoupledAssistedTrans<SMT_MODEL>::printWeights(std::ostream &outS)
{
  outS<<"psutw: "<<psutw<<" , ";
  outS<<"putw: "<<putw;
}

//---------------------------------
template<class SMT_MODEL>
void WgUncoupledAssistedTrans<SMT_MODEL>::clear(void)
{
  resetPrefix();
}

//---------------------------------
template<class SMT_MODEL>
WgUncoupledAssistedTrans<SMT_MODEL>::~WgUncoupledAssistedTrans(void)
{
  delete wg_ptr;
}

//--------------- WgUncoupledAssistedTrans template class function definitions

//---------------------------------
template<class SMT_MODEL>
void WgUncoupledAssistedTrans<SMT_MODEL>::set_wgp(float _wgp)
{
  wgp=_wgp;
}

#endif
