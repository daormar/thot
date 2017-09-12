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
int init_lm(std::string soFileName,
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
SimpleDynClassLoader<BaseNgramLM<std::vector<WordIndex> > > baseNgramLMDynClassLoader;
BaseNgramLM<std::vector<WordIndex> >* lm;
_incrInterpNgramLM* incrInterpNgramLmPtr;
_incrJelMerNgramLM<Count,Count>* incrJelMerLmPtr;

//--------------- Function Definitions -------------------------------

//--------------------------------
int main(int argc,char *argv[])
{
  thot_lmwu_pars pars;

  if(handleParameters(argc,argv,pars)==THOT_ERROR)
  {
    return THOT_ERROR;
  }
  else
  {
        // Print parameters
    std::cerr<<"-lm option is "<<pars.langModelFilesPrefix<<std::endl;
    std::cerr<<"-c option is "<<pars.fileWithCorpus<<std::endl;
    std::cerr<<"-v option is "<<pars.verbosity<<std::endl;

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
                   thot_lmwu_pars& pars)
{
      // Take language model file name
  int err=readSTLstring(argc,argv, "-lm", &pars.langModelFilesPrefix);
  if(err==THOT_ERROR)
    return THOT_ERROR;
  
      // Take language model file name
  err=readSTLstring(argc,argv, "-c", &pars.fileWithCorpus);
  if(err==THOT_ERROR)
    return THOT_ERROR;

  if(readOption(argc,argv,"-v")==THOT_OK)
    pars.verbosity=true;
  else
    pars.verbosity=false;
    
  return THOT_OK;
}

//--------------------------------
int checkParameters(thot_lmwu_pars& pars)
{  
  if(pars.langModelFilesPrefix.empty())
  {
    std::cerr<<"Error: parameter -lm not given!"<<std::endl;
    return THOT_ERROR;   

  }

  if(pars.fileWithCorpus.empty())
  {
    std::cerr<<"Error: parameter -c not given!"<<std::endl;
    return THOT_ERROR;   
  }
  
  return THOT_OK;
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
    std::cerr<<"Processing language model descriptor: "<<pars.langModelFilesPrefix<<std::endl;
  }

      // Obtain info about translation model entries
  std::vector<ModelDescriptorEntry> modelDescEntryVec;
  if(extractModelEntryInfo(pars.langModelFilesPrefix,modelDescEntryVec)==THOT_OK)
  {
        // Process descriptor entries
    for(unsigned int i=0;i<modelDescEntryVec.size();++i)
    {
      int ret=process_lm_entry(pars.fileWithCorpus,modelDescEntryVec[i],pars.verbosity);
      if(ret==THOT_ERROR)
        return THOT_ERROR;
    }

    return THOT_OK;
  }
  else
  {
    return THOT_ERROR;
  }
}

//--------------------------------
int process_lm_files_prefix(const thot_lmwu_pars& pars)
{
  if(pars.verbosity)
  {
    std::cerr<<"Processing language model files prefix: "<<pars.langModelFilesPrefix<<std::endl;
  }

      // Obtain default model type
  std::string defaultLangSoFile;
  int ret=obtain_default_lm_type(defaultLangSoFile);
  if(ret==THOT_ERROR)
    return THOT_ERROR;
  
      // Create model descriptor entry
  ModelDescriptorEntry modelDescEntry;
  modelDescEntry.statusStr="main";
  modelDescEntry.modelInitInfo=defaultLangSoFile;
  modelDescEntry.modelFileName=pars.langModelFilesPrefix;
  modelDescEntry.absolutizedModelFileName=pars.langModelFilesPrefix;

      // Process entry
  ret=process_lm_entry(pars.fileWithCorpus,modelDescEntry,pars.verbosity);
  if(ret==THOT_ERROR)
    return THOT_ERROR;
  
  return THOT_OK;  
}

//--------------------------------
int obtain_default_lm_type(std::string& soFileName)
{
      // Define variables to obtain base class infomation
  std::string baseClassName;
  std::string initPars;

      ////////// Obtain info for BaseNgramLM class
  baseClassName="BaseNgramLM";
  if(dynClassFileHandler.getInfoForBaseClass(baseClassName,soFileName,initPars)==THOT_ERROR)
  {
    std::cerr<<"Error: ini file does not contain information about "<<baseClassName<<" class"<<std::endl;
    std::cerr<<"Please check content of master.ini file or execute \"thot_handle_ini_files -r\" to reset it"<<std::endl;
    return THOT_ERROR;
  }

  return THOT_OK;
}

//--------------------------------
int process_lm_entry(std::string corpusFile,
                     const ModelDescriptorEntry& modelDescEntry,
                     int verbosity)
{
      // Initialize language model
  init_lm(modelDescEntry.modelInitInfo,verbosity);

      // Check if the model has weights to be updated
  incrJelMerLmPtr=dynamic_cast<_incrJelMerNgramLM<Count,Count>* >(lm);
  incrInterpNgramLmPtr=dynamic_cast<_incrInterpNgramLM* >(lm);
  if(!incrJelMerLmPtr && !incrInterpNgramLmPtr)
  {
    std::cerr<<"Current model does not have weights to be updated"<<std::endl;
    release_lm(verbosity);
    return THOT_OK;
  }
      // Update language model weights
  int retVal=update_lm_weights(corpusFile,modelDescEntry.absolutizedModelFileName,verbosity);

      // Release language model
  release_lm(verbosity);
    
  return retVal;
}

//---------------
int init_lm(std::string soFileName,
            int verbosity)
{  
      // Open module
  if(!baseNgramLMDynClassLoader.open_module(soFileName,verbosity))
  {
    std::cerr<<"Error: so file ("<<soFileName<<") could not be opened"<<std::endl;
    return THOT_ERROR;
  }

      // Create lm file pointer
  lm=baseNgramLMDynClassLoader.make_obj("");

  if(lm==NULL)
  {
    std::cerr<<"Error: BaseNgramLM pointer could not be instantiated"<<std::endl;    
    return THOT_ERROR;
  }
    
  return THOT_OK;
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
      return THOT_OK;
  }
}

//---------------
int update_lm_weights_jel_mer(std::string corpusFile,
                              std::string modelFile,
                              int verbosity)
{
      // Load model
  int retVal=incrJelMerLmPtr->load(modelFile.c_str());
  if(retVal==THOT_ERROR)
    return THOT_ERROR;
  
      // Update weights
  retVal=incrJelMerLmPtr->updateModelWeights(corpusFile.c_str(),verbosity);
  if(retVal==THOT_ERROR)
    return THOT_ERROR;

      // Print updated weights
  retVal=incrJelMerLmPtr->printWeights(modelFile.c_str());
  if(retVal==THOT_ERROR)
    return THOT_ERROR;
  
  return THOT_OK;  
}

//---------------
int update_lm_weights_interp(std::string corpusFile,
                             std::string modelFile,
                             int verbosity)
{
      // Load model
  int retVal=incrInterpNgramLmPtr->load(modelFile.c_str());
  if(retVal==THOT_ERROR)
    return THOT_ERROR;
      
      // Update weights
  retVal=incrInterpNgramLmPtr->updateModelWeights(corpusFile.c_str(),verbosity);
  if(retVal==THOT_ERROR)
    return THOT_ERROR;
      
      // Print updated weights
  retVal=incrInterpNgramLmPtr->printWeights(modelFile.c_str());
  if(retVal==THOT_ERROR)
    return THOT_ERROR;
      
  return THOT_OK;     
}

//--------------------------------
void printUsage(void)
{
  std::cerr<<"thot_lm_weight_upd -lm <string> -c <string> [-v]"<<std::endl;
  std::cerr<<"                   [--help] [--version]"<<std::endl;
  std::cerr<<std::endl;
  std::cerr<<"-lm <string>       Prefix of language model files."<<std::endl;
  std::cerr<<"                   (Warning: current weights will be overwritten)."<<std::endl;
  std::cerr<<"-c <string>        Development corpus."<<std::endl;
  std::cerr<<"-v                 Enable verbose mode."<<std::endl;
  std::cerr<<"--help             Display this help and exit."<<std::endl;
  std::cerr<<"--version          Output version information and exit."<<std::endl;
}

//--------------------------------
void version(void)
{
  std::cerr<<"thot_lm_weight_upd is part of the thot package"<<std::endl;
  std::cerr<<"thot version "<<THOT_VERSION<<std::endl;
  std::cerr<<"thot is GNU software written by Daniel Ortiz"<<std::endl;
}
