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

#include <math.h>
#include "WbaIncrPhraseModel.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include "options.h"
#include "ctimer.h"

using namespace std;

//--------------- Constants ------------------------------------------


//--------------- Type definitions -----------------------------------


//--------------- Global variables -----------------------------------

char aligFileName[256];
char outputFilesPrefix[256];
char outBestAligFileName[256];
char srcInputVocabFileName[256];
char trgInputVocabFileName[256];
bool printVocabs;
bool printInverseTable;
PhraseExtractParameters phePars;
bool BRF;
int outputFormat;
int verbose;

//--------------- Function Definitions -------------------------------

void printUsage(void);
void version(void);
int TakeParameters(int argc,char *argv[]);
void printDesc(void);

//--------------- main function

int main(int argc,char *argv[])
{
  char srcVcbFileName[256];
  char trgVcbFileName[256];
  char outFileName[256];
  char segmLengthTableFileName[256];
  char logFileName[256];
  char cad[512];
  WbaIncrPhraseModel wbaIncrPhraseModel;
  
 if(TakeParameters(argc,argv)==0)
 { // Create log file
   sprintf(logFileName,"%s.log",outputFilesPrefix); 	 
   wbaIncrPhraseModel.createLogFile(logFileName);
	
   // Load vocabularies
   if(srcInputVocabFileName[0]!=0)
   {
     if(wbaIncrPhraseModel.loadSrcVocab(srcInputVocabFileName)) 
     {
       cerr<<"Error while reading source vocabulary!\n"<<endl;
       return ERROR;
     }
     else
     {
       sprintf(cad,"Read source vocabulary from file %s\n",srcInputVocabFileName);
       cerr<<cad;
       wbaIncrPhraseModel.addToLogFile(cad);
     }
     if(wbaIncrPhraseModel.loadTrgVocab(trgInputVocabFileName)) 
     {
       cerr<<"Error while reading target vocabulary!\n"<<endl;
       return ERROR;
     }
     else
     {
       sprintf(cad,"Read target vocabulary from file %s\n",trgInputVocabFileName);
       cerr<<cad;
       wbaIncrPhraseModel.addToLogFile(cad);
     }
   }
 
  if(aligFileName[0]!=0)	 
  { // generate phrase model given a GIZA alignment file

    if(wbaIncrPhraseModel.generateWbaIncrPhraseModel(aligFileName,phePars,BRF,verbose))
      return ERROR;
	 
    // print model
    sprintf(outFileName,"%s.ttable",outputFilesPrefix); 
    // output in thot native format
    wbaIncrPhraseModel.printTTable(outFileName);
   	
    // print segmentation length table
    if(BRF==1)
    {
      sprintf(segmLengthTableFileName,"%s.seglentable",outputFilesPrefix); 
      wbaIncrPhraseModel.printSegmLengthTable(segmLengthTableFileName);
    }	 
  }

  // print vocabularies
  if(printVocabs)
  {
    sprintf(srcVcbFileName,"%s.src.vcb",outputFilesPrefix); 
    wbaIncrPhraseModel.printSrcVocab(srcVcbFileName);
       	   
    sprintf(trgVcbFileName,"%s.trg.vcb",outputFilesPrefix); 
    wbaIncrPhraseModel.printTrgVocab(trgVcbFileName);
  }
  wbaIncrPhraseModel.closeLogFile();

  return THOT_OK;
 }
 else return ERROR;
}


//--------------- TakeParameters function

int TakeParameters(int argc,char *argv[])
{
 int err;

 if(argc==1)
 {
   printDesc();
   return ERROR;   
 }

 /* Verify --help option */
 err=readOption(argc,argv, "--help");
 if(err!=-1)
 {
   printUsage();
   return ERROR;
 }

 /* Verify --version option */
 err=readOption(argc,argv, "--version");
 if(err!=-1)
 {
   version();
   return ERROR;
 }

 /* Take the .A3.final file name */
 err=readString(argc,argv, "-g", aligFileName);
 if(err==-1)
 {
   aligFileName[0]=0;
 }
 
 // Verify if -g or -l option was introduced
 if(aligFileName[0]==0)
 {
   printUsage();
   return ERROR;
 }
 
 /* Take the output files prefix */
 err=readString(argc,argv, "-o", outputFilesPrefix);
 if(err==-1)
 {
   printUsage();
   return ERROR;
 }
   
 /* Take the maximum source phrase length */
 err=readInt(argc,argv, "-m", &phePars.maxTrgPhraseLength);
 if(err==-1)
 {
   phePars.maxTrgPhraseLength=MAX_SENTENCE_LENGTH;
 }
    
 /* Verify monotone-model option */
 err=readOption(argc,argv, "-mon");
 phePars.monotone=1;
 if(err==-1)
 {
   phePars.monotone=0;
 }
  
  /* Verify brf option */
 err=readOption(argc,argv, "-brf");
 BRF=1;
 if(err==-1)
 {
   BRF=0;
 }
   
 /* Verify print vocabularies option */
 err=readOption(argc,argv, "-p");
 printVocabs=1;
 if(err==-1)
 {
   printVocabs=0;
 }
 
 /* Verify output format options */
 outputFormat=THOT_COUNT_OUTPUT;
 
 /* Take the input source vocabulary file */
 err=readString(argc,argv, "-s", srcInputVocabFileName);
 if(err==-1)
 {
   srcInputVocabFileName[0]=0; 
 }
   
 /* Take the input target vocabulary file */
 err=readString(argc,argv, "-t", trgInputVocabFileName);
 if(err==-1)
 {
   trgInputVocabFileName[0]=0;
 }  
   
 /* Verify verbose option */
 verbose=0;
   
 err=readOption(argc,argv, "-v");
 if(err==0) verbose=1;
    
 err=readOption(argc,argv, "-v1");
 if(err==0) verbose=2; 
 	 
 return THOT_OK;  
}

//--------------- printDesc() function
void printDesc(void)
{
  cerr<<"thot_gen_phr_model written by Daniel Ortiz\n";
  cerr<<"thot_gen_phr_model trains phrase-based models\n";
  cerr<<"type \"thot_gen_phr_model --help\" to get usage information.\n";
}

//--------------------------------
void printUsage(void)
{
 cerr<<"Usage: thot_gen_phr_model -g <string> [-s <string> -t <string>]\n";
 cerr<<"                          [-m <int>] [-brf] -o <string> [-p]\n";
 cerr<<"                          [-v | -v1] [--help] [--version]\n\n";
 cerr<<"-g <string>               Name of the alignment file in GIZA format for\n";
 cerr<<"                          generating a phrase model.\n\n"; 
 cerr<<"-m <int>                  Set maximum target phrase length (target is the\n";
 cerr<<"                          target language of the GIZA alignment file).\n\n";
 cerr<<"-s <string>               Source vocabulary file.\n\n";	
 cerr<<"-t <string>               Target vocabulary file.\n\n";	
 cerr<<"-brf                      Obtain bisegmentation-based RF model (RF by\n";
 cerr<<"                          default).\n\n";
 cerr<<"-o <string>               Set output files prefix name.\n\n";
 cerr<<"-p                        Print vocabularies.\n\n";
 cerr<<"-v | -v1                  Verbose mode | more verbosity\n\n";
 cerr<<"--help                    Display this help and exit\n\n";
 cerr<<"--version                 Output version information and exit\n\n";
 
}

//--------------- version function

void version(void)
{
  cerr<<"thot_gen_phr_model is part of the Thot toolkit\n";
  cerr<<"Thot version "<<THOT_VERSION<<endl;
  cerr<<"Thot is GNU software written by Daniel Ortiz\n";
}
