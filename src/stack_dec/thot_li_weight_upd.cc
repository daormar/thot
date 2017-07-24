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
#include "ModelDescriptorUtils.h"
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
  std::string testCorpusFile;
  std::string refCorpusFile;
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
int initPhrModel(std::string phrModelFilePrefix);
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

  if(handleParameters(argc,argv,pars)==THOT_ERROR)
  {
    return THOT_ERROR;
  }
  else
  {
        // Print parameters
    cerr<<"-tm option is "<<pars.phrModelFilePrefix<<endl;
    cerr<<"-t option is "<<pars.testCorpusFile<<endl;
    cerr<<"-r option is "<<pars.refCorpusFile<<endl;
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
                   thot_liwu_pars& pars)
{
      // Take language model file name
  int err=readSTLstring(argc,argv, "-tm", &pars.phrModelFilePrefix);
  if(err==THOT_ERROR)
    return THOT_ERROR;
  
      // Take language model file name
  err=readSTLstring(argc,argv, "-t", &pars.testCorpusFile);
  if(err==THOT_ERROR)
    return THOT_ERROR;

      // Take language model file name
  err=readSTLstring(argc,argv, "-r", &pars.refCorpusFile);
  if(err==THOT_ERROR)
    return THOT_ERROR;

  if(readOption(argc,argv,"-v")==THOT_OK)
    pars.verbosity=true;
  else
    pars.verbosity=false;
    
  return THOT_OK;
}

//--------------------------------
int checkParameters(thot_liwu_pars& pars)
{  
  if(pars.phrModelFilePrefix.empty())
  {
    cerr<<"Error: parameter -tm not given!"<<endl;
    return THOT_ERROR;   

  }

  if(pars.testCorpusFile.empty())
  {
    cerr<<"Error: parameter -t not given!"<<endl;
    return THOT_ERROR;   
  }

  if(pars.refCorpusFile.empty())
  {
    cerr<<"Error: parameter -r not given!"<<endl;
    return THOT_ERROR;   
  }

  return THOT_OK;
}

//--------------------------------
int initPhrModel(std::string phrModelFilePrefix)
{
      // Initialize class factories
  int err=dynClassFactoryHandler.init_smt(THOT_MASTER_INI_PATH);
  if(err==THOT_ERROR)
    return THOT_ERROR;

      // Obtain info about translation model entries
  unsigned int numTransModelEntries;
  Vector<ModelDescriptorEntry> modelDescEntryVec;
  if(extractModelEntryInfo(phrModelFilePrefix.c_str(),modelDescEntryVec)==THOT_OK)
  {
    numTransModelEntries=modelDescEntryVec.size();
  }
  else
  {
    numTransModelEntries=1;
  }
  
      // Instantiate pointers
  phrModelInfoPtr=new PhraseModelInfo;
  phrModelInfoPtr->invPbModelPtr=dynClassFactoryHandler.basePhraseModelDynClassLoader.make_obj(dynClassFactoryHandler.basePhraseModelInitPars);
  if(phrModelInfoPtr->invPbModelPtr==NULL)
  {
    cerr<<"Error: BasePhraseModel pointer could not be instantiated"<<endl;
    return THOT_ERROR;
  }

      // Add one swm pointer per each translation model entry
  swModelInfoPtr=new SwModelInfo;
  for(unsigned int i=0;i<numTransModelEntries;++i)
  {
    swModelInfoPtr->swAligModelPtrVec.push_back(dynClassFactoryHandler.baseSwAligModelDynClassLoader.make_obj(dynClassFactoryHandler.baseSwAligModelInitPars));
    if(swModelInfoPtr->swAligModelPtrVec[0]==NULL)
    {
      cerr<<"Error: BaseSwAligModel pointer could not be instantiated"<<endl;
      return THOT_ERROR;
    }
  }

      // Add one inverse swm pointer per each translation model entry
  for(unsigned int i=0;i<numTransModelEntries;++i)
  {
    swModelInfoPtr->invSwAligModelPtrVec.push_back(dynClassFactoryHandler.baseSwAligModelDynClassLoader.make_obj(dynClassFactoryHandler.baseSwAligModelInitPars));
    if(swModelInfoPtr->invSwAligModelPtrVec[0]==NULL)
    {
      cerr<<"Error: BaseSwAligModel pointer could not be instantiated"<<endl;
      return THOT_ERROR;
    }
  }

      // Link pointers
  phrLocalSwLiTmPtr->link_pm_info(phrModelInfoPtr);
  phrLocalSwLiTmPtr->link_swm_info(swModelInfoPtr);
  
  return THOT_OK;
}

//--------------------------------
void releasePhrModel(void)
{
  delete phrModelInfoPtr->invPbModelPtr;
  delete phrModelInfoPtr;
  for(unsigned int i=0;i<swModelInfoPtr->swAligModelPtrVec.size();++i)
    delete swModelInfoPtr->swAligModelPtrVec[i];
  for(unsigned int i=0;i<swModelInfoPtr->swAligModelPtrVec.size();++i)
    delete swModelInfoPtr->invSwAligModelPtrVec[i];
  delete swModelInfoPtr;

  dynClassFactoryHandler.release_smt();
}

//--------------------------------
int update_li_weights(const thot_liwu_pars& pars)
{
  int retVal;

      // Initialize phrase model
  retVal=initPhrModel(pars.phrModelFilePrefix);
  if(retVal==THOT_ERROR)
    return THOT_ERROR;
  
      // Load model
  retVal=phrLocalSwLiTmPtr->loadAligModel(pars.phrModelFilePrefix.c_str());
  if(retVal==THOT_ERROR)
    return THOT_ERROR;
  
      // Update weights
  retVal=phrLocalSwLiTmPtr->updateLinInterpWeights(pars.testCorpusFile,pars.refCorpusFile,pars.verbosity);
  if(retVal==THOT_ERROR)
    return THOT_ERROR;

      // Print updated weights
  retVal=phrLocalSwLiTmPtr->printAligModel(pars.phrModelFilePrefix.c_str());
  if(retVal==THOT_ERROR)
    return THOT_ERROR;

      // Release phrase model
  releasePhrModel();
  
  return THOT_OK;
}

//--------------------------------
void printUsage(void)
{
  cerr<<"thot_li_weight_upd -tm <string> -t <string> -r <string>"<<endl;
  cerr<<"                   [-v] [--help] [--version]"<<endl;
  cerr<<endl;
  cerr<<"-tm <string>       Prefix or descriptor of translation model files."<<endl;
  cerr<<"                   (Warning: current weights will be overwritten)."<<endl;
  cerr<<"-t <string>        File with test sentences."<<endl;
  cerr<<"-r <string>        File with reference sentences."<<endl;
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
