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
/* Module: thot_ms_alig                                             */
/*                                                                  */
/* Definitions file: thot_ms_alig.cc                                */
/*                                                                  */
/* Description: Implements a phrase-based alignment system          */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "_stackDecoderRec.h"
#include "BaseStackDecoder.h"
#include THOT_SMTMODEL_H // Define SmtModel type. It is set in
                         // configure by checking SMTMODEL_H
                         // variable (default value: SmtModel.h)
#include "FeatureHandler.h"
#include "_pbTransModel.h"
#include "_phrSwTransModel.h"
#include "_phraseBasedTransModel.h"
#include "BasePbTransModel.h"
#include "SwModelInfo.h"
#include "PhraseModelInfo.h"
#include "LangModelInfo.h"
#include "BaseTranslationMetadata.h"
#include "BaseLogLinWeightUpdater.h"
#include "ModelDescriptorUtils.h"
#include "DynClassFactoryHandler.h"
#include "ctimer.h"
#include "options.h"
#include "ErrorDefs.h"
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <vector>
#include <string>
#include <map>
#include <set>

//--------------- Constants ------------------------------------------

#define PALIG_W_DEFAULT 10

#ifdef MULTI_STACK_USE_GRAN
 #define PALIG_S_DEFAULT 128
#else
 #define PALIG_S_DEFAULT 10
#endif

#define PALIG_A_DEFAULT 10
#define PALIG_E_DEFAULT 5
#define PALIG_I_DEFAULT 1
#define PALIG_G_DEFAULT 0
#define PALIG_H_DEFAULT NO_HEURISTIC
#define PALIG_NOMON_DEFAULT 0

//--------------- Type definitions -----------------------------------

struct thot_ms_alig_pars
{
  bool p_option;
  bool cov_option;
  bool be;
  float W;
  int A,E,nomon,S,I,G,heuristic,verbosity;
  std::string sourceSentencesFile;
  std::string refSentencesFile;
  std::string languageModelFileName;
  std::string transModelPref;
  std::string wordGraphFileName;
  float wgPruningThreshold;
  std::vector<float> weightVec;

  thot_ms_alig_pars()
    {
      p_option=false;
      cov_option=false;
      W=PALIG_W_DEFAULT;
      S=PALIG_S_DEFAULT;
      A=PALIG_A_DEFAULT;
      E=PALIG_E_DEFAULT;
      nomon=PALIG_NOMON_DEFAULT;
      I=PALIG_I_DEFAULT;
      G=PALIG_G_DEFAULT;
      heuristic=PALIG_H_DEFAULT;
      be=0;
      wgPruningThreshold=DISABLE_WORDGRAPH;
      wgPruningThreshold=UNLIMITED_DENSITY;
      verbosity=0;
    }
};

//--------------- Function Declarations ------------------------------

int init_translator_legacy_impl(const thot_ms_alig_pars& tap);
void set_default_models(void);
int add_model_features(const thot_ms_alig_pars& tap);
int init_translator_feat_impl(const thot_ms_alig_pars& tap);
bool featureBasedImplIsEnabled(void);
int init_translator(const thot_ms_alig_pars& tap);
void release_translator_legacy_impl(void);
void release_translator_feat_impl(void);
void release_translator(void);
int align_corpus(const thot_ms_alig_pars& tap);
std::vector<std::string> stringToStringVector(std::string s);
void version(void);
void print_alig_a3_final(std::string srcstr,
                         std::string trgstr,
                         SmtModel::Hypothesis hyp,
                         unsigned int sentNo,
                         const thot_ms_alig_pars& tap);
int handleParameters(int argc,
                     char *argv[],
                     thot_ms_alig_pars& pars);
int takeParameters(int argc,
                   char *argv[],
                   thot_ms_alig_pars& tap);
int takeParametersFromCfgFile(std::string cfgFileName,
                              thot_ms_alig_pars& tap);
void takeParametersGivenArgcArgv(int argc,
                                 char *argv[],
                                 thot_ms_alig_pars& tap);
int checkParameters(const thot_ms_alig_pars& tap);
void printParameters(const thot_ms_alig_pars& tap);
void printUsage(void);

//--------------- Global variables -----------------------------------

DynClassFactoryHandler dynClassFactoryHandler;
LangModelInfo* langModelInfoPtr;
PhraseModelInfo* phrModelInfoPtr;
SwModelInfo* swModelInfoPtr;
BaseTranslationMetadata<SmtModel::HypScoreInfo>* trMetadataPtr;
BaseLogLinWeightUpdater* llWeightUpdaterPtr;
BasePbTransModel<SmtModel::Hypothesis>* smtModelPtr;
BaseStackDecoder<SmtModel>* stackDecoderPtr;
_stackDecoderRec<SmtModel>* stackDecoderRecPtr;

    // Variables related to feature-based implementation
FeatureHandler featureHandler;
bool featureBasedImplEnabled;

//--------------- Function Definitions -------------------------------

//---------------
int main(int argc, char *argv[])
{
      // Take and check parameters
  thot_ms_alig_pars tap;
  if(handleParameters(argc,argv,tap)==THOT_ERROR)
  {
    return THOT_ERROR;
  }
  else
  {
        // init translator    
    if(init_translator(tap)==THOT_ERROR)
    {      
      std::cerr<<"Error during the initialization of the translator"<<std::endl;
      return THOT_ERROR;
    }
    else
    {
      unsigned int ret;

      ret=align_corpus(tap);
      release_translator();
      if(ret==THOT_ERROR) return THOT_ERROR;
      else return THOT_OK;
    }
  }
}

//---------------
int init_translator(const thot_ms_alig_pars& tap)
{
      // Print library directory path for so files
  std::cerr<<StrProcUtils::getLibDirVarNameValue()<<" = "<<StrProcUtils::getLibDir()<<std::endl;

      // Determine which implementation is being used
  featureBasedImplEnabled=featureBasedImplIsEnabled();

      // Call the appropriate initialization for current implementation
  if(featureBasedImplEnabled)
    return init_translator_feat_impl(tap);
  else
    return init_translator_legacy_impl(tap);
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

//---------------
int init_translator_legacy_impl(const thot_ms_alig_pars& tap)
{
  int ret;
  
  std::cerr<<"\n- Initializing translator...\n\n";

      // Show static types
  std::cerr<<"Static types:"<<std::endl;
  std::cerr<<"- SMT model type (SmtModel): "<<SMT_MODEL_TYPE_NAME<<" ("<<THOT_SMTMODEL_H<<")"<<std::endl;
  std::cerr<<"- Language model state (LM_Hist): "<<LM_STATE_TYPE_NAME<<" ("<<THOT_LM_STATE_H<<")"<<std::endl;
  std::cerr<<"- Partial probability information for single word models (PpInfo): "<<PPINFO_TYPE_NAME<<" ("<<THOT_PPINFO_H<<")"<<std::endl;

        // Obtain info about translation model entries
  unsigned int numTransModelEntries;
  std::vector<ModelDescriptorEntry> modelDescEntryVec;
  if(extractModelEntryInfo(tap.transModelPref.c_str(),modelDescEntryVec)==THOT_OK)
    numTransModelEntries=modelDescEntryVec.size();
  else
    numTransModelEntries=1;

      // Initialize class factories
  ret=dynClassFactoryHandler.init_smt(THOT_MASTER_INI_PATH);
  if(ret==THOT_ERROR)
    return THOT_ERROR;

      // Create decoder variables
  langModelInfoPtr=new LangModelInfo;
  langModelInfoPtr->wpModelPtr=dynClassFactoryHandler.baseWordPenaltyModelDynClassLoader.make_obj(dynClassFactoryHandler.baseWordPenaltyModelInitPars);
  if(langModelInfoPtr->wpModelPtr==NULL)
  {
    std::cerr<<"Error: BaseWordPenaltyModel pointer could not be instantiated"<<std::endl;
    return THOT_ERROR;
  }

  langModelInfoPtr->lModelPtr=dynClassFactoryHandler.baseNgramLMDynClassLoader.make_obj(dynClassFactoryHandler.baseNgramLMInitPars);
  if(langModelInfoPtr->lModelPtr==NULL)
  {
    std::cerr<<"Error: BaseNgramLM pointer could not be instantiated"<<std::endl;
    return THOT_ERROR;
  }

  phrModelInfoPtr=new PhraseModelInfo;
  phrModelInfoPtr->invPbModelPtr=dynClassFactoryHandler.basePhraseModelDynClassLoader.make_obj(dynClassFactoryHandler.basePhraseModelInitPars);
  if(phrModelInfoPtr->invPbModelPtr==NULL)
  {
    std::cerr<<"Error: BasePhraseModel pointer could not be instantiated"<<std::endl;
    return THOT_ERROR;
  }

      // Add one swm pointer per each translation model entry
  swModelInfoPtr=new SwModelInfo;
  for(unsigned int i=0;i<numTransModelEntries;++i)
  {
    swModelInfoPtr->swAligModelPtrVec.push_back(dynClassFactoryHandler.baseSwAligModelDynClassLoader.make_obj(dynClassFactoryHandler.baseSwAligModelInitPars));
    if(swModelInfoPtr->swAligModelPtrVec[i]==NULL)
    {
      std::cerr<<"Error: BaseSwAligModel pointer could not be instantiated"<<std::endl;
      return THOT_ERROR;
    }
  }

      // Add one inverse swm pointer per each translation model entry
  for(unsigned int i=0;i<numTransModelEntries;++i)
  {
    swModelInfoPtr->invSwAligModelPtrVec.push_back(dynClassFactoryHandler.baseSwAligModelDynClassLoader.make_obj(dynClassFactoryHandler.baseSwAligModelInitPars));
    if(swModelInfoPtr->invSwAligModelPtrVec[i]==NULL)
    {
      std::cerr<<"Error: BaseSwAligModel pointer could not be instantiated"<<std::endl;
      return THOT_ERROR;
    }
  }

  llWeightUpdaterPtr=dynClassFactoryHandler.baseLogLinWeightUpdaterDynClassLoader.make_obj(dynClassFactoryHandler.baseLogLinWeightUpdaterInitPars);
  if(llWeightUpdaterPtr==NULL)
  {
    std::cerr<<"Error: BaseLogLinWeightUpdater pointer could not be instantiated"<<std::endl;
    return THOT_ERROR;
  }

  trMetadataPtr=dynClassFactoryHandler.baseTranslationMetadataDynClassLoader.make_obj(dynClassFactoryHandler.baseTranslationMetadataInitPars);
  if(trMetadataPtr==NULL)
  {
    std::cerr<<"Error: BaseTranslationMetadata pointer could not be instantiated"<<std::endl;
    return THOT_ERROR;
  }

      // Instantiate smt model
  smtModelPtr=new SmtModel();
  
      // Link translation constraints
  smtModelPtr->link_trans_metadata(trMetadataPtr);

      // Link language model, phrase model and single word model if
      // appliable
  _phraseBasedTransModel<SmtModel::Hypothesis>* phrbtm_ptr=dynamic_cast<_phraseBasedTransModel<SmtModel::Hypothesis>* >(smtModelPtr);
  if(phrbtm_ptr)
  {
    phrbtm_ptr->link_lm_info(langModelInfoPtr);
    phrbtm_ptr->link_pm_info(phrModelInfoPtr);
  }
  _phrSwTransModel<SmtModel::Hypothesis>* base_pbswtm_ptr=dynamic_cast<_phrSwTransModel<SmtModel::Hypothesis>* >(smtModelPtr);
  if(base_pbswtm_ptr)
  {
    base_pbswtm_ptr->link_swm_info(swModelInfoPtr);
  }

  if(phrbtm_ptr)
  {
    ret=phrbtm_ptr->loadLangModel(tap.languageModelFileName.c_str());
    if(ret==THOT_ERROR)
    {
      release_translator();
      return THOT_ERROR;
    }
    
    ret=phrbtm_ptr->loadAligModel(tap.transModelPref.c_str());
    if(ret==THOT_ERROR)
    {
      release_translator();
      return THOT_ERROR;
    }
  }

      // Set heuristic
  smtModelPtr->setHeuristic(tap.heuristic);

      // Set weights
  smtModelPtr->setWeights(tap.weightVec);
  smtModelPtr->printWeights(std::cerr);
  std::cerr<<std::endl;

      // Set model parameters
  smtModelPtr->set_W_par(tap.W);
  smtModelPtr->set_A_par(tap.A);
  smtModelPtr->set_U_par(tap.nomon);

      // Set verbosity
  smtModelPtr->setVerbosity(tap.verbosity);
    
      // Create a translator instance
  stackDecoderPtr=dynClassFactoryHandler.baseStackDecoderDynClassLoader.make_obj(dynClassFactoryHandler.baseStackDecoderInitPars);
  if(stackDecoderPtr==NULL)
  {
    std::cerr<<"Error: BaseStackDecoder pointer could not be instantiated"<<std::endl;
    return THOT_ERROR;
  }

      // Determine if the translator incorporates hypotheses recombination
  stackDecoderRecPtr=dynamic_cast<_stackDecoderRec<SmtModel>*>(stackDecoderPtr);

      // Link translation model
  ret=stackDecoderPtr->link_smt_model(smtModelPtr);
  if(ret==THOT_ERROR)
  {
    std::cerr<<"Error while linking smt model to decoder, revise master.ini file"<<std::endl;
    return THOT_ERROR;
  }
  
      // Set translator parameters
  stackDecoderPtr->set_S_par(tap.S);
  stackDecoderPtr->set_I_par(tap.I);
  stackDecoderPtr->set_G_par(tap.G);

      // Enable best score pruning if the decoder is not going to obtain
      // n-best translations or word-graphs
  if(tap.wgPruningThreshold==DISABLE_WORDGRAPH)
    stackDecoderPtr->useBestScorePruning(true);

      // Set breadthFirst flag
  stackDecoderPtr->set_breadthFirst(!tap.be);

  if(stackDecoderRecPtr)
  {
        // Enable word graph according to wgPruningThreshold
    if(tap.wordGraphFileName!="")
    {
      if(tap.wgPruningThreshold!=DISABLE_WORDGRAPH)
        stackDecoderRecPtr->enableWordGraph();
    }
  }
      // Set translator verbosity
  stackDecoderPtr->setVerbosity(tap.verbosity);

  return THOT_OK;
}

//---------------
void set_default_models(void)
{
  featureHandler.setWordPenSoFile(dynClassFactoryHandler.baseWordPenaltyModelSoFileName);
  featureHandler.setDefaultLangSoFile(dynClassFactoryHandler.baseNgramLMSoFileName);
  featureHandler.setDefaultTransSoFile(dynClassFactoryHandler.basePhraseModelSoFileName);
  featureHandler.setDefaultSingleWordSoFile(dynClassFactoryHandler.baseSwAligModelSoFileName);
}

//---------------
int add_model_features(const thot_ms_alig_pars& tap)
{
      // Add word penalty model feature
  int ret=featureHandler.addWpFeat(tap.verbosity);
  if(ret==THOT_ERROR)
    return THOT_ERROR;

      // Add language model features
  ret=featureHandler.addLmFeats(tap.languageModelFileName,tap.verbosity);
  if(ret==THOT_ERROR)
    return THOT_ERROR;

      // Add translation model features
  ret=featureHandler.addTmFeats(tap.transModelPref,tap.verbosity);
  if(ret==THOT_ERROR)
    return THOT_ERROR;

  return THOT_OK;
}

//---------------
int init_translator_feat_impl(const thot_ms_alig_pars& tap)
{
  int ret;
  
  std::cerr<<"\n- Initializing translator...\n\n";

      // Show static types
  std::cerr<<"Static types:"<<std::endl;
  std::cerr<<"- SMT model type (SmtModel): "<<SMT_MODEL_TYPE_NAME<<" ("<<THOT_SMTMODEL_H<<")"<<std::endl;
  std::cerr<<"- Language model state (LM_Hist): "<<LM_STATE_TYPE_NAME<<" ("<<THOT_LM_STATE_H<<")"<<std::endl;
  std::cerr<<"- Partial probability information for single word models (PpInfo): "<<PPINFO_TYPE_NAME<<" ("<<THOT_PPINFO_H<<")"<<std::endl;

      // Initialize class factories
  ret=dynClassFactoryHandler.init_smt(THOT_MASTER_INI_PATH);
  if(ret==THOT_ERROR)
    return THOT_ERROR;

      // Create decoder variables
  llWeightUpdaterPtr=dynClassFactoryHandler.baseLogLinWeightUpdaterDynClassLoader.make_obj(dynClassFactoryHandler.baseLogLinWeightUpdaterInitPars);
  if(llWeightUpdaterPtr==NULL)
  {
    std::cerr<<"Error: BaseLogLinWeightUpdater pointer could not be instantiated"<<std::endl;
    return THOT_ERROR;
  }

  trMetadataPtr=dynClassFactoryHandler.baseTranslationMetadataDynClassLoader.make_obj(dynClassFactoryHandler.baseTranslationMetadataInitPars);
  if(trMetadataPtr==NULL)
  {
    std::cerr<<"Error: BaseTranslationMetadata pointer could not be instantiated"<<std::endl;
    return THOT_ERROR;
  }

      // Instantiate smt model
  smtModelPtr=new SmtModel();
  
      // Link translation constraints
  smtModelPtr->link_trans_metadata(trMetadataPtr);

      // Link features information
  _pbTransModel<SmtModel::Hypothesis>* pbtm_ptr=dynamic_cast<_pbTransModel<SmtModel::Hypothesis>* >(smtModelPtr);
  if(pbtm_ptr)
    pbtm_ptr->link_feats_info(featureHandler.getFeatureInfoPtr());

      // Set default models for feature handler
  set_default_models();
  
      // Add model features
  ret=add_model_features(tap);
  if(ret==THOT_ERROR)
    return THOT_ERROR;
  
      // Set heuristic
  smtModelPtr->setHeuristic(tap.heuristic);

      // Set weights
  smtModelPtr->setWeights(tap.weightVec);
  smtModelPtr->printWeights(std::cerr);
  std::cerr<<std::endl;

      // Set model parameters
  smtModelPtr->set_W_par(tap.W);
  smtModelPtr->set_A_par(tap.A);
  smtModelPtr->set_U_par(tap.nomon);

      // Set verbosity
  smtModelPtr->setVerbosity(tap.verbosity);
    
      // Create a translator instance
  stackDecoderPtr=dynClassFactoryHandler.baseStackDecoderDynClassLoader.make_obj(dynClassFactoryHandler.baseStackDecoderInitPars);
  if(stackDecoderPtr==NULL)
  {
    std::cerr<<"Error: BaseStackDecoder pointer could not be instantiated"<<std::endl;
    return THOT_ERROR;
  }

      // Determine if the translator incorporates hypotheses recombination
  stackDecoderRecPtr=dynamic_cast<_stackDecoderRec<SmtModel>*>(stackDecoderPtr);

      // Link translation model
  ret=stackDecoderPtr->link_smt_model(smtModelPtr);
  if(ret==THOT_ERROR)
  {
    std::cerr<<"Error while linking smt model to decoder, revise master.ini file"<<std::endl;
    return THOT_ERROR;
  }

      // Set translator parameters
  stackDecoderPtr->set_S_par(tap.S);
  stackDecoderPtr->set_I_par(tap.I);
  stackDecoderPtr->set_G_par(tap.G);

      // Enable best score pruning if the decoder is not going to obtain
      // n-best translations or word-graphs
  if(tap.wgPruningThreshold==DISABLE_WORDGRAPH)
    stackDecoderPtr->useBestScorePruning(true);

      // Set breadthFirst flag
  stackDecoderPtr->set_breadthFirst(!tap.be);

  if(stackDecoderRecPtr)
  {
        // Enable word graph according to wgPruningThreshold
    if(tap.wordGraphFileName!="")
    {
      if(tap.wgPruningThreshold!=DISABLE_WORDGRAPH)
        stackDecoderRecPtr->enableWordGraph();
    }
  }
      // Set translator verbosity
  stackDecoderPtr->setVerbosity(tap.verbosity);
  
  return THOT_OK;
}

//---------------
void release_translator(void)
{
  if(featureBasedImplEnabled)
    release_translator_feat_impl();
  else
    release_translator_legacy_impl();
}

//---------------
void release_translator_legacy_impl(void)
{
  delete langModelInfoPtr->lModelPtr;
  delete langModelInfoPtr->wpModelPtr;
  delete langModelInfoPtr;
  delete phrModelInfoPtr->invPbModelPtr;
  delete phrModelInfoPtr;
  for(unsigned int i=0;i<swModelInfoPtr->swAligModelPtrVec.size();++i)
    delete swModelInfoPtr->swAligModelPtrVec[i];
  for(unsigned int i=0;i<swModelInfoPtr->invSwAligModelPtrVec.size();++i)
    delete swModelInfoPtr->invSwAligModelPtrVec[i];
  delete swModelInfoPtr;
  delete stackDecoderPtr;
  delete llWeightUpdaterPtr;
  delete trMetadataPtr;
  delete smtModelPtr;

      // Release class factory handler
  dynClassFactoryHandler.release_smt();
}

//---------------
void release_translator_feat_impl(void)
{
  delete stackDecoderPtr;
  delete llWeightUpdaterPtr;
  delete trMetadataPtr;
  delete smtModelPtr;

      // Delete features information
  featureHandler.clear();
  
      // Release class factory handler
  dynClassFactoryHandler.release_smt();
}

//---------------
int align_corpus(const thot_ms_alig_pars& tap)
{
  SmtModel::Hypothesis result;     // Results of the translation
  SmtModel::Hypothesis anotherTrans;     // Another results of the translation
  int sentNo=0;    
  double elapsed_ant,elapsed,ucpu,scpu,total_time=0;
      
  std::ifstream testCorpusFile;                // Test corpus file stream
  std::ifstream refCorpusFile;                 // reference corpus file stream
  std::string srcSentenceString,trgSentenceString,s;
  

      // Open test corpus file
  testCorpusFile.open(tap.sourceSentencesFile.c_str());
  if(testCorpusFile.fail())
  {
    std::cerr<<"Error while opening file with test sentences "<<tap.sourceSentencesFile<<std::endl;
    return THOT_ERROR;
  }
  testCorpusFile.seekg(0, std::ios::beg);

      // Open ref corpus file
  refCorpusFile.open(tap.refSentencesFile.c_str());
  if(refCorpusFile.fail())
  {
    std::cerr<<"Error while opening file with references "<<tap.refSentencesFile<<std::endl;
    return THOT_ERROR;
  }
  refCorpusFile.seekg(0, std::ios::beg);

  std::cerr<<"\n- Processing corpora...\n\n";

  if(!testCorpusFile)
  {
    std::cerr<<"Test corpus error!"<<std::endl;
    return THOT_ERROR;
  }
  else
  {
        // Align corpus sentences
    while(!testCorpusFile.eof())
    {
      getline(testCorpusFile,srcSentenceString);
      getline(refCorpusFile,trgSentenceString);

          // Discard last sentence pair if it is empty
      if(srcSentenceString=="" && trgSentenceString=="" && testCorpusFile.eof())
        break;

      ++sentNo;
        
      if(tap.verbosity)
      {
        std::cerr<<sentNo<<std::endl<<srcSentenceString<<std::endl;
        ctimer(&elapsed_ant,&ucpu,&scpu);
      }
       
          //------- Align sentence
      if(tap.p_option)
      {
            // Translate with prefix
        result=stackDecoderPtr->translateWithPrefix(srcSentenceString,trgSentenceString);
      }
      else
      {
        if(tap.cov_option)
        {
              // Verify model coverage
          result=stackDecoderPtr->verifyCoverageForRef(srcSentenceString,trgSentenceString);
        }
        else
        {
              // Translate with reference
          result=stackDecoderPtr->translateWithRef(srcSentenceString,trgSentenceString);
        }
      }
          //--------------------------
      if(tap.verbosity) ctimer(&elapsed,&ucpu,&scpu);
               
      print_alig_a3_final(srcSentenceString,trgSentenceString,result,sentNo,tap);
          
      if(tap.verbosity)
      {
        smtModelPtr->printHyp(result,std::cerr,tap.verbosity);
#         ifdef THOT_STATS
        stackDecoderPtr->printStats();
#         endif

        std::cerr<<"- Elapsed Time: "<<elapsed-elapsed_ant<<std::endl<<std::endl;
        total_time+=elapsed-elapsed_ant;
      }

      if(stackDecoderRecPtr)
      {
            // Print wordgraph if the -wg option was given
        if(tap.wordGraphFileName!="")
        {
          char wgFileNameForSent[256];
          sprintf(wgFileNameForSent,"%s_%06d",tap.wordGraphFileName.c_str(),sentNo);
          stackDecoderRecPtr->pruneWordGraph(tap.wgPruningThreshold);
          stackDecoderRecPtr->printWordGraph(wgFileNameForSent);
        }
      }
      
#ifdef THOT_ENABLE_GRAPH
      char printGraphFileName[256];
      ofstream outS;
      sprintf(printGraphFileName,"sent%d.graph_file",sentNo);
      outS.open(printGraphFileName,ios::out);
      if(!outS) std::cerr<<"Error while printing search graph to file."<<std::endl;
      else
      {
        stackDecoderPtr->printSearchGraphStream(outS);
        outS<<"Stack ID. Out\n";
        stackDecoderPtr->printGraphForHyp(result,outS);
        outS.close();        
      }
#endif        
    }
    testCorpusFile.close(); 
  }

  if(tap.verbosity)
  {
    std::cerr<<"- Time per sentence: "<<total_time/sentNo<<std::endl;
  }

  return THOT_OK;
}

//---------------------------------------
void print_alig_a3_final(std::string srcstr,
                         std::string trgstr,
                         SmtModel::Hypothesis hyp,
                         unsigned int sentNo,
                         const thot_ms_alig_pars& tap)
{
  SmtModel::Hypothesis::DataType dataType;
  std::vector<std::string> sysTrgVec;
  std::vector<std::string> trgVec;
    
  sysTrgVec=smtModelPtr->getTransInPlainTextVec(hyp);
  trgVec=stringToStringVector(trgstr);
  dataType=hyp.getData();
  std::cout<<"# "<<sentNo <<" ; Align. score= "<<hyp.getScore()<<std::endl;
  std::cout<<srcstr<<std::endl;
  std::cout<<"NULL ({ })";
  if(sysTrgVec!=trgVec && !tap.p_option)
  {
        // If the alignment is incomplete, align each target word with
        // each source word
    unsigned int srcsize=stringToStringVector(srcstr).size();
    for(unsigned int i=0;i<trgVec.size();++i)
    {
      std::cout<<" "<<trgVec[i]<<" ({ ";
      for(unsigned int j=1;j<=srcsize;++j) std::cout<<j<<" ";
      std::cout<<"})";
    }
    std::cout<<std::endl;
  }
  else
  {
    unsigned int i;
    i=1;
    for(unsigned int k=0;k<dataType.targetSegmentCuts.size();++k)
    {
      for(;i<=dataType.targetSegmentCuts[k];++i)
      {
        std::cout<<" "<<sysTrgVec[i-1]<<" ({ ";
        for(unsigned int j=dataType.sourceSegmentation[k].first;j<=dataType.sourceSegmentation[k].second;++j)
        {
          std::cout<<j<<" ";
        }
        std::cout<<"})";
      }
    }
    std::cout<<std::endl;
  }
}

//---------------
int handleParameters(int argc,
                     char *argv[],
                     thot_ms_alig_pars& tap)
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
  if(takeParameters(argc,argv,tap)==THOT_ERROR)
  {
    return THOT_ERROR;
  }
  else
  {
    if(checkParameters(tap)==THOT_OK)
    {
      printParameters(tap);
      return THOT_OK;
    }
    else
    {
      return THOT_ERROR;
    }
  }
}

//---------------
int takeParameters(int argc,
                   char *argv[],
                   thot_ms_alig_pars& tap)
{
      // Check if a configuration file was provided
  std::string cfgFileName;
  int err=readSTLstring(argc,argv, "-c", &cfgFileName);
  if(!err)
  {
        // Process configuration file
    err=takeParametersFromCfgFile(cfgFileName,tap);
    if(err==THOT_ERROR) return THOT_ERROR;
  }
      // process command line parameters
  takeParametersGivenArgcArgv(argc,argv,tap);
  return THOT_OK;
}

//---------------
int takeParametersFromCfgFile(std::string cfgFileName,
                              thot_ms_alig_pars& tap)
{
  std::cerr<<"Processing configuration file ("<<cfgFileName<<")"<<std::endl;

      // Extract parameters from configuration file
  std::string comment="#";
  int cfgFileArgc;
  std::vector<std::string> cfgFileArgvStl;
  int ret=extractParsFromFile(cfgFileName.c_str(),cfgFileArgc,cfgFileArgvStl,comment);
  if(ret==THOT_ERROR) return THOT_ERROR;

      // Create argv for cfg file
  char** cfgFileArgv=(char**) malloc(cfgFileArgc*sizeof(char*));
  for(unsigned int i=0;i<cfgFileArgvStl.size();++i)
  {
    cfgFileArgv[i]=(char*) malloc((cfgFileArgvStl[i].size()+1)*sizeof(char));
    strcpy(cfgFileArgv[i],cfgFileArgvStl[i].c_str());
  }
      // Process extracted parameters
  takeParametersGivenArgcArgv(cfgFileArgc,cfgFileArgv,tap);

      // Release allocated memory
  for(unsigned int i=0;i<cfgFileArgvStl.size();++i)
  {
    free(cfgFileArgv[i]);
  }
  free(cfgFileArgv);

      // Return without error
  return THOT_OK;
}

//---------------
void takeParametersGivenArgcArgv(int argc,
                                 char *argv[],
                                 thot_ms_alig_pars& tap)
{
 int err;

 err=readOption(argc,argv,"-p");
 if(err!=-1)
 {
   tap.p_option=true;
 }    

 err=readOption(argc,argv,"-cov");
 if(err!=-1)
 {
   tap.cov_option=true;
 }    
 
     // Take W parameter 
 err=readFloat(argc,argv, "-W", &tap.W);

     // Take S parameter 
 err=readInt(argc,argv, "-S", &(tap.S));

     // Take A parameter 
 err=readInt(argc,argv, "-A", &tap.A);

     // Take E parameter 
 err=readInt(argc,argv, "-E", &tap.E);

     // Take nomon parameter 
 err=readInt(argc,argv, "-nomon", &tap.nomon);

     // Take N parameter 
 err=readInt(argc,argv, "-I", &tap.I);

     // Take I parameter 
 err=readInt(argc,argv, "-G", &tap.G);

     // Take h parameter 
 err=readInt(argc,argv, "-h", &tap.heuristic);

     // Take language model file name
 err=readSTLstring(argc,argv, "-lm", &tap.languageModelFileName);

     // Take read table prefix 
 err=readSTLstring(argc,argv, "-tm", &tap.transModelPref);
 
     // Take file name with the test sentences 
 err=readSTLstring(argc,argv, "-t",&tap.sourceSentencesFile);

     // Take file name with the reference sentences
 err=readSTLstring(argc,argv, "-r",&tap.refSentencesFile);

        // Take -be option
 err=readOption(argc,argv,"-be");
 if(err!=-1)
 {
   tap.be=1;
 }      

     // Take -tmw parameter
 err=readFloatSeq(argc,argv, "-tmw", tap.weightVec);

      // Take -wg parameter
 err=readSTLstring(argc,argv, "-wg", &tap.wordGraphFileName);
 if(err!=-1)
 {
       // Take -wgp parameter 
   err=readFloat(argc,argv, "-wgp", &tap.wgPruningThreshold);
 }
 
     // Take verbosity parameter
 err=readOption(argc,argv,"-v");
 if(err==-1)
 {
       // -v not found
   err=readOption(argc,argv,"-v1");
   if(err==-1)
   {
         // -v1 not found
     err=readOption(argc,argv,"-v2");
     if(err==-1)
     {
           // -v2 not found
       tap.verbosity=0;
     }
     else
     {
           // -v2 found
       tap.verbosity=3;
     }
   }
   else
   {
         // -v1 found
     tap.verbosity=2;
   }
 }
 else
 {
       // -v found
   tap.verbosity=1;
 }
}

//---------------
int checkParameters(const thot_ms_alig_pars& tap)
{
  if(tap.languageModelFileName.empty())
  {
    std::cerr<<"Error: parameter -lm not given!"<<std::endl;
    return THOT_ERROR;   
  }
  
  if(tap.transModelPref.empty())
  {
    std::cerr<<"Error: parameter -tm not given!"<<std::endl;
    return THOT_ERROR;   
  }

  if(tap.sourceSentencesFile.empty())
  {
    std::cerr<<"Error: parameter -t not given!"<<std::endl;
    return THOT_ERROR;   
  }

  if(tap.refSentencesFile.empty())
  {
    std::cerr<<"Error: parameter -r not given!"<<std::endl;
    return THOT_ERROR;   
  }

  if(tap.p_option && tap.cov_option)
  {
     std::cerr<<"Error: -p and -cov options cannot be given simultaneously"<<std::endl;
     return THOT_ERROR;
  }

  return THOT_OK;
}

//---------------
void printParameters(const thot_ms_alig_pars& tap)
{
 std::cerr<<"p option: "<<tap.p_option<<std::endl;
 std::cerr<<"cov option: "<<tap.cov_option<<std::endl;
 std::cerr<<"W: "<<tap.W<<std::endl;   
 std::cerr<<"S: "<<tap.S<<std::endl;   
 std::cerr<<"A: "<<tap.A<<std::endl;
 std::cerr<<"E: "<<tap.E<<std::endl;
 std::cerr<<"I: "<<tap.I<<std::endl;
#ifdef MULTI_STACK_USE_GRAN
 std::cerr<<"G: "<<tap.G<<std::endl;
#endif
 std::cerr<<"h: "<<tap.heuristic<<std::endl;
 std::cerr<<"be: "<<tap.be<<std::endl;
 std::cerr<<"nomon: "<<tap.nomon<<std::endl;
 std::cerr<<"weight vector:";
 for(unsigned int i=0;i<tap.weightVec.size();++i)
   std::cerr<<" "<<tap.weightVec[i];
 std::cerr<<std::endl;
 std::cerr<<"lmfile: "<<tap.languageModelFileName<<std::endl;   
 std::cerr<<"tm files prefix: "<<tap.transModelPref<<std::endl;   
 std::cerr<<"test file: "<<tap.sourceSentencesFile<<std::endl;   
 std::cerr<<"ref file: "<<tap.refSentencesFile<<std::endl;
 if(tap.wordGraphFileName!="")
 {
   std::cerr<<"word graph file prefix: "<<tap.wordGraphFileName<<std::endl;
   if(tap.wgPruningThreshold==UNLIMITED_DENSITY)
     std::cerr<<"word graph pruning threshold: word graph density unrestricted"<<std::endl;
   else
     std::cerr<<"word graph pruning threshold: "<<tap.wgPruningThreshold<<std::endl;
 }
 else
 {
   std::cerr<<"word graph file prefix not given (wordgraphs will not be generated)"<<std::endl;
 }
 std::cerr<<"verbosity level: "<<tap.verbosity<<std::endl;
}

//---------------
std::vector<std::string> stringToStringVector(std::string s)
{
 std::vector<std::string> vs;	
 std::string aux="";
 unsigned int i;	

 for(i=0;i<s.size();++i)
 {
   if(s[i]!=' ') aux+=s[i];
   else
   {
     if(aux!="")
     {
       vs.push_back(aux); aux="";
     }
   }
 }
 
 if(aux!="") vs.push_back(aux); 	
 return vs;	
}

//---------------
void printUsage(void)
{
  std::cerr << "thot_ms_alig   [-c <string>] [-tm <string>] [-lm <string>]"<<std::endl;
  std::cerr << "               -t <string> -r <string>"<<std::endl;
  std::cerr << "               [-p|-cov] [-W <float>]"<<std::endl;
  std::cerr << "               [-S <int>] [-A <int>] [-E <int>] [-I <int>]"<<std::endl;
  std::cerr << "               [-G <int>] [-h <int>] [-be] [-nomon <int>]"<<std::endl;
  std::cerr << "               [-tmw <float> ... <float>]"<<std::endl;
  std::cerr << "               [-wg <string> [-wgp <float>] ]"<<std::endl;
  std::cerr << "               [-v|-v1|-v2] [--help] [--version]"<<std::endl<<std::endl;
  std::cerr << " -c <string>           : Configuration file (command-line options override"<<std::endl;
  std::cerr << "                         configuration file options)."<<std::endl;
  std::cerr << " -tm <string>          : Prefix of translation model files or model descriptor."<<std::endl;
  std::cerr << " -lm <string>          : Language model file name or model descriptor."<<std::endl;
  std::cerr << " -t <string>           : File with the test sentences."<<std::endl;
  std::cerr << " -r <string>           : File with the reference sentences."<<std::endl;
  std::cerr << " -p                    : Treat the reference sentences as prefixes."<<std::endl;
  std::cerr << " -cov                  : Verify model coverage for the reference sentence."<<std::endl;
  std::cerr << " -W <float>            : Maximum number of translation options to be considered"<<std::endl;
  std::cerr << "                         per each source phrase ("<<PALIG_W_DEFAULT<<" by default)."<<std::endl;
  std::cerr << " -S <int>              : Maximum number of hypotheses that can be stored in"<<std::endl;
  std::cerr << "                         each stack ("<<PALIG_S_DEFAULT<<" by default)."<<std::endl;    
  std::cerr << " -A <int>              : Maximum length in words of the source phrases to be"<<std::endl;
  std::cerr << "                         aligned ("<<PALIG_A_DEFAULT<<" by default)."<<std::endl;
  std::cerr << " -E <int>              : Constrain the target phrase length to be in the"<<std::endl;
  std::cerr << "                         interval [splen-<int> , splen+<int>] where splen is"<<std::endl;
  std::cerr << "                         the length of the source phrase to be aligned"<<std::endl;
  std::cerr << "                         ("<<PALIG_E_DEFAULT<<" by default)."<<std::endl;
  std::cerr << " -I <int>              : Number of hypotheses expanded at each iteration"<<std::endl;
  std::cerr << "                         ("<<PALIG_I_DEFAULT<<" by default)."<<std::endl;
#ifdef MULTI_STACK_USE_GRAN
  std::cerr << " -G <int>              : Granularity parameter ("<<PALIG_G_DEFAULT<<"by default)."<<std::endl;
#else
  std::cerr << " -G <int>              : Parameter not available with the given configuration."<<std::endl;
#endif
  std::cerr << " -h <int>              : Heuristic function used: "<<NO_HEURISTIC<<"->None, "<<LOCAL_T_HEURISTIC<<"->LOCAL_T, "<<std::endl;
  std::cerr << "                         "<<LOCAL_TD_HEURISTIC<<"->LOCAL_TD ("<<PALIG_H_DEFAULT<<" by default)."<<std::endl;
  std::cerr << " -be                   : Execute a best-first algorithm (breadth-first search"<<std::endl;
  std::cerr << "                         is executed by default)."<<std::endl;
  std::cerr << " -nomon <int>          : Perform a non-monotonic search, allowing the decoder"<<std::endl;
  std::cerr << "                         to skip up to <int> words from the last aligned source"<<std::endl;
  std::cerr << "                         words. If <int> is equal to zero, then a monotonic"<<std::endl;
  std::cerr << "                         search is performed ("<<PALIG_NOMON_DEFAULT<<" is the default value)."<<std::endl;
  std::cerr << " -tmw <float>...<float>: Set model weights, the number of weights and their"<<std::endl;
  std::cerr << "                         meaning depends on the model type (use --config"<<std::endl;
  std::cerr << "                         option)."<<std::endl;
  std::cerr << " -wg <string>          : Print word graph after each translation, the prefix" <<std::endl;
  std::cerr << "                         of the files is given as parameter."<<std::endl;
  std::cerr << " -wgp <float>          : Prune word-graph using the given threshold.\n";
  std::cerr << "                         Threshold=0 -> no pruning is performed.\n";
  std::cerr << "                         Threshold=1 -> only the best arc arriving to each\n";
  std::cerr << "                                        state is retained.\n";
  std::cerr << "                         If not given, the number of arcs is not\n";
  std::cerr << "                         restricted.\n";
  std::cerr << "                         not restricted."<<std::endl;
  std::cerr << " -v|-v1|-v2            : verbose modes."<<std::endl;
  std::cerr << " --help                : Display this help and exit."<<std::endl;
  std::cerr << " --version             : Output version information and exit."<<std::endl;
  std::cerr << " --config              : Show current configuration."<<std::endl;
}

//---------------
void version(void)
{
  std::cerr<<"thot_ms_alig is part of the thot package "<<std::endl;
  std::cerr<<"thot version "<<THOT_VERSION<<std::endl;
  std::cerr<<"thot is GNU software written by Daniel Ortiz"<<std::endl;
}
