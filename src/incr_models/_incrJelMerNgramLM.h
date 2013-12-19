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
/* Module: _incrJelMerNgramLM                                       */
/*                                                                  */
/* Prototype file: _incrJelMerNgramLM.h                             */
/*                                                                  */
/* Description: Base class to manage encoded incremental            */
/*              Jelinek-Mercer ngram language                       */
/*              models p(x|Vector<x>).                              */
/*                                                                  */
/********************************************************************/

#ifndef __incrJelMerNgramLM
#define __incrJelMerNgramLM

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "IncrNgramLM.h"

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------

//--------------- _incrJelMerNgramLM class

template<class SRC_INFO,class SRCTRG_INFO>
class _incrJelMerNgramLM: public _incrNgramLM<SRC_INFO,SRCTRG_INFO>
{
 public:

  typedef typename _incrNgramLM<SRC_INFO,SRCTRG_INFO>::SrcTableNode SrcTableNode;
  typedef typename _incrNgramLM<SRC_INFO,SRCTRG_INFO>::TrgTableNode TrgTableNode;

      // Constructor
  _incrJelMerNgramLM():_incrNgramLM<SRC_INFO,SRCTRG_INFO>()
    {
      weights.push_back(0.5);
      weights.push_back(0.5);
      weights.push_back(0.5);
      numBucketsPerOrder=1;
      sizeOfBucket=0;
    }

      // basic vecx_x_ecpm function redefinitions
  Prob pTrgGivenSrc(const Vector<WordIndex>& s,const WordIndex& t);

      // Functions to load and print the model
  bool load(const char *fileName);
  bool print(const char *fileName);

      // Destructor
  ~_incrJelMerNgramLM();
   
 protected:
  Vector<double> weights;
  unsigned int numBucketsPerOrder;
  double sizeOfBucket;
  
      // Weights related functions
  double getWeight(const Vector<WordIndex>& s,
                   const WordIndex& t);
  virtual double freqOfNgram(const Vector<WordIndex>& s);
  bool loadWeights(const char *fileName);
  bool printWeights(const char *fileName);

      // recursive function to interpolate models
  Prob pTrgGivenSrcRec(const Vector<WordIndex>& s,
                       const WordIndex& t);
};

//--------------- Template function definitions

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
Prob _incrJelMerNgramLM<SRC_INFO,SRCTRG_INFO>::pTrgGivenSrc(const Vector<WordIndex>& s,
                                                            const WordIndex& t)
{
      // Remove extra BOS symbols
  bool found;
  Vector<WordIndex> aux_s;
  if(s.size()>=2)
  {
    unsigned int i=0;
    while(i<s.size() && s[i]==this->getBosId(found))
    {
      ++i;
    }
    if(i>0) --i;
    for(;i<s.size();++i)
      aux_s.push_back(s[i]);
  }
  else aux_s=s;

      // Calculate interpolated probability
  Prob p=pTrgGivenSrcRec(aux_s,t);
  return p;
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
Prob _incrJelMerNgramLM<SRC_INFO,SRCTRG_INFO>::pTrgGivenSrcRec(const Vector<WordIndex>& s,
                                                               const WordIndex& t)
{
  if(s.size()==0)
  {
    double weight=getWeight(s,t);
    double zerogramprob=(double)1.0/(double)this->getVocabSize();

    return (weight * (double) this->tablePtr->pTrgGivenSrc(s,t))+((1-weight) * zerogramprob);  
  }
  else
  {
    Vector<WordIndex> s_shifted;
    if(s.size()>1)
    {
      for(unsigned int i=1;i<s.size();++i)
      {
        s_shifted.push_back(s[i]);
      }
    }
    double weight=getWeight(s,t);
    return weight * (double) this->tablePtr->pTrgGivenSrc(s,t)+ (1-weight) * (double) pTrgGivenSrcRec(s_shifted,t);
  }
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
double _incrJelMerNgramLM<SRC_INFO,SRCTRG_INFO>::getWeight(const Vector<WordIndex>& s,
                                                           const WordIndex& /*t*/)
{
  if(numBucketsPerOrder==1)
  {
//    cerr<<numBucketsPerOrder<<endl;
    return weights[s.size()];
  }
  else
  {
        // Init variables
    unsigned int order=s.size()+1;
    double c=freqOfNgram(s);
    unsigned int bucketIdx=(unsigned int) trunc(c/sizeOfBucket);
    if(bucketIdx>numBucketsPerOrder-1)
      bucketIdx=numBucketsPerOrder-1;

//    cerr<<numBucketsPerOrder<<" "<<sizeOfBucket<<" "<<c<<" "<<order<<" "<<bucketIdx<<" "<<((order-1)*numBucketsPerOrder)+bucketIdx<<" "<<weights.size()<<endl;
    
        // Return weight
    return weights[((order-1)*numBucketsPerOrder)+bucketIdx];
  }
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
double _incrJelMerNgramLM<SRC_INFO,SRCTRG_INFO>::freqOfNgram(const Vector<WordIndex>& s)
{
  return (double)this->tablePtr->cSrc(s);
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
bool _incrJelMerNgramLM<SRC_INFO,SRCTRG_INFO>::load(const char *fileName)
{
  bool retval;

      // load weights
  std::string fileNameW=fileName;
  fileNameW=fileNameW+".weights";
  retval=loadWeights(fileNameW.c_str());
  if(retval==ERROR) return ERROR;

      // load n-grams
  retval=_incrNgramLM<SRC_INFO,SRCTRG_INFO>::load(fileName);
  if(retval==ERROR) return ERROR;

  return OK;
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
bool _incrJelMerNgramLM<SRC_INFO,SRCTRG_INFO>::loadWeights(const char *fileName)
{
      // load weights
  awkInputStream awk;
  weights.clear();
  if(awk.open(fileName)==ERROR)
  {
    cerr<<"Error, file with weights "<<fileName<<" cannot be read"<<endl;
    return ERROR;
  }  
  else
  {
    cerr<<"Loading weights from "<<fileName<<endl;
    if(awk.getline())
    {
      this->ngramOrder=atoi(awk.dollar(1).c_str());
      numBucketsPerOrder=atoi(awk.dollar(2).c_str());
      sizeOfBucket=(double)atof(awk.dollar(3).c_str());
      for(unsigned int i=4;i<=awk.NF;++i)
      {
        weights.push_back((double)atof(awk.dollar(i).c_str()));
      }
      awk.close();
      return OK;
    }
    else
    {
      cerr<<"Error while loading file with weights: "<<fileName<<endl;
      awk.close();
      return ERROR;
    }
  }
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
bool _incrJelMerNgramLM<SRC_INFO,SRCTRG_INFO>::print(const char *fileName)
{
  bool retval;
  
      // Print weights
  std::string fileNameW=fileName;
  fileNameW=fileNameW+".weights";
  retval=printWeights(fileNameW.c_str());
  if(retval==ERROR) return ERROR;

      // print n-grams
  retval=_incrNgramLM<SRC_INFO,SRCTRG_INFO>::print(fileName);
  if(retval==ERROR) return ERROR;

  return OK;
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
bool _incrJelMerNgramLM<SRC_INFO,SRCTRG_INFO>::printWeights(const char *fileName)
{
  FILE *filePtr;

      // print weights
  filePtr=fopen(fileName,"w");
  if(filePtr==NULL)
  {
    cerr<<"Error while printing file with lm weights"<<endl;
    return ERROR;
  }

  fprintf(filePtr,"%d ",this->getNgramOrder());  
  fprintf(filePtr,"%d ",numBucketsPerOrder);
  fprintf(filePtr,"%f ",sizeOfBucket);
  for(unsigned int i=0;i<weights.size();++i)
  {
    fprintf(filePtr,"%f ",weights[i]);
  }
  fprintf(filePtr,"\n");
  fclose(filePtr);

  return OK;
}  

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
_incrJelMerNgramLM<SRC_INFO,SRCTRG_INFO>::~_incrJelMerNgramLM()
{
  
}

#endif
