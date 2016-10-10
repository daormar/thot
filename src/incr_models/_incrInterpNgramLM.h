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
/* Module: _incrInterpNgramLM                                       */
/*                                                                  */
/* Prototype file: _incrInterpNgramLM.h                             */
/*                                                                  */
/* Description: Base class to manage incremental encoded            */
/*              interpolated ngram language models p(x|Vector<x>).  */
/*                                                                  */
/********************************************************************/

#ifndef __incrInterpNgramLM
#define __incrInterpNgramLM

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseIncrNgramLM.h"
#include <lm_ienc.h>
#include <im_pair.h>
#include <MathFuncs.h>
#include <string>

//--------------- Constants ------------------------------------------

#define DHS_INTERP_LM_FTOL       0.0000001
#define DHS_INTERP_LM_SCALE_PAR  1
#define INVALID_LMODEL_INDEX    -1

//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------

//--------------- _incrInterpNgramLM class

class _incrInterpNgramLM: public BaseIncrNgramLM<Vector<WordIndex> >
{
 public:

  typedef BaseIncrNgramLM<Vector<WordIndex> >::LM_State LM_State;

      // Constructor
  _incrInterpNgramLM();

      // Functions to access model counts
  Count cHist(const Vector<WordIndex>& vu);
  Count cNgram(const WordIndex& w,
               const Vector<WordIndex>& vu);
  Count cHistStr(const Vector<std::string>& rq);
  Count cNgramStr(const std::string& s,
                  const Vector<std::string>& rq);

      // Functions to incrementally extend the model
  void incrCountsOfNgramStr(const std::string& s,
                            const Vector<std::string>& rq,
                            Count c);
  void incrCountsOfNgram(const WordIndex& w,
                         const Vector<WordIndex>& vu,
                         Count c);

      // Probability functions
  LgProb getNgramLgProb(WordIndex w,const Vector<WordIndex>& vu);
      // returns the probability of a n-gram uv[0] stores the n-1'th
      // word of the n-gram, uv[1] the n-2'th one and so on
  LgProb getNgramLgProbStr(string s,const Vector<string>& rq);
      // returns the probability of a n-gram. Each string represents a
      // single word
  LgProb getLgProbEnd(const Vector<WordIndex>& vu);
  LgProb getLgProbEndStr(const Vector<string>& rq);

      // Probability functions using states
  bool getStateForWordSeq(const Vector<WordIndex>& wordSeq,
                          Vector<WordIndex>& state); 
  void getStateForBeginOfSentence(Vector<WordIndex> &state);
  LgProb getNgramLgProbGivenState(WordIndex w,Vector<WordIndex> &state);
  LgProb getNgramLgProbGivenStateStr(std::string s,Vector<WordIndex> &state);
  LgProb getLgProbEndGivenState(Vector<WordIndex> &state);
   
      // encoding-related functions
  bool existSymbol(string s)const;
  WordIndex addSymbol(string s);
  unsigned int getVocabSize(void);
  WordIndex stringToWordIndex(string s)const;
  string wordIndexToString(WordIndex w)const;
  bool loadVocab(const char *prefixFileName);
      // Load encoding information given a prefix file name
  bool printVocab(const char *prefixFileName);
      // Prints encoding information
  void clearVocab(void);
      // Clears encoding information

      // Functions to update model weights
  virtual int updateModelWeights(const char *corpusFileName,
                                 int verbose=0)=0;

      // Functions to load and print the model (including model weights)
  bool load(const char *fileName)=0;
  bool print(const char *fileName)=0;

      // Functions to load and print model weights
  virtual bool loadWeights(const char *fileName)=0;
  virtual bool printWeights(const char *fileName)=0;

      // size and clear functions
  size_t size(void);
  void clear(void);

      // Destructor
  virtual ~_incrInterpNgramLM();
   
 protected:
  
      // typedefs
  typedef std::map<WordIndex,WordIndex> GlobalToLocalDataMap;
  
      // data members
  Vector<double> weights;
  Vector<double> normWeights;
  Vector<BaseNgramLM<Vector<WordIndex> >* > modelPtrVec;
  Vector<GlobalToLocalDataMap> gtlDataMapVec;
  lm_ienc* encPtr;
  int modelIndex;
      // modelIndex determines to which model are applied the functions
      // addTableEntry, infSrcTrg and getEntriesForTarget

      // Auxiliary functions to handle vocabularies
  bool globalStringToWordIndex(const std::string& ht,
                               WordIndex& t)const;
  bool globalStrVecToWidxVec(const Vector<std::string>& rq,
                             Vector<WordIndex>& vu)const;

      // Auxiliary encoding functions
  Vector<WordIndex> mapGlobalToLocalWidxVec(unsigned int index,
                                            const Vector<WordIndex>& global_s);
  WordIndex mapGlobalToLocalWidx(unsigned int index,
                                 const WordIndex& global_t);
  
      // Specific functions for interpolated ngram language models
  void setWeights(const Vector<double>& _weights);
  Vector<double> obtainNormWeights(const Vector<double>& unnormWeights);

      // Release data structures
  void release(void);
};

#endif
