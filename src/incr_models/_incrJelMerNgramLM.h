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
/*              Jelinek-Mercer n-gram language                      */
/*              models p(x|Vector<x>).                              */
/*                                                                  */
/********************************************************************/

#ifndef __incrJelMerNgramLM
#define __incrJelMerNgramLM

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

extern "C" {
#include "step_by_step_dhs.h"
}

#include "IncrNgramLM.h"
#include <stdio.h>

//--------------- Constants ------------------------------------------

#define DHS_LM_FTOL       0.01
#define DHS_LM_SCALE_PAR  1

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

      // Functions to update model weights
  virtual int updateModelWeights(const char *corpusFileName,
                                 int verbose=0);

      // Functions to load and print the model (including model weights)
  bool load(const char *fileName);
  bool print(const char *fileName);

      // Functions to load and print model weights
  bool loadWeights(const char *prefixOfLmFiles);
  bool printWeights(const char *prefixOfLmFiles);

      // Destructor
  ~_incrJelMerNgramLM();
   
 protected:
  Vector<double> weights;
  unsigned int numBucketsPerOrder;
  double sizeOfBucket;

      // Downhill-simplex related functions
  int new_dhs_eval(const char *corpusFileName,
                   FILE* tmp_file,
                   double* x,
                   double& obj_func);

      // Weights related functions
  double getJelMerWeight(const Vector<WordIndex>& s,
                         const WordIndex& t);
  virtual double freqOfNgram(const Vector<WordIndex>& s);

      // Recursive function to interpolate models
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
    double weight=getJelMerWeight(s,t);
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
    double weight=getJelMerWeight(s,t);
    return weight * (double) this->tablePtr->pTrgGivenSrc(s,t)+ (1-weight) * (double) pTrgGivenSrcRec(s_shifted,t);
  }
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
int _incrJelMerNgramLM<SRC_INFO,SRCTRG_INFO>::updateModelWeights(const char *corpusFileName,
                                                                 int verbose/*=0*/)
{
      // Initialize downhill simplex input parameters
  Vector<double> initial_weights=weights;
  int ndim=initial_weights.size();
  double* start=(double*) malloc(ndim*sizeof(double));
  int nfunk=0;
  double* x=(double*) malloc(ndim*sizeof(double));
  double y;

      // Create temporary file
  FILE* tmp_file=tmpfile();
  
  if(tmp_file==0)
  {
    cerr<<"Error updating of Jelinek Mercer's language model weights, tmp file could not be created"<<endl;
    return ERROR;
  }
    
      // Execute downhill simplex algorithm
  int ret;
  bool end=false;
  while(!end)
  {
        // Set initial weights (each call to step_by_step_simplex starts
        // from the initial weights)
    for(unsigned int i=0;i<initial_weights.size();++i)
      start[i]=initial_weights[i];
    
        // Execute step by step simplex
    double curr_dhs_ftol=DBL_MAX;
    ret=step_by_step_simplex(start,ndim,DHS_LM_FTOL,DHS_LM_SCALE_PAR,NULL,tmp_file,&nfunk,&y,x,&curr_dhs_ftol,false);

    switch(ret)
    {
      case OK: end=true;
        break;
      case DSO_NMAX_ERROR: cerr<<"Error updating of Jelinek Mercer's language model weights, maximum number of iterations exceeded"<<endl;
        end=true;
        break;
      case DSO_EVAL_FUNC: // A new function evaluation is requested by downhill simplex
        double perp;
        int retEval=new_dhs_eval(corpusFileName,tmp_file,x,perp);
        if(retEval==ERROR)
        {
          end=true;
          break;
        }
            // Print verbose information
        if(verbose>=1)
        {
          cerr<<"niter= "<<nfunk<<" ; current ftol= "<<curr_dhs_ftol<<" (FTOL="<<DHS_LM_FTOL<<") ; ";
          cerr<<"weights=";
          for(unsigned int i=0;i<weights.size();++i)
            cerr<<" "<<weights[i];
          cerr<<" ; perp= "<<perp<<endl; 
        }
        break;
    }
  }
  
      // Set new weights if updating was successful
  if(ret==OK)
  {
    for(unsigned int i=0;i<weights.size();++i)
      weights[i]=start[i];
  }
  else
  {
    weights=initial_weights;
  }
  
      // Clear variables
  free(start);
  free(x);
  fclose(tmp_file);

  if(ret!=OK)
    return ERROR;
  else
    return OK;
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
int _incrJelMerNgramLM<SRC_INFO,SRCTRG_INFO>::new_dhs_eval(const char *corpusFileName,
                                                           FILE* tmp_file,
                                                           double* x,
                                                           double& obj_func)
{
  unsigned int numOfSentences;
  unsigned int numWords;
  LgProb totalLogProb;
  bool weightsArePositive=true;
  bool weightsAreBelowOne=true;
  int retVal;
  
      // Fix weights to be evaluated
  for(unsigned int i=0;i<weights.size();++i)
  {
    weights[i]=x[i];
    if(weights[i]<0) weightsArePositive=false;
    if(weights[i]>=1) weightsAreBelowOne=false;
  }
  if(weightsArePositive && weightsAreBelowOne)
  {
        // Obtain perplexity
    retVal=this->perplexity(corpusFileName,numOfSentences,numWords,totalLogProb,obj_func);
  }
  else
  {
    obj_func=DBL_MAX;
    retVal=OK;
  }
      // Print result to tmp file
  fprintf(tmp_file,"%g\n",obj_func);
  fflush(tmp_file);
      // step_by_step_simplex needs that the file position
      // indicator is set at the start of the stream
  rewind(tmp_file);

  return retVal;
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
double _incrJelMerNgramLM<SRC_INFO,SRCTRG_INFO>::getJelMerWeight(const Vector<WordIndex>& s,
                                                                 const WordIndex& /*t*/)
{
  if(numBucketsPerOrder==1)
  {
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
  retval=loadWeights(fileName);
  if(retval==ERROR) return ERROR;

      // load n-grams
  retval=_incrNgramLM<SRC_INFO,SRCTRG_INFO>::load(fileName);
  if(retval==ERROR) return ERROR;

  return OK;
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
bool _incrJelMerNgramLM<SRC_INFO,SRCTRG_INFO>::loadWeights(const char *prefixOfLmFiles)
{
      // Obtain name of file with weights
  std::string weightFileName;
  std::string mainFileName;
  if(this->fileIsDescriptor(prefixOfLmFiles,mainFileName))
  {
        // File is descriptor
    std::string descFileName=prefixOfLmFiles;
    std::string absolutizedMainFileName=this->absolutizeDescrFileName(descFileName,mainFileName);
    weightFileName=absolutizedMainFileName+".weights";
  }
  else
  {
        // File is not descriptor
    weightFileName=prefixOfLmFiles;
    weightFileName=weightFileName+".weights";
  }

      // load weights
  awkInputStream awk;
  weights.clear();
  if(awk.open(weightFileName.c_str())==ERROR)
  {
    cerr<<"Error, file with weights "<<weightFileName<<" cannot be read"<<endl;
    return ERROR;
  }  
  else
  {
    cerr<<"Loading weights from "<<weightFileName<<endl;
    if(awk.getln())
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
      cerr<<"Error while loading file with weights: "<<weightFileName<<endl;
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
  retval=printWeights(fileName);
  if(retval==ERROR) return ERROR;

      // print n-grams
  retval=_incrNgramLM<SRC_INFO,SRCTRG_INFO>::print(fileName);
  if(retval==ERROR) return ERROR;

  return OK;
}

//---------------
template<class SRC_INFO,class SRCTRG_INFO>
bool _incrJelMerNgramLM<SRC_INFO,SRCTRG_INFO>::printWeights(const char *prefixOfLmFiles)
{
        // Obtain name of file with weights
  std::string weightFileName;
  std::string mainFileName;
  if(this->fileIsDescriptor(prefixOfLmFiles,mainFileName))
  {
        // File is descriptor
    std::string descFileName=prefixOfLmFiles;
    std::string absolutizedMainFileName=this->absolutizeDescrFileName(descFileName,mainFileName);
    weightFileName=absolutizedMainFileName+".weights";
  }
  else
  {
        // File is not descriptor
    weightFileName=prefixOfLmFiles;
    weightFileName=weightFileName+".weights";
  }
  
      // print weights
  FILE *filePtr=fopen(weightFileName.c_str(),"w");
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
