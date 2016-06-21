/*
incr_models package for statistical machine translation
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
/* Module: thot_li_weight_upd.cc                                    */
/*                                                                  */
/* Definitions file: thot_li_weight_upd.cc                          */
/*                                                                  */
/* Description: Implements a linear interpolation weight updater    */
/*              for phrase-based models given a development         */
/*              corpus.                                             */
/*                                                                  */
/********************************************************************/

/**
 * @file thot_li_weight_upd.cc
 *
 * @brief Implements a linear interpolation weight updater for
 * phrase-based models given a development corpus.
 */

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "PhrLocalSwLiTm.h"
#include "DynClassFactoryHandler.h"
#include "ErrorDefs.h"
#include "options.h"
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

//--------------- Constants ------------------------------------------

struct thot_liwu_pars
{
  std::string srcCorpusFile;
  std::string trgCorpusFile;
  std::string phrModelFilePrefix;
  int verbosity;
};

//--------------- Function Declarations ------------------------------

int handleParameters(int argc,
                     char *argv[],
                     thot_liwu_pars& pars);
int takeParameters(int argc,
                   char *argv[],
                   thot_liwu_pars& pars);
int checkParameters(thot_liwu_pars& pars);
int initPhrModel(void);
void releasePhrModel(void);
int update_li_weights(const thot_liwu_pars& pars);
void printUsage(void);
void version(void);

//--------------- Global variables -----------------------------------

DynClassFactoryHandler dynClassFactoryHandler;
PhraseModelInfo* phrModelInfoPtr;
SwModelInfo* swModelInfoPtr;
PhrLocalSwLiTm* phrLocalSwLiTmPtr;

//--------------- Function Definitions -------------------------------

//--------------------------------
int main(int argc,char *argv[])
{
  thot_liwu_pars pars;

  if(handleParameters(argc,argv,pars)==ERROR)
  {
    return ERROR;
  }
  else
  {
        // Print parameters
    cerr<<"-tm option is "<<pars.phrModelFilePrefix<<endl;
    cerr<<"-s option is "<<pars.srcCorpusFile<<endl;
    cerr<<"-t option is "<<pars.trgCorpusFile<<endl;
    cerr<<"-v option is "<<pars.verbosity<<endl;

        // Initialize weight updater
    phrLocalSwLiTmPtr=new PhrLocalSwLiTm;
    
        // Update language model weights
    int retVal=update_li_weights(pars);

        // Release weight updater
    delete phrLocalSwLiTmPtr;
    
    return retVal;
  }
}

//--------------------------------
int handleParameters(int argc,
                     char *argv[],
                     thot_liwu_pars& pars)
{
  if(argc==1 || readOption(argc,argv,"--version")!=-1)
  {
    version();
    return ERROR;
  }
  if(readOption(argc,argv,"--help")!=-1)
  {
    printUsage();
    return ERROR;   
  }
  if(takeParameters(argc,argv,pars)==ERROR)
  {
    return ERROR;
  }
  else
  {
    if(checkParameters(pars)==OK)
    {
      return OK;
    }
    else
    {
      return ERROR;
    }
  }
}

//--------------------------------
int takeParameters(int argc,
                   char *argv[],
                   thot_liwu_pars& pars)
{
      // Take language model file name
  int err=readSTLstring(argc,argv, "-tm", &pars.phrModelFilePrefix);
  if(err==ERROR)
    return ERROR;
  
      // Take language model file name
  err=readSTLstring(argc,argv, "-s", &pars.srcCorpusFile);
  if(err==ERROR)
    return ERROR;

      // Take language model file name
  err=readSTLstring(argc,argv, "-t", &pars.trgCorpusFile);
  if(err==ERROR)
    return ERROR;

  if(readOption(argc,argv,"-v")==OK)
    pars.verbosity=true;
  else
    pars.verbosity=false;
    
  return OK;
}

//--------------------------------
int checkParameters(thot_liwu_pars& pars)
{  
  if(pars.phrModelFilePrefix.empty())
  {
    cerr<<"Error: parameter -tm not given!"<<endl;
    return ERROR;   

  }

  if(pars.srcCorpusFile.empty())
  {
    cerr<<"Error: parameter -s not given!"<<endl;
    return ERROR;   
  }

  if(pars.trgCorpusFile.empty())
  {
    cerr<<"Error: parameter -t not given!"<<endl;
    return ERROR;   
  }

  return OK;
}

//--------------------------------
int initPhrModel(void)
{
      // Initialize class factories
  int err=dynClassFactoryHandler.init_smt(THOT_MASTER_INI_PATH);
  if(err==ERROR)
    return ERROR;

      // Instantiate pointers
  phrModelInfoPtr=new PhraseModelInfo;
  phrModelInfoPtr->invPbModelPtr=dynClassFactoryHandler.basePhraseModelDynClassLoader.make_obj(dynClassFactoryHandler.basePhraseModelInitPars);
  if(phrModelInfoPtr->invPbModelPtr==NULL)
  {
    cerr<<"Error: BasePhraseModel pointer could not be instantiated"<<endl;
    return ERROR;
  }

  swModelInfoPtr=new SwModelInfo;
  swModelInfoPtr->swAligModelPtr=dynClassFactoryHandler.baseSwAligModelDynClassLoader.make_obj(dynClassFactoryHandler.baseSwAligModelInitPars);
  if(swModelInfoPtr->swAligModelPtr==NULL)
  {
    cerr<<"Error: BaseSwAligModel pointer could not be instantiated"<<endl;
    return ERROR;
  }

  swModelInfoPtr->invSwAligModelPtr=dynClassFactoryHandler.baseSwAligModelDynClassLoader.make_obj(dynClassFactoryHandler.baseSwAligModelInitPars);
  if(swModelInfoPtr->invSwAligModelPtr==NULL)
  {
    cerr<<"Error: BaseSwAligModel pointer could not be instantiated"<<endl;
    return ERROR;
  }

      // Link pointers
  phrLocalSwLiTmPtr->link_pm_info(phrModelInfoPtr);
  phrLocalSwLiTmPtr->link_swm_info(swModelInfoPtr);
  
  return OK;
}

//--------------------------------
void releasePhrModel(void)
{
  delete phrModelInfoPtr->invPbModelPtr;
  delete phrModelInfoPtr;
  delete swModelInfoPtr->swAligModelPtr;
  delete swModelInfoPtr->invSwAligModelPtr;
  delete swModelInfoPtr;

  dynClassFactoryHandler.release_smt();
}

//--------------------------------
int update_li_weights(const thot_liwu_pars& pars)
{
  int retVal;

      // Initialize phrase model
  retVal=initPhrModel();
  if(retVal==ERROR)
    return ERROR;
  
      // Load model
  retVal=phrLocalSwLiTmPtr->loadAligModel(pars.phrModelFilePrefix.c_str());
  if(retVal==ERROR)
    return ERROR;
  
      // Update weights
  retVal=phrLocalSwLiTmPtr->updateLinInterpWeights(pars.srcCorpusFile,pars.trgCorpusFile,pars.verbosity);
  if(retVal==ERROR)
    return ERROR;

      // Print updated weights
  retVal=phrLocalSwLiTmPtr->printAligModel(pars.phrModelFilePrefix.c_str());
  if(retVal==ERROR)
    return ERROR;

      // Release phrase model
  releasePhrModel();
  
  return OK;
}

//--------------------------------
void printUsage(void)
{
  cerr<<"thot_li_weight_upd -tm <string> -s <string> -t <string>"<<endl;
  cerr<<"                   [-v] [--help] [--version]"<<endl;
  cerr<<endl;
  cerr<<"-tm <string>       Prefix of translation model files."<<endl;
  cerr<<"                   (Warning: current weights will be overwritten)."<<endl;
  cerr<<"-s <string>        Source development corpus."<<endl;
  cerr<<"-t <string>        Target development corpus."<<endl;
  cerr<<"-v                 Enable verbose mode."<<endl;
  cerr<<"--help             Display this help and exit."<<endl;
  cerr<<"--version          Output version information and exit."<<endl;
}

//--------------------------------
void version(void)
{
  cerr<<"thot_li_weight_upd is part of the thot package"<<endl;
  cerr<<"thot version "<<THOT_VERSION<<endl;
  cerr<<"thot is GNU software written by Daniel Ortiz"<<endl;
}
