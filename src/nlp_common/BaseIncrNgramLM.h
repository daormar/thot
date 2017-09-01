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
/* Module: BaseIncrNgramLM                                          */
/*                                                                  */
/* Prototype file: BaseIncrNgramLM.h                                */
/*                                                                  */
/* Description: Abstract class to manage incremental n-gram         */
/*              language models                                     */
/*                                                                  */
/********************************************************************/

#ifndef _BaseIncrNgramLM
#define _BaseIncrNgramLM

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <BaseNgramLM.h>
#include "LM_Defs.h"
#include <math.h>
#include <string>
#include <map>
#include <vector>

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------

//--------------- BaseIncrNgramLM class

template<class LM_STATE>
class BaseIncrNgramLM: public BaseNgramLM<LM_STATE>
{
 public:

  typedef LM_STATE LM_State;
  
      // Probability functions
  virtual LgProb getNgramLgProb(WordIndex w,
                                const std::vector<WordIndex>& vu)=0;
      // returns the probability of an n-gram, uv[0] stores the n-1'th
      // word of the n-gram, uv[1] the n-2'th one and so on
  virtual LgProb getNgramLgProbStr(std::string s,
                                   const std::vector<std::string>& rq)=0;
      // returns the probability of an n-gram. Each string represents a
      // single word
  virtual LgProb getLgProbEnd(const std::vector<WordIndex>& vu)=0;
  virtual LgProb getLgProbEndStr(const std::vector<std::string>& rq)=0;
  virtual Prob getZeroGramProb(void);

      // Probability functions using states
  virtual bool getStateForWordSeq(const std::vector<WordIndex>& wordSeq,
                                  LM_STATE& state)=0;
  virtual void getStateForBeginOfSentence(LM_STATE &state)=0;
  virtual LgProb getNgramLgProbGivenState(WordIndex w,
                                          LM_STATE &state)=0;
  virtual LgProb getNgramLgProbGivenStateStr(std::string s,
                                             LM_STATE &state)=0;
  virtual LgProb getLgProbEndGivenState(LM_STATE &state)=0;
      // In these functions, the state is updated once the
      // function is executed
   
      // Encoding-related functions
  virtual bool existSymbol(std::string s)const=0;
  virtual WordIndex addSymbol(std::string s)=0;
  virtual unsigned int getVocabSize(void)=0;
  virtual WordIndex stringToWordIndex(std::string s)const=0;
  virtual std::string wordIndexToString(WordIndex w)const=0;
  virtual WordIndex getBosId(bool &found)const=0;
  virtual WordIndex getEosId(bool &found)const=0;
  virtual bool loadVocab(const char *fileName)=0;
      // Load encoding information given a prefix file name
  virtual bool printVocab(const char *fileName)=0;
      // Prints encoding information
  virtual void clearVocab(void)=0;
      // Clears encoding information

      // Functions to access model counts
  virtual Count cHist(const std::vector<WordIndex>& vu)=0;
  virtual Count cNgram(const WordIndex& w,
                       const std::vector<WordIndex>& vu)=0;
  virtual Count cHistStr(const std::vector<std::string>& rq)=0;
  virtual Count cNgramStr(const std::string& s,
                          const std::vector<std::string>& rq)=0;
  
      // Functions to extend the model
  virtual int trainSentence(std::vector<std::string> strVec,
                            Count c=1,
                            Count lowerBound=0,
                            int verbose=0);

  virtual int trainSentenceVec(std::vector<std::vector<std::string> > vecOfStrVec,
                               Count c=1,
                               Count lowerBound=0,
                               int verbose=0);

      // Functions to incrementally extend the model
  virtual void incrCountsOfNgramStr(const std::string& s,
                                    const std::vector<std::string>& rq,
                                    Count c)=0;
  virtual void incrCountsOfNgram(const WordIndex& w,
                                 const std::vector<WordIndex>& vu,
                                 Count c)=0;

      // Functions to load and print the model
  virtual bool load(const char *fileName)=0;
  virtual bool print(const char *fileName)=0;
  virtual unsigned int getNgramOrder(void)=0;
  virtual void setNgramOrder(int _ngramOrder)=0;

      // size and clear functions
  virtual size_t size(void)=0;
  virtual void clear(void)=0;

      // Destructor
  virtual ~BaseIncrNgramLM()=0;
   
 protected:

  typedef std::map<std::pair<std::string,std::vector<std::string> >,Count> NgramCountMap;

  void collectNgramCounts(std::vector<std::string> strVec,
                          NgramCountMap& ngramCountMap);
  void accumNgramCounts(NgramCountMap& ngramCountMap,
                        std::string word,
                        std::vector<std::string>& hist);
  virtual void updateNgramInfo(std::string word,
                               std::vector<std::string> hist,
                               Count extractedCount,
                               Count c,
                               Count lowerBound);
};

//--------------- Template function definitions


//---------------
template<class LM_STATE>
BaseIncrNgramLM<LM_STATE>::~BaseIncrNgramLM()
{
  
}

//---------------
template<class LM_STATE>
Prob BaseIncrNgramLM<LM_STATE>::getZeroGramProb(void)
{
  return (double) 1/getVocabSize();  
}

//---------------
template<class LM_STATE>
int BaseIncrNgramLM<LM_STATE>::trainSentence(std::vector<std::string> strVec,
                                             Count c/*=1*/,
                                             Count lowerBound/*=0*/,
                                             int verbose)
{
  unsigned int ngram_order=getNgramOrder();
  if(ngram_order==0) return THOT_ERROR;
  else
  {
        // Collect n-gram counts
    NgramCountMap ngramCountMap;
    NgramCountMap::iterator ngramCountMapIter;
    collectNgramCounts(strVec,ngramCountMap);

        // Process n-gram counts
    for(ngramCountMapIter=ngramCountMap.begin();ngramCountMapIter!=ngramCountMap.end();++ngramCountMapIter)
    {
          // Extract n-gram information
      std::string word=ngramCountMapIter->first.first;
      std::vector<std::string> hist=ngramCountMapIter->first.second;
      Count ngc=ngramCountMapIter->second;

          // Print current counts
      if(verbose)
      {
        for(unsigned int k=0;k<hist.size();++k)
          std::cerr<<hist[k]<<" ";
        std::cerr<<word<<" "<<cNgramStr(word,hist)<<std::endl;
      }

          // Update ngram info
      updateNgramInfo(word,hist,ngc,c,lowerBound);
    }
    return THOT_OK;
  }
}
//---------------
template<class LM_STATE>
void BaseIncrNgramLM<LM_STATE>::collectNgramCounts(std::vector<std::string> strVec,
                                                   NgramCountMap& ngramCountMap)
{
  unsigned int ngram_order=getNgramOrder();
  bool bos_added;
  std::vector<std::string> hist;

      // Add begin of sentence symbol
  accumNgramCounts(ngramCountMap,BOS_STR,hist);
    
      // Increase counts of n-grams (1<n<=ngram_order)
  for(unsigned int n=1;n<=ngram_order;++n)
  {
        // Train sentence
    for(int i=0;i<(int)strVec.size();++i)
    {
      hist.clear();
      bos_added=false;
      for(int j=(int)n-1;j>=1;--j)
      {
        if(i-j<0)
        {
          if(!bos_added)
          {
            hist.push_back(BOS_STR);
            bos_added=true;
          }
        }
        else hist.push_back(strVec[i-j]);	
      }
          // Add n-gram iff its size is equal to the order under
          // consideration
      if(hist.size()+1==n)
      {
            // Add n-gram iff the final count is above the lower bound
        accumNgramCounts(ngramCountMap,strVec[i],hist);
      }
    }
    
    hist.clear();
    bos_added=false;
    for(int j=(int)n-1;j>=1;--j)
    {
      if((int)strVec.size()-j<0)
      {
        if(!bos_added)
        {
          hist.push_back(BOS_STR);
          bos_added=true;
        }
      }
      else hist.push_back(strVec[strVec.size()-j]);	
    }
        // Add n-gram iff its size is equal to the order under
        // consideration
    if(hist.size()+1==n)
    {
          // Add n-gram iff the final count is above the lower bound
      accumNgramCounts(ngramCountMap,EOS_STR,hist);
    }
  }
}

//---------------
template<class LM_STATE>
void BaseIncrNgramLM<LM_STATE>::accumNgramCounts(NgramCountMap& ngramCountMap,
                                                 std::string word,
                                                 std::vector<std::string>& hist)
{
  NgramCountMap::iterator ngramCountMapIter=ngramCountMap.find(std::make_pair(word,hist));
  if(ngramCountMapIter!=ngramCountMap.end())
  {
    ngramCountMapIter->second=ngramCountMapIter->second+(Count)1;
  }
  else
  {
    ngramCountMap[std::make_pair(word,hist)]=1;
  }
}

//---------------
template<class LM_STATE>
void BaseIncrNgramLM<LM_STATE>::updateNgramInfo(std::string word,
                                                std::vector<std::string> hist,
                                                Count extractedCount,
                                                Count c,
                                                Count lowerBound)
{
  if(cNgramStr(word,hist)+(c*extractedCount)>lowerBound)
    incrCountsOfNgramStr(word,hist,(c*extractedCount));
}

//---------------
template<class LM_STATE>
int BaseIncrNgramLM<LM_STATE>::trainSentenceVec(std::vector<std::vector<std::string> > vecOfStrVec,
                                                Count c/*=1*/,
                                                Count lowerBound/*=0*/,
                                                int verbose)
{
  for(unsigned int i=0;i<vecOfStrVec.size();++i)
  {
    int ret=trainSentence(vecOfStrVec[i],c,lowerBound,verbose);
    if(ret==THOT_ERROR) return THOT_ERROR;
  }
  return THOT_OK;
}

#endif
