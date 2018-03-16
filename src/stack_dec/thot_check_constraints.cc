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

/**
 * @file thot_check_constraints.cc
 * 
 * @brief Checks translation constraints for a source sentence and a set
 * of translations with alignment information.
 */

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseTranslationMetadata.h"
#include "DynClassFactoryHandler.h"
#include "AwkInputStream.h"
#include "StrProcUtils.h"
#include "ErrorDefs.h"
#include "options.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdlib.h>

//--------------- Type definitions -----------------------------------

struct input_pars
{
  std::string srcSentFile;
  std::string trgTransFile;
  std::string aligFile;
  int verbosity;
};

//--------------- Function Declarations ------------------------------

int handleParameters(int argc,
                     char *argv[],
                     input_pars& pars);
int takeParameters(int argc,
                   char *argv[],
                   input_pars& pars);
int checkParameters(input_pars pars);
int check_constraints(input_pars pars);
int extract_align_info(std::vector<std::string> alignInfoLine,
                       SourceSegmentation& sourceSegmentation,
                       std::vector<PositionIndex>& targetSegmentCuts);
void printUsage(void);
void version(void);

//--------------- Global variables -----------------------------------

DynClassFactoryHandler dynClassFactoryHandler;
BaseTranslationMetadata<SmtModel::HypScoreInfo>* baseTransMetadataPtr;

//--------------- Function Definitions -------------------------------

//--------------------------------
int main(int argc,char *argv[])
{
  input_pars pars;

  if(handleParameters(argc,argv,pars)==THOT_ERROR)
  {
    return THOT_ERROR;
  }
  else
  {
        // Initialize pointers
    int err=dynClassFactoryHandler.init_smt(THOT_MASTER_INI_PATH,false);
    if(err==THOT_ERROR)
      return THOT_ERROR;

    baseTransMetadataPtr=dynClassFactoryHandler.baseTranslationMetadataDynClassLoader.make_obj(dynClassFactoryHandler.baseScorerInitPars);
    if(baseTransMetadataPtr==NULL)
    {
      std::cerr<<"Error: BaseTranslationMetadata pointer could not be instantiated"<<std::endl;
      return THOT_ERROR;
    }

        // Check constraints for translations
    int retVal=check_constraints(pars);

        // Release pointers
    delete baseTransMetadataPtr;

        // Release class factories
    dynClassFactoryHandler.release_smt(false);

    return retVal;
  }
}

//--------------------------------
int handleParameters(int argc,
                     char *argv[],
                     input_pars& pars)
{
  if(argc==1 || readOption(argc,argv,"--version")!=-1)
  {
    version();
    return THOT_ERROR;
  }
  if(readOption(argc,argv,"--help")!=-1)
  {
    printUsage();
    return THOT_ERROR;   
  }
  if(takeParameters(argc,argv,pars)==THOT_ERROR)
  {
    return THOT_ERROR;
  }
  else
  {
    if(checkParameters(pars)==THOT_OK)
    {
      return THOT_OK;
    }
    else
    {
      return THOT_ERROR;
    }
  }
}

//--------------------------------
int takeParameters(int argc,
                   char *argv[],
                   input_pars& pars)
{
  int err=readSTLstring(argc,argv, "-s", &pars.srcSentFile);
  if(err==THOT_ERROR)
    return THOT_ERROR;

  err=readSTLstring(argc,argv, "-t", &pars.trgTransFile);
  if(err==THOT_ERROR)
    return THOT_ERROR;

  err=readSTLstring(argc,argv, "-a", &pars.aligFile);
  if(err==THOT_ERROR)
    return THOT_ERROR;

  if(readOption(argc,argv,"-v")==THOT_OK)
    pars.verbosity=true;
  else
    pars.verbosity=false;

  return THOT_OK;
}

//--------------------------------
int checkParameters(input_pars pars)
{  
  if(pars.srcSentFile.empty())
  {
    std::cerr<<"Error: parameter -s not given!"<<std::endl;
    return THOT_ERROR;   
  }

  if(pars.trgTransFile.empty())
  {
    std::cerr<<"Error: parameter -t not given!"<<std::endl;
    return THOT_ERROR;   
  }

  if(pars.aligFile.empty())
  {
    std::cerr<<"Error: parameter -a not given!"<<std::endl;
    return THOT_ERROR;   
  }

  return THOT_OK;
}

//--------------------------------
int check_constraints(input_pars pars)
{
      // Obtain source sentence
  AwkInputStream awkSrc;

  if(awkSrc.open(pars.srcSentFile.c_str())==THOT_ERROR)
  {
    std::cerr<<"Error while opening file "<<pars.srcSentFile<<std::endl;
    return THOT_ERROR;
  }  

  awkSrc.getln();

      // Obtain translation constraints
  int verbosity=false;
  baseTransMetadataPtr->obtainTransConstraints(awkSrc.dollar(0),verbosity);

      // Open file with target translations
  AwkInputStream awkTrgTrans;

  if(awkTrgTrans.open(pars.trgTransFile.c_str())==THOT_ERROR)
  {
    std::cerr<<"Error while opening file "<<pars.trgTransFile<<std::endl;
    return THOT_ERROR;
  }

      // Open file with alignment information
  AwkInputStream awkAlig;

  if(awkAlig.open(pars.aligFile.c_str())==THOT_ERROR)
  {
    std::cerr<<"Error while opening file "<<pars.aligFile<<std::endl;
    return THOT_ERROR;
  }

      // Iterate over target translations and their alignments
  while(awkTrgTrans.getln())
  {
    if(pars.verbosity)
      std::cerr<<"Processing target sentence "<<awkTrgTrans.FNR<<" ..."<<std::endl;

    bool ok=awkAlig.getln();
    if(!ok)
    {
      std::cerr<<"Unexpected end of alignment file."<<std::endl;
      return THOT_ERROR;      
    }

        // Obtain target sentence
    std::vector<std::string> trgSentVec=StrProcUtils::stringToStringVector(awkTrgTrans.dollar(0));
    if(trgSentVec.empty())
    {
      std::cerr<<"Warning: target sentence empty at line "<<awkTrgTrans.FNR<<std::endl;
      std::cout<<"ERROR: TARGET SENTENCE EMPTY"<<std::endl;
      break;
    }
    
        // Obtain alignment information
    SourceSegmentation sourceSegmentation;
    std::vector<PositionIndex> targetSegmentCuts;
    std::vector<std::string> alignInfoStrVec=StrProcUtils::stringToStringVector(awkAlig.dollar(0));
    if(alignInfoStrVec.empty())
    {
      std::cerr<<"Warning: alignment information empty at line "<<awkAlig.FNR<<std::endl;
      std::cout<<"ERROR: ALIGNMENT INFORMATION EMPTY"<<std::endl;
      break;
    }
    int ret=extract_align_info(alignInfoStrVec,sourceSegmentation,targetSegmentCuts);
    if(ret==THOT_ERROR)
    {
      std::cerr<<"Warning: alignment information incorrect at line "<<awkAlig.FNR<<std::endl;
      std::cout<<"ERROR: ALIGNMENT INFORMATION INCORRECT"<<std::endl;
      break;
    }
    
        // Check constraints
    bool constraintsSatisfied=baseTransMetadataPtr->translationSatisfiesConstraints(sourceSegmentation,
                                                                                    targetSegmentCuts,
                                                                                    trgSentVec);
        // Print result
    if(constraintsSatisfied)
      std::cout<<"CONSTRAINTS SATISFIED"<<std::endl;
    else
      std::cout<<"CONSTRAINTS NOT SATISFIED"<<std::endl;
  }

  return THOT_OK;
}

//--------------------------------
int extract_align_info(std::vector<std::string> alignInfoStrVec,
                       SourceSegmentation& sourceSegmentation,
                       std::vector<PositionIndex>& targetSegmentCuts)
{
      // Clear output variables
  sourceSegmentation.clear();
  targetSegmentCuts.clear();
  
      // Parse line
  bool parsingSrcSegmInfo=true;
  bool end=false;
  unsigned int i=0;
  while(!end)
  {
    if(parsingSrcSegmInfo)
    {
      if(alignInfoStrVec[i]=="|")
      {
        parsingSrcSegmInfo=false;
        i+=1;
      }
      else
      {
        PositionIndex leftmostPos=atoi(alignInfoStrVec[i+1].c_str());
        PositionIndex rightmostPos=atoi(alignInfoStrVec[i+3].c_str());
        sourceSegmentation.push_back(std::make_pair(leftmostPos,rightmostPos));
        i+=5;
      }
    }
    else
    {
      targetSegmentCuts.push_back(std::stoi(alignInfoStrVec[i]));
      i+=1;
    }
        // Detect ending condition
    if(i>=alignInfoStrVec.size())
      end=true;
  }

  if(sourceSegmentation.size()>0 && sourceSegmentation.size()==targetSegmentCuts.size())
    return THOT_OK;
  else
    return THOT_ERROR;
}

//--------------------------------
void printUsage(void)
{
  std::cerr<<"thot_check_constraints -s <string> -t <string> -a <string> [-v]"<<std::endl;
  std::cerr<<"                       [--help] [--version]"<<std::endl;
  std::cerr<<std::endl;
  std::cerr<<"-s <string>            File with metadata for source sentence."<<std::endl;
  std::cerr<<"-t <string>            File with target translations."<<std::endl;
  std::cerr<<"-a <string>            File with alignment information for target translations."<<std::endl;
  std::cerr<<"-v                     Enable verbose mode."<<std::endl;
  std::cerr<<"--help                 Display this help and exit."<<std::endl;
  std::cerr<<"--version              Output version information and exit."<<std::endl;
}

//--------------------------------
void version(void)
{
  std::cerr<<"thot_check_constraints is part of the thot package"<<std::endl;
  std::cerr<<"thot version "<<THOT_VERSION<<std::endl;
  std::cerr<<"thot is GNU software written by Daniel Ortiz"<<std::endl;
}
