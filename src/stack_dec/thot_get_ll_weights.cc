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

#include THOT_SMTMODEL_H // Define SmtModel type. It is set in
                              // configure by checking SMTMODEL_H
                              // variable (default value: SmtModel.h)
#include "BasePbTransModel.h"
#include "_phrSwTransModel.h"
#include "_phraseBasedTransModel.h"
#include "SwModelInfo.h"
#include "PhraseModelInfo.h"
#include "LangModelInfo.h"
#include "BaseTranslationConstraints.h"
#include "BaseLogLinWeightUpdater.h"
#include "BaseScorer.h"
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
BaseErrorCorrectionModel* ecModelPtr;
BaseTranslationConstraints* trConstraintsPtr;
BaseLogLinWeightUpdater* llWeightUpdaterPtr;
BaseScorer* scorerPtr;
BasePbTransModel<SmtModel::Hypothesis>* smtModelPtr;
BaseWgProcessorForAnlp* wgpPtr;
BaseAssistedTrans<SmtModel>* assistedTransPtr;

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
      // Show static types
  if(pars.verbosity>=1)
  {
    cerr<<"Static types:"<<endl;
    cerr<<"- SMT model type (SmtModel): "<<SMT_MODEL_TYPE_NAME<<" ("<<THOT_SMTMODEL_H<<")"<<endl;
    cerr<<"- Language model state (LM_Hist): "<<LM_STATE_TYPE_NAME<<" ("<<THOT_LM_STATE_H<<")"<<endl;
    cerr<<"- Partial probability information for single word models (PpInfo): "<<PPINFO_TYPE_NAME<<" ("<<THOT_PPINFO_H<<")"<<endl;
  }
  
      // Initialize class factories
  int err=dynClassFactoryHandler.init_smt_and_imt(THOT_MASTER_INI_PATH,pars.verbosity);
  if(err==ERROR)
    return ERROR;

  langModelInfoPtr=new LangModelInfo;
  langModelInfoPtr->wpModelPtr=dynClassFactoryHandler.baseWordPenaltyModelDynClassLoader.make_obj(dynClassFactoryHandler.baseWordPenaltyModelInitPars);
  if(langModelInfoPtr->wpModelPtr==NULL)
  {
    cerr<<"Error: BaseWordPenaltyModel pointer could not be instantiated"<<endl;
    return ERROR;
  }

  langModelInfoPtr->lModelPtr=dynClassFactoryHandler.baseNgramLMDynClassLoader.make_obj(dynClassFactoryHandler.baseNgramLMInitPars);
  if(langModelInfoPtr->lModelPtr==NULL)
  {
    cerr<<"Error: BaseNgramLM pointer could not be instantiated"<<endl;
    return ERROR;
  }

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

  ecModelPtr=dynClassFactoryHandler.baseErrorCorrectionModelDynClassLoader.make_obj(dynClassFactoryHandler.baseErrorCorrectionModelInitPars);
  if(ecModelPtr==NULL)
  {
    cerr<<"Error: BaseErrorCorrectionModel pointer could not be instantiated"<<endl;
    return ERROR;
  }

  scorerPtr=dynClassFactoryHandler.baseScorerDynClassLoader.make_obj(dynClassFactoryHandler.baseScorerInitPars);
  if(scorerPtr==NULL)
  {
    cerr<<"Error: BaseScorer pointer could not be instantiated"<<endl;
    return ERROR;
  }

  llWeightUpdaterPtr=dynClassFactoryHandler.baseLogLinWeightUpdaterDynClassLoader.make_obj(dynClassFactoryHandler.baseLogLinWeightUpdaterInitPars);
  if(llWeightUpdaterPtr==NULL)
  {
    cerr<<"Error: BaseLogLinWeightUpdater pointer could not be instantiated"<<endl;
    return ERROR;
  }

      // Link scorer to weight updater
  if(!llWeightUpdaterPtr->link_scorer(scorerPtr))
  {
    cerr<<"Error: BaseLogLinWeightUpdater pointer could not be instantiated"<<endl;
    return ERROR;
  }

  trConstraintsPtr=dynClassFactoryHandler.baseTranslationConstraintsDynClassLoader.make_obj(dynClassFactoryHandler.baseTranslationConstraintsInitPars);
  if(trConstraintsPtr==NULL)
  {
    cerr<<"Error: BaseTranslationConstraints pointer could not be instantiated"<<endl;
    return ERROR;
  }

      // Instantiate smt model
  smtModelPtr=new SmtModel();
      // Link pointers
  smtModelPtr->link_ll_weight_upd(llWeightUpdaterPtr);
  smtModelPtr->link_trans_constraints(trConstraintsPtr);
  _phraseBasedTransModel<SmtModel::Hypothesis>* base_pbtm_ptr=dynamic_cast<_phraseBasedTransModel<SmtModel::Hypothesis>* >(smtModelPtr);
  if(base_pbtm_ptr)
  {
    base_pbtm_ptr->link_lm_info(langModelInfoPtr);
    base_pbtm_ptr->link_pm_info(phrModelInfoPtr);
  }
  _phrSwTransModel<SmtModel::Hypothesis>* base_pbswtm_ptr=dynamic_cast<_phrSwTransModel<SmtModel::Hypothesis>* >(smtModelPtr);
  if(base_pbswtm_ptr)
  {
    base_pbswtm_ptr->link_swm_info(swModelInfoPtr);
  }
      // Set smt model weights
  if(!pars.tmWeightVec.empty())
    smtModelPtr->setWeights(pars.tmWeightVec);
  
      // Print smt model weights
  cout<<"- SMT model weights= ";
  smtModelPtr->printWeights(cout);
  cout<<endl;

      // Check if error correction model is valid for word graphs
  wgpPtr=dynClassFactoryHandler.baseWgProcessorForAnlpDynClassLoader.make_obj(dynClassFactoryHandler.baseWgProcessorForAnlpInitPars);
  if(wgpPtr==NULL)
  {
    cerr<<"Error: BaseWgProcessorForAnlp pointer could not be instantiated"<<endl;
    return ERROR;
  }

      // Instantiate assisted translator
  assistedTransPtr=dynClassFactoryHandler.baseAssistedTransDynClassLoader.make_obj(dynClassFactoryHandler.baseAssistedTransInitPars);
  if(assistedTransPtr==NULL)
  {
    cerr<<"Error: BaseAssistedTrans pointer could not be instantiated"<<endl;
    return ERROR;
  }

      // Set assisted translator weights
  if(!pars.catWeightVec.empty())
    assistedTransPtr->setWeights(pars.catWeightVec);

      // Print assisted translator weights
  WgUncoupledAssistedTrans<SmtModel>* wgUncoupledAssistedTransPtr=dynamic_cast<WgUncoupledAssistedTrans<SmtModel>*>(assistedTransPtr);
  if(!wgUncoupledAssistedTransPtr)
  {
    cout<<"- Assisted translator weights= ";
    assistedTransPtr->printWeights(cout);
    cout << endl;
  }
  else
  {
    if(wgpPtr->link_ecm_wg(ecModelPtr))
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
  delete scorerPtr;
  delete llWeightUpdaterPtr;
  delete trConstraintsPtr;
  delete ecModelPtr;
  delete wgpPtr;
  delete assistedTransPtr;

      // Release class factories
  dynClassFactoryHandler.release_smt_and_imt(pars.verbosity);

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
