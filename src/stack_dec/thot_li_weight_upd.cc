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

#include THOT_SMTMODEL_H // Define SmtModel type. It is set in
                         // configure by checking SMTMODEL_H
                         // variable (default value: SmtModel.h)
#include "FeatureHandler.h"
#include "_pbTransModel.h"
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
bool featureBasedImplIsEnabled(void);
int initPhrModelLegacyImpl(std::string phrModelFilePrefix);
int initPhrModelFeatImpl(std::string phrModelFilePrefix);
void set_default_models(void);
int add_model_features(std::string phrModelFilePrefix);
void releaseMemLegacyImpl(void);
void releaseMemFeatImpl(void);
int update_li_weights_legacy_impl(const thot_liwu_pars& pars);
int update_li_weights_feat_impl(const thot_liwu_pars& pars);
void printUsage(void);
void version(void);

//--------------- Global variables -----------------------------------

DynClassFactoryHandler dynClassFactoryHandler;
PhraseModelInfo* phrModelInfoPtr;
SwModelInfo* swModelInfoPtr;
PhrLocalSwLiTm* phrLocalSwLiTmPtr;

    // Variables related to feature-based implementation
FeatureHandler featureHandler;

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
    cerr<<"-t option is "<<pars.testCorpusFile<<endl;
    cerr<<"-r option is "<<pars.refCorpusFile<<endl;
    cerr<<"-v option is "<<pars.verbosity<<endl;
    
        // Update language model weights
    if(featureBasedImplIsEnabled())
      return update_li_weights_feat_impl(pars);
    else
      return update_li_weights_legacy_impl(pars);
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
  err=readSTLstring(argc,argv, "-t", &pars.testCorpusFile);
  if(err==ERROR)
    return ERROR;

      // Take language model file name
  err=readSTLstring(argc,argv, "-r", &pars.refCorpusFile);
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

  if(pars.testCorpusFile.empty())
  {
    cerr<<"Error: parameter -t not given!"<<endl;
    return ERROR;   
  }

  if(pars.refCorpusFile.empty())
  {
    cerr<<"Error: parameter -r not given!"<<endl;
    return ERROR;   
  }

  return OK;
}

//--------------------------
bool featureBasedImplIsEnabled(void)
{
  BasePbTransModel<SmtModel::Hypothesis>* tmpSmtModelPtr=new SmtModel();
  _pbTransModel<SmtModel::Hypothesis>* pbtm_ptr=dynamic_cast<_pbTransModel<SmtModel::Hypothesis>* >(tmpSmtModelPtr);
  if(pbtm_ptr)
  {
    delete tmpSmtModelPtr;
    return true;
  }
  else
  {
    delete tmpSmtModelPtr;
    return false;
  }
}

//--------------------------------
int initPhrModelLegacyImpl(std::string phrModelFilePrefix)
{
      // Show static types
  cerr<<"Static types:"<<endl;
  cerr<<"- SMT model type (SmtModel): "<<SMT_MODEL_TYPE_NAME<<" ("<<THOT_SMTMODEL_H<<")"<<endl;
  cerr<<"- Language model state (LM_Hist): "<<LM_STATE_TYPE_NAME<<" ("<<THOT_LM_STATE_H<<")"<<endl;
  cerr<<"- Partial probability information for single word models (PpInfo): "<<PPINFO_TYPE_NAME<<" ("<<THOT_PPINFO_H<<")"<<endl;

      // Initialize weight updater
  phrLocalSwLiTmPtr=new PhrLocalSwLiTm;

      // Initialize class factories
  int err=dynClassFactoryHandler.init_smt(THOT_MASTER_INI_PATH);
  if(err==ERROR)
    return ERROR;

      // Obtain info about translation model entries
  unsigned int numTransModelEntries;
  Vector<ModelDescriptorEntry> modelDescEntryVec;
  if(extractModelEntryInfo(phrModelFilePrefix.c_str(),modelDescEntryVec)==OK)
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
    return ERROR;
  }

      // Add one swm pointer per each translation model entry
  swModelInfoPtr=new SwModelInfo;
  for(unsigned int i=0;i<numTransModelEntries;++i)
  {
    swModelInfoPtr->swAligModelPtrVec.push_back(dynClassFactoryHandler.baseSwAligModelDynClassLoader.make_obj(dynClassFactoryHandler.baseSwAligModelInitPars));
    if(swModelInfoPtr->swAligModelPtrVec[0]==NULL)
    {
      cerr<<"Error: BaseSwAligModel pointer could not be instantiated"<<endl;
      return ERROR;
    }
  }

      // Add one inverse swm pointer per each translation model entry
  for(unsigned int i=0;i<numTransModelEntries;++i)
  {
    swModelInfoPtr->invSwAligModelPtrVec.push_back(dynClassFactoryHandler.baseSwAligModelDynClassLoader.make_obj(dynClassFactoryHandler.baseSwAligModelInitPars));
    if(swModelInfoPtr->invSwAligModelPtrVec[0]==NULL)
    {
      cerr<<"Error: BaseSwAligModel pointer could not be instantiated"<<endl;
      return ERROR;
    }
  }

      // Link pointers
  phrLocalSwLiTmPtr->link_pm_info(phrModelInfoPtr);
  phrLocalSwLiTmPtr->link_swm_info(swModelInfoPtr);
  
  return OK;
}

//--------------------------------
int initPhrModelFeatImpl(std::string phrModelFilePrefix)
{
      // Show static types
  cerr<<"Static types:"<<endl;
  cerr<<"- SMT model type (SmtModel): "<<SMT_MODEL_TYPE_NAME<<" ("<<THOT_SMTMODEL_H<<")"<<endl;
  cerr<<"- Language model state (LM_Hist): "<<LM_STATE_TYPE_NAME<<" ("<<THOT_LM_STATE_H<<")"<<endl;
  cerr<<"- Partial probability information for single word models (PpInfo): "<<PPINFO_TYPE_NAME<<" ("<<THOT_PPINFO_H<<")"<<endl;

      // Initialize class factories
  int ret=dynClassFactoryHandler.init_smt(THOT_MASTER_INI_PATH);
  if(ret==ERROR)
    return ERROR;
  
      // Set default models for feature handler
  set_default_models();
  
      // Add model features
  add_model_features(phrModelFilePrefix);
    
  return OK;
}

//---------------
void set_default_models(void)
{
  featureHandler.setDefaultTransModelType(dynClassFactoryHandler.basePhraseModelSoFileName);
  featureHandler.setDefaultSingleWordModelType(dynClassFactoryHandler.baseSwAligModelSoFileName);
}

//---------------
int add_model_features(std::string phrModelFilePrefix)
{
      // Add translation model features
  int verbosity=false;
  int ret=featureHandler.addTmFeats(phrModelFilePrefix,verbosity);
  if(ret==ERROR)
    return ERROR;

  return OK;
}

//--------------------------------
void releaseMemLegacyImpl(void)
{
  delete phrModelInfoPtr->invPbModelPtr;
  delete phrModelInfoPtr;
  for(unsigned int i=0;i<swModelInfoPtr->swAligModelPtrVec.size();++i)
    delete swModelInfoPtr->swAligModelPtrVec[i];
  for(unsigned int i=0;i<swModelInfoPtr->swAligModelPtrVec.size();++i)
    delete swModelInfoPtr->invSwAligModelPtrVec[i];
  delete swModelInfoPtr;
  delete phrLocalSwLiTmPtr;

  dynClassFactoryHandler.release_smt();
}

//--------------------------------
void releaseMemFeatImpl(void)
{
      // Delete features information
  featureHandler.clear();
  
      // Release class factory handler
  dynClassFactoryHandler.release_smt();
}

//--------------------------------
int update_li_weights_legacy_impl(const thot_liwu_pars& pars)
{
  int retVal;

      // Initialize phrase model
  retVal=initPhrModelLegacyImpl(pars.phrModelFilePrefix);
  if(retVal==ERROR)
    return ERROR;
  
      // Load model
  retVal=phrLocalSwLiTmPtr->loadAligModel(pars.phrModelFilePrefix.c_str());
  if(retVal==ERROR)
    return ERROR;
  
      // Update weights
  retVal=phrLocalSwLiTmPtr->updateLinInterpWeights(pars.testCorpusFile,pars.refCorpusFile,pars.verbosity);
  if(retVal==ERROR)
    return ERROR;

      // Print updated weights
  retVal=phrLocalSwLiTmPtr->printAligModel(pars.phrModelFilePrefix.c_str());
  if(retVal==ERROR)
    return ERROR;

      // Release phrase model
  releaseMemLegacyImpl();

  return OK;
}

//--------------------------------
int update_li_weights_feat_impl(const thot_liwu_pars& pars)
{
      // Initialize phrase model
  int retVal=initPhrModelFeatImpl(pars.phrModelFilePrefix);
  if(retVal==ERROR)
    return ERROR;

      // Update weights
  featureHandler.updatePmLinInterpWeights(pars.testCorpusFile,pars.refCorpusFile,pars.verbosity);
  
      // Print updated weights
  featureHandler.printAligModels(pars.phrModelFilePrefix);
  
      // Release phrase model
  releaseMemFeatImpl();

  return OK;
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
