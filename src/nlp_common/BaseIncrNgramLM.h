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
/* Description: Abstract class to manage incremental n'gram         */
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
#include "myVector.h"
#include <string>

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
                                const Vector<WordIndex>& vu)=0;
      // returns the probability of an n-gram, uv[0] stores the n-1'th
      // word of the n-gram, uv[1] the n-2'th one and so on
  virtual LgProb getNgramLgProbStr(string s,
                                   const Vector<string>& rq)=0;
      // returns the probability of an n-gram. Each string represents a
      // single word
  virtual LgProb getLgProbEnd(const Vector<WordIndex>& vu)=0;
  virtual LgProb getLgProbEndStr(const Vector<string>& rq)=0;
  virtual Prob getZeroGramProb(void);

      // Probability functions using states
  virtual bool getStateForWordSeq(const Vector<WordIndex>& wordSeq,
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
  virtual bool existSymbol(string s)const=0;
  virtual WordIndex addSymbol(string s)=0;
  virtual unsigned int getVocabSize(void)=0;
  virtual WordIndex stringToWordIndex(string s)const=0;
  virtual string wordIndexToString(WordIndex w)const=0;
  virtual WordIndex getBosId(bool &found)const=0;
  virtual WordIndex getEosId(bool &found)const=0;
  virtual bool loadVocab(const char *fileName)=0;
      // Load encoding information given a prefix file name
  virtual bool printVocab(const char *fileName)=0;
      // Prints encoding information
  virtual void clearVocab(void)=0;
      // Clears encoding information

      // Functions to obtain probability for a given sentence
  virtual LgProb getSentenceLog10Prob(Vector<WordIndex> s,
                                      int verbose=0);
      // Calculates the log-probability of the sentence 's' given as a
      // WordIndex vector
  virtual LgProb getSentenceLog10ProbStr(Vector<std::string> s,
                                         int verbose=0);
      // Calculates the log-probability of the sentence 's' given as a
      // string vector
      // Functions to obtain perplexity for a given corpus
  virtual int perplexity(const char *corpusFileName,
                         unsigned int& numOfSentences,
                         unsigned int& numWords,
                         LgProb& perp,
                         int verbose=0);

      // Functions to access model counts
  virtual Count cHist(const Vector<WordIndex>& vu)=0;
  virtual Count cNgram(const WordIndex& w,
                       const Vector<WordIndex>& vu)=0;
  virtual Count cHistStr(const Vector<std::string>& rq)=0;
  virtual Count cNgramStr(const std::string& s,
                          const Vector<std::string>& rq)=0;
  
      // Functions to extend the model
  virtual int trainSentence(Vector<std::string> strVec,
                            Count c=1,
                            Count lowerBound=0,
                            int verbose=0);

  virtual int trainSentenceVec(Vector<Vector<std::string> > vecOfStrVec,
                               Count c=1,
                               Count lowerBound=0,
                               int verbose=0);

      // Functions to incrementally extend the model
  virtual void incrCountsOfNgramStr(const std::string& s,
                                    const Vector<std::string>& rq,
                                    Count c)=0;
  virtual void incrCountsOfNgram(const WordIndex& w,
                                 const Vector<WordIndex>& vu,
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

  typedef map<pair<std::string,Vector<std::string> >,Count> NgramCountMap;

  void collectNgramCounts(Vector<std::string> strVec,
                          NgramCountMap& ngramCountMap);
  void accumNgramCounts(NgramCountMap& ngramCountMap,
                        std::string word,
                        Vector<std::string>& hist);
  virtual void updateNgramInfo(std::string word,
                               Vector<std::string> hist,
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
LgProb BaseIncrNgramLM<LM_STATE>::getSentenceLog10ProbStr(Vector<std::string> s,
                                                          int verbose/*=false*/)
{
  Vector<WordIndex> vwi;

  for(unsigned int i=0;i<s.size();++i)
  {
    vwi.push_back(stringToWordIndex(s[i]));
  }
  return getSentenceLog10Prob(vwi,verbose);
}
//---------------
template<class LM_STATE>
LgProb BaseIncrNgramLM<LM_STATE>::getSentenceLog10Prob(Vector<WordIndex> s,
                                                       int verbose/*=false*/)
{
  LgProb lp;
  LgProb total_lp=0;	
  Vector<WordIndex> hist,aux;
  int i,j,k;
  LM_STATE state;
  getStateForBeginOfSentence(state);	
  unsigned int ngram_order=getNgramOrder();
  bool found;
  
  for(i=0;i<(int)s.size();++i) aux.push_back(s[s.size()-1-i]);
  s=aux;	 
  for(i=(int)s.size()-1;i>=0;--i)
  {
    hist.clear();
    for(j=1;j<(int)ngram_order;++j)
    {
      if(i+j>=(int)s.size()) hist.push_back(getBosId(found));   
      else hist.push_back(s[i+j]);	
    }
    lp=getNgramLgProbGivenState(s[i],state);  
    total_lp=total_lp+lp;  
   
    if(verbose)
    {
      cout<<"   P( "<<wordIndexToString(s[i])<<" | ";    
      for(k=0;k<(int)hist.size();++k) cout << wordIndexToString(hist[hist.size()-1-k]) <<" ";    
      cout<<") = " << exp((double)lp) <<" "<<exp((double)total_lp)<<"\n";	    
    } 
  }
  hist.clear();
 
  for(j=0;j<(int)ngram_order-1;++j)
  {
    if(j>=(int)s.size()) hist.push_back(getBosId(found));   
    else hist.push_back(s[j]);		
  } 
  lp=getLgProbEndGivenState(state);    
  total_lp=total_lp+lp; 
  
  if(verbose)
  {
    cout<<"   P( "<<EOS_STR<<" | ";    
    for(k=0;k<(int)hist.size();++k) cout << wordIndexToString(hist[hist.size()-1-k]) <<" ";  
    cout<<") = " << exp((double)lp)  <<" "<<exp((double)total_lp)<< "\n";
  }	
 
  return total_lp*((double)1/M_LN10);
}

//---------------
template<class LM_STATE>
int BaseIncrNgramLM<LM_STATE>::perplexity(const char *corpusFileName,
                                          unsigned int& numOfSentences,
                                          unsigned int& numWords,
                                          LgProb& perp,
                                          int verbose)
{
  LgProb logp;
  perp=0;	
  awkInputStream awk;
  Vector<std::string> v;	

  numWords=0;
  numOfSentences=0;
      // Open corpus file
  if(awk.open(corpusFileName)==ERROR)
  {
    cerr<<"Error while loading corpus file "<<corpusFileName<<endl;
    return ERROR;
  }  

  while(awk.getln())
  {
        // Process each sentence
    if(awk.NF>=1)
    {
      numWords+=awk.NF;
          
      if(verbose==2) cout<<"*** Sentence "<<numOfSentences<<endl;
      
          // Store the sentence into the vector "v"
      v.clear();
      for(unsigned int i=1;i<=awk.NF;++i)
      {
        v.push_back(awk.dollar(i));
      }
          // Calculate the probability of the sentence
      if(verbose>0) logp=getSentenceLog10ProbStr(v,verbose-1);
      else logp=getSentenceLog10ProbStr(v,verbose);
      if(verbose==1)
      {
        cout<<logp<<" ";
        for(unsigned int i=0;i<v.size();++i)
        {
          if(i<v.size()-1) cout<<v[i]<<" ";
          else cout<<v[i]<<endl;
        }
      }
    }
    perp+=logp; 
    ++numOfSentences;	 
  }
  return OK;
}

//---------------
template<class LM_STATE>
int BaseIncrNgramLM<LM_STATE>::trainSentence(Vector<std::string> strVec,
                                             Count c/*=1*/,
                                             Count lowerBound/*=0*/,
                                             int verbose)
{
  unsigned int ngram_order=getNgramOrder();
  if(ngram_order==0) return ERROR;
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
      Vector<std::string> hist=ngramCountMapIter->first.second;
      Count ngc=ngramCountMapIter->second;

          // Print current counts
      if(verbose)
      {
        for(unsigned int k=0;k<hist.size();++k)
          cerr<<hist[k]<<" ";
        cerr<<word<<" "<<cNgramStr(word,hist)<<endl;
      }

          // Update ngram info
      updateNgramInfo(word,hist,ngc,c,lowerBound);
    }
    return OK;
  }
}
//---------------
template<class LM_STATE>
void BaseIncrNgramLM<LM_STATE>::collectNgramCounts(Vector<std::string> strVec,
                                                   NgramCountMap& ngramCountMap)
{
  unsigned int ngram_order=getNgramOrder();
  bool bos_added;
  Vector<std::string> hist;

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
                                                 Vector<std::string>& hist)
{
  NgramCountMap::iterator ngramCountMapIter=ngramCountMap.find(make_pair(word,hist));
  if(ngramCountMapIter!=ngramCountMap.end())
  {
    ngramCountMapIter->second=ngramCountMapIter->second+(Count)1;
  }
  else
  {
    ngramCountMap[make_pair(word,hist)]=1;
  }
}

//---------------
template<class LM_STATE>
void BaseIncrNgramLM<LM_STATE>::updateNgramInfo(std::string word,
                                                Vector<std::string> hist,
                                                Count extractedCount,
                                                Count c,
                                                Count lowerBound)
{
  if(cNgramStr(word,hist)+(c*extractedCount)>lowerBound)
    incrCountsOfNgramStr(word,hist,(c*extractedCount));
}

//---------------
template<class LM_STATE>
int BaseIncrNgramLM<LM_STATE>::trainSentenceVec(Vector<Vector<std::string> > vecOfStrVec,
                                                Count c/*=1*/,
                                                Count lowerBound/*=0*/,
                                                int verbose)
{
  for(unsigned int i=0;i<vecOfStrVec.size();++i)
  {
    int ret=trainSentence(vecOfStrVec[i],c,lowerBound,verbose);
    if(ret==ERROR) return ERROR;
  }
  return OK;
}

#endif
