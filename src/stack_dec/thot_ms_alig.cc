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
#include "BasePbTransModel.h"
#include "_phrSwTransModel.h"
#include "_phraseBasedTransModel.h"
#include "SwModelInfo.h"
#include "PhraseModelInfo.h"
#include "LangModelInfo.h"
#include "BaseTranslationConstraints.h"
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

using namespace std;

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
  Vector<float> weightVec;

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

int init_translator(const thot_ms_alig_pars& tap);
void release_translator(void);
int align_corpus(const thot_ms_alig_pars& tap);
Vector<string> stringToStringVector(string s);
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
BaseTranslationConstraints* trConstraintsPtr;
BaseLogLinWeightUpdater* llWeightUpdaterPtr;
BasePbTransModel<SmtModel::Hypothesis>* smtModelPtr;
BaseStackDecoder<SmtModel>* stackDecoderPtr;
_stackDecoderRec<SmtModel>* stackDecoderRecPtr;

//--------------- Function Definitions -------------------------------

//---------------
int main(int argc, char *argv[])
{
      // Take and check parameters
  thot_ms_alig_pars tap;
  if(handleParameters(argc,argv,tap)==ERROR)
  {
    return ERROR;
  }
  else
  {
        // init translator    
    if(init_translator(tap)==ERROR)
    {      
      cerr<<"Error during the initialization of the translator"<<endl;
      return ERROR;
    }
    else
    {
      unsigned int ret;

      ret=align_corpus(tap);
      release_translator();
      if(ret==ERROR) return ERROR;
      else return OK;
    }
  }
}

//---------------
int init_translator(const thot_ms_alig_pars& tap)
{
  int err;
  
  cerr<<"\n- Initializing aligner...\n\n";

      // Show static types
  cerr<<"Static types:"<<endl;
  cerr<<"- SMT model type (SmtModel): "<<SMT_MODEL_TYPE_NAME<<" ("<<THOT_SMTMODEL_H<<")"<<endl;
  cerr<<"- Language model state (LM_Hist): "<<LM_STATE_TYPE_NAME<<" ("<<THOT_LM_STATE_H<<")"<<endl;
  cerr<<"- Partial probability information for single word models (PpInfo): "<<PPINFO_TYPE_NAME<<" ("<<THOT_PPINFO_H<<")"<<endl;

      // Obtain info about translation model entries
  unsigned int numTransModelEntries;
  Vector<ModelDescriptorEntry> modelDescEntryVec;
  if(extractModelEntryInfo(tap.transModelPref.c_str(),modelDescEntryVec)==OK)
  {
    numTransModelEntries=modelDescEntryVec.size();
  }
  else
  {
    numTransModelEntries=1;
  }

      // Initialize class factories
  err=dynClassFactoryHandler.init_smt(THOT_MASTER_INI_PATH);
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

  llWeightUpdaterPtr=dynClassFactoryHandler.baseLogLinWeightUpdaterDynClassLoader.make_obj(dynClassFactoryHandler.baseLogLinWeightUpdaterInitPars);
  if(llWeightUpdaterPtr==NULL)
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

  err=smtModelPtr->loadLangModel(tap.languageModelFileName.c_str());
  if(err==ERROR)
  {
    release_translator();
    return ERROR;
  }

  err=smtModelPtr->loadAligModel(tap.transModelPref.c_str());
  if(err==ERROR)
  {
    release_translator();
    return ERROR;
  }

      // Set heuristic
  smtModelPtr->setHeuristic(tap.heuristic);

      // Set weights
  smtModelPtr->setWeights(tap.weightVec);
  smtModelPtr->printWeights(cerr);
  cerr<<endl;

      // Set model parameters
  smtModelPtr->set_W_par(tap.W);
  smtModelPtr->set_A_par(tap.A);
  smtModelPtr->set_E_par(tap.E);
  smtModelPtr->set_U_par(tap.nomon);

      // Set verbosity
  smtModelPtr->setVerbosity(tap.verbosity);

      // Create a translator instance
  stackDecoderPtr=dynClassFactoryHandler.baseStackDecoderDynClassLoader.make_obj(dynClassFactoryHandler.baseStackDecoderInitPars);
  if(stackDecoderPtr==NULL)
  {
    cerr<<"Error: BaseStackDecoder pointer could not be instantiated"<<endl;
    return ERROR;
  }

      // Determine if the translator incorporates hypotheses recombination
  stackDecoderRecPtr=dynamic_cast<_stackDecoderRec<SmtModel>*>(stackDecoderPtr);

      // Link translation model
  stackDecoderPtr->link_smt_model(smtModelPtr);
    
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

  return OK;
}

//---------------
void release_translator(void)
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
  delete trConstraintsPtr;
  delete smtModelPtr;

  dynClassFactoryHandler.release_smt();
}

//---------------
int align_corpus(const thot_ms_alig_pars& tap)
{
  SmtModel::Hypothesis result;     // Results of the translation
  SmtModel::Hypothesis anotherTrans;     // Another results of the translation
  int sentNo=0;    
  double elapsed_ant,elapsed,ucpu,scpu,total_time=0;
      
  ifstream testCorpusFile;                // Test corpus file stream
  ifstream refCorpusFile;                 // reference corpus file stream
  string srcSentenceString,trgSentenceString,s;
  

      // Open test corpus file
  testCorpusFile.open(tap.sourceSentencesFile.c_str());
  if(testCorpusFile.fail())
  {
    cerr<<"Error while opening file with test sentences "<<tap.sourceSentencesFile<<endl;
    return ERROR;
  }
  testCorpusFile.seekg(0, ios::beg);

      // Open ref corpus file
  refCorpusFile.open(tap.refSentencesFile.c_str());
  if(refCorpusFile.fail())
  {
    cerr<<"Error while opening file with references "<<tap.refSentencesFile<<endl;
    return ERROR;
  }
  refCorpusFile.seekg(0, ios::beg);

  cerr<<"\n- Processing corpora...\n\n";

  if(!testCorpusFile)
  {
    cerr<<"Test corpus error!"<<endl;
    return ERROR;
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
        cerr<<sentNo<<endl<<srcSentenceString<<endl;
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
        smtModelPtr->printHyp(result,cerr,tap.verbosity);
#         ifdef THOT_STATS
        stackDecoderPtr->printStats();
#         endif

        cerr<<"- Elapsed Time: "<<elapsed-elapsed_ant<<endl<<endl;
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
      if(!outS) cerr<<"Error while printing search graph to file."<<endl;
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
    cerr<<"- Time per sentence: "<<total_time/sentNo<<endl;
  }

  return OK;
}

//---------------------------------------
void print_alig_a3_final(std::string srcstr,
                         std::string trgstr,
                         SmtModel::Hypothesis hyp,
                         unsigned int sentNo,
                         const thot_ms_alig_pars& tap)
{
  SmtModel::Hypothesis::DataType dataType;
  Vector<std::string> sysTrgVec;
  Vector<std::string> trgVec;
    
  sysTrgVec=smtModelPtr->getTransInPlainTextVec(hyp);
  trgVec=stringToStringVector(trgstr);
  dataType=hyp.getData();
  cout<<"# "<<sentNo <<" ; Align. score= "<<hyp.getScore()<<endl;
  cout<<srcstr<<endl;
  cout<<"NULL ({ })";
  if(sysTrgVec!=trgVec && !tap.p_option)
  {
        // If the alignment is incomplete, align each target word with
        // each source word
    unsigned int srcsize=stringToStringVector(srcstr).size();
    for(unsigned int i=0;i<trgVec.size();++i)
    {
      cout<<" "<<trgVec[i]<<" ({ ";
      for(unsigned int j=1;j<=srcsize;++j) cout<<j<<" ";
      cout<<"})";
    }
    cout<<endl;
  }
  else
  {
    unsigned int i;
    i=1;
    for(unsigned int k=0;k<dataType.targetSegmentCuts.size();++k)
    {
      for(;i<=dataType.targetSegmentCuts[k];++i)
      {
        cout<<" "<<sysTrgVec[i-1]<<" ({ ";
        for(unsigned int j=dataType.sourceSegmentation[k].first;j<=dataType.sourceSegmentation[k].second;++j)
        {
          cout<<j<<" ";
        }
        cout<<"})";
      }
    }
    cout<<endl;
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
    return ERROR;
  }
  if(readOption(argc,argv,"--help")!=-1)
  {
    printUsage();
    return ERROR;   
  }
  if(takeParameters(argc,argv,tap)==ERROR)
  {
    return ERROR;
  }
  else
  {
    if(checkParameters(tap)==OK)
    {
      printParameters(tap);
      return OK;
    }
    else
    {
      return ERROR;
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
    if(err==ERROR) return ERROR;
  }
      // process command line parameters
  takeParametersGivenArgcArgv(argc,argv,tap);
  return OK;
}

//---------------
int takeParametersFromCfgFile(std::string cfgFileName,
                              thot_ms_alig_pars& tap)
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
    takeParametersGivenArgcArgv(cfgFileArgc,cfgFileArgv,tap);

        // Release allocated memory
    for(unsigned int i=0;i<cfgFileArgvStl.size();++i)
    {
      free(cfgFileArgv[i]);
    }
    free(cfgFileArgv);

        // Return without error
    return OK;
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
    cerr<<"Error: parameter -lm not given!"<<endl;
    return ERROR;   
  }
  
  if(tap.transModelPref.empty())
  {
    cerr<<"Error: parameter -tm not given!"<<endl;
    return ERROR;   
  }

  if(tap.sourceSentencesFile.empty())
  {
    cerr<<"Error: parameter -t not given!"<<endl;
    return ERROR;   
  }

  if(tap.refSentencesFile.empty())
  {
    cerr<<"Error: parameter -r not given!"<<endl;
    return ERROR;   
  }

  if(tap.p_option && tap.cov_option)
  {
     cerr<<"Error: -p and -cov options cannot be given simultaneously"<<endl;
     return ERROR;
  }

  return OK;
}

//---------------
void printParameters(const thot_ms_alig_pars& tap)
{
 cerr<<"p option: "<<tap.p_option<<endl;
 cerr<<"cov option: "<<tap.cov_option<<endl;
 cerr<<"W: "<<tap.W<<endl;   
 cerr<<"S: "<<tap.S<<endl;   
 cerr<<"A: "<<tap.A<<endl;
 cerr<<"E: "<<tap.E<<endl;
 cerr<<"I: "<<tap.I<<endl;
#ifdef MULTI_STACK_USE_GRAN
 cerr<<"G: "<<tap.G<<endl;
#endif
 cerr<<"h: "<<tap.heuristic<<endl;
 cerr<<"be: "<<tap.be<<endl;
 cerr<<"nomon: "<<tap.nomon<<endl;
 cerr<<"weight vector:";
 for(unsigned int i=0;i<tap.weightVec.size();++i)
   cerr<<" "<<tap.weightVec[i];
 cerr<<endl;
 cerr<<"lmfile: "<<tap.languageModelFileName<<endl;   
 cerr<<"tm files prefix: "<<tap.transModelPref<<endl;   
 cerr<<"test file: "<<tap.sourceSentencesFile<<endl;   
 cerr<<"ref file: "<<tap.refSentencesFile<<endl;
 if(tap.wordGraphFileName!="")
 {
   cerr<<"word graph file prefix: "<<tap.wordGraphFileName<<endl;
   if(tap.wgPruningThreshold==UNLIMITED_DENSITY)
     cerr<<"word graph pruning threshold: word graph density unrestricted"<<endl;
   else
     cerr<<"word graph pruning threshold: "<<tap.wgPruningThreshold<<endl;
 }
 else
 {
   cerr<<"word graph file prefix not given (wordgraphs will not be generated)"<<endl;
 }
 cerr<<"verbosity level: "<<tap.verbosity<<endl;
}

//--------------- stringToStringVector function
Vector<string> stringToStringVector(string s)
{
 Vector<string> vs;	
 string aux="";
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
  cerr << "thot_ms_alig   [-c <string>] [-tm <string>] [-lm <string>]"<<endl;
  cerr << "               -t <string> -r <string>"<<endl;
  cerr << "               [-p|-cov] [-W <float>]"<<endl;
  cerr << "               [-S <int>] [-A <int>] [-E <int>] [-I <int>]"<<endl;
  cerr << "               [-G <int>] [-h <int>] [-be] [-nomon <int>]"<<endl;
  cerr << "               [-tmw <float> ... <float>]"<<endl;
  cerr << "               [-wg <string> [-wgp <float>] ]"<<endl;
  cerr << "               [-v|-v1|-v2] [--help] [--version]"<<endl<<endl;
  cerr << " -c <string>           : Configuration file (command-line options override"<<endl;
  cerr << "                         configuration file options)."<<endl;
  cerr << " -tm <string>          : Prefix of the translation model files."<<endl;
  cerr << " -lm <string>          : Language model file name."<<endl;
  cerr << " -t <string>           : File with the test sentences."<<endl;
  cerr << " -r <string>           : File with the reference sentences."<<endl;
  cerr << " -p                    : Treat the reference sentences as prefixes."<<endl;
  cerr << " -cov                  : Verify model coverage for the reference sentence."<<endl;
  cerr << " -W <float>            : Maximum number of translation options to be considered"<<endl;
  cerr << "                         per each source phrase ("<<PALIG_W_DEFAULT<<" by default)."<<endl;
  cerr << " -S <int>              : Maximum number of hypotheses that can be stored in"<<endl;
  cerr << "                         each stack ("<<PALIG_S_DEFAULT<<" by default)."<<endl;    
  cerr << " -A <int>              : Maximum length in words of the source phrases to be"<<endl;
  cerr << "                         aligned ("<<PALIG_A_DEFAULT<<" by default)."<<endl;
  cerr << " -E <int>              : Constrain the target phrase length to be in the"<<endl;
  cerr << "                         interval [splen-<int> , splen+<int>] where splen is"<<endl;
  cerr << "                         the length of the source phrase to be aligned"<<endl;
  cerr << "                         ("<<PALIG_E_DEFAULT<<" by default)."<<endl;
  cerr << " -I <int>              : Number of hypotheses expanded at each iteration"<<endl;
  cerr << "                         ("<<PALIG_I_DEFAULT<<" by default)."<<endl;
#ifdef MULTI_STACK_USE_GRAN
  cerr << " -G <int>              : Granularity parameter ("<<PALIG_G_DEFAULT<<"by default)."<<endl;
#else
  cerr << " -G <int>              : Parameter not available with the given configuration."<<endl;
#endif
  cerr << " -h <int>              : Heuristic function used: "<<NO_HEURISTIC<<"->None, "<<LOCAL_T_HEURISTIC<<"->LOCAL_T, "<<endl;
  cerr << "                         "<<LOCAL_TD_HEURISTIC<<"->LOCAL_TD ("<<PALIG_H_DEFAULT<<" by default)."<<endl;
  cerr << " -be                   : Execute a best-first algorithm (breadth-first search"<<endl;
  cerr << "                         is executed by default)."<<endl;
  cerr << " -nomon <int>          : Perform a non-monotonic search, allowing the decoder"<<endl;
  cerr << "                         to skip up to <int> words from the last aligned source"<<endl;
  cerr << "                         words. If <int> is equal to zero, then a monotonic"<<endl;
  cerr << "                         search is performed ("<<PALIG_NOMON_DEFAULT<<" is the default value)."<<endl;
  cerr << " -tmw <float>...<float>: Set model weights, the number of weights and their"<<endl;
  cerr << "                         meaning depends on the model type (use --config"<<endl;
  cerr << "                         option)."<<endl;
  cerr << " -wg <string>          : Print word graph after each translation, the prefix" <<endl;
  cerr << "                         of the files is given as parameter."<<endl;
  cerr << " -wgp <float>          : Prune word-graph using the given threshold.\n";
  cerr << "                         Threshold=0 -> no pruning is performed.\n";
  cerr << "                         Threshold=1 -> only the best arc arriving to each\n";
  cerr << "                                        state is retained.\n";
  cerr << "                         If not given, the number of arcs is not\n";
  cerr << "                         restricted.\n";
  cerr << "                         not restricted."<<endl;
  cerr << " -v|-v1|-v2            : verbose modes."<<endl;
  cerr << " --help                : Display this help and exit."<<endl;
  cerr << " --version             : Output version information and exit."<<endl;
  cerr << " --config              : Show current configuration."<<endl;
}

//---------------
void version(void)
{
  cerr<<"thot_ms_alig is part of the thot package "<<endl;
  cerr<<"thot version "<<THOT_VERSION<<endl;
  cerr<<"thot is GNU software written by Daniel Ortiz"<<endl;
}
