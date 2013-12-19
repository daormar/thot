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

#include "IncrJelMerNgramLM.h"
#include "ctimer.h"
#include "options.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <iomanip>

//--------------- Constants ------------------------------------------

#define INTERPO_LM        2

//--------------- Function Declarations ------------------------------

int TakeParameters(int argc,char *argv[]);
void printUsage(void);

//--------------- Type definitions -----------------------------------


//--------------- Global variables -----------------------------------

char lmFileName[256];
char corpusFileName[256];
char vocFileName[256];
char context[256];
int verbose=0;
int lmType=INTERPO_LM;
unsigned int order;

//--------------- Function Definitions -------------------------------

//--------------- main function
int main(int argc,char *argv[])
{
  LgProb total_logp=0;	
  std::string s;
  vector<string> v;	
  unsigned int sentenceNo=0,numWords=0;	
  double total_time=0,elapsed_ant,elapsed,ucpu,scpu;

  if(TakeParameters(argc,argv)==OK)
  {
    BaseIncrNgramLM<Vector<WordIndex> >* lm;

        // Load language model
    switch(lmType)
    {
      case INTERPO_LM: lm=new IncrJelMerNgramLM;
        break;
      default: lm=new IncrJelMerNgramLM;
        break;
    }

    if(lm->load(lmFileName)==ERROR)
    {
      cerr<<"Error while loading language model"<<endl;
      delete lm;
      return ERROR;
    }
    else
    {
      lm->setNgramOrder(order);
      
      ctimer(&elapsed_ant,&ucpu,&scpu);
      int ret=lm->perplexity(corpusFileName,sentenceNo,numWords,total_logp,verbose);
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
      cout<<"* Perplexity: "<<exp(-((double)total_logp/(numWords+sentenceNo))*M_LN10)<<endl;
      cout<<"* Retrieving time: "<<total_time<<endl; 	   

      delete lm;
   
      return OK;
    }
  }
  else return ERROR;
}

//--------------- TakeParameters function
int TakeParameters(int argc,char *argv[])
{
  int err;
 
      /* Take the corpus file name */
 err=readString(argc,argv, "-c", corpusFileName);
 if(err==-1 || argc<2)
 {
   printUsage();
   return ERROR;
 }

     /* Take the language model file name */
 err=readString(argc,argv, "-lm", lmFileName);
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

 //     /* Check -io option */
 // err=readOption(argc,argv, "-io");
 // if(err!=-1)
 // {
 //   lmType=INTERPO_LM;
 // }

 return OK;  
}

//--------------------------------
void printUsage(void)
{
 printf("Usage: thot_ilm_perp -c <string> -lm <string> -n <int> [-v|-v1]\n\n");
 printf("-c <string>          Corpus file to be processed.\n\n"); 
 printf("-lm <string>         Language model file name.\n\n");
 printf("-n <int>             Order of the n-grams.\n\n");
 // printf("-io                            Use interpolated n'grams of different orders.\n\n");
 printf("-v|-v1               Verbose modes.\n\n");
}

//--------------------------------
