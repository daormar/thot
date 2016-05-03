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
/* Module: thot_get_ll_weights                                      */
/*                                                                  */
/* Definitions file: thot_get_ll_weights.cc                         */
/*                                                                  */
/* Description: Prints the values of the log-linear weights for a   */
/*              given configuration file.                           */
/*                                                                  */
/********************************************************************/

/**
 * @file thot_get_ll_weights.cc
 *
 * @brief Prints the values of the log-linear weights for a given
 *        configuration file.
 */

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "AssistedTransTypes.h"
#include "SmtModelTypes.h"
#include "BasePbTransModel.h"
#include "_phrSwTransModel.h"
#include "_phraseBasedTransModel.h"
#include "StackDecSwModelTypes.h"
#include "SwModelInfo.h"
#include "PhraseModelInfo.h"
#include "StackDecLmTypes.h"
#include "LangModelInfo.h"
#include "WordPenaltyModel.h"
#include "KbMiraLlWu.h"
#include "BaseLogLinWeightUpdater.h"
#include "StackDecEcModelTypes.h"
#include "BaseEcmForWg.h"
#include "BaseErrorCorrectionModel.h"

#include "DynClassFactoryHandler.h"
#include "options.h"
#include "ErrorDefs.h"
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <vector>
#include <string>
#include <map>
#include <set>

using namespace std;

//--------------- Constants ------------------------------------------


//--------------- Type definitions -----------------------------------

struct thot_get_ll_weights_pars
{
  Vector<float> tmWeightVec;
  Vector<float> catWeightVec;
  Vector<float> ecmWeightVec;
  int verbosity;
};

//--------------- Function Declarations ------------------------------

int get_ll_weights(const thot_get_ll_weights_pars& tdp);
int translate_corpus(const thot_get_ll_weights_pars& tdp);
Vector<string> stringToStringVector(string s);
void version(void);
int handleParameters(int argc,
                     char *argv[],
                     thot_get_ll_weights_pars& pars);
int takeParameters(int argc,
                   char *argv[],
                   thot_get_ll_weights_pars& tdp);
int takeParametersFromCfgFile(std::string cfgFileName,
                              thot_get_ll_weights_pars& tdp);
void takeParametersGivenArgcArgv(int argc,
                                 char *argv[],
                                 thot_get_ll_weights_pars& tdp);
void printUsage(void);

//--------------- Global variables -----------------------------------

DynClassFactoryHandler dynClassFactoryHandler;
LangModelInfo* langModelInfoPtr;
PhraseModelInfo* phrModelInfoPtr;
SwModelInfo* swModelInfoPtr;
BaseLogLinWeightUpdater* llWeightUpdaterPtr;
BasePbTransModel<CURR_MODEL_TYPE::Hypothesis>* smtModelPtr;

//--------------- Function Definitions -------------------------------

//--------------- main function
int main(int argc, char *argv[])
{
      // Take and check parameters
  thot_get_ll_weights_pars pars;
  if(handleParameters(argc,argv,pars)==ERROR)
  {
    return ERROR;
  }
  else
  {
    int ret=get_ll_weights(pars);
    return ret;
  }
}

//--------------- get_ll_weights function
int get_ll_weights(const thot_get_ll_weights_pars& pars)
{
      // Initialize class factories
  int err=dynClassFactoryHandler.init_smt(THOT_MASTER_INI_PATH,pars.verbosity);
  if(err==ERROR)
    return ERROR;

  langModelInfoPtr=new LangModelInfo;
  langModelInfoPtr->lModelPtr=new THOT_CURR_LM_TYPE;

  phrModelInfoPtr=new PhraseModelInfo;
  phrModelInfoPtr->invPbModelPtr=dynClassFactoryHandler.basePhraseModelDynClassLoader.make_obj(dynClassFactoryHandler.basePhraseModelInitPars);
  if(phrModelInfoPtr->invPbModelPtr==NULL)
  {
    cerr<<"Error: BasePhraseModel pointer could not be instantiated"<<endl;
    return ERROR;
  }

  swModelInfoPtr=new SwModelInfo;
  swModelInfoPtr->swAligModelPtr=new CURR_SWM_TYPE;
  swModelInfoPtr->invSwAligModelPtr=new CURR_SWM_TYPE;
  langModelInfoPtr->wpModelPtr=new WordPenaltyModel;
  BaseErrorCorrectionModel* ecModelPtr=new CURR_ECM_TYPE();
  BaseLogLinWeightUpdater* llWeightUpdaterPtr=new KbMiraLlWu;

      // Instantiate smt model
  smtModelPtr=new CURR_MODEL_TYPE();
      // Link pointers
  smtModelPtr->link_ll_weight_upd(llWeightUpdaterPtr);
  _phraseBasedTransModel<CURR_MODEL_TYPE::Hypothesis>* base_pbtm_ptr=dynamic_cast<_phraseBasedTransModel<CURR_MODEL_TYPE::Hypothesis>* >(smtModelPtr);
  if(base_pbtm_ptr)
  {
    base_pbtm_ptr->link_lm_info(langModelInfoPtr);
    base_pbtm_ptr->link_pm_info(phrModelInfoPtr);
  }
  _phrSwTransModel<CURR_MODEL_TYPE::Hypothesis>* base_pbswtm_ptr=dynamic_cast<_phrSwTransModel<CURR_MODEL_TYPE::Hypothesis>* >(smtModelPtr);
  if(base_pbswtm_ptr)
  {
    base_pbswtm_ptr->link_swm_info(swModelInfoPtr);
  }
      // Set smt model weights
  if(!pars.tmWeightVec.empty())
    smtModelPtr->setWeights(pars.tmWeightVec);
  
      // Print SMT model weights
  cout<<"- SMT model weights= ";
  smtModelPtr->printWeights(cout);
  cout<<endl;

      // Instantiate assisted translator
  BaseAssistedTrans<CURR_MODEL_TYPE>* assistedTransPtr=new CURR_AT_TYPE<CURR_MODEL_TYPE>();

      // Set assisted translator weights
  if(!pars.catWeightVec.empty())
    assistedTransPtr->setWeights(pars.catWeightVec);

      // Print assisted translator weights
  WgUncoupledAssistedTrans<CURR_MODEL_TYPE>* wgUncoupledAssistedTransPtr=dynamic_cast<WgUncoupledAssistedTrans<CURR_MODEL_TYPE>*>(assistedTransPtr);
  if(!wgUncoupledAssistedTransPtr)
  {
    cout<<"- Assisted translator weights= ";
    assistedTransPtr->printWeights(cout);
    cout << endl;
  }
  else
  {
    BaseEcmForWg<CURR_ECM_TYPE::EcmScoreInfo>* base_ecm_wg_ptr=dynamic_cast<BaseEcmForWg<CURR_ECM_TYPE::EcmScoreInfo>* >(ecModelPtr);
    if(base_ecm_wg_ptr)
    {
      cout<<"- Assisted translator weights= ";
      assistedTransPtr->printWeights(cout);
      cout << endl;
    }
    else
    {
      cout<<"Warning: current error correcting model cannot be combined with word-graph based assisted translators"<<endl;
    }
  }

      // Set error correction model weights
  if(!pars.ecmWeightVec.empty())
    ecModelPtr->setWeights(pars.ecmWeightVec);

      // Print error correction model weights
  cout<<"- Error correction model weights= ";
  ecModelPtr->printWeights(cout);
  cout<<endl;
  
      // Delete pointers
  delete langModelInfoPtr->lModelPtr;
  delete langModelInfoPtr->wpModelPtr;
  delete langModelInfoPtr;
  delete phrModelInfoPtr->invPbModelPtr;
  delete phrModelInfoPtr;
  delete swModelInfoPtr->swAligModelPtr;
  delete swModelInfoPtr->invSwAligModelPtr;
  delete swModelInfoPtr;
  delete smtModelPtr;
  delete llWeightUpdaterPtr;
  delete ecModelPtr;
  delete assistedTransPtr;

      // Release class factories
  dynClassFactoryHandler.release_smt(pars.verbosity);

  return OK;
}

//--------------- handleParameters function
int handleParameters(int argc,
                     char *argv[],
                     thot_get_ll_weights_pars& pars)
{
  if(readOption(argc,argv,"--version")==OK)
  {
    version();
    return ERROR;
  }
  if(readOption(argc,argv,"--help")==OK)
  {
    printUsage();
    return ERROR;   
  }
  if(readOption(argc,argv,"-v")==OK)
  {
    pars.verbosity=1;
  }
  else
    pars.verbosity=0;
      
  if(takeParameters(argc,argv,pars)==ERROR)
  {
    return ERROR;
  }
  else
  {
    return OK;
  }
}

//--------------- takeParameters function
int takeParameters(int argc,
                   char *argv[],
                   thot_get_ll_weights_pars& pars)
{
      // Check if a configuration file was provided
  std::string cfgFileName;
  int err=readSTLstring(argc,argv, "-c", &cfgFileName);
  if(!err)
  {
        // Process configuration file
    err=takeParametersFromCfgFile(cfgFileName,pars);
    if(err==ERROR) return ERROR;
  }
  else
  {
//    cerr<<"Warning: -c option was not given (execute --help for additional information)"<<endl;
    return OK;
  }
      // process command line parameters
  takeParametersGivenArgcArgv(argc,argv,pars);
  return OK;
}

//--------------- processParameters function
int takeParametersFromCfgFile(std::string cfgFileName,
                              thot_get_ll_weights_pars& pars)
{
      // Extract parameters from configuration file
    std::string comment="#";
    int cfgFileArgc;
    Vector<std::string> cfgFileArgvStl;
    int ret=extractParsFromFile(cfgFileName.c_str(),cfgFileArgc,cfgFileArgvStl,comment);
    if(ret==ERROR) return ERROR;

        // Create argv for cfg file
    char** cfgFileArgv=(char**) malloc(cfgFileArgc*sizeof(char*));
    for(unsigned int i=0;i<cfgFileArgvStl.size();++i)
    {
      cfgFileArgv[i]=(char*) malloc((cfgFileArgvStl[i].size()+1)*sizeof(char));
      strcpy(cfgFileArgv[i],cfgFileArgvStl[i].c_str());
    }
        // Process extracted parameters
    takeParametersGivenArgcArgv(cfgFileArgc,cfgFileArgv,pars);

        // Release allocated memory
    for(unsigned int i=0;i<cfgFileArgvStl.size();++i)
    {
      free(cfgFileArgv[i]);
    }
    free(cfgFileArgv);

        // Return without error
    return OK;
}

//--------------- processParameters function
void takeParametersGivenArgcArgv(int argc,
                                 char *argv[],
                                 thot_get_ll_weights_pars& pars)
{
      // Take -tmw parameter
  readFloatSeq(argc,argv, "-tmw", pars.tmWeightVec);
      // Take -catw parameter
  readFloatSeq(argc,argv, "-catw", pars.catWeightVec);
      // Take -ecw parameter
  readFloatSeq(argc,argv, "-ecw", pars.ecmWeightVec);
}

//---------------
void printUsage(void)
{
  cerr << "thot_get_ll_weights [-c <string>] [-v]"<<endl;
  cerr << "                    [--help] [--version]"<<endl<<endl;
  cerr << "-c <string>           : Configuration file."<<endl;
  cerr << "-v                    : Enable verbose model."<<endl;
  cerr << "--help                : Display this help and exit."<<endl;
  cerr << "--version             : Output version information and exit."<<endl;
}

//---------------
void version(void)
{
  cerr<<"thot_get_ll_weights is part of the thot package "<<endl;
  cerr<<"thot version "<<THOT_VERSION<<endl;
  cerr<<"thot is GNU software written by Daniel Ortiz"<<endl;
}
