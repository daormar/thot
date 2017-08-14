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
/* Module: thot_gen_phr_model                                       */
/*                                                                  */
/* Definitions file: thot_gen_phr_model.cc                          */
/*                                                                  */
/* Description: Main functionality of the Thot toolkit.             */
/*                                                                  */   
/********************************************************************/


//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "PhraseExtractUtils.h"
#include "IncrPhraseModel.h"
#include "WbaIncrPhraseModel.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <math.h>
#include "options.h"
#include "ctimer.h"

using namespace std;

//--------------- Constants ------------------------------------------


//--------------- Type definitions -----------------------------------

struct thot_gen_phr_model_pars
{
  std::string aligFileName;
  std::string outputFilesPrefix;
  PhraseExtractParameters phePars;
  bool BRF;
  int verbose;
};

//--------------- Global variables -----------------------------------


//--------------- Function Definitions -------------------------------

int genPhrModel(thot_gen_phr_model_pars pars);
int genPhrModelBasedOnAligns(thot_gen_phr_model_pars pars,
                             _incrPhraseModel* _incrPhraseModelPtr);
void extendModelFromAlignments(PhraseExtractParameters phePars,
                               bool BRF,
                               _incrPhraseModel* _incrPhraseModelPtr,
                               AlignmentExtractor& alignmentExtractor,
                               int verbose=0);
void extendModelFromPairPlusAlig(PhraseExtractParameters phePars,
                                 _incrPhraseModel* _incrPhraseModelPtr,
                                 Vector<std::string>& ns,
                                 Vector<std::string>& t,
                                 WordAligMatrix& waMatrix,
                                 float numReps,
                                 int verbose=0);
void extendModelFromPairPlusAligBrf(PhraseExtractParameters phePars,
                                    _incrPhraseModel* _incrPhraseModelPtr,
                                    Vector<std::string>& ns,
                                    Vector<std::string>& t,
                                    WordAligMatrix& waMatrix,
                                    float numReps,
                                    int verbose=0);
void printUsage(void);
void version(void);
int takeParameters(int argc,
                   char *argv[],
                   thot_gen_phr_model_pars& pars);
void printDesc(void);

//---------------
int main(int argc,char *argv[])
{
  thot_gen_phr_model_pars pars;
  if(takeParameters(argc,argv,pars)==0)
  {
    return genPhrModel(pars);
  }
  else return THOT_ERROR;
}

//---------------
int genPhrModel(thot_gen_phr_model_pars pars)
{	 
      // create model pointer
  _incrPhraseModel* _incrPhraseModelPtr=new IncrPhraseModel;

      // generate phrase model given a GIZA alignment file
  int ret=genPhrModelBasedOnAligns(pars,_incrPhraseModelPtr);
  if(ret==THOT_ERROR)
  {
    delete _incrPhraseModelPtr;
    return THOT_ERROR;
  }
  
       // print model
   std::string outFileName=pars.outputFilesPrefix;
   outFileName+=".ttable";
       // output in thot native format
   _incrPhraseModelPtr->printTTable(outFileName.c_str());
   
       // print segmentation length table
   if(pars.BRF==1)
   {
     std::string segmLengthTableFileName=pars.outputFilesPrefix;
     segmLengthTableFileName+=".seglentable";
     _incrPhraseModelPtr->printSegmLengthTable(segmLengthTableFileName.c_str());
   }

       // Delete model pointer
   delete _incrPhraseModelPtr;
   
   return THOT_OK;
}

//---------------
int genPhrModelBasedOnAligns(thot_gen_phr_model_pars pars,
                             _incrPhraseModel* _incrPhraseModelPtr)
{
      // Initialize alignment extractor
  AlignmentExtractor alignmentExtractor;
  int ret=alignmentExtractor.open(pars.aligFileName.c_str(),GIZA_ALIG_FILE_FORMAT);
  if(ret==THOT_ERROR) 
  {
    cerr<<"Error while reading alignment file."<<endl;
    return THOT_ERROR;
  }
      // Extend phrase model using the alignments provided by the
      // extractor
  extendModelFromAlignments(pars.phePars,pars.BRF,_incrPhraseModelPtr,alignmentExtractor,pars.verbose);
  
  alignmentExtractor.close();
  
  return THOT_OK;  
}

//---------------
void extendModelFromAlignments(PhraseExtractParameters phePars,
                               bool BRF,
                               _incrPhraseModel* _incrPhraseModelPtr,
                               AlignmentExtractor& alignmentExtractor,
                               int verbose/*=0*/)
{
      // Iterate over alignments
  int numSent=0;	
  while(alignmentExtractor.getNextAlignment())
  {
    ++numSent;
    if((numSent%10)==0 && BRF)
      cerr<<"Processing sent. pair #"<<numSent<<"..."<<endl;

        // Obtain alignment information
    Vector<string> t=alignmentExtractor.get_t();
    Vector<string> ns=alignmentExtractor.get_ns();	
    WordAligMatrix waMatrix=alignmentExtractor.get_wamatrix();
    float numReps=alignmentExtractor.get_numReps();

    if(t.size()<MAX_SENTENCE_LENGTH && ns.size()-1<MAX_SENTENCE_LENGTH)
    {
      if(verbose)
      {
        cerr<<"* Processing sent. pair "<<numSent<<" (t length: "<< t.size()<<" , s length: "<< ns.size()-1<<" , numReps: "<<numReps<<")";
        cerr<<endl;
      }
          // Extend model from individual alignment
      if(BRF)
        extendModelFromPairPlusAligBrf(phePars,_incrPhraseModelPtr,ns,t,waMatrix,numReps,verbose);
      else
        extendModelFromPairPlusAlig(phePars,_incrPhraseModelPtr,ns,t,waMatrix,numReps,verbose);
    }
    else
      cerr<< "  Warning: Max. sentence length exceeded for sentence pair "<<numSent<<endl;
  }
}

//---------------
void extendModelFromPairPlusAlig(PhraseExtractParameters phePars,
                                 _incrPhraseModel* _incrPhraseModelPtr,
                                 Vector<std::string>& ns,
                                 Vector<std::string>& t,
                                 WordAligMatrix& waMatrix,
                                 float numReps,
                                 int verbose/*=0*/)
{
      // Extract phrase using BRF estimation
  Vector<PhrasePair> vecPhPair;
  PhraseExtractUtils::extractPhrasesFromPairPlusAlig(phePars,ns,t,waMatrix,vecPhPair,verbose);

      // Store phrases in model
  for(unsigned int i=0;i<vecPhPair.size();++i)
  {
    _incrPhraseModelPtr->strIncrCountsOfEntry(vecPhPair[i].s_,vecPhPair[i].t_,numReps*vecPhPair[i].weight);
  }
}

//---------------
void extendModelFromPairPlusAligBrf(PhraseExtractParameters phePars,
                                    _incrPhraseModel* _incrPhraseModelPtr,
                                    Vector<std::string>& ns,
                                    Vector<std::string>& t,
                                    WordAligMatrix& waMatrix,
                                    float numReps,
                                    int verbose/*=0*/)
{
      // Extract phrase using BRF estimation
  Vector<PhrasePair> vecPhPair;
  PhraseExtractUtils::extractPhrasesFromPairPlusAligBrf(phePars,ns,t,waMatrix,vecPhPair,verbose);

      // Store phrases in model
  for(unsigned int x=0;x<vecPhPair.size();++x)
  {
    _incrPhraseModelPtr->strIncrCountsOfEntry(vecPhPair[x].s_,vecPhPair[x].t_,numReps*vecPhPair[x].weight);
  }
}

//---------------
int takeParameters(int argc,
                   char *argv[],
                   thot_gen_phr_model_pars& pars)
{
 int err;

 if(argc==1)
 {
   printDesc();
   return THOT_ERROR;   
 }

 /* Verify --help option */
 err=readOption(argc,argv, "--help");
 if(err!=-1)
 {
   printUsage();
   return THOT_ERROR;
 }

 /* Verify --version option */
 err=readOption(argc,argv, "--version");
 if(err!=-1)
 {
   version();
   return THOT_ERROR;
 }

 /* Take the .A3.final file name */
 err=readSTLstring(argc,argv, "-g", &pars.aligFileName);
 if(err==-1)
 {
   printUsage();
   return THOT_ERROR;
 }
  
 /* Take the output files prefix */
 err=readSTLstring(argc,argv, "-o", &pars.outputFilesPrefix);
 if(err==-1)
 {
   printUsage();
   return THOT_ERROR;
 }
   
 /* Take the maximum source phrase length */
 err=readInt(argc,argv, "-m", &pars.phePars.maxTrgPhraseLength);
 if(err==-1)
 {
   pars.phePars.maxTrgPhraseLength=MAX_SENTENCE_LENGTH;
 }
    
 /* Verify monotone-model option */
 err=readOption(argc,argv, "-mon");
 pars.phePars.monotone=1;
 if(err==-1)
 {
   pars.phePars.monotone=0;
 }
  
  /* Verify brf option */
 err=readOption(argc,argv, "-brf");
 pars.BRF=1;
 if(err==-1)
 {
   pars.BRF=0;
 }
      
 /* Verify verbose option */
 pars.verbose=0;
   
 err=readOption(argc,argv, "-v");
 if(err==0) pars.verbose=1;
    
 err=readOption(argc,argv, "-v1");
 if(err==0) pars.verbose=2; 
 	 
 return THOT_OK;  
}

//---------------
void printDesc(void)
{
  cerr<<"thot_gen_phr_model written by Daniel Ortiz\n";
  cerr<<"thot_gen_phr_model trains phrase-based models\n";
  cerr<<"type \"thot_gen_phr_model --help\" to get usage information.\n";
}

//---------------
void printUsage(void)
{
 cerr<<"Usage: thot_gen_phr_model -g <string> [-m <int>] [-mon]\n";
 cerr<<"                          [-brf] -o <string> [-p]\n";
 cerr<<"                          [-v | -v1] [--help] [--version]\n\n";
 cerr<<"-g <string>               Name of the alignment file in GIZA format for\n";
 cerr<<"                          generating a phrase model.\n\n"; 
 cerr<<"-m <int>                  Set maximum target phrase length (target is the\n";
 cerr<<"                          target language of the GIZA alignment file).\n\n";
 cerr<<"-mon                      Generate monotone model.\n\n";
 cerr<<"-brf                      Obtain bisegmentation-based RF model (RF by\n";
 cerr<<"                          default).\n\n";
 cerr<<"-o <string>               Set output files prefix name.\n\n";
 cerr<<"-v | -v1                  Verbose mode | more verbosity\n\n";
 cerr<<"--help                    Display this help and exit\n\n";
 cerr<<"--version                 Output version information and exit\n\n";
 
}

//---------------
void version(void)
{
  cerr<<"thot_gen_phr_model is part of the Thot toolkit\n";
  cerr<<"Thot version "<<THOT_VERSION<<endl;
  cerr<<"Thot is GNU software written by Daniel Ortiz\n";
}
