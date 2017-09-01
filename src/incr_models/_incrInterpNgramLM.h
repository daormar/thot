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
/*              interpolated ngram language models                  */
/*              p(x|std::vector<x>).                                */
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

class _incrInterpNgramLM: public BaseIncrNgramLM<std::vector<WordIndex> >
{
 public:

  typedef BaseIncrNgramLM<std::vector<WordIndex> >::LM_State LM_State;

      // Constructor
  _incrInterpNgramLM();

      // Functions to access model counts
  Count cHist(const std::vector<WordIndex>& vu);
  Count cNgram(const WordIndex& w,
               const std::vector<WordIndex>& vu);
  Count cHistStr(const std::vector<std::string>& rq);
  Count cNgramStr(const std::string& s,
                  const std::vector<std::string>& rq);

      // Functions to incrementally extend the model
  void incrCountsOfNgramStr(const std::string& s,
                            const std::vector<std::string>& rq,
                            Count c);
  void incrCountsOfNgram(const WordIndex& w,
                         const std::vector<WordIndex>& vu,
                         Count c);

      // Probability functions
  LgProb getNgramLgProb(WordIndex w,const std::vector<WordIndex>& vu);
      // returns the probability of a n-gram uv[0] stores the n-1'th
      // word of the n-gram, uv[1] the n-2'th one and so on
  LgProb getNgramLgProbStr(string s,const std::vector<string>& rq);
      // returns the probability of a n-gram. Each string represents a
      // single word
  LgProb getLgProbEnd(const std::vector<WordIndex>& vu);
  LgProb getLgProbEndStr(const std::vector<string>& rq);

      // Probability functions using states
  bool getStateForWordSeq(const std::vector<WordIndex>& wordSeq,
                          std::vector<WordIndex>& state); 
  void getStateForBeginOfSentence(std::vector<WordIndex> &state);
  LgProb getNgramLgProbGivenState(WordIndex w,std::vector<WordIndex> &state);
  LgProb getNgramLgProbGivenStateStr(std::string s,std::vector<WordIndex> &state);
  LgProb getLgProbEndGivenState(std::vector<WordIndex> &state);
   
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

      // size function
  size_t size(void);

      // Destructor
  virtual ~_incrInterpNgramLM();
   
 protected:
  
      // typedefs
  typedef std::map<WordIndex,WordIndex> GlobalToLocalDataMap;
  
      // data members
  std::vector<double> weights;
  std::vector<double> normWeights;
  std::vector<BaseNgramLM<std::vector<WordIndex> >* > modelPtrVec;
  std::vector<GlobalToLocalDataMap> gtlDataMapVec;
  lm_ienc* encPtr;
  int modelIndex;
      // modelIndex determines to which model are applied the functions
      // addTableEntry, infSrcTrg and getEntriesForTarget

      // Auxiliary functions to handle vocabularies
  bool globalStringToWordIndex(const std::string& ht,
                               WordIndex& t)const;
  bool globalStrVecToWidxVec(const std::vector<std::string>& rq,
                             std::vector<WordIndex>& vu)const;

      // Auxiliary encoding functions
  std::vector<WordIndex> mapGlobalToLocalWidxVec(unsigned int index,
                                            const std::vector<WordIndex>& global_s);
  WordIndex mapGlobalToLocalWidx(unsigned int index,
                                 const WordIndex& global_t);
  
      // Specific functions for interpolated ngram language models
  void setWeights(const std::vector<double>& _weights);
  std::vector<double> obtainNormWeights(const std::vector<double>& unnormWeights);
};

#endif
