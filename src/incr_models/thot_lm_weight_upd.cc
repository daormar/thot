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

#include THOT_LM_STATE_H // Define LM_State type. It is set in
                         // configure by checking LM_STATE_H
                         // variable (default value: LM_State.h)
#include "_incrInterpNgramLM.h"
#include "_incrJelMerNgramLM.h"
#include "BaseNgramLM.h"
#include "SimpleDynClassLoader.h"
#include "ModelDescriptorUtils.h"
#include "DynClassFileHandler.h"
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
  std::string langModelFilesPrefix;
  int verbosity;
};

//--------------- Function Declarations ------------------------------

int handleParameters(int argc,
                     char *argv[],
                     thot_lmwu_pars& pars);
int takeParameters(int argc,
                   char *argv[],
                   thot_lmwu_pars& pars);
int checkParameters(thot_lmwu_pars& pars);
int process_input(const thot_lmwu_pars& pars);
int process_lm_descriptor(const thot_lmwu_pars& pars);
int process_lm_files_prefix(const thot_lmwu_pars& pars);
int obtain_default_lm_type(std::string& soFileName);
int process_lm_entry(std::string corpusFile,
                     const ModelDescriptorEntry& modelDescEntry,
                     int verbosity);
int init_lm(std::string modelType,
            int verbosity);
void release_lm(int verbosity);
int update_lm_weights(std::string corpusFile,
                      std::string modelFile,
                      int verbosity);
int update_lm_weights_jel_mer(std::string corpusFile,
                              std::string modelFile,
                              int verbosity);
int update_lm_weights_interp(std::string corpusFile,
                             std::string modelFile,
                             int verbosity);
void printUsage(void);
void version(void);

//--------------- Global variables -----------------------------------

DynClassFileHandler dynClassFileHandler;
SimpleDynClassLoader<BaseNgramLM<Vector<WordIndex> > > baseNgramLMDynClassLoader;
BaseNgramLM<Vector<WordIndex> >* lm;
_incrInterpNgramLM* incrInterpNgramLmPtr;
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
    cerr<<"-lm option is "<<pars.langModelFilesPrefix<<endl;
    cerr<<"-c option is "<<pars.fileWithCorpus<<endl;
    cerr<<"-v option is "<<pars.verbosity<<endl;

    return process_input(pars);
  }
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
  int err=readSTLstring(argc,argv, "-lm", &pars.langModelFilesPrefix);
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
  if(pars.langModelFilesPrefix.empty())
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
int process_input(const thot_lmwu_pars& pars)
{
  std::string mainFileName;
  if(fileIsDescriptor(pars.langModelFilesPrefix,mainFileName))
    return process_lm_descriptor(pars);
  else
    return process_lm_files_prefix(pars);
}

//--------------------------------
int process_lm_descriptor(const thot_lmwu_pars& pars)
{
  if(pars.verbosity)
  {
    cerr<<"Processing language model descriptor: "<<pars.langModelFilesPrefix<<endl;
  }

      // Obtain info about translation model entries
  Vector<ModelDescriptorEntry> modelDescEntryVec;
  if(extractModelEntryInfo(pars.langModelFilesPrefix,modelDescEntryVec)==OK)
  {
        // Process descriptor entries
    for(unsigned int i=0;i<modelDescEntryVec.size();++i)
    {
      int ret=process_lm_entry(pars.fileWithCorpus,modelDescEntryVec[i],pars.verbosity);
      if(ret==ERROR)
        return ERROR;
    }

    return OK;
  }
  else
  {
    return ERROR;
  }
}

//--------------------------------
int process_lm_files_prefix(const thot_lmwu_pars& pars)
{
  if(pars.verbosity)
  {
    cerr<<"Processing language model files prefix: "<<pars.langModelFilesPrefix<<endl;
  }

      // Obtain default model type
  std::string defaultLangModelType;
  int ret=obtain_default_lm_type(defaultLangModelType);
  if(ret==ERROR)
    return ERROR;
  
      // Create model descriptor entry
  ModelDescriptorEntry modelDescEntry;
  modelDescEntry.statusStr="main";
  modelDescEntry.modelType=defaultLangModelType;
  modelDescEntry.modelFileName=pars.langModelFilesPrefix;
  modelDescEntry.absolutizedModelFileName=pars.langModelFilesPrefix;

      // Process entry
  ret=process_lm_entry(pars.fileWithCorpus,modelDescEntry,pars.verbosity);
  if(ret==ERROR)
    return ERROR;
  
  return OK;  
}

//--------------------------------
int obtain_default_lm_type(std::string& soFileName)
{
      // Define variables to obtain base class infomation
  std::string baseClassName;
  std::string initPars;

      ////////// Obtain info for BaseNgramLM class
  baseClassName="BaseNgramLM";
  if(dynClassFileHandler.getInfoForBaseClass(baseClassName,soFileName,initPars)==ERROR)
  {
    cerr<<"Error: ini file does not contain information about "<<baseClassName<<" class"<<endl;
    cerr<<"Please check content of master.ini file or execute \"thot_handle_ini_files -r\" to reset it"<<endl;
    return ERROR;
  }

  return OK;
}

//--------------------------------
int process_lm_entry(std::string corpusFile,
                     const ModelDescriptorEntry& modelDescEntry,
                     int verbosity)
{
      // Initialize language model
  init_lm(modelDescEntry.modelType,verbosity);

      // Check if the model has weights to be updated
  incrJelMerLmPtr=dynamic_cast<_incrJelMerNgramLM<Count,Count>* >(lm);
  incrInterpNgramLmPtr=dynamic_cast<_incrInterpNgramLM* >(lm);
  if(!incrJelMerLmPtr && !incrInterpNgramLmPtr)
  {
    cerr<<"Current model does not have weights to be updated"<<endl;
    release_lm(verbosity);
    return OK;
  }
      // Update language model weights
  int retVal=update_lm_weights(corpusFile,modelDescEntry.absolutizedModelFileName,verbosity);

      // Release language model
  release_lm(verbosity);
    
  return retVal;
}

//---------------
int init_lm(std::string modelType,
            int verbosity)
{  
      // Open module
  if(!baseNgramLMDynClassLoader.open_module(modelType,verbosity))
  {
    cerr<<"Error: so file ("<<modelType<<") could not be opened"<<endl;
    return ERROR;
  }

      // Create lm file pointer
  lm=baseNgramLMDynClassLoader.make_obj("");

  if(lm==NULL)
  {
    cerr<<"Error: BaseNgramLM pointer could not be instantiated"<<endl;    
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
int update_lm_weights(std::string corpusFile,
                      std::string modelFile,
                      int verbosity)
{
  if(incrJelMerLmPtr)
  {
    return update_lm_weights_jel_mer(corpusFile,modelFile,verbosity);
  }
  else
  {
    if(incrInterpNgramLmPtr)
    {
      return update_lm_weights_interp(corpusFile,modelFile,verbosity);
    }
    else
      return OK;
  }
}

//---------------
int update_lm_weights_jel_mer(std::string corpusFile,
                              std::string modelFile,
                              int verbosity)
{
      // Load model
  int retVal=incrJelMerLmPtr->load(modelFile.c_str());
  if(retVal==ERROR)
    return ERROR;
  
      // Update weights
  retVal=incrJelMerLmPtr->updateModelWeights(corpusFile.c_str(),verbosity);
  if(retVal==ERROR)
    return ERROR;

      // Print updated weights
  retVal=incrJelMerLmPtr->printWeights(modelFile.c_str());
  if(retVal==ERROR)
    return ERROR;
  
  return OK;  
}

//---------------
int update_lm_weights_interp(std::string corpusFile,
                             std::string modelFile,
                             int verbosity)
{
      // Load model
  int retVal=incrInterpNgramLmPtr->load(modelFile.c_str());
  if(retVal==ERROR)
    return ERROR;
      
      // Update weights
  retVal=incrInterpNgramLmPtr->updateModelWeights(corpusFile.c_str(),verbosity);
  if(retVal==ERROR)
    return ERROR;
      
      // Print updated weights
  retVal=incrInterpNgramLmPtr->printWeights(modelFile.c_str());
  if(retVal==ERROR)
    return ERROR;
      
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
