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
 * @file thot_get_srcsents_from_metadata.cc
 * 
 * @brief Gets source sentences from file containing metadata for each
 * one.
 */

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseTranslationMetadata.h"
#include "DynClassFactoryHandler.h"
#include "AwkInputStream.h"
#include "ErrorDefs.h"
#include "options.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <iomanip>

//--------------- Type definitions -----------------------------------

struct input_pars
{
  std::string srcSentsFile;
};

//--------------- Function Declarations ------------------------------

int handleParameters(int argc,
                     char *argv[],
                     input_pars& pars);
int takeParameters(int argc,
                   char *argv[],
                   input_pars& pars);
int checkParameters(input_pars pars);
int get_srcsents(input_pars pars);
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

        // Calculate score
    int retVal=get_srcsents(pars);

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
  int err=readSTLstring(argc,argv, "-f", &pars.srcSentsFile);
  if(err==THOT_ERROR)
    return THOT_ERROR;

  return THOT_OK;
}

//--------------------------------
int checkParameters(input_pars pars)
{  
  if(pars.srcSentsFile.empty())
  {
    std::cerr<<"Error: parameter -f not given!"<<std::endl;
    return THOT_ERROR;   
  }

  return THOT_OK;
}

//--------------------------------
int get_srcsents(input_pars pars)
{
      // Fill output variable
  AwkInputStream awk;

  if(awk.open(pars.srcSentsFile.c_str())==THOT_ERROR)
  {
    std::cerr<<"Error while opening file "<<pars.srcSentsFile<<std::endl;
    return THOT_ERROR;
  }  
  
  while(awk.getln())
  {
        // Obtain translation constraints
    int verbosity=false;
    baseTransMetadataPtr->obtainTransConstraints(awk.dollar(0),verbosity);

        // Get source sentence
    std::vector<std::string> srcSentVec=baseTransMetadataPtr->getSrcSentVec();

        // Print source sentence
    for(unsigned int i=0;i<srcSentVec.size();++i)
    {
      if(i!=0) std::cout<<" ";
      std::cout<<srcSentVec[i];
    }
    std::cout<<std::endl;
  }

  return THOT_OK;
}

//--------------------------------
void printUsage(void)
{
  std::cerr<<"thot_get_srcsents_from_metadata -f <string>"<<std::endl;
  std::cerr<<"                                [--help] [--version]"<<std::endl;
  std::cerr<<std::endl;
  std::cerr<<"-f <string>                     File with metadata for sentences."<<std::endl;
  std::cerr<<"--help                          Display this help and exit."<<std::endl;
  std::cerr<<"--version                       Output version information and exit."<<std::endl;
}

//--------------------------------
void version(void)
{
  std::cerr<<"thot_get_srcsents_from_metadata is part of the thot package"<<std::endl;
  std::cerr<<"thot version "<<THOT_VERSION<<std::endl;
  std::cerr<<"thot is GNU software written by Daniel Ortiz"<<std::endl;
}
