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
/* Module: thot_lm_perp.cc                                          */
/*                                                                  */
/* Definitions file: thot_lm_perp.cc                                */
/*                                                                  */
/* Description: Calculates the perplexity of a                      */
/*              language model given a test corpus containing       */
/*              one sentence per line.                              */
/*                                                                  */   
/********************************************************************/


//--------------- Include files --------------------------------------

#include "LM_Defs.h"                                                    
  // NOTE: this file should be included first, since it defines the
  // _FILE_OFFSET_BITS constant. This constant has to be defined
  // before including any STL header files to avoid conflicts.

#include "BaseNgramLM.h"
#include "WordIndex.h"
#include "DynClassFileHandler.h"
#include "SimpleDynClassLoader.h"
#include "ctimer.h"
#include "options.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <map>
#include <math.h>

//--------------- Constants ------------------------------------------


//--------------- Function Declarations ------------------------------

int init_lm(int verbosity);
void release_lm(int verbosity);
int TakeParameters(int argc,char *argv[]);
void printUsage(void);

//--------------- Type definitions -----------------------------------


//--------------- Global variables -----------------------------------

std::string lmFileName;
std::string corpusFileName;
int verbose=0;
unsigned int order;
SimpleDynClassLoader<BaseNgramLM<std::vector<WordIndex> > > baseNgramLMDynClassLoader;
BaseNgramLM<std::vector<WordIndex> >* lm;

//--------------- Function Definitions -------------------------------

//---------------
int main(int argc,char *argv[])
{
  LgProb total_logp=0;	
  std::string s;
  std::vector<std::string> v;	
  unsigned int sentenceNo=0,numWords=0;
  double perp;
  double total_time=0,elapsed_ant,elapsed,ucpu,scpu;

  if(TakeParameters(argc,argv)==THOT_OK)
  {
    if(init_lm(true)==THOT_ERROR)
      return THOT_ERROR;
    
        // Load language model
    if(lm->load(lmFileName.c_str())==THOT_ERROR)
    {
      std::cerr<<"Error while loading language model"<<std::endl;
      release_lm(true);
      return THOT_ERROR;
    }
    else
    {
      lm->setNgramOrder(order);
      
      ctimer(&elapsed_ant,&ucpu,&scpu);
      int ret=lm->perplexity(corpusFileName.c_str(),sentenceNo,numWords,total_logp,perp,verbose);
      if(ret==THOT_ERROR)
      {
        release_lm(true);
        return THOT_ERROR;
      }
        
      ctimer(&elapsed,&ucpu,&scpu);  

      total_time+=elapsed-elapsed_ant;

          // Print results
      std::cout<<"* Number of sentences: "<<sentenceNo<<std::endl;
      std::cout<<"* Number of words: "<<numWords<<std::endl;	  
      std::cout<<"* Total log10 prob: "<<total_logp<<std::endl;
      std::cout<<"* Average-Log10-Likelihood (total_log10_prob/num_ngrams): "<<(float)total_logp/(numWords+sentenceNo)<<std::endl;
      std::cout<<"* Perplexity: "<<perp<<std::endl;
      std::cout<<"* Retrieving time: "<<total_time<<std::endl; 	   

      release_lm(true);
   
      return THOT_OK;
    }
  }
  else return THOT_ERROR;
}

//---------------
int init_lm(int verbosity)
{
      // Initialize dynamic class file handler
  DynClassFileHandler dynClassFileHandler;
  if(dynClassFileHandler.load(THOT_MASTER_INI_PATH,verbosity)==THOT_ERROR)
  {
    std::cerr<<"Error while loading ini file"<<std::endl;
    return THOT_ERROR;
  }
      // Define variables to obtain base class infomation
  std::string baseClassName;
  std::string soFileName;
  std::string initPars;

      ////////// Obtain info for BaseNgramLM class
  baseClassName="BaseNgramLM";
  if(dynClassFileHandler.getInfoForBaseClass(baseClassName,soFileName,initPars)==THOT_ERROR)
  {
    std::cerr<<"Error: ini file does not contain information about "<<baseClassName<<" class"<<std::endl;
    std::cerr<<"Please check content of master.ini file or execute \"thot_handle_ini_files -r\" to reset it"<<std::endl;
    return THOT_ERROR;
  }
   
      // Load class derived from BaseSwAligModel dynamically
  if(!baseNgramLMDynClassLoader.open_module(soFileName,verbosity))
  {
    std::cerr<<"Error: so file ("<<soFileName<<") could not be opened"<<std::endl;
    return THOT_ERROR;
  }

  lm=baseNgramLMDynClassLoader.make_obj(initPars);
  if(lm==NULL)
  {
    std::cerr<<"Error: BaseNgramLM pointer could not be instantiated"<<std::endl;
    baseNgramLMDynClassLoader.close_module();
    
    return THOT_ERROR;
  }

  return THOT_OK;
}

//---------------
void release_lm(int verbosity)
{
  delete lm;
  baseNgramLMDynClassLoader.close_module(verbosity);
}

//---------------
int TakeParameters(int argc,char *argv[])
{
  int err;
 
      /* Take the corpus file name */
 err=readSTLstring(argc,argv, "-c", &corpusFileName);
 if(err==-1 || argc<2)
 {
   printUsage();
   return THOT_ERROR;
 }

     /* Take the language model file name */
 err=readSTLstring(argc,argv, "-lm", &lmFileName);
 if(err==-1)
 {
   printUsage();
   return THOT_ERROR;
 }

      /* Take order of the n-grams */
 err=readUnsignedInt(argc,argv, "-n", &order);
 if(err==-1)
 {
   printUsage();
   return THOT_ERROR;
 }

     /* Check verbosity option */
 err=readOption(argc,argv, "-v");
 if(err!=-1)
 {
   verbose=1;
 }

 err=readOption(argc,argv, "-v1");
 if(err!=-1)
 {
   verbose=2;
 }

 return THOT_OK;  
}

//---------------
void printUsage(void)
{
 printf("Usage: thot_lm_perp -c <string> -lm <string> -n <int>\n");
 printf("                     [-v|-v1]\n");
 printf("-c <string>          Corpus file to be processed.\n\n"); 
 printf("-lm <string>         Language model file name.\n\n");
 printf("-n <int>             Order of the n-grams.\n\n");
 printf("-v|-v1               Verbose modes.\n\n");
}
