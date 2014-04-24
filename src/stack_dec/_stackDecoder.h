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
/* Module: _stackDecoder                                            */
/*                                                                  */
/* Prototypes file: _stackDecoder.h                                 */
/*                                                                  */
/* Description: Declares the _stackDecoder abstract template        */
/*              class, this class is derived from the               */
/*              BaseStackDecoder class and serves as a first step   */
/*              in the implementation of other stack-based decoders.*/
/*                                                                  */
/********************************************************************/

/**
 * @file _stackDecoder.h
 *
 * @brief Declares the _stackDecoder abstract template class, this class
 * is derived from the BaseStackDecoder class and serves as a first step
 * in the implementation of other stack-based decoders.
 */

#ifndef __stackDecoder_h
#define __stackDecoder_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "Bitset.h"
#include "PositionIndex.h"
#include "WordIndex.h"
#include "Prob.h"
#include "StrProcUtils.h"
#include "SmtDefs.h"
#include "BaseStackDecoder.h"
#include "SmtStack.h"
#include "BaseSmtStack.h"
#include "BaseSmtMultiStack.h"
#include "_stack_decoder_statistics.h"
#include "float.h"

//--------------- Constants ------------------------------------------

#define MAX_NUM_OF_ITER        1000000
#define PRINT_GRAPH_STEP       1
#define G_EPSILON              -100000000
#define DEC_IDLE_STATE         1
#define DEC_TRANS_STATE        2
#define DEC_TRANSREF_STATE     3
#define DEC_VER_STATE          4
#define DEC_TRANSPREFIX_STATE  5

//--------------- Classes --------------------------------------------


//--------------- _stackDecoder template class: Implements stack-based decoding

/**
 * @brief The _stackDecoder abstract template class is derived from the
 * BaseStackDecoder class and serves as a first step in the
 * implementation of other stack-based decoders.
 */

template<class SMT_MODEL>
class _stackDecoder: public BaseStackDecoder<SMT_MODEL>
{
 public:

  typedef typename BaseStackDecoder<SMT_MODEL>::Hypothesis Hypothesis;
  
  _stackDecoder(void); 
      // Constructor. 

      // Link statistical translation model with the decoder
  void link_smt_model(SMT_MODEL* _smtm_ptr);

      // Get pointer to the statistical translation model
  SMT_MODEL* get_smt_model_ptr(void);

      // Functions for setting the decoder parameters
  void set_S_par(unsigned int _S);
  void set_I_par(unsigned int _I);
  void set_breadthFirst(bool b);
    
      // Basic services
  Hypothesis translate(std::string s); 
      // Translates the sentence 's' using the model fixed previously
      // with 'setModel'
  Hypothesis getNextTrans(void);
      // Obtains the next hypothesis that the algorithm yields
  Hypothesis translateWithRef(std::string s,
                              std::string ref);   
      // Obtains the best alignment for the source and ref sentence pair
  virtual Hypothesis verifyCoverageForRef(std::string s,
                                          std::string ref);
      // Verifies coverage of the translation model given a source
      // sentence s and the desired output ref. For this purpose, the
      // decoder filters those translations of s that are compatible
      // with ref. The resulting hypothesis won't be complete if the
      // model can't generate the reference
  Hypothesis translateWithSuggestion(std::string s,
                                     typename Hypothesis::DataType sug);
      // Translates string s using hypothesis sug as suggestion instead
      // of using the null hypothesis
  Hypothesis translateWithPrefix(std::string s,
                                 std::string pref);  
      // Translates string s using pref as prefix

      // Set different options 
  void useBestScorePruning(bool b);
  void setWorstScoreAllowed(Score _worstScoreAllowed);

      // Functions to report information about the search
  void printGraphForHyp(const Hypothesis& hyp,ostream &outS);

  void clear(void);
      // Clear temporary data structures

      // Set verbosity level
  void setVerbosity(int _verbosity);

      // Destructor
  virtual ~_stackDecoder();
  
# ifdef THOT_STATS
  void printStats(void);
  _stack_decoder_statistics _stack_decoder_stats;
# endif

 protected:

  SMT_MODEL* smtm_ptr; // Pointer to a statistical machine translation
                       // model
  BaseSmtStack<Hypothesis>* stack_ptr; // Pointer to a stack-based container
  BaseSmtMultiStack<Hypothesis>* baseSmtMultiStackPtr;
      // Pointer to a multiple stack container (it is instantiated from
      // the set_breadthFirst() function and used as an auxiliary
      // variable)

  unsigned int state;            // Stores the state of the decoder
  
  std::string srcSentence;       // srcSentence stores the sentence to be translated
  std::string refSentence;       // refSentence stores the reference sentence
  std::string prefixSentence;    // prefixSentence stores the prefix given by the user
    
  unsigned int S;                // Maximum stack size
  unsigned int I;                // Number of hypotheses to be expanded
                                 // at each iteration
  
  bool useRef;                   // If useRef=true, the expand process
                                 // will be based on a target reference
                                 // sentence

  bool usePrefix;	             // Translate using a prefix (CAT
                                 // paradigm)	

  bool breadthFirst;             // Decides wether to use breadth-first
                                 // search
  
  Score bestCompleteHypScore;
  Hypothesis bestCompleteHyp;
  bool applyBestScorePruning;    // Decides wether to apply pruning
                                 // based on the best score found during
                                 // the translation process
  Score worstScoreAllowed;
  
  int verbosity;                 // Verbosity level
    
  void addgToHyp(Hypothesis& hyp);
  void sustractgToHyp(Hypothesis& hyp);
  
  void pre_trans_actions(std::string srcsent);
  void pre_trans_actions_ref(std::string srcsent,
                             std::string refsent);
  void pre_trans_actions_ver(std::string srcsent,
                             std::string refsent);
  void pre_trans_actions_prefix(std::string srcsent,
                                std::string prefix);

  void suggest(const Hypothesis& sug);
      // Inserts a hypothesis into the stack
  void suggestNullHyp(void);
      // Inserts the null hypothesis into the stack

  bool push(Hypothesis hyp);	
      // push operation: push the 'hyp' hypothesis in the stack with key
      // 'key', if that stack does not exist, it is created The push
      // function also adds a heuristic value to hyp. This function
      // returns true if hyp is inserted into a stack.
      // NOTE: hyp is not passed by reference to avoid collateral effects
  Hypothesis pop(void);	
      // pop operation: pops the top of the stack
      // the pop function also sustracts a heuristic value to hyp
  
  virtual void pushGivenPredHyp(const Hypothesis& pred_hyp,
                                const Vector<Score>& scrComps,
                                const Hypothesis& succ_hyp);
      // Push hypothesis succ_hyp given its predecessor pred_hyp. This
      // function can be overridden by derived classes which use
      // hypotheses-recombination
    
      // Implementation of decoding processes
  Hypothesis decode(void);
  Hypothesis decodeWithRef(void);
  Hypothesis decodeVer(void);
  Hypothesis decodeWithPrefix(void);

      // Puts the decoder in the initial state
  void init_state(void);

      ////////////////////////////////////////////////////////////
      // PURE VIRTUAL FUNCTIONS
      ////////////////////////////////////////////////////////////

  virtual void specific_pre_trans_actions(std::string srcsent)=0;
  virtual void specific_pre_trans_actions_ref(std::string srcsent,
                                              std::string refsent)=0;
  virtual void specific_pre_trans_actions_ver(std::string srcsent,
                                              std::string refsent)=0;
  virtual void specific_pre_trans_actions_prefix(std::string srcsent,
                                                 std::string prefix)=0;

      // Utilities
  Score testHeuristic(std::string sentence,
                      Score optimalTransScore);
};

//--------------- _stackDecoder template class method definitions

template<class SMT_MODEL>
_stackDecoder<SMT_MODEL>::_stackDecoder(void)
{
  state=DEC_IDLE_STATE;
  worstScoreAllowed=-FLT_MAX;
  useBestScorePruning(false);
  breadthFirst=false;
  S=10;
  I=1;
  smtm_ptr=NULL;
  stack_ptr=NULL;
  verbosity=0;
}

//---------------------------------------
template<class SMT_MODEL>
void _stackDecoder<SMT_MODEL>::link_smt_model(SMT_MODEL* _smtm_ptr)
{
  smtm_ptr=_smtm_ptr;  
}

//---------------------------------------
template<class SMT_MODEL>
SMT_MODEL* _stackDecoder<SMT_MODEL>::get_smt_model_ptr(void)
{
  return smtm_ptr;
}

//---------------------------------------
template<class SMT_MODEL>
void _stackDecoder<SMT_MODEL>::set_S_par(unsigned int _S)
{
  S=_S;
  stack_ptr->setMaxStackSize(S);
}

//---------------------------------------
template<class SMT_MODEL>
void _stackDecoder<SMT_MODEL>::set_I_par(unsigned int _I)
{
  I=_I;
}

//---------------------------------------
template<class SMT_MODEL>
void _stackDecoder<SMT_MODEL>::set_breadthFirst(bool b)
{
      // Initialize breadthFirst flag
  breadthFirst=b;

      // Initialize pointer to multiple stack if the dynamic cast is
      // succesfull (this pointer is used in the pop() function)
  baseSmtMultiStackPtr=dynamic_cast<BaseSmtMultiStack<Hypothesis>*>(stack_ptr);
}

//---------------------------------------
template<class SMT_MODEL>
void _stackDecoder<SMT_MODEL>::addgToHyp(Hypothesis& hyp)
{
  unsigned int i;
  double g;
    
  i=smtm_ptr->distToNullHyp(hyp);
  g=(double)i*(double)G_EPSILON;
  hyp.addHeuristic(g);
}

//---------------------------------------
template<class SMT_MODEL>
void _stackDecoder<SMT_MODEL>::sustractgToHyp(Hypothesis& hyp)
{
  unsigned int i;
  double g;
    
  i=smtm_ptr->distToNullHyp(hyp);
  g=(double)i*(double)G_EPSILON;
  hyp.sustractHeuristic(g);
}

//---------------------------------------
template<class SMT_MODEL>
typename _stackDecoder<SMT_MODEL>::Hypothesis
_stackDecoder<SMT_MODEL>::translate(std::string s)
{
  return translateWithSuggestion(s,
                                 smtm_ptr->nullHypothesisHypData());
}

//---------------------------------------
template<class SMT_MODEL>
typename _stackDecoder<SMT_MODEL>::Hypothesis
_stackDecoder<SMT_MODEL>::getNextTrans(void)  
{
  if(smtm_ptr==NULL)
  {
    Hypothesis emptyHyp;
    cerr<<"Error! model not initialized\n";
    return emptyHyp;
  }
  else
  {
#ifdef THOT_STATS
    _stack_decoder_stats.clear();
    smtm_ptr->clearStats();
    ++_stack_decoder_stats.sentencesTranslated;
#endif
        // reset bestCompleteHypScore
    this->bestCompleteHypScore=worstScoreAllowed;
        // reset bestCompleteHyp
    bestCompleteHyp=smtm_ptr->nullHypothesis();

        // get next translation depending on the state of the decoder
    switch(state)
    {
      case DEC_TRANS_STATE: return decode();
      case DEC_TRANSREF_STATE: return decodeWithRef();
      case DEC_VER_STATE: return decodeVer();
      case DEC_TRANSPREFIX_STATE: return decodeWithPrefix();
      default: Hypothesis emptyHyp;
        return emptyHyp;
    }
  }
}

//---------------------------------------
template<class SMT_MODEL>
typename _stackDecoder<SMT_MODEL>::Hypothesis
_stackDecoder<SMT_MODEL>::translateWithRef(std::string s,
                                           std::string ref) 
{
  if(smtm_ptr==NULL)
  {
    Hypothesis emptyHyp;
    cerr<<"Error! model not initialized\n";
    return emptyHyp;
  }
  else
  {
#ifdef THOT_STATS
    _stack_decoder_stats.clear();
    smtm_ptr->clearStats();
#endif	

        // verify sentence length
    if(StrProcUtils::stringToStringVector(s).size()>=MAX_SENTENCE_LENGTH_ALLOWED)
    {
      cerr<<"Error: the sentence to translate is too long (MAX= "<<MAX_SENTENCE_LENGTH_ALLOWED<<" words)\n";
      init_state();
      Hypothesis nullHyp;
      nullHyp=smtm_ptr->nullHypothesis();
      return nullHyp;
    }

        // Execute actions previous to the translation process
    pre_trans_actions_ref(s,ref);
  	    
        // Insert Null hypothesis
    clear();
    suggestNullHyp(); 
  
    return decodeWithRef();
  }
}

//---------------------------------------
template<class SMT_MODEL>
typename _stackDecoder<SMT_MODEL>::Hypothesis
_stackDecoder<SMT_MODEL>::verifyCoverageForRef(std::string s,
                                               std::string ref)
{
  if(smtm_ptr==NULL)
  {
    Hypothesis emptyHyp;
    cerr<<"Error! model not initialized\n";
    return emptyHyp;
  }
  else
  {
#ifdef THOT_STATS
    _stack_decoder_stats.clear();
    smtm_ptr->clearStats();
#endif	

        // verify sentence length
    if(StrProcUtils::stringToStringVector(s).size()>=MAX_SENTENCE_LENGTH_ALLOWED)
    {
      cerr<<"Error: the sentence to translate is too long (MAX= "<<MAX_SENTENCE_LENGTH_ALLOWED<<" words)\n";
      init_state();
      Hypothesis nullHyp;
      nullHyp=smtm_ptr->nullHypothesis();
      return nullHyp;
    }

        // Execute actions previous to the translation process
    pre_trans_actions_ver(s,ref);
  	    
        // Insert Null hypothesis
    clear();
    suggestNullHyp(); 
  
    return decodeVer();
  }
}

//---------------------------------------
template<class SMT_MODEL>
typename _stackDecoder<SMT_MODEL>::Hypothesis
_stackDecoder<SMT_MODEL>::translateWithSuggestion(std::string s,
                                                  typename Hypothesis::DataType sug)
{
  if(smtm_ptr==NULL)
  {
    Hypothesis emptyHyp;
    cerr<<"Error! model not initialized\n";
    return emptyHyp;
  }
  else
  {
#ifdef THOT_STATS
    _stack_decoder_stats.clear();
    smtm_ptr->clearStats();
    ++_stack_decoder_stats.sentencesTranslated;
#endif

        // verify sentence length
    if(StrProcUtils::stringToStringVector(s).size()>=MAX_SENTENCE_LENGTH_ALLOWED)
    {
      cerr<<"Error: the sentence to translate is too long (MAX= "<<MAX_SENTENCE_LENGTH_ALLOWED<<" words)\n";
      init_state();
      Hypothesis nullHyp;
      nullHyp=smtm_ptr->nullHypothesis();
      return nullHyp;
    }

    Hypothesis initialHyp;
  
        // Execute actions previous to the translation process
    pre_trans_actions(s);

        // Obtain initialHyp
    smtm_ptr->obtainHypFromHypData(sug,initialHyp);
      
        // Insert null hypothesis
    clear();
    suggest(initialHyp);
        // Initializes the multi-stack decoder algorithm with a stack
        // containing the initial hypothesis "initialHyp"
  
        // Translate sentence
    return decode();
  }
}
//---------------------------------------
template<class SMT_MODEL>
typename _stackDecoder<SMT_MODEL>::Hypothesis
_stackDecoder<SMT_MODEL>::translateWithPrefix(std::string s,
                                              std::string pref)
{
  if(smtm_ptr==NULL)
  {
    Hypothesis emptyHyp;
    cerr<<"Error! model not initialized\n";
    return emptyHyp;
  }
  else
  {
#ifdef THOT_STATS
    _stack_decoder_stats.clear();
    smtm_ptr->clearStats();
#endif

        // verify sentence length
    if(StrProcUtils::stringToStringVector(s).size()>=MAX_SENTENCE_LENGTH_ALLOWED)
    {
      cerr<<"Error: the sentence to translate is too long (MAX= "<<MAX_SENTENCE_LENGTH_ALLOWED<<" words)\n";
      init_state();
      Hypothesis nullHyp;
      nullHyp=smtm_ptr->nullHypothesis();
      return nullHyp;
    }

        // Execute actions previous to the translation process
    pre_trans_actions_prefix(s,pref);

        // Insert Null hypothesis
    clear();
    suggestNullHyp(); 

    return decodeWithPrefix();
  }
}

//---------------------------------------
template<class SMT_MODEL>
void _stackDecoder<SMT_MODEL>::useBestScorePruning(bool b)
{
  applyBestScorePruning=b;
}

//---------------------------------------
template<class SMT_MODEL>
void _stackDecoder<SMT_MODEL>::setWorstScoreAllowed(Score _worstScoreAllowed)
{
  worstScoreAllowed=_worstScoreAllowed;
}

//---------------------------------------
template<class SMT_MODEL>
void _stackDecoder<SMT_MODEL>::printGraphForHyp(const Hypothesis& hyp,ostream &outS)
{
  Hypothesis aux_hyp;
  aux_hyp=hyp;
  
  if(!smtm_ptr->obtainPredecessor(aux_hyp))
  {
        // print null hypothesis
    smtm_ptr->printHyp(hyp,outS);
    outS<<endl;
  }
  else
  {
    Hypothesis pred;

    pred=hyp;
    aux_hyp=pred;
    while(smtm_ptr->obtainPredecessor(pred))
    {
      smtm_ptr->printHyp(pred,outS);
      smtm_ptr->printHyp(aux_hyp,outS);
      outS<<endl;
      aux_hyp=pred;
    }
  }
}

//---------------------------------------
template<class SMT_MODEL>
void _stackDecoder<SMT_MODEL>::clear(void)
{
  stack_ptr->clear();
}

//--------------- _stackDecoder template class functions
//

//---------------------------------------
template<class SMT_MODEL>
void _stackDecoder<SMT_MODEL>::init_state(void)
{
  clear();
  state=DEC_IDLE_STATE;
  srcSentence.clear();
  refSentence.clear();
  prefixSentence.clear(); 
}

//---------------------------------------
template<class SMT_MODEL>
Score _stackDecoder<SMT_MODEL>::testHeuristic(std::string sentence,
                                              Score optimalTransScore)
{
  if(smtm_ptr==NULL)
  {
    cerr<<"Error! model not initialized\n";
    return 0;
  }
  else
  {
    Vector<WordIndex> emptyVector; 
    Hypothesis nullHyp;
    Score difference;

    nullHyp=smtm_ptr->nullHypothesis();
    smtm_ptr->pre_trans_actions(sentence);

    smtm_ptr->addHeuristicToHyp(nullHyp);
	
    difference=optimalTransScore - nullHyp.getScore(); 
#ifdef THOT_STATS
    _stack_decoder_stats.nullHypHeuristicValue+= nullHyp.getScore();
    _stack_decoder_stats.scoreOfOptimalHyp+= optimalTransScore;
#endif
    return difference;
  }
}

//-------------------------
template<class SMT_MODEL>
void _stackDecoder<SMT_MODEL>::pre_trans_actions(std::string srcsent)
{
  state=DEC_TRANS_STATE;
  srcSentence=srcsent;
  smtm_ptr->pre_trans_actions(srcsent);
  specific_pre_trans_actions(srcsent);
  bestCompleteHypScore=worstScoreAllowed;
  bestCompleteHyp=smtm_ptr->nullHypothesis();
}

//-------------------------
template<class SMT_MODEL>
void _stackDecoder<SMT_MODEL>::pre_trans_actions_ref(std::string srcsent,
                                                     std::string refsent)
{
  state=DEC_TRANSREF_STATE;
  srcSentence=srcsent;
  refSentence=refsent;
  smtm_ptr->pre_trans_actions_ref(srcsent,refsent);
  specific_pre_trans_actions_ref(srcsent,refsent);

  bestCompleteHypScore=worstScoreAllowed;
  bestCompleteHyp=smtm_ptr->nullHypothesis();
}

//-------------------------
template<class SMT_MODEL>
void _stackDecoder<SMT_MODEL>::pre_trans_actions_ver(std::string srcsent,
                                                     std::string refsent)
{
  state=DEC_VER_STATE;
  srcSentence=srcsent;
  refSentence=refsent;
  smtm_ptr->pre_trans_actions_ver(srcsent,refsent);
  specific_pre_trans_actions_ver(srcsent,refsent);

  bestCompleteHypScore=worstScoreAllowed;
  bestCompleteHyp=smtm_ptr->nullHypothesis();
}

//-------------------------
template<class SMT_MODEL>
void _stackDecoder<SMT_MODEL>::pre_trans_actions_prefix(std::string srcsent,
                                                        std::string prefix)
{
  state=DEC_TRANSPREFIX_STATE;
  srcSentence=srcsent;
  prefixSentence=prefix;
  smtm_ptr->pre_trans_actions_prefix(srcsent,prefix);
  specific_pre_trans_actions_prefix(srcsent,prefix);

  bestCompleteHypScore=worstScoreAllowed;
  bestCompleteHyp=smtm_ptr->nullHypothesis();
}

//---------------------------------------
template<class SMT_MODEL>
void _stackDecoder<SMT_MODEL>::suggest(const Hypothesis& sug)
{
  push(sug);	
}

//---------------------------------------
template<class SMT_MODEL>
void _stackDecoder<SMT_MODEL>::suggestNullHyp(void)
{
  Hypothesis nullHyp;

  nullHyp=smtm_ptr->nullHypothesis();
  push(nullHyp);
}

//---------------------------------------
template<class SMT_MODEL>
bool _stackDecoder<SMT_MODEL>::push(Hypothesis hyp)
{
      // Only insert hyp if its score is not equal to -inf. This may
      // ocurr when the translation model assigns zero probability to an
      // unseen event
  bool inserted=false;
  if((double)hyp.getScore()>=-FLT_MAX)
  {
        // Add heuristic and g values
    smtm_ptr->addHeuristicToHyp(hyp);
    if(breadthFirst) addgToHyp(hyp);
      
        // Check whether best score pruning is applied
    if(!applyBestScorePruning)
    {
      inserted=stack_ptr->push(hyp);
      if(inserted && smtm_ptr->isComplete(hyp))
      {
        this->bestCompleteHypScore=hyp.getScore();
        this->bestCompleteHyp=hyp;
      }
    }
    else
    {
          // apply best score pruning
          // The following check is done to perform best score
          // pruning
      if((double)hyp.getScore()>=(double)bestCompleteHypScore) 
      {
        inserted=stack_ptr->push(hyp);
        if(inserted && smtm_ptr->isComplete(hyp))
        {
          this->bestCompleteHypScore=hyp.getScore();
          this->bestCompleteHyp=hyp;
        }
      }
      else
      {
        inserted=false;
#      ifdef THOT_STATS
        ++this->_stack_decoder_stats.pushAborted;
#      endif
      }
    }
#  ifdef THOT_STATS
    ++this->_stack_decoder_stats.totalPushNo;
    ++this->_stack_decoder_stats.pushPerIter;  
#  endif
  }
  return inserted;
}

//---------------------------------------
template<class SMT_MODEL>
typename _stackDecoder<SMT_MODEL>::Hypothesis _stackDecoder<SMT_MODEL>::pop(void)
{
  if(breadthFirst)
  {
        // Breadth-first search
    if(baseSmtMultiStackPtr)
          // Set breadth-first flag of the multiple stack container to
          // true
      baseSmtMultiStackPtr->set_bf(true);

    Hypothesis hyp;
    if(smtm_ptr->isComplete(stack_ptr->top()))
    {
          // If the hypothesis is complete, the breadth-first flag of
          // the stack requires special treatment. Specifically it must
          // be set to false before calling the pop() function in order
          // to get the best complete hypothesis instead of the
          // hypothesis stored in the first container according to the
          // ordering function
      if(baseSmtMultiStackPtr)
        baseSmtMultiStackPtr->set_bf(false);
      hyp=stack_ptr->pop();
      if(baseSmtMultiStackPtr)
        baseSmtMultiStackPtr->set_bf(true);
    }
    else
    {
      hyp=stack_ptr->pop();
    }
    sustractgToHyp(hyp);
    smtm_ptr->sustractHeuristicToHyp(hyp);
    return hyp;
  }
  else
  {
        // Non breadth-first search
    Hypothesis hyp;
    hyp=stack_ptr->pop();
    smtm_ptr->sustractHeuristicToHyp(hyp);
    return hyp;
  }
}

//---------------------------------------
template<class SMT_MODEL>
void _stackDecoder<SMT_MODEL>::pushGivenPredHyp(const Hypothesis& /*pred_hyp*/,
                                                const Vector<Score>& /*scrComps*/,
                                                const Hypothesis& succ_hyp)
{
  push(succ_hyp);
}

//---------------------------------------
template<class SMT_MODEL>
typename _stackDecoder<SMT_MODEL>::Hypothesis _stackDecoder<SMT_MODEL>::decode(void)
{
  bool end=false;
  Hypothesis result;
  Vector<Hypothesis> hypsToExpand;
  unsigned int iterNo=1;
    
  while(!end && iterNo<MAX_NUM_OF_ITER)
  {
#ifdef THOT_DEBUG
#ifdef THOT_ENABLE_GRAPH
    if((iterNo%PRINT_GRAPH_STEP)==0)
    {
      char printGraphFileName[128];
      sprintf(printGraphFileName,"%d.graph_file",iterNo);
      this->printSearchGraph(printGraphFileName);
    }
#endif      
#endif
        // Select hypothesis to be expanded
    hypsToExpand.clear();
    while(!stack_ptr->empty() && hypsToExpand.size()<I)
    {
      hypsToExpand.push_back(pop());
    }

    if(verbosity>1)
    {
      cerr<<endl;
      cerr<<"* IterNo: "<<iterNo<<endl;
      cerr<<"  Number of queues/hypotheses: "<<stack_ptr->size()<<endl;
      cerr<<"  hypsToExpand: "<<hypsToExpand.size()<<endl;
    }

#ifdef THOT_STATS
    this->_stack_decoder_stats.pushPerIter=0;
    ++this->_stack_decoder_stats.numIter;	  
#endif
        // Finish if there is not any hypothesis to be expanded
    if(hypsToExpand.empty()) end=true;
    else	   
    {
          // Expand each hypothesis
      for(unsigned int i=0;i<hypsToExpand.size();++i)
      {
            // If the hypothesis is complete, finish the decoding
            // process, but expand the remaining hypotheses (required by
            // getNextTrans)
        if(smtm_ptr->isComplete(hypsToExpand[i]))
        {
          if(!end)
          {
                // Return the first complete hypothesis as the final
                // translation
            bestCompleteHyp=hypsToExpand[i];
            end=true;
          }
          else push(hypsToExpand[i]);
        }
        else
        {
          Vector<Hypothesis> expandedHyps;
          Vector<Vector<Score> > scrCompVec;
          int numExpHyp=0;
          
              // If the hypothesis is not complete, expand it
#        ifdef THOT_STATS
          ++this->_stack_decoder_stats.totalExpansionNo;
#        endif  

          if(verbosity>1)
          {
            cerr<<"  Expanding hypothesis: ";
            smtm_ptr->printHyp(hypsToExpand[i],cerr);
          }
          smtm_ptr->expand(hypsToExpand[i],expandedHyps,scrCompVec);
          if(verbosity>1)
            cerr<<"  Generated "<<expandedHyps.size()<<" expansions"<<endl;

          while(!expandedHyps.empty())
          {
            if(verbosity>2)
            {
              ++numExpHyp;
              cerr<<"  Expanded hypothesis "<<numExpHyp<<" : ";
              smtm_ptr->printHyp(expandedHyps.back(),cerr);
            }
                // Push expanded hyp into the stack container
            pushGivenPredHyp(hypsToExpand[i],scrCompVec.back(),expandedHyps.back());
            scrCompVec.pop_back();
            expandedHyps.pop_back();
          }
        }
      }
    }
    ++iterNo;
  }
  	  
  if(iterNo>=MAX_NUM_OF_ITER) cerr<<"Maximum number of iterations exceeded!\n";
  return bestCompleteHyp; 
}

//---------------------------------------
template<class SMT_MODEL>
typename _stackDecoder<SMT_MODEL>::Hypothesis _stackDecoder<SMT_MODEL>::decodeWithRef(void)
{
  bool end=false;
  Hypothesis result;
  Vector<Hypothesis> hypsToExpand;
  unsigned int iterNo=1;

  while(!end && iterNo<MAX_NUM_OF_ITER)
  {
#ifdef THOT_DEBUG
#ifdef THOT_ENABLE_GRAPH
    if((iterNo%PRINT_GRAPH_STEP)==0)
    {
      char printGraphFileName[128];
      sprintf(printGraphFileName,"%d.graph_file",iterNo);
      this->printSearchGraph(printGraphFileName);
    }
#endif      
#endif
        // Select hypothesis to be expanded
    hypsToExpand.clear();
    while(!stack_ptr->empty() && hypsToExpand.size()<I)
    {
      hypsToExpand.push_back(pop());
    }
    
    if(verbosity>1)
    {
      cerr<<endl;
      cerr<<"* IterNo: "<<iterNo<<endl;
      cerr<<"  Number of queues/hypotheses: "<<stack_ptr->size()<<endl;
      cerr<<"  hypsToExpand: "<<hypsToExpand.size()<<endl;
    }

#ifdef THOT_STATS
    this->_stack_decoder_stats.pushPerIter=0;
    ++this->_stack_decoder_stats.numIter;	  
#endif
        // Finish if there is not any hypothesis to be expanded
    if(hypsToExpand.empty()) end=true;
    else	   
    {
          // Expand each hypothesis
      for(unsigned int i=0;i<hypsToExpand.size();++i)
      {
            // If the hypothesis is complete, finish the decoding
            // process, but expand the remaining hypotheses (required by
            // getNextTrans)
        if(smtm_ptr->isComplete(hypsToExpand[i]))
        {
          if(!end)
          {
                // Return the first complete hypothesis as the final
                // translation
            bestCompleteHyp=hypsToExpand[i];
            end=true;
          }
          else push(hypsToExpand[i]);
        }
        else
        {
          Vector<Hypothesis> expandedHyps;
          Vector<Vector<Score> > scrCompVec;
          int numExpHyp=0;
          
              // If the hypothesis is not complete, expand it
#        ifdef THOT_STATS
          ++this->_stack_decoder_stats.totalExpansionNo;
#        endif  

          if(verbosity>1)
          {
            cerr<<"  Expanding hypothesis: ";
            smtm_ptr->printHyp(hypsToExpand[i],cerr);
          }
          smtm_ptr->expand_ref(hypsToExpand[i],expandedHyps,scrCompVec);
          if(verbosity>1)
            cerr<<"  Generated "<<expandedHyps.size()<<" expansions"<<endl;

          while(!expandedHyps.empty())
          {
            if(verbosity>2)
            {
              ++numExpHyp;
              cerr<<"  Expanded hypothesis "<<numExpHyp<<" : ";
              smtm_ptr->printHyp(expandedHyps.back(),cerr);
            }
                // Push expanded hyp into the stack container
            pushGivenPredHyp(hypsToExpand[i],scrCompVec.back(),expandedHyps.back());
            scrCompVec.pop_back();
            expandedHyps.pop_back();
          }
        }
      }
    }
    ++iterNo;
  }
  	  
  if(iterNo>=MAX_NUM_OF_ITER) cerr<<"Maximum number of iterations exceeded!\n";
  return bestCompleteHyp;	 
}

//---------------------------------------
template<class SMT_MODEL>
typename _stackDecoder<SMT_MODEL>::Hypothesis _stackDecoder<SMT_MODEL>::decodeVer(void)
{
  bool end=false;
  Hypothesis result;
  Vector<Hypothesis> hypsToExpand;
  unsigned int iterNo=1;

  while(!end && iterNo<MAX_NUM_OF_ITER)
  {
#ifdef THOT_DEBUG
#ifdef THOT_ENABLE_GRAPH
    if((iterNo%PRINT_GRAPH_STEP)==0)
    {
      char printGraphFileName[128];
      sprintf(printGraphFileName,"%d.graph_file",iterNo);
      this->printSearchGraph(printGraphFileName);
    }
#endif      
#endif
        // Select hypothesis to be expanded
    hypsToExpand.clear();
    while(!stack_ptr->empty() && hypsToExpand.size()<I)
    {
      hypsToExpand.push_back(pop());
    }
    
    if(verbosity>1)
    {
      cerr<<endl;
      cerr<<"* IterNo: "<<iterNo<<endl;
      cerr<<"  Number of queues/hypotheses: "<<stack_ptr->size()<<endl;
      cerr<<"  hypsToExpand: "<<hypsToExpand.size()<<endl;
    }

#ifdef THOT_STATS
    this->_stack_decoder_stats.pushPerIter=0;
    ++this->_stack_decoder_stats.numIter;	  
#endif
        // Finish if there is not any hypothesis to be expanded
    if(hypsToExpand.empty()) end=true;
    else	   
    {
          // Expand each hypothesis
      for(unsigned int i=0;i<hypsToExpand.size();++i)
      {
            // If the hypothesis is complete, finish the decoding
            // process, but expand the remaining hypotheses (required by
            // getNextTrans)
        if(smtm_ptr->isComplete(hypsToExpand[i]))
        {
          if(!end)
          {
                // Return the first complete hypothesis as the final
                // translation
            bestCompleteHyp=hypsToExpand[i];
            end=true;
          }
          else push(hypsToExpand[i]);
        }
        else
        {
          Vector<Hypothesis> expandedHyps;
          Vector<Vector<Score> > scrCompVec;
          int numExpHyp=0;
          
              // If the hypothesis is not complete, expand it
#        ifdef THOT_STATS
          ++this->_stack_decoder_stats.totalExpansionNo;
#        endif  

          if(verbosity>1)
          {
            cerr<<"  Expanding hypothesis: ";
            smtm_ptr->printHyp(hypsToExpand[i],cerr);
          }
          smtm_ptr->expand_ver(hypsToExpand[i],expandedHyps,scrCompVec);
          if(verbosity>1)
            cerr<<"  Generated "<<expandedHyps.size()<<" expansions"<<endl;

          while(!expandedHyps.empty())
          {
            if(verbosity>2)
            {
              ++numExpHyp;
              cerr<<"  Expanded hypothesis "<<numExpHyp<<" : ";
              smtm_ptr->printHyp(expandedHyps.back(),cerr);
            }
                // Push expanded hyp into the stack container
            pushGivenPredHyp(hypsToExpand[i],scrCompVec.back(),expandedHyps.back());
            scrCompVec.pop_back();
            expandedHyps.pop_back();
          }
        }
      }
    }
    ++iterNo;
  }
  	  
  if(iterNo>=MAX_NUM_OF_ITER) cerr<<"Maximum number of iterations exceeded!\n";
  return bestCompleteHyp;	 
}

//---------------------------------------
template<class SMT_MODEL>
typename _stackDecoder<SMT_MODEL>::Hypothesis _stackDecoder<SMT_MODEL>::decodeWithPrefix(void)
{
  bool end=false;
  Hypothesis result;
  Vector<Hypothesis> hypsToExpand;
  unsigned int iterNo=1;

  while(!end && iterNo<MAX_NUM_OF_ITER)
  {
#ifdef THOT_DEBUG
#ifdef THOT_ENABLE_GRAPH
    if((iterNo%PRINT_GRAPH_STEP)==0)
    {
      char printGraphFileName[128];
      sprintf(printGraphFileName,"%d.graph_file",iterNo);
      this->printSearchGraph(printGraphFileName);
    }
#endif      
#endif
        // Select hypothesis to be expanded
    hypsToExpand.clear();
    while(!stack_ptr->empty() && hypsToExpand.size()<I)
    {
      hypsToExpand.push_back(pop());
    }
    
    if(verbosity>1)
    {
      cerr<<endl;
      cerr<<"* IterNo: "<<iterNo<<endl;
      cerr<<"  Number of queues/hypotheses: "<<stack_ptr->size()<<endl;
      cerr<<"  hypsToExpand: "<<hypsToExpand.size()<<endl;
    }

#ifdef THOT_STATS
    this->_stack_decoder_stats.pushPerIter=0;
    ++this->_stack_decoder_stats.numIter;	  
#endif
        // Finish if there is not any hypothesis to be expanded
    if(hypsToExpand.empty()) end=true;
    else	   
    {
          // Expand each hypothesis
      for(unsigned int i=0;i<hypsToExpand.size();++i)
      {
            // If the hypothesis is complete, finish the decoding
            // process, but expand the remaining hypotheses (required by
            // getNextTrans)
        if(smtm_ptr->isComplete(hypsToExpand[i]))
        {
          if(!end)
          {
                // Return the first complete hypothesis as the final
                // translation
            bestCompleteHyp=hypsToExpand[i];
            end=true;
          }
          else push(hypsToExpand[i]);
        }
        else
        {
          Vector<Hypothesis> expandedHyps;
          Vector<Vector<Score> > scrCompVec;
          int numExpHyp=0;
          
              // If the hypothesis is not complete, expand it
#        ifdef THOT_STATS
          ++this->_stack_decoder_stats.totalExpansionNo;
#        endif  

          if(verbosity>1)
          {
            cerr<<"  Expanding hypothesis: ";
            smtm_ptr->printHyp(hypsToExpand[i],cerr);
          }
          smtm_ptr->expand_prefix(hypsToExpand[i],expandedHyps,scrCompVec);
          if(verbosity>1)
            cerr<<"  Generated "<<expandedHyps.size()<<" expansions"<<endl;

          while(!expandedHyps.empty())
          {
            if(verbosity>2)
            {
              ++numExpHyp;
              cerr<<"  Expanded hypothesis "<<numExpHyp<<" : ";
              smtm_ptr->printHyp(expandedHyps.back(),cerr);
            }
                // Push expanded hyp into the stack container
            pushGivenPredHyp(hypsToExpand[i],scrCompVec.back(),expandedHyps.back());
            scrCompVec.pop_back();
            expandedHyps.pop_back();
          }
        }
      }
    }
    ++iterNo;
  }
  	  
  if(iterNo>=MAX_NUM_OF_ITER) cerr<<"Maximum number of iterations exceeded!\n";
  return bestCompleteHyp;	 
}

//---------------------------------------
template<class SMT_MODEL>
void _stackDecoder<SMT_MODEL>::setVerbosity(int _verbosity)
{
  verbosity=_verbosity;
}

//---------------------------------------
template<class SMT_MODEL>
_stackDecoder<SMT_MODEL>::~_stackDecoder()
{
  
} 
//---------------

#ifdef THOT_STATS
template<class SMT_MODEL>
void _stackDecoder<SMT_MODEL>::printStats(void)
{
  _stack_decoder_stats.print(cerr);
  cerr<<" * Push op's aborted due to S     : "<<stack_ptr->discardedPushOpsDueToSize<<endl;
  cerr<<" * Push op's aborted due to rec.  : "<<stack_ptr->discardedPushOpsDueToRec<<endl;
  smtm_ptr->printStats(cerr);
}
#endif

#endif
