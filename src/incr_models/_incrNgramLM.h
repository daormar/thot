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
/* Module: _incrNgramLM                                             */
/*                                                                  */
/* Prototype file: _incrNgramLM.h                                   */
/*                                                                  */
/* Description: Base class to manage incremental encoded ngram      */
/*              language models p(x|std::vector<x>).                */
/*                                                                  */
/********************************************************************/

#ifndef __incrNgramLM
#define __incrNgramLM

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "vecx_x_incr_ecpm.h"
#include <lm_ienc.h>
#include "ModelDescriptorUtils.h"
#include <string>
#include "BaseIncrNgramLM.h"

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------

//--------------- _incrNgramLM class

template<class SRC_INFO,class SRCTRG_INFO>
class _incrNgramLM: public _incrEncCondProbModel<std::vector<std::string>,std::string,std::vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>,public BaseIncrNgramLM<std::vector<WordIndex> >
{
 public:

  typedef typename _incrEncCondProbModel<std::vector<std::string>,std::string,std::vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::SrcTableNode SrcTableNode;
  typedef typename _incrEncCondProbModel<std::vector<std::string>,std::string,std::vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::TrgTableNode TrgTableNode;
  typedef typename BaseIncrNgramLM<std::vector<WordIndex> >::LM_State LM_State;

      // Constructor
  _incrNgramLM():_incrEncCondProbModel<std::vector<std::string>,std::string,std::vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>()
  {
    this->encPtr=new lm_ienc;
    ngramOrder=3;
  }

      // Basic function redefinitions
  void addTableEntryHigh(const std::vector<std::string>& hs,
                         const std::string& ht,
                         im_pair<SRC_INFO,SRCTRG_INFO> inf);
  bool loadEncodingInfo(const char *prefixFileName);

  // _incrNgramLM function definitions

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
  LgProb getNgramLgProbStr(std::string s,const std::vector<std::string>& rq);
      // returns the probability of a n-gram. Each string represents a
      // single word
  LgProb getLgProbEnd(const std::vector<WordIndex>& vu);
  LgProb getLgProbEndStr(const std::vector<std::string>& rq);

      // Probability functions using states
  bool getStateForWordSeq(const std::vector<WordIndex>& wordSeq,
                          std::vector<WordIndex>& state); 
  void getStateForBeginOfSentence(std::vector<WordIndex> &state);
  void addNextWordToState(WordIndex word,
                          LM_State& state);
  LgProb getNgramLgProbGivenState(WordIndex w,std::vector<WordIndex> &state);
  LgProb getNgramLgProbGivenStateStr(std::string s,std::vector<WordIndex> &state);
  LgProb getLgProbEndGivenState(std::vector<WordIndex> &state);
   
      // encoding-related functions
  bool existSymbol(std::string s)const;
  WordIndex addSymbol(std::string s);
  unsigned int getVocabSize(void);
  WordIndex stringToWordIndex(std::string s)const;
  std::string wordIndexToString(WordIndex w)const;
  WordIndex getBosId(bool &found)const;
  WordIndex getEosId(bool &found)const;
  bool loadVocab(const char *prefixFileName);
      // Load encoding information given a prefix file name
  bool printVocab(const char *prefixFileName);
      // Prints encoding information
  void clearVocab(void);
      // Clears encoding information

      // Functions to load and print the model
  bool load(const char *fileName);
  bool print(const char *fileName);
  std::ostream& print(std::ostream &outS);

      // n-gram order related functions
  void setNgramOrder(int _ngramOrder);
  unsigned int getNgramOrder(void);

      // size and clear functions
  size_t size(void);
  void clear(void);
  
      // Destructor
  virtual ~_incrNgramLM(){}
   
 protected:

  unsigned int ngramOrder;

      // Auxiliary functions to load and print the model
  bool load_ngrams(const char *fileName);
};

// Function definitions ---------------------------------------------
//

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
void _incrNgramLM<SRC_INFO,SRCTRG_INFO>::addTableEntryHigh(const std::vector<std::string>& hs,
                                                           const std::string& ht,
                                                           im_pair<SRC_INFO,SRCTRG_INFO> inf)
{
  std::string std_str;
  
  _incrEncCondProbModel<std::vector<std::string>,std::string,std::vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::addTableEntryHigh(hs,ht,inf);
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
bool _incrNgramLM<SRC_INFO,SRCTRG_INFO>::loadEncodingInfo(const char *prefixFileName)
{
  std::string std_str;
  
  if(_incrEncCondProbModel<std::vector<std::string>,std::string,std::vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::loadEncodingInfo(prefixFileName))
  {
    return true;
  }
  else
  {
    return false;
  }
}
//---------------
template<class SRC_INFO,class SRCTRG_INFO>
void _incrNgramLM<SRC_INFO,SRCTRG_INFO>::setNgramOrder(int _ngramOrder)
{
  ngramOrder=_ngramOrder;
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
Count _incrNgramLM<SRC_INFO,SRCTRG_INFO>::cHist(const std::vector<WordIndex>& vu)
{
  return _incrEncCondProbModel<std::vector<std::string>,std::string,std::vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::cSrc(vu);
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
Count _incrNgramLM<SRC_INFO,SRCTRG_INFO>::cNgram(const WordIndex& w,
                                                 const std::vector<WordIndex>& vu)
{
  return _incrEncCondProbModel<std::vector<std::string>,std::string,std::vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::cSrcTrg(vu,w);  
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
Count _incrNgramLM<SRC_INFO,SRCTRG_INFO>::cHistStr(const std::vector<std::string>& rq)
{
  return _incrEncCondProbModel<std::vector<std::string>,std::string,std::vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::cHSrc(rq);  
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
Count _incrNgramLM<SRC_INFO,SRCTRG_INFO>::cNgramStr(const std::string& s,
                                                    const std::vector<std::string>& rq)
{
  return _incrEncCondProbModel<std::vector<std::string>,std::string,std::vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::cHSrcHTrg(rq,s);    
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
void _incrNgramLM<SRC_INFO,SRCTRG_INFO>::incrCountsOfNgramStr(const std::string& s,
                                                              const std::vector<std::string>& rq,
                                                              Count c)
{
      // Revise vocabularies if necessary
  if(!existSymbol(s)) addSymbol(s);
  for(unsigned int i=0;i<rq.size();++i)
    if(!existSymbol(rq[i])) addSymbol(rq[i]);

      // Increase counts
  WordIndex w=stringToWordIndex(s);
  std::vector<WordIndex> vu;
  for(unsigned int i=0;i<rq.size();++i)
    vu.push_back(stringToWordIndex(rq[i]));
  incrCountsOfNgram(w,vu,c);
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
void _incrNgramLM<SRC_INFO,SRCTRG_INFO>::incrCountsOfNgram(const WordIndex& w,
                                                           const std::vector<WordIndex>& vu,
                                                           Count c)
{
  _incrEncCondProbModel<std::vector<std::string>,std::string,std::vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::incrCountsOfEntry(vu,w,c);
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
LgProb _incrNgramLM<SRC_INFO,SRCTRG_INFO>::getNgramLgProb(WordIndex w,
                                                          const std::vector<WordIndex>& vu)
{
  return _incrNgramLM<SRC_INFO,SRCTRG_INFO>::logpTrgGivenSrc(vu,w);  
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
LgProb _incrNgramLM<SRC_INFO,SRCTRG_INFO>::getNgramLgProbStr(std::string s,
                                                             const std::vector<std::string>& rq)
{
  return _incrNgramLM<SRC_INFO,SRCTRG_INFO>::logpHTrgGivenHSrc(rq,s);    
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
LgProb _incrNgramLM<SRC_INFO,SRCTRG_INFO>::getLgProbEnd(const std::vector<WordIndex>& vu)
{
  bool found;
  return _incrNgramLM<SRC_INFO,SRCTRG_INFO>::logpTrgGivenSrc(vu,getEosId(found));  
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
LgProb _incrNgramLM<SRC_INFO,SRCTRG_INFO>::getLgProbEndStr(const std::vector<std::string>& rq)
{
  return _incrNgramLM<SRC_INFO,SRCTRG_INFO>::logpHTrgGivenHSrc(rq,EOS_STR);    
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
bool _incrNgramLM<SRC_INFO,SRCTRG_INFO>::getStateForWordSeq(const std::vector<WordIndex>& wordSeq,
                                                            std::vector<WordIndex>& state)
{
  state=wordSeq;
  return true;
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
void _incrNgramLM<SRC_INFO,SRCTRG_INFO>::getStateForBeginOfSentence(std::vector<WordIndex> &state)
{
  std::vector<WordIndex> keySeq;
  bool found;
  
  state.clear();

  if(ngramOrder>0)
  {
    for(unsigned int i=0;i<ngramOrder-1;++i)
      state.push_back(getBosId(found));
  }
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
void _incrNgramLM<SRC_INFO,SRCTRG_INFO>::addNextWordToState(WordIndex word,
                                                            LM_State& state)
{
  for(unsigned int i=1;i<state.size();++i) state[i-1]=state[i];
  if(state.size()>0) state[state.size()-1]=word;  
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
LgProb _incrNgramLM<SRC_INFO,SRCTRG_INFO>::getNgramLgProbGivenState(WordIndex w,
                                                                    std::vector<WordIndex> &state)
{
  LgProb lp;

  lp=getNgramLgProb(w,state);
  addNextWordToState(w,state);
  return lp;
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
LgProb _incrNgramLM<SRC_INFO,SRCTRG_INFO>::getNgramLgProbGivenStateStr(std::string s,
                                                                       std::vector<WordIndex> &state)
{
 WordIndex w;
	
 w=stringToWordIndex(s);
 return getNgramLgProbGivenState(w,state);    
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
LgProb _incrNgramLM<SRC_INFO,SRCTRG_INFO>::getLgProbEndGivenState(std::vector<WordIndex> &state)
{
  LgProb lp;
  bool found;
  
  lp=getLgProbEnd(state);
  addNextWordToState(getEosId(found),state);
  return lp;   
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
bool _incrNgramLM<SRC_INFO,SRCTRG_INFO>::existSymbol(std::string s)const
{
  WordIndex w;
  
  return _incrEncCondProbModel<std::vector<std::string>,std::string,std::vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::HighTrg_to_Trg(s,w);  
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
WordIndex _incrNgramLM<SRC_INFO,SRCTRG_INFO>::addSymbol(std::string s)
{
  return _incrEncCondProbModel<std::vector<std::string>,std::string,std::vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::addHTrgCode(s);
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
unsigned int _incrNgramLM<SRC_INFO,SRCTRG_INFO>::getVocabSize(void)
{
  return _incrEncCondProbModel<std::vector<std::string>,std::string,std::vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::sizeTrgEnc();
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
WordIndex _incrNgramLM<SRC_INFO,SRCTRG_INFO>::stringToWordIndex(std::string s)const
{
  WordIndex w;
  
  bool found=_incrEncCondProbModel<std::vector<std::string>,std::string,std::vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::HighTrg_to_Trg(s,w);
  if(!found) w=UNK_SYMBOL;
  
  return w;
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
std::string _incrNgramLM<SRC_INFO,SRCTRG_INFO>::wordIndexToString(WordIndex w)const
{
  std::string s;
  
  bool found=_incrEncCondProbModel<std::vector<std::string>,std::string,std::vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::Trg_to_HighTrg(w,s);
  if(!found) s=UNK_SYMBOL_STR;
  
  return s;
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
WordIndex _incrNgramLM<SRC_INFO,SRCTRG_INFO>::getBosId(bool &found)const
{
      // Note: since lm_ienc is used as encoder, getBosId() returns
      // S_BEGIN and found is set to true
  WordIndex bosId=0;
  found=false;
  if(_incrEncCondProbModel<std::vector<std::string>,std::string,std::vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::HighTrg_to_Trg(BOS_STR,bosId))
    found=true;
  return bosId;
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
WordIndex _incrNgramLM<SRC_INFO,SRCTRG_INFO>::getEosId(bool &found)const
{
      // Note: since lm_ienc is used as encoder, getEosId() returns
      // S_END and found is set to true
  WordIndex eosId=0;
  found=false;
  if(_incrEncCondProbModel<std::vector<std::string>,std::string,std::vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::HighTrg_to_Trg(EOS_STR,eosId))
    found=true;
  return eosId;
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
bool _incrNgramLM<SRC_INFO,SRCTRG_INFO>::loadVocab(const char *fileName)
{
  return _incrEncCondProbModel<std::vector<std::string>,std::string,std::vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::loadEncodingInfo(fileName);
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
bool _incrNgramLM<SRC_INFO,SRCTRG_INFO>::printVocab(const char *fileName)
{
  return _incrEncCondProbModel<std::vector<std::string>,std::string,std::vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::printEncodingInfo(fileName);  
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
void _incrNgramLM<SRC_INFO,SRCTRG_INFO>::clearVocab(void)
{
  _incrEncCondProbModel<std::vector<std::string>,std::string,std::vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::clearEncodingInfo();
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
bool _incrNgramLM<SRC_INFO,SRCTRG_INFO>::load(const char *fileName)
{
  std::string mainFileName;
  if(fileIsDescriptor(fileName,mainFileName))
  {
    std::string descFileName=fileName;
    std::string absolutizedMainFileName=absolutizeModelFileName(descFileName,mainFileName);
    return load_ngrams(absolutizedMainFileName.c_str());
  }
  else
  {
    return load_ngrams(fileName);
  }  
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
bool _incrNgramLM<SRC_INFO,SRCTRG_INFO>::load_ngrams(const char *fileName)
{
  std::vector<std::string> hs;
  std::string ht;
  im_pair<SRC_INFO,SRCTRG_INFO> inf;
  awkInputStream awk;
  unsigned int i;
  unsigned int ngramOrderAux=ngramOrder;
    
  if(awk.open(fileName)==THOT_ERROR)
  {
    std::cerr<<"Error while loading language model file "<<fileName<<std::endl;
    return THOT_ERROR;
  }  
  else
  {
    std::cerr<<"Loading language model file "<<fileName<<std::endl;

    ngramOrder=0;
    this->tablePtr->clear();
    this->modelFileName=fileName;

        // Obtain n-gram order (it is obtained first because some models
        // need to know the order before adding any n-gram)
    while(awk.getln())
    {
      if(ngramOrder<awk.NF-2) ngramOrder=awk.NF-2;
    }

        // Rewind file
    awk.rwd();
    
        // Read n-grams
    while(awk.getln())
    {
      if(awk.NF>1)
      {
        hs.clear();
        if(awk.NF>2)
        {
          for(i=1;i<awk.NF-2;++i)
          {
            hs.push_back(awk.dollar(i));
          }
        }
        ht=awk.dollar(awk.NF-2);
        inf.first=atof(awk.dollar(awk.NF-1).c_str());
        inf.second=atof(awk.dollar(awk.NF).c_str());       
        addTableEntryHigh(hs,ht,inf);
      }
    }
  }
  if(ngramOrder==0) ngramOrder=ngramOrderAux;
  
  return THOT_OK;
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
bool _incrNgramLM<SRC_INFO,SRCTRG_INFO>::print(const char *fileName)
{
  std::string lmFileName;
  std::string mainFileName;
  if(fileIsDescriptor(fileName,mainFileName))
  {
        // File is descriptor
    std::string descFileName=fileName;
    std::string absolutizedMainFileName=absolutizeModelFileName(descFileName,mainFileName);
    lmFileName=absolutizedMainFileName;
  }
  else
  {
        // File is not descriptor
    lmFileName=fileName;
  }
  
  std::ofstream outF;
  outF.open(lmFileName.c_str(),std::ios::out);
  if(!outF)
  {
    std::cerr<<"Error while printing model to file."<<std::endl;
    return THOT_ERROR;
  }
  else
  {
    print(outF);
    outF.close();	
    return THOT_OK;
  }
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
std::ostream& _incrNgramLM<SRC_INFO,SRCTRG_INFO>::print(std::ostream &outS)
{
  std::vector<std::string> hs;
  std::vector<WordIndex> s;
  std::string ht;
  vecx_x_incr_cptable<WordIndex,SRC_INFO,SRCTRG_INFO>* tableCptPtr=0;
  unsigned int i;

  tableCptPtr=dynamic_cast<vecx_x_incr_cptable<WordIndex,SRC_INFO,SRCTRG_INFO>*>(this->tablePtr);
  
  if(tableCptPtr) // C++ RTTI
  {
    typename vecx_x_incr_cptable<WordIndex,SRC_INFO,SRCTRG_INFO>::const_iterator tableIter;
      
        // Set float precision.
    outS.setf( std::ios::fixed, std::ios::floatfield );
    outS.precision(8);

    for(tableIter=tableCptPtr->begin();tableIter!=tableCptPtr->end();++tableIter)
    {
      if(tableIter->first.size()>0 && (double)tableIter->second.get_c_st()>0)
      {
        s.clear();
        for(i=0;i<tableIter->first.size()-1;++i)
        {
          s.push_back(tableIter->first[i]);
        }
        this->Src_to_HighSrc(s,hs);
        this->Trg_to_HighTrg(tableIter->first.back(),ht);        

        for(i=0;i<hs.size();++i)
        {
          outS<<hs[i]<<" ";
        }
        bool found;
        outS<<ht<<" "<<this->getSrcInfo(s,found)<<" "<<tableIter->second<<std::endl;
      }
    }     
  }
  return outS; 
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
unsigned int _incrNgramLM<SRC_INFO,SRCTRG_INFO>::getNgramOrder(void)
{
  return ngramOrder;
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
size_t _incrNgramLM<SRC_INFO,SRCTRG_INFO>::size(void)
{
  return _incrEncCondProbModel<std::vector<std::string>,std::string,std::vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::size();
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
void _incrNgramLM<SRC_INFO,SRCTRG_INFO>::clear(void)
{
  _incrEncCondProbModel<std::vector<std::string>,std::string,std::vector<WordIndex>,WordIndex,SRC_INFO,SRCTRG_INFO>::clear();
}

//-----------------------------------------------------------------

#endif
