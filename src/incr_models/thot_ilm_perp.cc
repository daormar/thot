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
/* Module: thot_ilm_perp.cc                                         */
/*                                                                  */
/* Definitions file: thot_ilm_perp.cc                               */
/*                                                                  */
/* Description: Calculates the perplexity of an incremental         */
/*              language model given a test corpus containing       */
/*              one sentence per line.                              */
/*                                                                  */   
/********************************************************************/


//--------------- Include files --------------------------------------

#include "LM_Defs.h"                                                    
  // NOTE: this file should be included first, since it defines the
  // _FILE_OFFSET_BITS constant. This constant has to be defined
  // before including any STL header files to avoid conflicts.

#include "IncrJelMerNgramLM.h"
#include "IncrInterpNgramLM.h"
#include "ctimer.h"
#include "options.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <iomanip>

//--------------- Constants ------------------------------------------

#define INTERP_LM         1
#define JEL_MER_LM        2
#define CACHE_JEL_MER_LM  3

//--------------- Function Declarations ------------------------------

int TakeParameters(int argc,char *argv[]);
void printUsage(void);

//--------------- Type definitions -----------------------------------


//--------------- Global variables -----------------------------------

std::string lmFileName;
std::string corpusFileName;
int verbose=0;
int lmType=JEL_MER_LM;
unsigned int order;

//--------------- Function Definitions -------------------------------

//--------------- main function
int main(int argc,char *argv[])
{
  LgProb total_logp=0;	
  std::string s;
  vector<string> v;	
  unsigned int sentenceNo=0,numWords=0;
  double perp;
  double total_time=0,elapsed_ant,elapsed,ucpu,scpu;

  if(TakeParameters(argc,argv)==THOT_OK)
  {
    BaseIncrNgramLM<Vector<WordIndex> >* lm;

        // Load language model
    switch(lmType)
    {
      case INTERP_LM: lm=new IncrInterpNgramLM;
        break;
      case JEL_MER_LM: lm=new IncrJelMerNgramLM;
        break;
      default: lm=new IncrJelMerNgramLM;
        break;
    }

    if(lm->load(lmFileName.c_str())==ERROR)
    {
      cerr<<"Error while loading language model"<<endl;
      delete lm;
      return ERROR;
    }
    else
    {
      lm->setNgramOrder(order);
      
      ctimer(&elapsed_ant,&ucpu,&scpu);
      int ret=lm->perplexity(corpusFileName.c_str(),sentenceNo,numWords,total_logp,perp,verbose);
      if(ret==ERROR)
      {
        delete lm;
        return ERROR;
      }
        
      ctimer(&elapsed,&ucpu,&scpu);  

      total_time+=elapsed-elapsed_ant;

          // Print results
      cout<<"* Number of sentences: "<<sentenceNo<<endl;
      cout<<"* Number of words: "<<numWords<<endl;	  
      cout<<"* Total log10 prob: "<<total_logp<<endl;
      cout<<"* Average-Log10-Likelihood (total_log10_prob/num_ngrams): "<<(float)total_logp/(numWords+sentenceNo)<<endl;
      cout<<"* Perplexity: "<<perp<<endl;
      cout<<"* Retrieving time: "<<total_time<<endl; 	   

      delete lm;
   
      return THOT_OK;
    }
  }
  else return ERROR;
}

//--------------- TakeParameters function
int TakeParameters(int argc,char *argv[])
{
  int err;
 
      /* Take the corpus file name */
 err=readSTLstring(argc,argv, "-c", &corpusFileName);
 if(err==-1 || argc<2)
 {
   printUsage();
   return ERROR;
 }

     /* Take the language model file name */
 err=readSTLstring(argc,argv, "-lm", &lmFileName);
 if(err==-1)
 {
   printUsage();
   return ERROR;
 }

      /* Take order of the n-grams */
 err=readUnsignedInt(argc,argv, "-n", &order);
 if(err==-1)
 {
   printUsage();
   return ERROR;
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

     /* Check -jm option */
 err=readOption(argc,argv, "-jm");
 if(err!=-1)
 {
   lmType=JEL_MER_LM;
 }

     /* Check -i option */
 err=readOption(argc,argv, "-i");
 if(err!=-1)
 {
   lmType=INTERP_LM;
 }

     /* Check -cjm option */
 err=readOption(argc,argv, "-cjm");
 if(err!=-1)
 {
   lmType=CACHE_JEL_MER_LM;
 }

 return THOT_OK;  
}

//--------------------------------
void printUsage(void)
{
 printf("Usage: thot_ilm_perp -c <string> -lm <string> -n <int>\n");
 printf("                     {-i | -jm | -cjm} \n");
 printf("                     [-v|-v1]\n");
 printf("-c <string>          Corpus file to be processed.\n\n"); 
 printf("-lm <string>         Language model file name.\n\n");
 printf("-n <int>             Order of the n-grams.\n\n");
 printf("-i                   Use interpolated model.\n\n");
 printf("-jm                  Use Jelinek-Mercer n-gram models.\n\n");
 printf("-cjm                 Use cache-based Jelinek-Mercer n-grams models.\n\n");
 printf("-v|-v1               Verbose modes.\n\n");
}

//--------------------------------
