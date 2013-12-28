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

#include "vecx_x_incr_interp_ecpm.h"
#include <string>
#include "BaseIncrNgramLM.h"
#include <lm_ienc.h>

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------

//--------------- _incrInterpNgramLM class

template<class SRC_INFO,class SRCTRG_INFO>
class _incrInterpNgramLM: public _incrInterpEncCondProbModel<Vector<std::string>,std::string,Vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>, public BaseIncrNgramLM<Vector<WordIndex> >
{
 public:

  typedef typename _incrInterpEncCondProbModel<Vector<std::string>,std::string,Vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::SrcTableNode SrcTableNode;
  typedef typename _incrInterpEncCondProbModel<Vector<std::string>,std::string,Vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::TrgTableNode TrgTableNode;
  typedef typename BaseIncrNgramLM<Vector<WordIndex> >::LM_State LM_State;

      // Constructor
  _incrInterpNgramLM():_incrInterpEncCondProbModel<Vector<std::string>,std::string,Vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>()
    {
      this->encPtr=new lm_ienc;
    }

      // basic vecx_x_incr_interp_ecpm function redefinitions
  void addTableEntryHigh(const Vector<std::string>& hs,const std::string& ht,SRCTRG_INFO info);
  bool loadEncodingInfo(const char *prefixFileName);

  // BaseIncrNgramLM function definitions

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
 bool getStateForWordSeq(const Vector<ngramWordIndex>& wordSeq,
                         Vector<ngramWordIndex>& state); 
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

      // size and clear functions
  size_t size(void);
  void clear(void);
  
      // Destructor
  virtual ~_incrInterpNgramLM()
  {
    delete this->encPtr;
  }
   
 protected:

};

//--------------- Template function definitions 

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
void _incrInterpNgramLM<SRC_INFO,SRCTRG_INFO>::addTableEntryHigh(const Vector<std::string>& hs,const std::string& ht,SRCTRG_INFO inf)
{  
  _incrInterpEncCondProbModel<Vector<std::string>,std::string,Vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::addTableEntryHigh(hs,ht,inf);
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
bool _incrInterpNgramLM<SRC_INFO,SRCTRG_INFO>::loadEncodingInfo(const char *prefixFileName)
{
  if(_incrInterpEncCondProbModel<Vector<std::string>,std::string,Vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::loadEncodingInfo(prefixFileName))
  {
    return true;
  }
  else
  {
    return false;
  }
}

// BaseIncrNgramLM<SRC_INFO,SRCTRG_INFO> function definitions --------------------------------


//---------------
template<class SRC_INFO,class SRCTRG_INFO>
Count _incrInterpNgramLM<SRC_INFO,SRCTRG_INFO>::cHist(const Vector<WordIndex>& vu)
{
  return _incrInterpEncCondProbModel<Vector<std::string>,std::string,Vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::cSrc(vu);
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
Count _incrInterpNgramLM<SRC_INFO,SRCTRG_INFO>::cNgram(const WordIndex& w,
                                                       const Vector<WordIndex>& vu)
{
  return _incrInterpEncCondProbModel<Vector<std::string>,std::string,Vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::cSrcTrg(vu,w);  
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
Count _incrInterpNgramLM<SRC_INFO,SRCTRG_INFO>::cHistStr(const Vector<std::string>& rq)
{
  return _incrInterpEncCondProbModel<Vector<std::string>,std::string,Vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::cHSrc(rq);
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
Count _incrInterpNgramLM<SRC_INFO,SRCTRG_INFO>::cNgramStr(const std::string& s,
                                                          const Vector<std::string>& rq)
{
  return _incrInterpEncCondProbModel<Vector<std::string>,std::string,Vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::cHSrcHTrg(rq,s);
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
void _incrInterpNgramLM<SRC_INFO,SRCTRG_INFO>::incrCountsOfNgramStr(const std::string& s,
                                                                    const Vector<std::string>& rq,
                                                                    Count c)
{  
  _incrInterpEncCondProbModel<Vector<std::string>,std::string,Vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::incrCountsOfEntryHigh(rq,s,c);
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
void _incrInterpNgramLM<SRC_INFO,SRCTRG_INFO>::incrCountsOfNgram(const WordIndex& w,
                                                                 const Vector<WordIndex>& vu,
                                                                 Count c)
{
  _incrInterpEncCondProbModel<Vector<std::string>,std::string,Vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::incrCountsOfEntry(vu,w,c);
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
LgProb _incrInterpNgramLM<SRC_INFO,SRCTRG_INFO>::getNgramLgProb(WordIndex w,
                                                                const Vector<WordIndex>& vu)
{
  return _incrInterpNgramLM<SRC_INFO,SRCTRG_INFO>::logpTrgGivenSrc(vu,w);  
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
LgProb _incrInterpNgramLM<SRC_INFO,SRCTRG_INFO>::getNgramLgProbStr(string s,
                                                                   const Vector<string>& rq)
{
  return _incrInterpNgramLM<SRC_INFO,SRCTRG_INFO>::logpHTrgGivenHSrc(rq,s);    
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
LgProb _incrInterpNgramLM<SRC_INFO,SRCTRG_INFO>::getLgProbEnd(const Vector<WordIndex>& vu)
{
  bool found;
  
  return _incrInterpNgramLM<SRC_INFO,SRCTRG_INFO>::logpTrgGivenSrc(vu,this->getEosId(found));  
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
LgProb _incrInterpNgramLM<SRC_INFO,SRCTRG_INFO>::getLgProbEndStr(const Vector<string>& rq)
{
  return _incrInterpNgramLM<SRC_INFO,SRCTRG_INFO>::logpHTrgGivenHSrc(rq,"</s>");    
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
bool _incrInterpNgramLM<SRC_INFO,SRCTRG_INFO>::getStateForWordSeq(const Vector<ngramWordIndex>& wordSeq,
                                                                  Vector<ngramWordIndex>& state)
{
  state=wordSeq;
  return true;
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
void _incrInterpNgramLM<SRC_INFO,SRCTRG_INFO>::getStateForBeginOfSentence(Vector<WordIndex> &state)
{
  Vector<WordIndex> keySeq;
  int i;
  bool found;
  
  for(i=0;i<(int)this->getNgramOrder()-1;++i)
    state.push_back(this->getBosId(found));  
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
LgProb _incrInterpNgramLM<SRC_INFO,SRCTRG_INFO>::getNgramLgProbGivenState(WordIndex w,
                                                                          Vector<WordIndex> &state)
{
  LgProb lp;

  lp=getNgramLgProb(w,state);
  for(unsigned int i=1;i<state.size();++i) state[i-1]=state[i];
  if(state.size()>0) state[state.size()-1]=w;
  return lp;
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
LgProb _incrInterpNgramLM<SRC_INFO,SRCTRG_INFO>::getNgramLgProbGivenStateStr(std::string s,
                                                                             Vector<WordIndex> &state)
{
 WordIndex w;
	
 w=stringToWordIndex(s);
 return getNgramLgProbGivenState(w,state);    
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
LgProb _incrInterpNgramLM<SRC_INFO,SRCTRG_INFO>::getLgProbEndGivenState(Vector<WordIndex> &state)
{
  LgProb lp;
  bool found;
  
  lp=getLgProbEnd(state);
  for(unsigned int i=1;i<state.size();++i) state[i-1]=state[i];
  if(state.size()>0) state[state.size()-1]=this->getEosId(found);
  return lp;   
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
bool _incrInterpNgramLM<SRC_INFO,SRCTRG_INFO>::existSymbol(string s)const
{
  WordIndex w;
  
  return _incrInterpEncCondProbModel<Vector<std::string>,std::string,Vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::HighTrg_to_Trg(s,w);  
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
WordIndex _incrInterpNgramLM<SRC_INFO,SRCTRG_INFO>::addSymbol(std::string s)
{
  return _incrInterpEncCondProbModel<Vector<std::string>,std::string,Vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::addHTrgCode(s);
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
unsigned int _incrInterpNgramLM<SRC_INFO,SRCTRG_INFO>::getVocabSize(void)
{
  return _incrInterpEncCondProbModel<Vector<std::string>,std::string,Vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::sizeTrgEnc();
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
WordIndex _incrInterpNgramLM<SRC_INFO,SRCTRG_INFO>::stringToWordIndex(string s)const
{
  WordIndex w;
  
  bool found=_incrInterpEncCondProbModel<Vector<std::string>,std::string,Vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::HighTrg_to_Trg(s,w);
  if(!found) w=UNK_SYMBOL;
    
  return w;
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
string _incrInterpNgramLM<SRC_INFO,SRCTRG_INFO>::wordIndexToString(WordIndex w)const
{
  std::string s;
  
  bool found=_incrInterpEncCondProbModel<Vector<std::string>,std::string,Vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::Trg_to_HighTrg(w,s);
  if(!found) s=UNK_SYMBOL_STR;
  
  return s;
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
bool _incrInterpNgramLM<SRC_INFO,SRCTRG_INFO>::loadVocab(const char *fileName)
{
  return _incrInterpEncCondProbModel<Vector<std::string>,std::string,Vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::loadEncodingInfo(fileName);
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
bool _incrInterpNgramLM<SRC_INFO,SRCTRG_INFO>::printVocab(const char *fileName)
{
  return _incrInterpEncCondProbModel<Vector<std::string>,std::string,Vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::printEncodingInfo(fileName);  
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
void _incrInterpNgramLM<SRC_INFO,SRCTRG_INFO>::clearVocab(void)
{
  _incrInterpEncCondProbModel<Vector<std::string>,std::string,Vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::clearEncodingInfo();
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
size_t _incrInterpNgramLM<SRC_INFO,SRCTRG_INFO>::size(void)
{
  return _incrInterpEncCondProbModel<Vector<std::string>,std::string,Vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::size();
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
void _incrInterpNgramLM<SRC_INFO,SRCTRG_INFO>::clear(void)
{
  _incrInterpEncCondProbModel<Vector<std::string>,std::string,Vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::clear();
}

//-----------------------------------------------------------------

#endif
