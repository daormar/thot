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
/* Module: thot_lm_weight_upd.cc                                    */
/*                                                                  */
/* Definitions file: thot_lm_weight_upd.cc                          */
/*                                                                  */
/* Description: Implements a language model weight updater given a  */
/*              development corpus.                                 */
/*                                                                  */
/********************************************************************/

/**
 * @file thot_lm_weight_upd.cc
 *
 * @brief Implements a language model weight updater given a development
 * corpus.
 */

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "_incrInterpNgramLM.h"
#include "_incrJelMerNgramLM.h"
#include "BaseNgramLM.h"
#include "DynClassFileHandler.h"
#include "SimpleDynClassLoader.h"
#include "ErrorDefs.h"
#include "options.h"
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

//--------------- Constants ------------------------------------------

struct thot_lmwu_pars
{
  std::string fileWithCorpus;
  std::string langModelFilePrefix;
  int verbosity;
};

//--------------- Function Declarations ------------------------------

int init_lm(int verbosity);
void release_lm(int verbosity);
int handleParameters(int argc,
                     char *argv[],
                     thot_lmwu_pars& pars);
int takeParameters(int argc,
                   char *argv[],
                   thot_lmwu_pars& pars);
int checkParameters(thot_lmwu_pars& pars);
int update_lm_weights(const thot_lmwu_pars& pars);
int update_lm_weights_jel_mer(const thot_lmwu_pars& pars);
  int update_lm_weights_interp(const thot_lmwu_pars& pars);
void printUsage(void);
void version(void);

//--------------- Global variables -----------------------------------

SimpleDynClassLoader<BaseNgramLM<Vector<WordIndex> > > baseNgramLMDynClassLoader;
BaseNgramLM<Vector<WordIndex> >* lm;
_incrInterpNgramLM<Count,Count>* incrInterpNgramLmPtr;
_incrJelMerNgramLM<Count,Count>* incrJelMerLmPtr;

//--------------- Function Definitions -------------------------------

//--------------------------------
int main(int argc,char *argv[])
{
  thot_lmwu_pars pars;

  if(handleParameters(argc,argv,pars)==ERROR)
  {
    return ERROR;
  }
  else
  {
        // Print parameters
    cerr<<"-lm option is "<<pars.langModelFilePrefix<<endl;
    cerr<<"-c option is "<<pars.fileWithCorpus<<endl;
    cerr<<"-v option is "<<pars.verbosity<<endl;

        // Initialize language model
    init_lm(pars.verbosity);

        // Check if the model has weights to be updated
    incrJelMerLmPtr=dynamic_cast<_incrJelMerNgramLM<Count,Count>* >(lm);
    incrInterpNgramLmPtr=dynamic_cast<_incrInterpNgramLM<Count,Count>* >(lm);
    if(!incrJelMerLmPtr && !incrInterpNgramLmPtr)
    {
      cerr<<"Current model does not have weights to be updated"<<endl;
      release_lm(pars.verbosity);
      return OK;
    }
        // Update language model weights
    int retVal=update_lm_weights(pars);

        // Release language model
    release_lm(pars.verbosity);
    
    return retVal;
  }
}

//--------------------------------
int update_lm_weights(const thot_lmwu_pars& pars)
{
  if(incrJelMerLmPtr)
  {
    return update_lm_weights_jel_mer(pars);
  }
  else
  {
    if(incrInterpNgramLmPtr)
    {
      return update_lm_weights_interp(pars);
    }
    else
      return ERROR;
  }
}

//---------------
int update_lm_weights_jel_mer(const thot_lmwu_pars& pars)
{
      // Load model
  int retVal=incrJelMerLmPtr->load(pars.langModelFilePrefix.c_str());
  if(retVal==ERROR)
    return ERROR;
  
      // Update weights
  retVal=incrJelMerLmPtr->updateModelWeights(pars.fileWithCorpus.c_str(),pars.verbosity);
  if(retVal==ERROR)
    return ERROR;

      // Print updated weights
  retVal=incrJelMerLmPtr->printWeights(pars.langModelFilePrefix.c_str());
  if(retVal==ERROR)
    return ERROR;
  
  return OK;  
}

//---------------
int update_lm_weights_interp(const thot_lmwu_pars& pars)
{
      // Load model
  int retVal=incrInterpNgramLmPtr->load(pars.langModelFilePrefix.c_str());
  if(retVal==ERROR)
    return ERROR;
      
      // Update weights
  retVal=incrInterpNgramLmPtr->updateModelWeights(pars.fileWithCorpus.c_str(),pars.verbosity);
  if(retVal==ERROR)
    return ERROR;
      
      // Print updated weights
  retVal=incrInterpNgramLmPtr->printWeights(pars.langModelFilePrefix.c_str());
  if(retVal==ERROR)
    return ERROR;
      
  return OK;     
}

//---------------
int init_lm(int verbosity)
{
      // Initialize dynamic class file handler
  DynClassFileHandler dynClassFileHandler;
  if(dynClassFileHandler.load(THOT_MASTER_INI_PATH,verbosity)==ERROR)
  {
    cerr<<"Error while loading ini file"<<endl;
    return ERROR;
  }
      // Define variables to obtain base class infomation
  std::string baseClassName;
  std::string soFileName;
  std::string initPars;

      ////////// Obtain info for BaseNgramLM class
  baseClassName="BaseNgramLM";
  if(dynClassFileHandler.getInfoForBaseClass(baseClassName,soFileName,initPars)==ERROR)
  {
    cerr<<"Error: ini file does not contain information about "<<baseClassName<<" class"<<endl;
    cerr<<"Please check content of master.ini file or execute \"thot_handle_ini_files -r\" to reset it"<<endl;
    return ERROR;
  }
   
      // Load class derived from BaseSwAligModel dynamically
  if(!baseNgramLMDynClassLoader.open_module(soFileName,verbosity))
  {
    cerr<<"Error: so file ("<<soFileName<<") could not be opened"<<endl;
    return ERROR;
  }

  lm=baseNgramLMDynClassLoader.make_obj(initPars);
  if(lm==NULL)
  {
    cerr<<"Error: BaseNgramLM pointer could not be instantiated"<<endl;
    baseNgramLMDynClassLoader.close_module();
    
    return ERROR;
  }

  return OK;
}

//---------------
void release_lm(int verbosity)
{
  delete lm;
  baseNgramLMDynClassLoader.close_module(verbosity);
}

//--------------------------------
int handleParameters(int argc,
                     char *argv[],
                     thot_lmwu_pars& pars)
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
                   thot_lmwu_pars& pars)
{
      // Take language model file name
  int err=readSTLstring(argc,argv, "-lm", &pars.langModelFilePrefix);
  if(err==ERROR)
    return ERROR;
  
      // Take language model file name
  err=readSTLstring(argc,argv, "-c", &pars.fileWithCorpus);
  if(err==ERROR)
    return ERROR;

  if(readOption(argc,argv,"-v")==OK)
    pars.verbosity=true;
  else
    pars.verbosity=false;
    
  return OK;
}

//--------------------------------
int checkParameters(thot_lmwu_pars& pars)
{  
  if(pars.langModelFilePrefix.empty())
  {
    cerr<<"Error: parameter -lm not given!"<<endl;
    return ERROR;   

  }

  if(pars.fileWithCorpus.empty())
  {
    cerr<<"Error: parameter -c not given!"<<endl;
    return ERROR;   
  }
  
  return OK;
}

//--------------------------------
void printUsage(void)
{
  cerr<<"thot_lm_weight_upd -lm <string> -c <string> [-v]"<<endl;
  cerr<<"                   [--help] [--version]"<<endl;
  cerr<<endl;
  cerr<<"-lm <string>       Prefix of language model files."<<endl;
  cerr<<"                   (Warning: current weights will be overwritten)."<<endl;
  cerr<<"-c <string>        Development corpus."<<endl;
  cerr<<"-v                 Enable verbose mode."<<endl;
  cerr<<"--help             Display this help and exit."<<endl;
  cerr<<"--version          Output version information and exit."<<endl;
}

//--------------------------------
void version(void)
{
  cerr<<"thot_lm_weight_upd is part of the thot package"<<endl;
  cerr<<"thot version "<<THOT_VERSION<<endl;
  cerr<<"thot is GNU software written by Daniel Ortiz"<<endl;
}
