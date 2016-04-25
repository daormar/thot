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
 * @brief implements a log-linear weight updater given a set of n-best
 * lists
 */

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "_phraseBasedTransModel.h"
#include "StackDecPbModelTypes.h"
#include "StackDecLmTypes.h"
#include "LangModelInfo.h"
#include "WordPenaltyModel.h"
#include "KbMiraLlWu.h"
#include "SmtModelTypes.h"
#include "StackDecSwModelTypes.h"
#include "BasePbTransModel.h"

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
  Vector<float> weightVec;
};

//--------------- Function Declarations ------------------------------

void get_ll_weights(const thot_get_ll_weights_pars& tdp);
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
void printConfig(void);

//--------------- Global variables -----------------------------------

PhraseModelInfo* phrModelInfoPtr;
LangModelInfo* langModelInfoPtr;
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
    get_ll_weights(pars);
    return OK;
  }
}

//--------------- get_ll_weights function
void get_ll_weights(const thot_get_ll_weights_pars& pars)
{
  phrModelInfoPtr=new PhraseModelInfo;
  phrModelInfoPtr->invPbModelPtr=new THOT_CURR_PBM_TYPE;
  langModelInfoPtr=new LangModelInfo;
  langModelInfoPtr->lModelPtr=new THOT_CURR_LM_TYPE;
  langModelInfoPtr->wpModelPtr=new WordPenaltyModel;
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
  
      // Set weights
  if(!pars.weightVec.empty())
    smtModelPtr->setWeights(pars.weightVec);
  smtModelPtr->printWeights(cout);
  cout<<endl;

        // Delete pointers
  delete phrModelInfoPtr->invPbModelPtr;
  delete phrModelInfoPtr;
  delete langModelInfoPtr->lModelPtr;
  delete langModelInfoPtr->wpModelPtr;
  delete langModelInfoPtr;
  delete smtModelPtr;
  delete llWeightUpdaterPtr;
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
  readFloatSeq(argc,argv, "-tmw", pars.weightVec);
}

//---------------
void printUsage(void)
{
  cerr << "thot_get_ll_weights [-c <string>]"<<endl;
  cerr << "                    [--help] [--version]"<<endl<<endl;
  cerr << "-c <string>           : Configuration file."<<endl;
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
