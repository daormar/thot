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

char phraseModelFilesPrefix[256];
char aligFileName[256];
char bestAligFileName[256];
char outputFilesPrefix[256];
char outBestAligFileName[256];
char srcInputVocabFileName[256];
char trgInputVocabFileName[256];
bool printVocabs;
bool printInverseTable;
PhraseExtractParameters phePars;
bool pseudoML;
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
  char invTableFileName[256];
  char outFileName[256];
  char segmLengthTableFileName[256];
  char logFileName[256];
  char cad[512];
  char phraseDictNameForLoad[256];
  char segmTableFileNameForLoad[256];
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

    if(bestAligFileName[0]!=0)
    {
      cerr<<"Warning: -best-al option ignored\n"<<endl;
    }    

    if(wbaIncrPhraseModel.generateWbaIncrPhraseModel(aligFileName,phePars,pseudoML,verbose))
      return ERROR;
	 
    // print model
    sprintf(outFileName,"%s.ttable",outputFilesPrefix); 
    if(outputFormat==PHARAOH_V1_OUTPUT)
    { // output in pharaoh v1 format
      wbaIncrPhraseModel.printTTablePharaoh_v1(outFileName);
    }
    else
    {
      if(outputFormat==THOT_COUNT_OUTPUT)
      {// output in thot native format
        wbaIncrPhraseModel.printTTable(outFileName);
      }
      else
      {
        if(outputFormat==MOSES_ID_OUTPUT)
        {
          wbaIncrPhraseModel.printTTableMoses_id(outFileName);
        }
        else
        {
              // output in moses format
          wbaIncrPhraseModel.printTTableMoses(outFileName);
        }
      }
    } 
   
	 // print inverse model
    if(outputFormat==PHARAOH_V1_OUTPUT)
    {
      sprintf(invTableFileName,"%s.ittable",outputFilesPrefix); 
      wbaIncrPhraseModel.printInverseTTable(invTableFileName);
    }
	
	 // print segmentation length table
    if(pseudoML==1)
    {
      sprintf(segmLengthTableFileName,"%s.seglentable",outputFilesPrefix); 
      wbaIncrPhraseModel.printSegmLengthTable(segmLengthTableFileName);
    }	 
  }
  else
  {// Load a phrase model file if the phrase model files prefix was given

    if(bestAligFileName[0]==0)
    {
      cerr<<"Error: -best-al option not given!\n"<<endl;
      return ERROR; 
    }    
    sprintf(phraseDictNameForLoad,"%s.ttable",phraseModelFilesPrefix); 
    if(wbaIncrPhraseModel.load_ttable(phraseDictNameForLoad)==0)
    {
          // Load file with segmentation size data if exists
      sprintf(segmTableFileNameForLoad,"%s.seglentable",phraseModelFilesPrefix);    
      wbaIncrPhraseModel.load_seglentable(segmTableFileNameForLoad);

	  // Generate best alignments for a GIZA alignment file
      if(bestAligFileName[0]!=0)
	  {
        sprintf(outBestAligFileName,"%s_best.A3.final",outputFilesPrefix); 
        wbaIncrPhraseModel.obtainBestAlignments(bestAligFileName,outBestAligFileName,phePars,verbose); 
	  }
    }
    else
    {
      cerr<<"Error while reading phrase model files!\n"<<endl;
      return ERROR;
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

  return OK;
 }
 else return ERROR;
}


//--------------- TakeParameters function

int TakeParameters(int argc,char *argv[])
{
 int err;
 float f;

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

 if(aligFileName[0]==0)
 {
       /* Take the phrase model files prefix */
  err=readString(argc,argv, "-l", phraseModelFilesPrefix);
  if(err==-1)
  {
    phraseModelFilesPrefix[0]=0;
  }
 }
 
 // Verify if -g or -l option was introduced
 if(aligFileName[0]==0 && phraseModelFilesPrefix[0]==0)
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
  
 /* Take the file for obtaining the best alignments */
 err=readString(argc,argv, "-best-al", bestAligFileName);
 if(err==-1)
 {
   bestAligFileName[0]=0;
 }
 
 /* Take the maximum source phrase length */
 err=readInt(argc,argv, "-m", &phePars.maxTrgPhraseLength);
 if(err==-1)
 {
   phePars.maxTrgPhraseLength=MAX_SENTENCE_LENGTH;
 }
 else
 {
       // Verify the -nospur option
   err=readOption(argc,argv, "-nospur");
   phePars.countSpurious=0;
   if(err==-1)
   {
     phePars.countSpurious=1;
   }
          // Verify the -ms option
   err=readOption(argc,argv, "-ms");
   phePars.constraintSrcLen=1;
   if(err==-1)
   {
     phePars.constraintSrcLen=0;
   }
 }
   
 /* Take the maximum number of combinations allocated in the segmentation table */
 err=readFloat(argc,argv, "-mc", &f);
 if(err==-1)
 {
   phePars.maxNumbOfCombsInTable=-1;
 }
 else
 {
   phePars.maxNumbOfCombsInTable=(int) (f*1000000);
 }
 
 /* Verify monotone-model option */
 err=readOption(argc,argv, "-mon");
 phePars.monotone=1;
 if(err==-1)
 {
   phePars.monotone=0;
 }
  
  /* Verify pml option */
 err=readOption(argc,argv, "-pml");
 pseudoML=1;
 if(err==-1)
 {
   pseudoML=0;
 }
   
 /* Verify print vocabularies option */
 err=readOption(argc,argv, "-p");
 printVocabs=1;
 if(err==-1)
 {
   printVocabs=0;
 }
 
 /* Verify output format options */
 outputFormat=MOSES_OUTPUT;

 err=readOption(argc,argv, "-pc");
 if(err==0)
 {
   outputFormat=THOT_COUNT_OUTPUT;
 }
 
 err=readOption(argc,argv, "-ph1");
 if(err==0)
 {
   outputFormat=PHARAOH_V1_OUTPUT;
 }

 err=readOption(argc,argv, "-pid");
 if(err==0)
 {
   outputFormat=MOSES_ID_OUTPUT;
 }

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
 
 /* Verify -va option */
 err=readOption(argc,argv,"-va");
 if(err==0) verbose=VERBOSE_AACHEN; 
	 
 return OK;  
}

//--------------- printDesc() function
void printDesc(void)
{
  cerr<<"thot_gen_phr_model written by Daniel Ortiz\n";
  cerr<<"thot_gen_phr_model implements the kernel of Thot\n";
  cerr<<"type \"thot_gen_phr_model --help\" to get usage information.\n";
}

//--------------------------------
void printUsage(void)
{
 cerr<<"Usage: thot_gen_phr_model {-g <string> [-pml] [-mon] [-pid | -pc | -ph1] [-p]\n";
 cerr<<"                |-l <string> -best-al <string>}\n"; 
 cerr<<"                [-s <string> -t <string>]\n";
 cerr<<"                [-m <int> [-nospur] [-ms]]\n";
#ifdef THOT_DISABLE_ITERATORS
 cerr<<"                [-mc <int>]\n";
#endif
 cerr<<"                {-o <string>} [-v | -v1 | -va]\n";
 cerr<<"                [--help] [--version]\n\n";
 cerr<<"-g <string>                     Name of the alignment file in GIZA format for\n";
 cerr<<"                                generating a phrase model.\n\n"; 
 cerr<<"                                format of the files is the one obtained\n";
 cerr<<"                                with -pc option.\n\n";
 cerr<<"-pml                            Obtain pseudo-ml model (RF by default).\n\n";
 cerr<<"-mon                            Obtain monotone model (non-monotone by default).\n\n";
 cerr<<"-pid                            Print direct and inverse models in the same \n";
 cerr<<"                                table.\n\n";
 cerr<<"-ph1                            Print output in pharaoh v1.0 format\n";
 cerr<<"                                (moses by default)\n\n";
 cerr<<"-pc                             Print output in Thot native format, which\n";
 cerr<<"                                includes source and joint counts.\n\n";
 cerr<<"-p                              Print vocabularies.\n\n";
 cerr<<"-l <string>                     Phrase-model file prefix for load. The expected\n";
 cerr<<"                                format of the files is the one obtained\n";
 cerr<<"                                with -pc option.\n\n";
 cerr<<"-best-al <string>               GIZA++ .A3 file for obtain the best alignments\n";
 cerr<<"                                using the phrase model.\n\n";	
 cerr<<"-m <int>                        Set maximum target phrase length (target is the\n";
 cerr<<"                                target language of the GIZA alignment file).\n\n";
 cerr<<"-nospur                         Do not take into account the spurious words when\n";
 cerr<<"                                calculating the source phrase length (source is\n";
 cerr<<"                                the source language of the GIZA alignment file).\n\n";
 cerr<<"-ms                             Also impose the maximum phrase-length\n";
 cerr<<"                                constraint to the source phrase (source is the\n";
 cerr<<"                                source language of the GIZA alignment file).\n\n";
#ifdef THOT_DISABLE_ITERATORS
 cerr<<"-mc <int>                       During a pseudo-ml estimation process, this\n";
 cerr<<"                                parameter sets the maximum number of segmentations\n";
 cerr<<"                                (in millions) that can be allocated in the\n";
 cerr<<"                                segmentation table (20 by default).\n\n";
#endif
 cerr<<"-s <string>\n";
 cerr<<"-t <string>                     Source and target input vocabulary files.\n\n";	
 cerr<<"-o <string>                     Set output files prefix name.\n\n";
 cerr<<"-v | -v1 | -va                  Verbose mode | more verbosity | verbose mode in\n";
 cerr<<"                                Aachen alignment format\n\n";
 cerr<<"--help                          Display this help and exit\n\n";
 cerr<<"--version                       Output version information and exit\n\n";
 
}

//--------------- version function

void version(void)
{
  cerr<<"thot_gen_phr_model is part of the Thot toolkit\n";
  cerr<<"Thot version "<<THOT_VERSION<<endl;
  cerr<<"Thot is GNU software written by Daniel Ortiz\n";
}
