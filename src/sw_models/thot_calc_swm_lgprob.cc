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
 
/*********************************************************************/
/*                                                                   */
/* Module: thot_calc_swm_lgprob.cc                                   */
/*                                                                   */
/* Definitions file: thot_calc_swm_lgprob.cc                         */
/*                                                                   */
/* Description: Calculates the log-prob of a sentence pair using a   */
/*              single word alignment model.                         */
/*                                                                   */   
/*********************************************************************/


//--------------- Include files ---------------------------------------

#include "IncrHmmAligModel.h"
#include "CachedHmmAligLgProb.h"
#include "BaseSwAligModel.h"
#include <WordAligMatrix.h>
#include <printAligFuncs.h>
#include "DynClassFileHandler.h"
#include "SimpleDynClassLoader.h"
#include <options.h>
#include <ctimer.h>
#include <StrProcUtils.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <math.h>

//--------------- Constants -------------------------------------------


//--------------- Function Declarations -------------------------------

int TakeParameters(int argc,char *argv[]);
void printUsage(void);
int init_swm(void);
void release_swm(void);
int processPairAligFile(BaseSwAligModel<std::vector<Prob> > *swAligModelPtr,
                        const char *pairPlusAligFile);
int processSentPairFile(BaseSwAligModel<std::vector<Prob> > *swAligModelPtr,
                        const char *sentPairFile);
void version(void);

//--------------- Type definitions ------------------------------------


//--------------- Global variables ------------------------------------

SimpleDynClassLoader<BaseSwAligModel<std::vector<Prob> > > baseSwAligModelDynClassLoader;
BaseSwAligModel<std::vector<Prob> >* swAligModelPtr;

char swFilePrefix[512];
char outputFilesPrefix[512];
char pairPlusAligFile[512];
char sentPairFile[512];
std::string srcSent;
std::string trgSent;
std::string alig;
int max_opt;
int alig_given;
int verbosity;

//--------------- Function Definitions --------------------------------


//---------------
int main(int argc,char *argv[])
{
 LgProb lp;
 bool ret;
 
 if(TakeParameters(argc,argv)==THOT_OK)
 {
       // Create model instance
  if(init_swm()==THOT_ERROR)
    return THOT_ERROR;

  if(pairPlusAligFile[0]==0 && sentPairFile[0]==0)
  {         
   std::cerr<<"s: "<<srcSent <<std::endl;
   std::cerr<<"t: "<<trgSent <<std::endl;   
   ret=swAligModelPtr->load(swFilePrefix);
   if(ret==THOT_ERROR)
   {
     release_swm();
     return THOT_ERROR;
   }
   if(alig_given) 
   {
         // Calculate log-prob given alignment
    std::vector<std::string> aligVec=StrProcUtils::stringToStringVector(alig);
    std::vector<PositionIndex> aligIndexVec;
    for(unsigned int i=0;i<alig.size();++i)
      aligIndexVec.push_back(atoi(aligVec[i].c_str()));
    WordAligMatrix waMatrix;
    waMatrix.putAligVec(aligIndexVec);
    
    std::cerr<<"a: "<<alig <<std::endl;
    lp=swAligModelPtr->calcLgProbForAligChar(srcSent.c_str(),
                                             trgSent.c_str(),
                                             waMatrix,
                                             verbosity); 
    cout<<"Single-word model logProbability= "<<lp<<std::endl;      
   }
   else
   {
         // Calculate log-prob without any alignment
     if(!max_opt)
     {
           // -max option was not given
       lp=swAligModelPtr->calcLgProbChar(srcSent.c_str(),
                                         trgSent.c_str(),
                                         verbosity);  
       cout<<"Single-word model logprob sum for each alignment= "<<lp<<std::endl;
     }
     else
     {
           // -max option was given
       WordAligMatrix waMatrix;
       
       lp=swAligModelPtr->obtainBestAlignmentChar(srcSent.c_str(),
                                                  trgSent.c_str(),
                                                  waMatrix);  
       cout<<"Single-word model logprob for the best alignment= "<<lp<<std::endl;
     }
   }
  }
  else
  {
    if(pairPlusAligFile[0]!=0)
    {
          // Process sentence pair + alignment file
      ret=processPairAligFile(swAligModelPtr,pairPlusAligFile);
      if(ret==THOT_ERROR)
      {
        release_swm();
        return THOT_ERROR;
      }
    }
    else
    {
          // Process sentence pair file
      ret=processSentPairFile(swAligModelPtr,sentPairFile);
      if(ret==THOT_ERROR)
      {
        release_swm();
        return THOT_ERROR;
      }
    }
  }

      // Release model instance
  release_swm();
 }
 
 return THOT_OK;
}

//---------------
int init_swm(void)
{
      // Initialize dynamic class file handler
  DynClassFileHandler dynClassFileHandler;
  if(dynClassFileHandler.load(THOT_MASTER_INI_PATH)==THOT_ERROR)
  {
    std::cerr<<"Error while loading ini file"<<std::endl;
    return THOT_ERROR;
  }
      // Define variables to obtain base class infomation
  std::string baseClassName;
  std::string soFileName;
  std::string initPars;

      ////////// Obtain info for BaseSwAligModel class
  baseClassName="BaseSwAligModel";
  if(dynClassFileHandler.getInfoForBaseClass(baseClassName,soFileName,initPars)==THOT_ERROR)
  {
    std::cerr<<"Error: ini file does not contain information about "<<baseClassName<<" class"<<std::endl;
    std::cerr<<"Please check content of master.ini file or execute \"thot_handle_ini_files -r\" to reset it"<<std::endl;
    return THOT_ERROR;
  }
   
      // Load class derived from BaseSwAligModel dynamically
  if(!baseSwAligModelDynClassLoader.open_module(soFileName))
  {
    std::cerr<<"Error: so file ("<<soFileName<<") could not be opened"<<std::endl;
    return THOT_ERROR;
  }

  swAligModelPtr=baseSwAligModelDynClassLoader.make_obj(initPars);
  if(swAligModelPtr==NULL)
  {
    std::cerr<<"Error: BaseSwAligModel pointer could not be instantiated"<<std::endl;
    baseSwAligModelDynClassLoader.close_module();
    
    return THOT_ERROR;
  }

  return THOT_OK;
}

//---------------
void release_swm(void)
{
  delete swAligModelPtr;
  baseSwAligModelDynClassLoader.close_module();
}

//---------------
int processPairAligFile(BaseSwAligModel<std::vector<Prob> > *swAligModelPtr,
                        const char *pairPlusAligFile)
{
 bool ret;
 awkInputStream awk;
 std::vector<std::string> fileSrcSentVec;
 std::vector<std::string> fileTrgSentVec;
 std::vector<PositionIndex> fileAligVec;
 WordAligMatrix waMatrix;
 LgProb lp;

 if(strcmp(pairPlusAligFile,"-")==0)
 {
       // read input from standard input
   awk.open_stream(stdin);
 }
 else
 {
       // read input from file
   if(awk.open(pairPlusAligFile)==THOT_ERROR)
   {
     std::cerr<<"Error in pairPlusAlig file, file "<<pairPlusAligFile<<" does not exist.\n";
     return THOT_ERROR;
   }
 }
     // Load model
 ret=swAligModelPtr->load(swFilePrefix);
 if(ret==THOT_ERROR) return THOT_ERROR;

     // Process input
 while(awk.getln())
 {
       // Extract source and target phrases
   unsigned int i=1; 
   fileSrcSentVec.clear();
   fileSrcSentVec.push_back(NULL_WORD_STR);
   while(i<=awk.NF && strcmp("|||",awk.dollar(i).c_str())!=0)	
   {
     fileSrcSentVec.push_back(awk.dollar(i)); 
     ++i;
   }
   ++i;
   fileTrgSentVec.clear();
   while(i<=awk.NF && strcmp("|||",awk.dollar(i).c_str())!=0)	
   {
     fileTrgSentVec.push_back(awk.dollar(i));			   
     ++i; 
   }
   fileAligVec.clear();
   while(i<=awk.NF && strcmp("|||",awk.dollar(i).c_str())!=0)	
   {
     fileAligVec.push_back(atoi(awk.dollar(i).c_str()));
     ++i; 
   }
   waMatrix.putAligVec(fileAligVec);
   
       // Process sentence pair plus alignment
   lp=swAligModelPtr->calcLgProbForAligVecStr(fileSrcSentVec,
                                      fileTrgSentVec,
                                      waMatrix,
                                      verbosity);
   
   cout<<awk.dollar(0)<<" ||| "<<lp<<std::endl;
 }   
 return THOT_OK;   
}

//---------------
int processSentPairFile(BaseSwAligModel<std::vector<Prob> > *swAligModelPtr,
                        const char *sentPairFile)
{
      // Define variables
 bool ret;
 awkInputStream awk;
 std::vector<std::string> srcSentVec;
 std::vector<std::string> trgSentVec;
 LgProb lp;

     // Define variables required to speed up generation of best
     // alignments for HMM alignment models
 CachedHmmAligLgProb cached_logap;
 IncrHmmAligModel* incrHmmAligModelPtr=dynamic_cast<IncrHmmAligModel*>(swAligModelPtr);
 
 if(strcmp(sentPairFile,"-")==0)
 {
       // read input from standard input
   awk.open_stream(stdin);
 }
 else
 {
       // read input from file
   if(awk.open(sentPairFile)==THOT_ERROR)
   {
     std::cerr<<"Error in sentPair file, file "<<sentPairFile<<" does not exist.\n";
     return THOT_ERROR;
   }
 }
     // Load model
 ret=swAligModelPtr->load(swFilePrefix);
 if(ret==THOT_ERROR) return THOT_ERROR;

     // Process input
 while(awk.getln())
 {
       // Extract source and target phrases
   unsigned int i=1; 
   srcSentVec.clear();
   while(i<=awk.NF && strcmp("|||",awk.dollar(i).c_str())!=0)	
   {
     srcSentVec.push_back(awk.dollar(i)); 
     ++i;
   }
   ++i;
   trgSentVec.clear();
   while(i<=awk.NF && strcmp("|||",awk.dollar(i).c_str())!=0)	
   {
     trgSentVec.push_back(awk.dollar(i));			   
     ++i; 
   }

   if(max_opt)
   {
         // -max option was given
     WordAligMatrix waMatrix;

         // Obtain best alignment
     if(incrHmmAligModelPtr)
     {
       lp=incrHmmAligModelPtr->obtainBestAlignmentVecStrCached(srcSentVec,
                                                               trgSentVec,
                                                               cached_logap,
                                                               waMatrix);
     }
     else
     {
       lp=swAligModelPtr->obtainBestAlignmentVecStr(srcSentVec,
                                                    trgSentVec,
                                                    waMatrix);
     }
     
         // Print alignment in GIZA format
     char header[256];
     sprintf(header,"# Alignment probability= %f",(double)lp);
     printAlignmentInGIZAFormat(cout,swAligModelPtr->addNullWordToStrVec(srcSentVec),trgSentVec,waMatrix,header);
   }
   else
   {
         // -max option was not given
     lp=swAligModelPtr->calcLgProbVecStr(srcSentVec,
                                         trgSentVec,
                                         verbosity);
     cout<<awk.dollar(0)<<" ||| "<<lp<<std::endl;
   }
 }
 
 return THOT_OK;   
}

//---------------
int TakeParameters(int argc,char *argv[])
{
 int err;

 if(argc==1)
 {
   version();
   return THOT_ERROR;   
 }

 err=readOption(argc,argv,"--help");
 if(err!=-1)
 {
   printUsage();
   return THOT_ERROR;   
 }      

 // Take the giza files prefix 
 err=readString(argc,argv, "-sw", swFilePrefix);
 if(err==-1)
 {
   swFilePrefix[0]=0;
 }
    
 // Verify if at least one of the main parameters were introduced
 if(swFilePrefix[0]==0 )
 {
   printUsage();
   return THOT_ERROR;
 }

 // Init variables related to -F and -P options
 pairPlusAligFile[0]=0;
 sentPairFile[0]=0;

 // Get pairPlusAlig file
 err=readString(argc,argv, "-F", pairPlusAligFile);
 if(err==-1 || argc<2)
 {

       // If pairPlusAlig file not given, get sentPairFile
   err=readString(argc,argv, "-P", sentPairFile);
   if(err==-1 || argc<2)
   {  
         // If sentPair file not given, get single pair with its
         // alignment
  
         // Get s sentence
     err=readSTLstring(argc,argv, "-ss",&srcSent);
     if(err==-1 || argc<2)
     {
       printUsage();
       return THOT_ERROR;
     } 
    
         // Get t sentence
     err=readSTLstring(argc,argv, "-ts",&trgSent);
     if(err==-1 || argc<2)
     {
       printUsage();
       return THOT_ERROR;
     } 
  
         // Get word alignment
     alig_given=1;
     err=readSTLstring(argc,argv, "-a",&alig);
     if(err==-1 || argc<2)
     {
       alig_given=0;
     }
   }
 }

     /* Verify -max option */
 max_opt=0;
 err=readOption(argc,argv,"-max");
 if(err!=-1)
 {
   max_opt=1;
 }

     /* Verify -v option */
 verbosity=0;
 err=readOption(argc,argv,"-v");
 if(err!=-1)
 {
   verbosity=1;
 }
 else
 {
   err=readOption(argc,argv,"-v1");
   if(err!=-1)
   {
     verbosity=2;
   }
 }
   
 return THOT_OK;  
}

//---------------
void printUsage(void)
{
 std::cerr<<"Usage: thot_calc_swm_lgprob -sw <string>\n";
 std::cerr<<"                            {-ss <string> -ts <string>\n";
 std::cerr<<"                            [-a <string>] [-max] | -F <string>\n";
 std::cerr<<"                            | -P <string> [-max]} [-v|-v1] \n";
 std::cerr<<"                            [--help]\n\n";
 std::cerr<<"-sw <string>                Prefix of the single-word model files\n";
 std::cerr<<"                            to load\n\n";
 std::cerr<<"-ss <string>                Source sentence\n\n";	
 std::cerr<<"-ts <string>                Target sentence\n\n";	
 std::cerr<<"-a <string>                 Word-alignment between s and t: \"0 1 ... n\"\n";
 std::cerr<<"                            If not given, the sum of IBM 1 log-prob\n";
 std::cerr<<"                            for each possible alignment is calculated\n\n";
 std::cerr<<"-max                        Obtain log-prob for the best alignment\n\n";
 std::cerr<<"-F <string>                 File with sentence pairs and their alignment.\n";
 std::cerr<<"                            If <string>=\"-\" then stdin is read.\n";
 std::cerr<<"                            Format: src ||| trg ||| alig\n\n";
 std::cerr<<"-P <string>                 File with sentence pairs without alignment.\n";
 std::cerr<<"                            If <string>=\"-\" then stdin is read.\n";
 std::cerr<<"                            Format: src ||| trg\n\n";
 std::cerr<<"-v | -v1                    Verbose mode\n\n";
 std::cerr<<"--help                      Display this help and exit\n\n";
}

//---------------
void version(void)
{
  std::cerr<<"thot_calc_swm_lgprob is part of the thot package "<<std::endl;
  std::cerr<<"thot version "<<THOT_VERSION<<std::endl;
  std::cerr<<"thot is GNU software written by Daniel Ortiz"<<std::endl;
}
