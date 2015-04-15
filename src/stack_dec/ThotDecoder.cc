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
/* Module: ThotDecoder                                              */
/*                                                                  */
/* Definitions file: ThotDecoder.cc                                 */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "ThotDecoder.h"

//--------------- ThotDecoder class functions

//--------------------------
ThotDecoder::ThotDecoder()
{
      // Create server variables
  tdCommonVars.wgHandlerPtr=new WgHandler;
  tdCommonVars.smtModelPtr=new CURR_MODEL_TYPE();
  tdCommonVars.ecModelPtr=new CURR_ECM_TYPE();

#ifdef THOT_ENABLE_UPDATE_LLWEIGHTS
  tdCommonVars.weightUpdateEnginePtr=new DRR();

      // Init weight update engine parameters
  tdCommonVars.weightUpdateEnginePtr->init(DRR_ALPHA,DRR_BETA);
#endif
  
      // Initialize mutexes and conditions
  pthread_mutex_init(&user_id_to_idx_mut,NULL);
  pthread_mutex_init(&atomic_op_mut,NULL);
  pthread_mutex_init(&non_atomic_op_mut,NULL);
  pthread_mutex_init(&preproc_mut,NULL);
  pthread_cond_init(&non_atomic_op_cond,NULL);
  non_atomic_ops_running=0;
}

//--------------------------
bool ThotDecoder::user_id_new(int user_id)
{
  int ret;
  pthread_mutex_lock(&user_id_to_idx_mut);
  /////////// begin of mutex 
  std::map<int,size_t>::iterator mapIter;
  
      // Obtain idx
  mapIter=userIdToIdx.find(user_id);
  if(mapIter!=userIdToIdx.end())
  {
    ret=false;
  }
  else
  {
    ret=true;
  }
  /////////// end of mutex 
  pthread_mutex_unlock(&user_id_to_idx_mut);

  return ret;
}

//--------------------------
void ThotDecoder::release_user_data(int user_id)
{
  pthread_mutex_lock(&user_id_to_idx_mut);
  /////////// begin of mutex 
  std::map<int,size_t>::iterator mapIter;
  
      // Obtain idx
  mapIter=userIdToIdx.find(user_id);
  if(mapIter!=userIdToIdx.end())
  {
        // Release user data
    release_idx_data(mapIter->second);
  }
  /////////// end of mutex 
  pthread_mutex_unlock(&user_id_to_idx_mut);  
}

//--------------------------
void ThotDecoder::release(void)
{
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

  tdCommonVars.wgHandlerPtr->clear();
  tdCommonVars.smtModelPtr->clear();
  tdCommonVars.ecModelPtr->clear();
  for(size_t i=0;i<tdPerUserVarsVec.size();++i)
  {
    release_idx_data(i);
  }
  tdPerUserVarsVec.clear();
  tdState.default_values();
  totalPrefixVec.clear();
  userIdToIdx.clear();
  idxDataReleased.clear();
  
  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);
}

//--------------------------
void ThotDecoder::config(void)
{
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

  CURR_MODEL_TYPE model;
  CURR_ECM_TYPE ecm_type;
  
      // Print server configuration
  cerr<<"* Server configuration"<<endl;
      // Print translation model information
  cerr<<"  - Statistical machine translation model type: "<<CURR_MODEL_LABEL<<endl;
  if(strlen(CURR_MODEL_NOTES)!=0)
  {
    cerr << "  - Model notes: "<<CURR_MODEL_NOTES<<endl;
  }
  cerr<<"  - Weights for the smt model and their default values: ";
  model.printWeights(cerr);
  cerr << endl;
      
      // Print language model information
  cerr << "  - Language model type: "<<THOT_CURR_LM_LABEL<<endl;
  if(strlen(THOT_CURR_LM_NOTES)!=0)
  {
    cerr << "  - Language model notes: "<<THOT_CURR_LM_NOTES<<endl;
  }
    
      // Print phrase-based model information
  cerr << "  - Phrase-based model type: "<<THOT_CURR_PBM_LABEL<<endl;
  if(strlen(THOT_CURR_PBM_NOTES)!=0)
  {
    cerr << "  - Phrase-based model notes: "<<THOT_CURR_PBM_NOTES<<endl;
  }

      // Print single-word model information
  cerr << "  - Single-word model type: "<<CURR_SWM_LABEL<<endl;
  if(strlen(CURR_SWM_NOTES)!=0)
  {
    cerr << "  - Single-word model notes: "<<CURR_SWM_NOTES<<endl;
  }

      // Print decoding algorithm information
  cerr<<"  - Translator type: "<<CURR_MSTACK_LABEL<<endl;
      
      // Print assisted translator information
  cerr<<"  - Assisted translator type: "<<CURR_AT_LABEL<<endl;
# if THOT_AT_TYPE != WG_UNCTRANS
  CURR_AT_TYPE<CURR_MODEL_TYPE> assisted_trans_type;
  cerr<<"  - Weights for the assisted translator and their default values: ";
  assisted_trans_type.printWeights(cerr);
  cerr << endl;
# else
#  if CURR_ECM_VALID_FOR_WG == 1
  CURR_AT_TYPE<CURR_MODEL_TYPE,CURR_ECM_TYPE> assisted_trans_type;
  cerr<<"  - Weights for the assisted translator and their default values: ";
  assisted_trans_type.printWeights(cerr);
  cerr << endl;
#  else
  cerr<<"Fatal error! current error correcting model cannot be combined with word-graph based assisted translators"<<endl;
#  endif
# endif

      // Print error correcting model information 
  cerr<<"  - EC model: "<<CURR_ECM_LABEL<<endl;
  cerr<<"  - Weights for the EC model and their default values: ";
  ecm_type.printWeights(cerr);
  cerr << endl;
  cerr<<"  - EC model valid for word-graphs: "<<CURR_ECM_VALID_FOR_WG<<endl;
    
      // Print error correcting model information for uncoupled cat
  cerr<<"  - EC model for n-best uncoupled cat type: "<<CURR_ECM_NB_UCAT_LABEL<<endl;
      
      // Print word-graph processor information
  cerr<<"  - Word-graph processor: "<<CURR_WGP_LABEL<<endl;
  cerr<<endl;

  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);
}

//--------------------------
int ThotDecoder::initUsingCfgFile(std::string cfgFile,
                                  ThotDecoderUserPars& tdup,
                                  int verbose)
{
  int ret;
  int argc;
  Vector<std::string> argv_stl;
  std::string comment="#";

      // Extract parameters from file
  ret=extractParsFromFile(cfgFile.c_str(),argc,argv_stl,comment);
  if(ret==ERROR)
  {
    return ERROR;
  }
  cerr<<"Processing configuration file ("<<cfgFile<<")..."<<endl;
  
      // Set default values for parameters
  std::string tm_str="/home/dortiz/traduccion/corpus/Xerox/en_es/v14may2003/simplified2/TM/my_ef";
  std::string lm_str="/home/dortiz/traduccion/corpus/Xerox/en_es/v14may2003/simplified2/LM/e_i3_c.lm";
  unsigned int nomon=TDEC_NOMON_DEFAULT;
  float W=TDEC_W_DEFAULT;
  unsigned int A=TDEC_A_DEFAULT;
  unsigned int E=TDEC_E_DEFAULT;
  unsigned int h=TDEC_HEUR_DEFAULT;
  std::string cm_str="";
  OnlineTrainingPars onlineTrainingPars;
  Vector<float> olParsVec;
  Vector<float> tmWeightsVec;
  Vector<float> ecWeightsVec;
  
      // Process parameters
  int i=1;
  unsigned int matched;
  while(i<argc)
  {
    matched=0;
        // -tm parameter
    if(argv_stl[i]=="-tm" && !matched)
    {
      if(i==argc-1)
      {
        cerr<<"Error: no value for -tm parameter."<<endl;
        return ERROR;
      }
      else
      {
        cerr<<"-tm parameter changed from \""<<tm_str<<"\" to \""<<argv_stl[i+1]<<"\""<<endl;
        tm_str=argv_stl[i+1];
        ++matched;
        ++i;
      }
    }

        // -lm parameter
    if(argv_stl[i]=="-lm" && !matched)
    {
      if(i==argc-1)
      {
        cerr<<"Error: no value for -lm parameter."<<endl;
        return ERROR;
      }
      else
      {
        cerr<<"-lm parameter changed from \""<<lm_str<<"\" to \""<<argv_stl[i+1]<<"\""<<endl;
        lm_str=argv_stl[i+1];
        ++matched;
        ++i;
      }
    }

        // -W parameter
    if(argv_stl[i]=="-W" && !matched)
    {
      if(i==argc-1)
      {
        cerr<<"Error: no value for -W parameter."<<endl;
        return ERROR;
      }
      else
      {
        cerr<<"-W parameter changed from \""<<W<<"\" to \""<<argv_stl[i+1]<<"\""<<endl;
        W=atof(argv_stl[i+1].c_str());
        ++matched;
        ++i;
      }
    }

        // -S parameter
    if(argv_stl[i]=="-S" && !matched)
    {
      if(i==argc-1)
      {
        cerr<<"Error: no value for -S parameter."<<endl;
        return ERROR;
      }
      else
      {
        cerr<<"-S parameter changed from \""<<tdup.S<<"\" to \""<<argv_stl[i+1]<<"\""<<endl;
        tdup.S=atoi(argv_stl[i+1].c_str());
        ++matched;
        ++i;
      }
    }

        // -A parameter
    if(argv_stl[i]=="-A" && !matched)
    {
      if(i==argc-1)
      {
        cerr<<"Error: no value for -A parameter."<<endl;
        return ERROR;
      }
      else
      {
        cerr<<"-A parameter changed from \""<<A<<"\" to \""<<argv_stl[i+1]<<"\""<<endl;
        A=atoi(argv_stl[i+1].c_str());
        ++matched;
        ++i;
      }
    }

        // -E parameter
    if(argv_stl[i]=="-E" && !matched)
    {
      if(i==argc-1)
      {
        cerr<<"Error: no value for -E parameter."<<endl;
        return ERROR;
      }
      else
      {
        cerr<<"-E parameter changed from \""<<E<<"\" to \""<<argv_stl[i+1]<<"\""<<endl;
        E=atoi(argv_stl[i+1].c_str());
        ++matched;
        ++i;
      }
    }
        // -nomon parameter
    if(argv_stl[i]=="-nomon" && !matched)
    {
      if(i==argc-1)
      {
        cerr<<"Error: no value for -nomon parameter."<<endl;
        return ERROR;
      }
      else
      {
        cerr<<"-nomon parameter changed from \""<<nomon<<"\" to \""<<argv_stl[i+1]<<"\""<<endl;
        nomon=atoi(argv_stl[i+1].c_str());
        ++matched;
        ++i;
      }
    }

        // -be parameter
    if(argv_stl[i]=="-be" && !matched)
    {
      cerr<<"-be parameter given (not given by default)"<<endl;
      tdup.be=true;
      ++matched;
    }

        // -G parameter
    if(argv_stl[i]=="-G" && !matched)
    {
      if(i==argc-1)
      {
        cerr<<"Error: no value for -G parameter."<<endl;
        return ERROR;
      }
      else
      {
        cerr<<"-G parameter changed from \""<<tdup.G<<"\" to \""<<argv_stl[i+1]<<"\""<<endl;
        tdup.G=atoi(argv_stl[i+1].c_str());
        ++matched;
        ++i;
      }
    }

        // -h parameter
    if(argv_stl[i]=="-h" && !matched)
    {
      if(i==argc-1)
      {
        cerr<<"Error: no value for -h parameter."<<endl;
        return ERROR;
      }
      else
      {
        cerr<<"-h parameter changed from \""<<h<<"\" to \""<<argv_stl[i+1]<<"\""<<endl;
        h=atoi(argv_stl[i+1].c_str());
        ++matched;
        ++i;
      }
    }

            // -olp parameter
    if(argv_stl[i]=="-olp" && !matched)
    {
      if(i==argc-1)
      {
        cerr<<"Error: no values for -olp parameter."<<endl;
        return ERROR;
      }
      else
      {
        ++matched;
        bool end=false;
        while(!end)
        {
          if(i==argc-1 || isOption(argv_stl[i+1].c_str()))
            end=true;
          else
          {
            olParsVec.push_back(atof(argv_stl[i+1].c_str()));
            ++i;
          }
        }
            // Fill onlineTrainingPars object
        if(olParsVec.size()>=1) onlineTrainingPars.onlineLearningAlgorithm=(unsigned int) olParsVec[0];
        if(olParsVec.size()>=2) onlineTrainingPars.learningRatePolicy=(unsigned int) olParsVec[1];
        if(olParsVec.size()>=3) onlineTrainingPars.learnStepSize=olParsVec[2];
        if(olParsVec.size()>=4) onlineTrainingPars.emIters=(unsigned int) olParsVec[3];
        if(olParsVec.size()>=5) onlineTrainingPars.E_par=(unsigned int) olParsVec[4];
        if(olParsVec.size()>=6) onlineTrainingPars.R_par=(unsigned int) olParsVec[5];

        cerr<<"-olp parameter given:";
        for(unsigned int i=0;i<olParsVec.size();++i)
          cerr<<" "<<olParsVec[i];
        cerr<<endl;
      }
    }

        // -tmw parameter
    if(argv_stl[i]=="-tmw" && !matched)
    {
      if(i==argc-1)
      {
        cerr<<"Error: no values for -tmw parameter."<<endl;
        return ERROR;
      }
      else
      {
        ++matched;
        bool end=false;
        while(!end)
        {
          if(i==argc-1 || isOption(argv_stl[i+1].c_str()))
            end=true;
          else
          {
            tmWeightsVec.push_back(atof(argv_stl[i+1].c_str()));
            ++i;
          }
        }
        cerr<<"-tmw parameter given:";
        for(unsigned int i=0;i<tmWeightsVec.size();++i)
          cerr<<" "<<tmWeightsVec[i];
        cerr<<endl;
      }
    }

        // -ecw parameter
    if(argv_stl[i]=="-ecw" && !matched)
    {
      if(i==argc-1)
      {
        cerr<<"Error: no values for -ecw parameter."<<endl;
        return ERROR;
      }
      else
      {
        ++matched;
        bool end=false;
        while(!end)
        {
          if(i==argc-1 || isOption(argv_stl[i+1].c_str()))
            end=true;
          else
          {
            ecWeightsVec.push_back(atof(argv_stl[i+1].c_str()));
            ++i;
          }
        }
        cerr<<"-ecw parameter given:";
        for(unsigned int i=0;i<ecWeightsVec.size();++i)
          cerr<<" "<<ecWeightsVec[i];
        cerr<<endl;
      }
    }

        // -catw parameter
    if(argv_stl[i]=="-catw" && !matched)
    {
      if(i==argc-1)
      {
        cerr<<"Error: no values for -catw parameter."<<endl;
        return ERROR;
      }
      else
      {
        ++matched;
        bool end=false;
        while(!end)
        {
          if(i==argc-1 || isOption(argv_stl[i+1].c_str()))
            end=true;
          else
          {
            tdup.catWeightsVec.push_back(atof(argv_stl[i+1].c_str()));
            ++i;
          }
        }
        cerr<<"-catw parameter given:";
        for(unsigned int i=0;i<tdup.catWeightsVec.size();++i)
          cerr<<" "<<tdup.catWeightsVec[i];
        cerr<<endl;
      }
    }

        // -np parameter
    if(argv_stl[i]=="-np" && !matched)
    {
      if(i==argc-1)
      {
        cerr<<"Error: no value for -np parameter."<<endl;
        return ERROR;
      }
      else
      {
        cerr<<"-np parameter changed from \""<<tdup.np<<"\" to \""<<argv_stl[i+1]<<"\""<<endl;
        tdup.np=atoi(argv_stl[i+1].c_str());
        ++matched;
        ++i;
      }
    }

        // -wgp parameter
    if(argv_stl[i]=="-wgp" && !matched)
    {
      if(i==argc-1)
      {
        cerr<<"Error: no value for -wgp parameter."<<endl;
        return ERROR;
      }
      else
      {
        cerr<<"-wgp parameter changed from \""<<tdup.wgp<<"\" to \""<<argv_stl[i+1]<<"\""<<endl;
        tdup.wgp=atof(argv_stl[i+1].c_str());
        ++matched;
        ++i;
      }
    }

        // -wgh parameter
    if(argv_stl[i]=="-wgh" && !matched)
    {
      if(i==argc-1)
      {
        cerr<<"Error: no value for -wgh parameter."<<endl;
        return ERROR;
      }
      else
      {
        cerr<<"-wgh parameter changed from \""<<tdup.wgh_str<<"\" to \""<<argv_stl[i+1]<<"\""<<endl;
        tdup.wgh_str=argv_stl[i+1];
        ++matched;
        ++i;
      }
    }

        // -sp option
    if(argv_stl[i]=="-sp" && !matched)
    {
      cerr<<"-sp parameter changed from \""<<false<<"\" to \""<<argv_stl[i+1]<<"\""<<endl;
      tdup.sp=atoi(argv_stl[i+1].c_str());
      ++matched;
      ++i;
    }

        // -uc parameter
    if(argv_stl[i]=="-uc" && !matched)
    {
      if(i==argc-1)
      {
        cerr<<"Error: no value for -uc parameter."<<endl;
        return ERROR;
      }
      else
      {
        cerr<<"-uc parameter changed from \""<<tdup.uc_str<<"\" to \""<<argv_stl[i+1]<<"\""<<endl;
        tdup.uc_str=argv_stl[i+1];
        ++matched;
        ++i;
      }
    }
    ++i;
  }

  // Initialize server

      // Load translation model
  ret=load_tm(tm_str.c_str(),verbose);
  if(ret==ERROR) return ERROR;

      // Load language model
  ret=load_lm(lm_str.c_str(),verbose);
  if(ret==ERROR) return ERROR;

      // Set non-monotonicity level
  setNonMonotonicity(nomon,verbose);

      // Set W parameter
  set_W(W,verbose);

      // Set A parameter
  set_A(A,verbose);

      // Set E parameter
  set_E(E,verbose);

      // Set h parameter
  set_h(h,verbose);

      // Set online training parameters
  setOnlineTrainPars(onlineTrainingPars,verbose);

      // Set tm weights
  set_tmw(tmWeightsVec,verbose);

      // Set ec weights
  set_ecw(ecWeightsVec,verbose);

      // Load wordgraph handler information if given
  if(tdup.wgh_str!="")
    ret=set_wgh(tdup.wgh_str.c_str(),verbose);
  if(ret==ERROR) return ERROR;

  return OK;
}

//--------------------------
int ThotDecoder::initUserPars(int user_id,
                              const ThotDecoderUserPars& tdup,
                              int verbose)
{
      // Set S parameter
  set_S(user_id,tdup.S,verbose);

      // Set be flag
  set_be(user_id,tdup.be,verbose);

      // Set G parameter
  int ret=set_G(user_id,tdup.G,verbose);

      // Set np parameter
  ret=set_np(user_id,tdup.np,verbose);

      // Set wgp parameter
  ret=set_wgp(user_id,tdup.wgp,verbose);

      // Set sp flag
  set_preproc(user_id,tdup.sp,verbose);

      // Load preproc. info if requested
  if(tdup.sp && tdup.uc_str!="")
    ret=use_caseconv(user_id,tdup.uc_str.c_str(),verbose);
  if(ret==ERROR) return ERROR;

      // Set cat weights
  set_catw(user_id,tdup.catWeightsVec,verbose);

  return OK;
}

//--------------------------
bool ThotDecoder::load_tm(const char* tmFilesPrefix,
                          int verbose/*=0*/)
{
  int ret;
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

      // Wait until all non-atomic operations have finished
  wait_on_non_atomic_op_cond();
    
  if(strcmp(tdState.tmFilesPrefixGiven.c_str(),tmFilesPrefix)==0)
  {
    if(verbose) cerr<<"Translation model already loaded"<<endl;
    ret=OK;
  }
  else
  {
    if(verbose)
    {
      cerr<<"Loading translation model given the prefix: "<<tmFilesPrefix<<endl;
    }
    
    ret=tdCommonVars.smtModelPtr->loadAligModel(tmFilesPrefix);
    if(ret==OK)
    {
      tdState.tmFilesPrefixGiven=tmFilesPrefix;
    }
  }

      // Unlock non_atomic_op_cond mutex
  unlock_non_atomic_op_mut();
  
  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);

  return ret;
}

//--------------------------
bool ThotDecoder::load_lm(const char* lmFileName,
                          int verbose/*=0*/)
{
  int ret;
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

  if(strcmp(tdState.lmfileLoaded.c_str(),lmFileName)==0)
  {
    if(verbose) cerr<<"Language model already loaded"<<endl;
    ret=OK;
  }
  else
  {
    if(verbose)
    {
      cerr<<"Loading language model from file: "<<lmFileName<<endl;
    }
    ret=tdCommonVars.smtModelPtr->loadLangModel(lmFileName);
    if(ret==OK)
    {
      tdState.lmfileLoaded=lmFileName;
    }
  }
  
  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);

  return ret;
}
  
//--------------------------
bool ThotDecoder::load_ecm(const char* ecmFilesPrefix,
                           int verbose/*=0*/)
{
  int ret;
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

  if(strcmp(tdState.ecmFilesPrefixGiven.c_str(),ecmFilesPrefix)==0)
  {
    if(verbose) cerr<<"Error correcting model already loaded"<<endl;
    ret=OK;
  }
  else
  {
    if(verbose)
    {
      cerr<<"Loading error correcting model given the prefix: "<<ecmFilesPrefix<<endl;
    }
    
    ret=tdCommonVars.ecModelPtr->load(ecmFilesPrefix);
    if(ret==OK)
    {
      tdState.ecmFilesPrefixGiven=ecmFilesPrefix;
    }
  }

  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);

  return ret;
}

//--------------------------
bool ThotDecoder::onlineTrainSentPair(int user_id,
                                      const char *srcSent,
                                      const char *refSent,
                                      int verbose/*=0*/)
{
  int ret;
  
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

      // Obtain index vector given user_id
  size_t idx=get_vecidx_for_user_id(user_id);
  if(verbose) cerr<<"user_id: "<<user_id<<", idx: "<<idx<<endl;

  if(verbose)
  {
    cerr<<"Training sentence pair:"<<endl;
    cerr<<" - source: "<<srcSent<<endl;
    cerr<<" - reference: "<<refSent<<endl;
  }

      // Check if pre/post processing is enabled
  if(tdState.preprocId)
  {
        // Pre/post processing enabled
    std::string preprocSrcSent=tdPerUserVarsVec[idx].prePosProcessorPtr->preprocLine(srcSent,tdState.caseconv,false);
    std::string preprocRefSent=tdPerUserVarsVec[idx].prePosProcessorPtr->preprocLine(refSent,tdState.caseconv,false);

        // Obtain system translation
    CURR_MODEL_TYPE::Hypothesis hyp=tdPerUserVarsVec[idx].translatorPtr->translate(preprocSrcSent.c_str());
    std::string preprocSysSent=tdCommonVars.smtModelPtr->getTransInPlainText(hyp);

    if(verbose)
    {
      cerr<<" - preproc. source: "<<preprocSrcSent<<endl;
      cerr<<" - preproc. reference: "<<preprocRefSent<<endl;
      cerr<<" - preproc. sys translation: "<<preprocSysSent<<endl;
    }
        // Add sentence to word-predictor
    if(verbose) cerr<<"Adding preprocessed target sentence to the word predictor..."<<endl;
    tdCommonVars.smtModelPtr->addSentenceToWordPred(StrProcUtils::stringToStringVector(preprocRefSent),verbose);

    if(verbose) cerr<<"Training models..."<<endl;

        // Measure training time
    double prevElapsedTime,elapsedTime,ucpu,scpu;
    ctimer(&prevElapsedTime,&ucpu,&scpu);
    
        // Train generative models
    ret=tdCommonVars.smtModelPtr->onlineTrainSentPair(preprocSrcSent.c_str(),
                                                      preprocRefSent.c_str(),
                                                      preprocSysSent.c_str(),
                                                      verbose);
    ctimer(&elapsedTime,&ucpu,&scpu);
    if(verbose)
    {
      cerr<<"Training process ended."<<endl;
      cerr<<"Training time: "<<elapsedTime-prevElapsedTime<<endl;
    }
  }
  else
  {
        // Pre/post processing disabled

        // Obtain system translation
#ifndef THOT_DISABLE_REC
    tdPerUserVarsVec[idx].translatorPtr->enableWordGraph();
#endif
    CURR_MODEL_TYPE::Hypothesis hyp=tdPerUserVarsVec[idx].translatorPtr->translate(srcSent);
    std::string sysSent=tdCommonVars.smtModelPtr->getTransInPlainText(hyp);

        // Add sentence to word-predictor
    if(verbose) cerr<<"Adding target sentence to the word predictor..."<<endl;
    tdCommonVars.smtModelPtr->addSentenceToWordPred(StrProcUtils::stringToStringVector(refSent),verbose);

    if(verbose) cerr<<"Training models..."<<endl;

        // Measure training time
    double prevElapsedTime,elapsedTime,ucpu,scpu;
    ctimer(&prevElapsedTime,&ucpu,&scpu);

#ifndef THOT_DISABLE_REC
        // Train log-linear weights

        // Retrieve pointer to wordgraph (use word-graph provided by the
        // word-graph handler if available)
    Vector<std::string> sentStrVec=StrProcUtils::stringToStringVector(srcSent);
    bool found;
    std::string wgPathStr=tdCommonVars.wgHandlerPtr->pathAssociatedToSentence(sentStrVec,found);
    if(found)
    {
      WordGraph wg;
      wg.load(wgPathStr.c_str());
      updateLogLinearWeights(refSent,&wg,verbose);
    }
    else
    {
      WordGraph* wgPtr=tdPerUserVarsVec[idx].translatorPtr->getWordGraphPtr();
      updateLogLinearWeights(refSent,wgPtr,verbose);
    }    
    tdPerUserVarsVec[idx].translatorPtr->disableWordGraph();
#endif

        // Train generative models
    ret=tdCommonVars.smtModelPtr->onlineTrainSentPair(srcSent,refSent,sysSent.c_str(),verbose);    
   
    ctimer(&elapsedTime,&ucpu,&scpu);
    if(verbose) cerr<<"Training time: "<<elapsedTime-prevElapsedTime<<endl;
  }

  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);

  return ret;
}

//--------------------------
#ifdef THOT_ENABLE_UPDATE_LLWEIGHTS
void ThotDecoder::updateLogLinearWeights(std::string refSent,
                                         WordGraph* wgPtr,
                                         int verbose/*=0*/)
{
# ifndef THOT_DISABLE_REC
        // Obtain n-best list
    unsigned int len=NBEST_LIST_SIZE_FOR_LLWEIGHT_UPDATE;
    Vector<pair<Score,std::string> > nblist;
    Vector<Vector<Score> > scoreCompsVec;
    wgPtr->obtainNbestList(len,nblist,scoreCompsVec);

        // Obtain current weights
    Vector<pair<std::string,float> > compWeights;
    tdCommonVars.smtModelPtr->getWeights(compWeights);
    vector<double> currentWeights;
    for(unsigned int i=0;i<compWeights.size();++i)
      currentWeights.push_back(compWeights[i].second);

        // Print verbose information
    if(verbose)
    {
      cerr<<"Training log linear combination weights (n-best list size= "<<nblist.size()<<")..."<<endl;
    }

        // Obtain new weights
    vector<double> newWeights;
        // Check if n-best list is empty 
    if(nblist.empty())
      newWeights=currentWeights;
    else
    {    
          // Invoke weight update engine
      std::string reference=refSent;
      vector<string> nblistWithNoScr;
      for(unsigned int i=0;i<nblist.size();++i) nblistWithNoScr.push_back(nblist[i].second);
      tdCommonVars.weightUpdateEnginePtr->computeNewWeights(currentWeights,
                                                            reference,
                                                            nblistWithNoScr,
                                                            scoreCompsVec,
                                                            newWeights);
    }
    if(verbose)
    {
      cerr<<"The weights of the loglinear combination have been trained:"<<endl;
      cerr<<" - Previous weights:";
      for(unsigned int i=0;i<currentWeights.size();++i) cerr<<" "<<currentWeights[i];
      cerr<<endl;
      cerr<<" - New weights     :";
      for(unsigned int i=0;i<newWeights.size();++i) cerr<<" "<<newWeights[i];
      cerr<<endl;
    }
        // Set new weights
    Vector<float> tmwVec;
    for(unsigned int i=0;i<newWeights.size();++i) tmwVec.push_back(newWeights[i]);
    set_tmw(tmwVec);
# else
    if(verbose)
      cerr<<"Weights of the loglinear combination will not be modified."<<endl;
# endif
}
#else
void ThotDecoder::updateLogLinearWeights(std::string /*refSent*/,
                                         WordGraph* /*wgPtr*/,
                                         int verbose/*=0*/)
{
    if(verbose)
      cerr<<"Updating of log-linear combination weights is disabled."<<endl;  
}
#endif

//--------------------------
void ThotDecoder::setOnlineTrainPars(OnlineTrainingPars onlineTrainingPars,
                                     int verbose/*=0*/)
{
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

  if(verbose)
  {
    cerr<<"Setting online training pars..."<<endl;

    cerr<<"onlineLearningAlgorithm= "<<onlineTrainingPars.onlineLearningAlgorithm<<" ; ";
    cerr<<"learningRatePolicy= "<<onlineTrainingPars.learningRatePolicy<<" ; ";
    cerr<<"learnStepSize= "<<onlineTrainingPars.learnStepSize<<" ; ";
    cerr<<"emIters= "<<onlineTrainingPars.emIters<<" ; ";
    cerr<<"E_par= "<<onlineTrainingPars.E_par<<" ; ";
    cerr<<"R_par= "<<onlineTrainingPars.R_par<<endl;
  }
  tdCommonVars.smtModelPtr->setOnlineTrainingPars(onlineTrainingPars,verbose);

  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);
}

//--------------------------
bool ThotDecoder::trainEcm(int user_id,
                           const char *strx,
                           const char *stry,
                           int verbose/*=0*/)
{
  int ret;
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

      // Obtain index vector given user_id
  size_t idx=get_vecidx_for_user_id(user_id);
  if(verbose) cerr<<"user_id: "<<user_id<<", idx: "<<idx<<endl;

  
  if(verbose)
  {
    cerr<<"Training ecm for string pair:"<<endl;
    cerr<<" - string x: "<<strx<<endl;
    cerr<<" - string y: "<<stry<<endl;
  }
    
  if(tdState.preprocId)
  {
    std::string preprocx=tdPerUserVarsVec[idx].prePosProcessorPtr->preprocLine(strx,tdState.caseconv,false);
    std::string preprocy=tdPerUserVarsVec[idx].prePosProcessorPtr->preprocLine(stry,tdState.caseconv,false);
    if(verbose)
    {
      cerr<<" - preproc. string x: "<<preprocx<<endl;
      cerr<<" - preproc. string y: "<<preprocy<<endl;
    }
    ret=tdCommonVars.ecModelPtr->trainStrPair(preprocx.c_str(),preprocy.c_str(),verbose);
  }
  else
  {
    ret=tdCommonVars.ecModelPtr->trainStrPair(strx,stry,verbose);
  }

  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);

  return ret;
}

//--------------------------
void ThotDecoder::translateSentence(int user_id,
                                    const char *sentenceToTranslate,
                                    std::string& result,
                                    int verbose/*=0*/)
{
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

      // Obtain index vector given user_id
  size_t idx=get_vecidx_for_user_id(user_id);
  if(verbose) cerr<<"user_id: "<<user_id<<", idx: "<<idx<<endl;

  if(verbose)
  {
    cerr<<"Translating sentence: "<<sentenceToTranslate<<endl;
  }
  if(tdState.preprocId)
  {
    std::string preprocSrcSent=tdPerUserVarsVec[idx].prePosProcessorPtr->preprocLine(sentenceToTranslate,tdState.caseconv,true);
    if(verbose)
    {
      cerr<<" - preproc. source: "<<preprocSrcSent<<endl;
    }

        // Obtain translation using precalculated word-graph or translator
    std::string aux=translateSentenceAux(idx,preprocSrcSent);
    
    result=tdPerUserVarsVec[idx].prePosProcessorPtr->postprocLine(aux.c_str(),tdState.caseconv);
    if(verbose)
    {
      cerr<<" - preproc. target: "<<aux<<endl;
      cerr<<" - posproc. target: "<<result<<endl;     
    }
  }
  else
  {
    result=translateSentenceAux(idx,sentenceToTranslate,verbose);
    if(verbose)
    {
      cerr<<" - target translation: "<<result<<endl;     
    }

  }
  // if(verbose)
  // {
  //   tdCommonVars.smtModelPtr->printHyp(hyp,cerr);
  // }

  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);
}

//--------------------------
std::string ThotDecoder::translateSentenceAux(size_t idx,
                                              std::string sentenceToTranslate,
                                              int verbose/*=0*/)
{
      // Obtain translation using precalculated word-graph or translator
  bool found;
  
  Vector<std::string> sentStrVec=StrProcUtils::stringToStringVector(sentenceToTranslate);
  std::string wgPathStr=tdCommonVars.wgHandlerPtr->pathAssociatedToSentence(sentStrVec,found);
  if(found)
  {
        // Use word graph
    WordGraph wg;

        // Load word graph
    wg.load(wgPathStr.c_str());

        // Obtain original word graph component weights
    Vector<pair<std::string,float> > originalWgCompWeights;
    wg.getCompWeights(originalWgCompWeights);

        // Print component weight info to the error output
    if(verbose)
    {
      cerr<<"Original word graph component vector:";
      for(unsigned int i=0;i<originalWgCompWeights.size();++i)
        cerr<<" "<<originalWgCompWeights[i].first<<": "<<originalWgCompWeights[i].second<<";";
      cerr<<endl;
    }

        // Set current component weights (this operation causes a
        // complete re-scoring of the word graph arcs if there exist
        // score component information for them)
    Vector<pair<std::string,float> > currCompWeights;
    tdCommonVars.smtModelPtr->getWeights(currCompWeights);
    wg.setCompWeights(currCompWeights);

        // Print component weight info to the error output
    if(verbose)
    {
      cerr<<"New word graph component vector:";
      for(unsigned int i=0;i<currCompWeights.size();++i)
        cerr<<" "<<currCompWeights[i].first<<": "<<currCompWeights[i].second<<";";
      cerr<<endl;
    }
    
        // Obtain best path
    std::set<WordGraphArcId> emptyExcludedArcsSet;
    Vector<WordGraphArc> arcVec;
    wg.bestPathFromFinalStateToIdx(INITIAL_STATE,emptyExcludedArcsSet,arcVec);

        // Obtain translation
    Vector<std::string> resultVec;
    for(Vector<WordGraphArc>::reverse_iterator riter=arcVec.rbegin();riter!=arcVec.rend();++riter)
    {
      for(unsigned int j=0;j<riter->words.size();++j)
        resultVec.push_back(riter->words[j]);
    }
    
        // Return result
    std::string result=StrProcUtils::stringVectorToString(resultVec);
    return result;
  }
  else
  {
        // Use translator
    CURR_MODEL_TYPE::Hypothesis hyp=tdPerUserVarsVec[idx].translatorPtr->translate(sentenceToTranslate.c_str());
    std::string result=tdCommonVars.smtModelPtr->getTransInPlainText(hyp);
    return result;
  }
}

//--------------------------
bool ThotDecoder::translateSentencePrintWg(int user_id,
                                           const char *sentenceToTranslate,
                                           std::string& result,
                                           const char* wgFilename,
                                           int verbose/*=0*/)
{
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

      // Obtain index vector given user_id
  size_t idx=get_vecidx_for_user_id(user_id);
  if(verbose) cerr<<"user_id: "<<user_id<<", idx: "<<idx<<endl;

      // Enable word graph generation
#ifndef THOT_DISABLE_REC
  tdPerUserVarsVec[idx].translatorPtr->enableWordGraph();
#else
  cerr<<"Warning! current configuration does not allow to generate word graphs"<<endl;
#endif
  
  if(verbose)
  {
    cerr<<"Translating sentence: "<<sentenceToTranslate<<endl;
  }
  CURR_MODEL_TYPE::Hypothesis hyp;
  if(tdState.preprocId)
  {
    std::string preprocSrcSent=tdPerUserVarsVec[idx].prePosProcessorPtr->preprocLine(sentenceToTranslate,tdState.caseconv,false);
    if(verbose)
    {
      cerr<<" - preproc. source: "<<preprocSrcSent<<endl;
    }
    hyp=tdPerUserVarsVec[idx].translatorPtr->translate(preprocSrcSent.c_str());
    std::string aux=tdCommonVars.smtModelPtr->getTransInPlainText(hyp);
    result=tdPerUserVarsVec[idx].prePosProcessorPtr->postprocLine(aux.c_str(),tdState.caseconv);
  }
  else
  {
    hyp=tdPerUserVarsVec[idx].translatorPtr->translate(sentenceToTranslate);
    result=tdCommonVars.smtModelPtr->getTransInPlainText(hyp);
  }
  if(verbose)
  {
    tdCommonVars.smtModelPtr->printHyp(hyp,cerr);
  }

      // Print word graph
  int ret=ERROR;
#ifndef THOT_DISABLE_REC
  ret=tdPerUserVarsVec[idx].translatorPtr->printWordGraph(wgFilename);
#endif
  
  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);

  return ret;
}

//--------------------------
bool ThotDecoder::sentPairVerCov(int user_id,
                                 const char *srcSent,
                                 const char *refSent,
                                 std::string& result,
                                 int verbose/*=0*/)
{
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

      // Obtain index vector given user_id
  size_t idx=get_vecidx_for_user_id(user_id);
  if(verbose) cerr<<"user_id: "<<user_id<<", idx: "<<idx<<endl;

  if(verbose)
  {
    cerr<<"Verifying model coverage for sentence pair: "<<srcSent<<" ||| "<<refSent<<endl;
  }
  CURR_MODEL_TYPE::Hypothesis hyp;
  if(tdState.preprocId)
  {
    std::string preprocSrcSent=tdPerUserVarsVec[idx].prePosProcessorPtr->preprocLine(srcSent,tdState.caseconv,false);
    std::string preprocRefSent=tdPerUserVarsVec[idx].prePosProcessorPtr->preprocLine(refSent,tdState.caseconv,false);
    if(verbose)
    {
      cerr<<" - preproc. source: "<<preprocSrcSent<<endl;
      cerr<<" - preproc. reference: "<<preprocRefSent<<endl;
    }
    hyp=tdPerUserVarsVec[idx].translatorPtr->verifyCoverageForRef(preprocSrcSent.c_str(),preprocRefSent.c_str());
    std::string aux=tdCommonVars.smtModelPtr->getTransInPlainText(hyp);
    result=tdPerUserVarsVec[idx].prePosProcessorPtr->postprocLine(aux.c_str(),tdState.caseconv);
  }
  else
  {
    hyp=tdPerUserVarsVec[idx].translatorPtr->verifyCoverageForRef(srcSent,refSent);
    result=tdCommonVars.smtModelPtr->getTransInPlainText(hyp);
  }
  if(verbose)
  {
    tdCommonVars.smtModelPtr->printHyp(hyp,cerr);
    if(!tdCommonVars.smtModelPtr->isComplete(hyp))
      cerr<<"No coverage for sentence pair!"<<endl;
  }

  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);

  if(!tdCommonVars.smtModelPtr->isComplete(hyp))
    return OK;
  else return ERROR;
}

//--------------------------
void ThotDecoder::setNonMonotonicity(int nomon,
                                     int verbose/*=0*/)
{
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

  if(verbose)
  {
    cerr<<"Non-monotonicity is now set to "<<nomon<<endl;
  }

      // Set appropriate model parameters
  if(nomon==0)
  {
    tdCommonVars.smtModelPtr->setMonotoneSearch();
  }
  else
  {
    tdCommonVars.smtModelPtr->resetMonotoneSearch();
    tdCommonVars.smtModelPtr->set_U_par(nomon);
  }

  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);
}

//--------------------------
void ThotDecoder::set_W(float W_par,
                        int verbose/*=0*/)
{
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

  if(verbose)
  {
    cerr<<"W parameter is set to "<<W_par<<endl;
  }
  tdCommonVars.smtModelPtr->set_W_par(W_par);

  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);
}
  

//--------------------------
void ThotDecoder::set_S(int user_id,
                        unsigned int S_par,
                        int verbose/*=0*/)
{
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

      // Obtain index vector given user_id
  size_t idx=get_vecidx_for_user_id(user_id);
  if(verbose) cerr<<"user_id: "<<user_id<<", idx: "<<idx<<endl;

  if(verbose)
  {
    cerr<<"S parameter is set to "<<S_par<<endl;
  }
  tdPerUserVarsVec[idx].translatorPtr->set_S_par(S_par);

  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);
}
  
//--------------------------
void ThotDecoder::set_A(unsigned int A_par,
                        int verbose/*=0*/)
{
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

  if(verbose)
  {
    cerr<<"A parameter is set to "<<A_par<<endl;
  }
  tdCommonVars.smtModelPtr->set_A_par(A_par);

  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);
}
  
//--------------------------
void ThotDecoder::set_E(unsigned int E_par,
                        int verbose/*=0*/)
{
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

  if(verbose)
  {
    cerr<<"E parameter is set to "<<E_par<<endl;
  }
  tdCommonVars.smtModelPtr->set_E_par(E_par);

  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);
}

//--------------------------
void ThotDecoder::set_be(int user_id,
                         int be_par,
                         int verbose/*=0*/)
{
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

      // Obtain index vector given user_id
  size_t idx=get_vecidx_for_user_id(user_id);
  if(verbose) cerr<<"user_id: "<<user_id<<", idx: "<<idx<<endl;

  if(verbose)
  {
    cerr<<"be parameter is set to "<<be_par<<endl;
  }
  tdPerUserVarsVec[idx].translatorPtr->set_breadthFirst(!be_par);

  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);
}

//--------------------------
#ifdef MULTI_STACK_USE_GRAN
bool ThotDecoder::set_G(int user_id,
                        unsigned int G_par,
                        int verbose/*=0*/)
{
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

      // Obtain index vector given user_id
  size_t idx=get_vecidx_for_user_id(user_id);
  if(verbose) cerr<<"user_id: "<<user_id<<", idx: "<<idx<<endl;

  if(verbose)
  {
    cerr<<"G parameter is set to "<<G_par<<endl;
  }
  tdPerUserVarsVec[idx].translatorPtr->set_G_par(G_par);

  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);

  return OK;
}
#else
bool ThotDecoder::set_G(int /*user_id*/,
                        unsigned int /*G_par*/,
                        int verbose/*=0*/)
{
  if(verbose)
    cerr<<"Warning! G parameter is not used with the current multi-stack algorithm."<<endl;
  return OK;
}
#endif

//--------------------------
void ThotDecoder::set_h(unsigned int h_par,
                        int verbose/*=0*/)
{
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

  if(verbose)
  {
    cerr<<"h parameter is set to "<<h_par<<endl;
  }
      // Set heuristic
  tdCommonVars.smtModelPtr->setHeuristic(h_par);

  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);
}
  
//--------------------------
bool ThotDecoder::set_np(int user_id,
                         unsigned int np_par,
                         int verbose/*=0*/)
{
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

      // Obtain index vector given user_id
  size_t idx=get_vecidx_for_user_id(user_id);
  if(verbose) cerr<<"user_id: "<<user_id<<", idx: "<<idx<<endl;

  if(verbose)
  {
    cerr<<"np parameter is set to "<<np_par<<endl;
  }
      // Set np value
  bool b;
  if(tdPerUserVarsVec[idx]._nbUncoupledAssistedTransPtr)
  {
    tdPerUserVarsVec[idx]._nbUncoupledAssistedTransPtr->set_n(np_par);
    b=OK;
  }
  else
  {
    if(verbose)
      cerr<<"warning! np parameter cannot be applied to coupled translators."<<endl;
    b=ERROR;
  }

  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);

  return b;  
}
  
//--------------------------
#if CURR_ECM_VALID_FOR_WG == 0
bool ThotDecoder::set_wgp(int /*user_id*/,
                          float /*wgp_par*/,
                          int /*verbose=0*/)
{
  cerr<<"Warning! wgp parameter is never used in the current configuration."<<endl;
  return OK;
}
#else
bool ThotDecoder::set_wgp(int user_id,
                          float wgp_par,
                          int verbose/*=0*/)
{
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

      // Obtain index vector given user_id
  size_t idx=get_vecidx_for_user_id(user_id);
  if(verbose) cerr<<"user_id: "<<user_id<<", idx: "<<idx<<endl;

  if(verbose)
  {
    cerr<<"wgp parameter is set to "<<wgp_par<<endl;
  }
      // Set wgp value
  if(tdPerUserVarsVec[idx].wgUncoupledAssistedTransPtr)
    tdPerUserVarsVec[idx].wgUncoupledAssistedTransPtr->set_wgp(wgp_par);
  else
  {
    cerr<<"warning! wgp parameter cannot be applied to translators that do not use word-graphs."<<endl;
  }

  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);

  return OK;
}
#endif

//--------------------------
void ThotDecoder::set_preproc(int user_id,
                              unsigned int preprocId_par,
                              int verbose/*=0*/)
{
      // Obtain index vector given user_id
  size_t idx=get_vecidx_for_user_id(user_id);
  if(verbose) cerr<<"user_id: "<<user_id<<", idx: "<<idx<<endl;

  tdState.preprocId=preprocId_par;
  if(tdPerUserVarsVec[idx].prePosProcessorPtr!=0)
    delete tdPerUserVarsVec[idx].prePosProcessorPtr;
  
  switch(preprocId_par)
  {
    case DISABLE_PREPROC:
      tdPerUserVarsVec[idx].prePosProcessorPtr=0;
      if(verbose)
        cerr<<"The Pre/pos-processing steps are disabled."<<endl;
      break;
#ifndef THOT_DISABLE_PREPROC_CODE
    case XRCE_PREPROC1: tdPerUserVarsVec[idx].prePosProcessorPtr=new XRCE_PrePosProcessor1();
      if(verbose)
        cerr<<"Pre/pos-processing steps enabled for the XRCE corpus, version 1."<<endl;
      break;
    case XRCE_PREPROC2: tdPerUserVarsVec[idx].prePosProcessorPtr=new XRCE_PrePosProcessor2();
      if(verbose)
        cerr<<"Pre/pos-processing steps enabled for the XRCE corpus, version 2."<<endl;
      break;
    case XRCE_PREPROC3: tdPerUserVarsVec[idx].prePosProcessorPtr=new XRCE_PrePosProcessor3();
      if(verbose)
        cerr<<"Pre/pos-processing steps enabled for the XRCE corpus, version 3."<<endl;
      break;
    case XRCE_PREPROC4: tdPerUserVarsVec[idx].prePosProcessorPtr=new XRCE_PrePosProcessor4();
      if(verbose)
        cerr<<"Pre/pos-processing steps enabled for the XRCE corpus, version 4."<<endl;
      break;
    case EU_PREPROC1: tdPerUserVarsVec[idx].prePosProcessorPtr=new EU_PrePosProcessor1();
      if(verbose)
        cerr<<"Pre/pos-processing steps enabled for the EU corpus, version 1."<<endl;
      break;
    case EU_PREPROC2: tdPerUserVarsVec[idx].prePosProcessorPtr=new EU_PrePosProcessor2();
      if(verbose)
        cerr<<"Pre/pos-processing steps enabled for the EU corpus, version 2."<<endl;
      break;
#endif
    default: tdPerUserVarsVec[idx].prePosProcessorPtr=0;
      if(verbose)
        cerr<<"Warning! invalid preprocId, the pre/pos-processing steps are disabled"<<endl;
      break;
  }
}
  
//--------------------------
void ThotDecoder::set_tmw(Vector<float> tmwVec_par,
                               int verbose/*=0*/)
{
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

      // Set translation model weights
  tdCommonVars.smtModelPtr->setWeights(tmwVec_par);
    
  if(verbose)
  {
    tdCommonVars.smtModelPtr->printWeights(cerr);
    cerr<<endl;
  }

  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);
}
  
//--------------------------
void ThotDecoder::set_ecw(Vector<float> ecwVec_par,
                          int verbose/*=0*/)
{
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

      // Set error correcting model weights
  tdCommonVars.ecModelPtr->setWeights(ecwVec_par);
    
  if(verbose)
  {
    tdCommonVars.ecModelPtr->printWeights(cerr);
    cerr<<endl;
  }

  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);
}
  
//--------------------------
void ThotDecoder::set_catw(int user_id,
                           Vector<float> catwVec_par,
                           int verbose/*=0*/)
{
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

      // Obtain index vector given user_id
  size_t idx=get_vecidx_for_user_id(user_id);
  if(verbose) cerr<<"user_id: "<<user_id<<", idx: "<<idx<<endl;

      // Set cat weights
  tdPerUserVarsVec[idx].assistedTransPtr->setWeights(catwVec_par);
    
  if(verbose)
  {
    tdPerUserVarsVec[idx].assistedTransPtr->printWeights(cerr);
    cerr<<endl;
  }

  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);
}

//--------------------------
bool ThotDecoder::set_wgh(const char *wgHandlerFileName,
                          int verbose/*=0*/)
{
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

  if(verbose)
    cerr<<"Loading worgraph handler information from file "<<wgHandlerFileName<<endl;
  
  bool ret=tdCommonVars.wgHandlerPtr->load(wgHandlerFileName);
  
  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);

  return ret;
}

//--------------------------
void ThotDecoder::startCat(int user_id,
                           const char *sentenceToTranslate,
                           std::string &catResult,
                           int verbose/*=0*/)
{
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

      // Obtain index vector given user_id
  size_t idx=get_vecidx_for_user_id(user_id);
  if(verbose) cerr<<"user_id: "<<user_id<<", idx: "<<idx<<endl;
  
  if(tdPerUserVarsVec[idx]._nbUncoupledAssistedTransPtr)
  {
        // Execute specific actions for uncoupled assisted translators
        // Disable best score pruning
    tdPerUserVarsVec[idx].translatorPtr->useBestScorePruning(false);
  }

  totalPrefixVec[idx]="";
  if(verbose)
  {
    cerr<<"***** Translating sentence: "<<sentenceToTranslate<<endl;
  }
  
  if(tdState.preprocId)
  {
    std::string aux;
    std::string preprocSent=tdPerUserVarsVec[idx].prePosProcessorPtr->preprocLine(sentenceToTranslate,tdState.caseconv,true);
    RejectedWordsSet emptyRejWordsSet;
    aux=tdPerUserVarsVec[idx].assistedTransPtr->translateWithPrefix(preprocSent,"",emptyRejWordsSet,verbose);
    catResult=tdPerUserVarsVec[idx].prePosProcessorPtr->postprocLine(aux.c_str(),tdState.caseconv);
    if(verbose)
    {
      cerr<<"* preprocessed sentence: "<<preprocSent<<endl;
      cerr<<"* translation: "<<aux<<endl;
      cerr<<"* postprocessed translation: "<<catResult<<endl;
    }
  }
  else
  {
        // No pre/post-processing steps are applied
    RejectedWordsSet emptyRejWordsSet;
    catResult=tdPerUserVarsVec[idx].assistedTransPtr->translateWithPrefix(sentenceToTranslate,"",emptyRejWordsSet,verbose);
    if(verbose)
    {
      cerr<<"* translation: "<<catResult<<endl;
    }
  }

  if(tdPerUserVarsVec[idx]._nbUncoupledAssistedTransPtr)
  {
        // Execute specific actions for uncoupled assisted translators
        // Enable best score pruning
    tdPerUserVarsVec[idx].translatorPtr->useBestScorePruning(true);
  }

  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);
}
  
//--------------------------
void ThotDecoder::addStrToPref(int user_id,
                               const char *strToAddToPref,
                               RejectedWordsSet& rejectedWords,
                               std::string &catResult,
                               int verbose/*=0*/)
{
// NOTE: this operation can only be executed as a non-atomic operation
// for wordgraph-based assisted translators. In addition to this, the
// pre/pos-processing code is not reentrant (it is internally based on
// non-reentrant flex code), because of this, a specific mutex has been
// added.

#if THOT_AT_TYPE != WG_UNCTRANS
  pthread_mutex_lock(&atomic_op_mut);
      /////////// begin of mutex
#else
      // Increase non_atomic_ops_running variable
  increase_non_atomic_ops_running();
#endif

      // Obtain index vector given user_id
  size_t idx=get_vecidx_for_user_id(user_id);
  if(verbose) cerr<<"user_id: "<<user_id<<", idx: "<<idx<<endl;

  if(tdPerUserVarsVec[idx]._nbUncoupledAssistedTransPtr)
  {
        // Execute specific actions for uncoupled assisted translators
        // Disable best score pruning
    tdPerUserVarsVec[idx].translatorPtr->useBestScorePruning(false);
  }
  
  if(totalPrefixVec[idx]=="") totalPrefixVec[idx]=strToAddToPref;
  else totalPrefixVec[idx]=totalPrefixVec[idx]+strToAddToPref;
  
  if(verbose)
  {
    cerr<<"Add string to prefix: "<<strToAddToPref<<"|"<<endl;
    cerr<<"Total prefix (not preprocessed): "<<totalPrefixVec[idx]<<"|"<<endl;
  }
  if(tdState.preprocId)
  {    
    std::string trans;
    std::string expLastWord;

    pthread_mutex_lock(&preproc_mut);
        /////////// begin of preproc mutex 
    std::string preprocPrefUnexpanded=tdPerUserVarsVec[idx].prePosProcessorPtr->preprocLine(totalPrefixVec[idx],tdState.caseconv,false);
        /////////// end of preproc mutex 
    pthread_mutex_unlock(&preproc_mut);
    
    std::string preprocPref=preprocPrefUnexpanded;
    
    expLastWord=expandLastWord(preprocPref);
    tdPerUserVarsVec[idx].assistedTransPtr->resetPrefix();
    trans=tdPerUserVarsVec[idx].assistedTransPtr->addStrToPrefix(preprocPref,
                                                           rejectedWords,
                                                           verbose);
    pthread_mutex_lock(&preproc_mut);
        /////////// begin of preproc mutex 
    catResult=robustObtainFinalOutput(tdPerUserVarsVec[idx].prePosProcessorPtr,
                                      totalPrefixVec[idx],
                                      preprocPrefUnexpanded,
                                      preprocPref,
                                      trans.c_str(),
                                      tdState.caseconv);
        /////////// end of preproc mutex 
    pthread_mutex_unlock(&preproc_mut);
    
    if(verbose)
    {
      cerr<<"Preprocessed prefix: "<<preprocPrefUnexpanded<<"|"<<endl;
      cerr<<"Expanded preprocessed prefix: "<<preprocPref<<"|"<<endl;
      cerr<<"Translation: "<<trans<<"|"<<endl;
      cerr<<"Postproc. trans. with user pref.: "<<catResult<<"|"<<endl;
    }
  }
  else
  {
        // No pre/post-processing steps are applied
    std::string expLastWord;
    std::string expPref;
    std::string trans;

    expPref=totalPrefixVec[idx];
    expLastWord=expandLastWord(expPref);
    tdPerUserVarsVec[idx].assistedTransPtr->resetPrefix();
    trans=tdPerUserVarsVec[idx].assistedTransPtr->addStrToPrefix(expPref,rejectedWords,verbose);
    catResult=robustMergeTransWithUserPref(trans,expPref);
    
    if(verbose)
    {
      cerr<<"Expanded prefix: "<<expPref<<"|"<<endl;
      cerr<<"Translation: "<<trans<<"|"<<endl;
      cerr<<"Final output: "<<catResult<<"|"<<endl;
    }
  }

  if(tdPerUserVarsVec[idx]._nbUncoupledAssistedTransPtr)
  {
        // Execute specific actions for uncoupled assisted translators
        // Enable best score pruning
    tdPerUserVarsVec[idx].translatorPtr->useBestScorePruning(true);
  }

#if THOT_AT_TYPE != WG_UNCTRANS
      /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);
#else
      // Decrease non_atomic_ops_running variable
  decrease_non_atomic_ops_running();
#endif
}

//--------------------------
void ThotDecoder::setPref(int user_id,
                          const char *prefStr,
                          RejectedWordsSet& rejectedWords,
                          std::string &catResult,
                          int verbose/*=0*/)
{
      // Obtain index vector given user_id
  size_t idx=get_vecidx_for_user_id(user_id);

  std::string strToAddToPref;
  
  if(StrProcUtils::isPrefix(totalPrefixVec[idx],prefStr))
  {
        // Old prefix is a prefix of the new one
    size_t prefStrLen=strlen(prefStr);
    for(size_t i=totalPrefixVec[idx].size();i<prefStrLen;++i)
      strToAddToPref.push_back(prefStr[i]);
  }
  else
  {
        // Old prefix is not a prefix of the new one
    resetPrefix(user_id,verbose);
    strToAddToPref=prefStr;
  }
      // Invoke addStrToPref() function
  addStrToPref(user_id,strToAddToPref.c_str(),rejectedWords,catResult,verbose);
}

//--------------------------
void ThotDecoder::resetPrefix(int user_id,
                              int verbose/*=0*/)
{
      // Obtain index vector given user_id
  size_t idx=get_vecidx_for_user_id(user_id);
  if(verbose) cerr<<"user_id: "<<user_id<<", idx: "<<idx<<endl;

  if(verbose)
  {
    cerr<<"Reset prefix"<<endl;
  }

  totalPrefixVec[idx]="";
  tdPerUserVarsVec[idx].assistedTransPtr->resetPrefix();
}

//--------------------------
bool ThotDecoder::use_caseconv(int user_id,
                               const char *caseConvFile,
                               int verbose/*=0*/)
{
  int ret;
  
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

      // Obtain index vector given user_id
  size_t idx=get_vecidx_for_user_id(user_id);
  if(verbose) cerr<<"user_id: "<<user_id<<", idx: "<<idx<<endl;

  if(tdState.preprocId)
  {
    ret=tdPerUserVarsVec[idx].prePosProcessorPtr->loadCapitInfo(caseConvFile);
    if(ret==OK)
    {
      tdState.caseconv=true;
    }
  }
  else
  {
    cerr<<"Warning! case conversion cannot be activated because pre/pos-processing steps are disabled."<<endl;
    ret=OK;
  }

  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);

  return ret;
}

//--------------------------
std::string ThotDecoder::preprocStr(int user_id,
                                    std::string str)
{
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

  std::string result;
  
  if(tdState.preprocId)
  {
        // Obtain index vector given user_id
    size_t idx=get_vecidx_for_user_id(user_id);

    result=tdPerUserVarsVec[idx].prePosProcessorPtr->preprocLine(str,tdState.caseconv,false);
  }
  else result=str;

  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);

  return result;
}

//--------------------------
std::string ThotDecoder::postprocStr(int user_id,
                                     std::string str)
{
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

  std::string result;
  
  if(tdState.preprocId)
  {
        // Obtain index vector given user_id
    size_t idx=get_vecidx_for_user_id(user_id);

    result=tdPerUserVarsVec[idx].prePosProcessorPtr->postprocLine(str,tdState.caseconv);
  }
  else result=str;

  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);

  return result;  
}

//--------------------------
void ThotDecoder::clearTrans(int /*verbose*//*=0*/)
{
  release();
}

//--------------------------
bool ThotDecoder::printModels(int verbose/*=0*/)
{
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

  if(verbose)
  {
    cerr<<"Printing models stored by the translator (tm files prefix: "<<tdState.tmFilesPrefixGiven<<endl<<" , lm files prefix: "<<tdState.lmfileLoaded<<" , ecm files prefix: "<<tdState.lmfileLoaded<<")"<<endl;;
  }
    
      // Print alignment model parameters
  int ret=tdCommonVars.smtModelPtr->printAligModel(tdState.tmFilesPrefixGiven);
  if(ret==OK)
  {
        // Print language model parameters
    int ret=tdCommonVars.smtModelPtr->printLangModel(tdState.lmfileLoaded);
    if(ret==OK)
    {
          // Print error correcting model parameters
      ret=tdCommonVars.ecModelPtr->print(tdState.ecmFilesPrefixGiven.c_str()); 
    }
  }

  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);

  return ret;
}

//--------------------------
int ThotDecoder::init_idx_data(size_t idx)
{    
      // Create a translator instance
  tdPerUserVarsVec[idx].translatorPtr=new CURR_MSTACK_TYPE<CURR_MODEL_TYPE>();

      // Set breadthFirst flag
  tdPerUserVarsVec[idx].translatorPtr->set_breadthFirst(false);

      // Link translation model
  tdPerUserVarsVec[idx].translatorPtr->link_smt_model(tdCommonVars.smtModelPtr);

      // Enable best score pruning
  tdPerUserVarsVec[idx].translatorPtr->useBestScorePruning(true);

      // Create error correcting model for uncoupled cat instance
  tdPerUserVarsVec[idx].ecModelForNbUcatPtr=new CURR_ECM_NB_UCAT_TYPE();

      // Link ecm for ucat with ecm
  tdPerUserVarsVec[idx].ecModelForNbUcatPtr->link_ecm(tdCommonVars.ecModelPtr);

      // Create assisted translator instance
#if THOT_AT_TYPE != WG_UNCTRANS
      // Create regular assisted translator instance
  tdPerUserVarsVec[idx].assistedTransPtr=new CURR_AT_TYPE<CURR_MODEL_TYPE>();
#else
# if CURR_ECM_VALID_FOR_WG == 1
      // Create word-graph based assisted translator instance
  tdPerUserVarsVec[idx].assistedTransPtr=new CURR_AT_TYPE<CURR_MODEL_TYPE,CURR_ECM_TYPE>();
# else
  cerr<<"Fatal error! current error correcting model cannot be combined with word-graph based assisted translators"<<endl;
  return ERROR;
# endif
#endif
  
      // Link translator with the assisted translator
  tdPerUserVarsVec[idx].assistedTransPtr->link_stack_trans(tdPerUserVarsVec[idx].translatorPtr);

      // Check if assistedTransPtr points to an uncoupled assisted
      // translator
  tdPerUserVarsVec[idx]._nbUncoupledAssistedTransPtr=dynamic_cast<_nbUncoupledAssistedTrans<CURR_MODEL_TYPE>*>(tdPerUserVarsVec[idx].assistedTransPtr);
  if(tdPerUserVarsVec[idx]._nbUncoupledAssistedTransPtr)
  {
        // Execute specific actions for uncoupled assisted translators
      
        // Link error correcting model with the assisted translator if it
        // is an uncoupled tranlator
    tdPerUserVarsVec[idx]._nbUncoupledAssistedTransPtr->link_cat_ec_model(tdPerUserVarsVec[idx].ecModelForNbUcatPtr);
      
        // Set the default size of n-best translations list used in
        // uncoupled assisted translation
    tdPerUserVarsVec[idx]._nbUncoupledAssistedTransPtr->set_n(TD_USER_NP_DEFAULT);
  }

      // Check if assistedTransPtr points to an uncoupled assisted
      // translator based on word-graphs
#if CURR_ECM_VALID_FOR_WG == 1
      // Create word-graph processor instance
  tdPerUserVarsVec[idx].wgpPtr=new CURR_WGP_TYPE<CURR_ECM_TYPE>;
    
  tdPerUserVarsVec[idx].wgUncoupledAssistedTransPtr=dynamic_cast<WgUncoupledAssistedTrans<CURR_MODEL_TYPE,CURR_ECM_TYPE>*>(tdPerUserVarsVec[idx].assistedTransPtr);
  if(tdPerUserVarsVec[idx].wgUncoupledAssistedTransPtr)
  {
        // Execute specific actions for uncoupled assisted translators
        // based on word-graphs
      
        // Link ecm for word-graphs to word-graph processor
    tdPerUserVarsVec[idx].wgpPtr->link_ecm_wg(tdCommonVars.ecModelPtr);
    
        // Link word-graph processor to uncoupled assisted translator
    tdPerUserVarsVec[idx].wgUncoupledAssistedTransPtr->link_wgp(tdPerUserVarsVec[idx].wgpPtr);

        // Link word-graph handler to uncoupled assisted translator
    tdPerUserVarsVec[idx].wgUncoupledAssistedTransPtr->link_wgh(tdCommonVars.wgHandlerPtr);

        // Set the default word-graph pruning threshold used in coupled
        // assisted translation
    tdPerUserVarsVec[idx].wgUncoupledAssistedTransPtr->set_wgp(TD_USER_WGP_DEFAULT);
  }
#endif

      // Initialize prePosProcessorPtr for idx
  tdPerUserVarsVec[idx].prePosProcessorPtr=NULL;

  return OK;
}

//--------------------------
void ThotDecoder::wait_on_non_atomic_op_cond(void)
{
  pthread_mutex_lock(&non_atomic_op_mut);
  /////////// begin of mutex
  while(non_atomic_ops_running>0)
  {
    pthread_cond_wait(&non_atomic_op_cond,&non_atomic_op_mut);
        // The pthread_cond_wait() function is used to block on a
        // condition variable. It is called with mutex locked by the
        // calling thread or undefined behaviour will result.

        // This function atomically release mutex and cause the calling
        // thread to block on the condition variable

        // Upon successful return, the mutex has been locked and is
        // owned by the calling thread.
  }

      // NOTE: this function do not force non-atomic ops to end, just
      // wait for it. Some extra code would be required to force
      // non-atomic ops to end (an extra mutex will be necessary when
      // increasing the non_atomic_ops_running variable).
}

//--------------------------
void ThotDecoder::unlock_non_atomic_op_mut(void)
{
  /////////// end of mutex 
  pthread_mutex_unlock(&non_atomic_op_mut);
}

//--------------------------
void ThotDecoder::increase_non_atomic_ops_running(void)
{
  pthread_mutex_lock(&non_atomic_op_mut);
  /////////// begin of mutex
      // Increase variable
  ++non_atomic_ops_running;
  /////////// end of mutex 
  pthread_mutex_unlock(&non_atomic_op_mut);
}

//--------------------------
void ThotDecoder::decrease_non_atomic_ops_running(void)
{
  pthread_mutex_lock(&non_atomic_op_mut);
  /////////// begin of mutex
  
      // Decrease variable
  --non_atomic_ops_running;

      // Restart threads waiting on non_atomic_op_cond
  if(non_atomic_ops_running==0)
    pthread_cond_broadcast(&non_atomic_op_cond);
      // The pthread_cond_broadcast() function shall unblock all threads
      // currently blocked on the specified condition variable.
  
  /////////// end of mutex 
  pthread_mutex_unlock(&non_atomic_op_mut);
}

//--------------------------
void ThotDecoder::release_idx_data(size_t idx)
{
      // Check if data is already released
  if(!idxDataReleased[idx])
  {
    delete tdPerUserVarsVec[idx].translatorPtr;
    delete tdPerUserVarsVec[idx].ecModelForNbUcatPtr;
#if CURR_ECM_VALID_FOR_WG == 1
    delete tdPerUserVarsVec[idx].wgpPtr;
#endif
    delete tdPerUserVarsVec[idx].assistedTransPtr;

    if(tdPerUserVarsVec[idx].prePosProcessorPtr!=NULL)
      delete tdPerUserVarsVec[idx].prePosProcessorPtr;
    tdPerUserVarsVec[idx].prePosProcessorPtr=NULL;

        // Register idx data as deleted
    idxDataReleased[idx]=true;
  }
}

//--------------------------
size_t ThotDecoder::get_vecidx_for_user_id(int user_id)
{
  pthread_mutex_lock(&user_id_to_idx_mut);
  /////////// begin of mutex 
  size_t idx;
  std::map<int,size_t>::iterator mapIter;
  
      // Obtain idx
  mapIter=userIdToIdx.find(user_id);
  if(mapIter!=userIdToIdx.end())
  {
    idx=mapIter->second;
  }
  else
  {
    idx=tdPerUserVarsVec.size();
    userIdToIdx[user_id]=idx;
  }

      // Initialize vectors
  while(tdPerUserVarsVec.size()<=idx)
  {
    idxDataReleased.push_back(false);
    ThotDecoderPerUserVars tdPerUserVars;
    tdPerUserVarsVec.push_back(tdPerUserVars);
    init_idx_data(tdPerUserVarsVec.size()-1);
    
    std::string totalPrefix;
    totalPrefixVec.push_back(totalPrefix);
  }
  
  /////////// end of mutex 
  pthread_mutex_unlock(&user_id_to_idx_mut);
   
  return idx;
}

//--------------------------
std::string ThotDecoder::robustObtainFinalOutput(BasePrePosProcessor* prePosProcessorPtr,
                                                 std::string unpreprocPref,
                                                 std::string preprocPrefUnexpanded,
                                                 std::string preprocPref,
                                                 std::string trans,
                                                 bool caseconv)
{
  Vector<std::string> preprocPrefVec=StrProcUtils::stringToStringVector(preprocPref);
  Vector<std::string> transVec=StrProcUtils::stringToStringVector(trans);
  std::string checkedTrans;
  
      // Check if translation does not contain the prefix
  if(!StrProcUtils::isPrefixStrVec(preprocPrefVec,transVec))
    checkedTrans=preprocPref;
  else
    checkedTrans=trans;

      // Process checked translation
  std::string postproctrans;
  std::string catResult;
  
  postproctrans=postprocWithCriticalError(prePosProcessorPtr,unpreprocPref,preprocPrefUnexpanded,preprocPref,checkedTrans,caseconv);
  catResult=robustMergePostProcTransWithUserPref(postproctrans,unpreprocPref);
  
  return catResult;
}

//--------------------------
std::string ThotDecoder::postprocWithCriticalError(BasePrePosProcessor* prePosProcessorPtr,
                                                   std::string unpreprocPref,
                                                   std::string preprocPrefUnexpanded,
                                                   std::string preprocPref,
                                                   std::string trans,
                                                   bool caseconv)
{
  std::string startStr;
  std::string endStr;
  std::string lastPrefWordCompletion;
  std::string endPrefMarker;
  std::string modifiedTrans;
  std::string postprocModifiedTrans;
  std::string suffixInModifiedTrans;
  std::string result;
  size_t i;
  bool lastPrefWordIsCategory;
  
      // This function is able to work even if a pre/post-processing
      // critical error occurs. This function introduces a special tag
      // to mark the end of the post-processed user prefix. This tag
      // allows to distinguish between the part of postprocessed
      // translation which accounts for the user prefix, and the part
      // which accounts for the CAT system suffix

      // Determine if the last preprocessed prefix word is a category
      // (last word must be a complete word so the existence of an
      // ending blank character is checked)

  if(preprocPrefUnexpanded.size()>0 && preprocPrefUnexpanded[preprocPrefUnexpanded.size()-1]==' ')
  {
    std::string lastPrefWord=StrProcUtils::getLastWord(preprocPrefUnexpanded);
    lastPrefWordIsCategory=prePosProcessorPtr->isCategory(lastPrefWord);
  }
  else
  {
    lastPrefWordIsCategory=false;
  }
  
      // Determine the end of the prefix in the translation
  size_t prefEndInTrans=preprocPrefUnexpanded.size();
  if(prefEndInTrans>0 && trans.size()>preprocPrefUnexpanded.size() && preprocPrefUnexpanded[preprocPrefUnexpanded.size()-1]!=' ')
  {
    while(prefEndInTrans<trans.size() && trans[prefEndInTrans]!=' ')
    {
      if(!lastPrefWordIsCategory)
        lastPrefWordCompletion.push_back(trans[prefEndInTrans]);
      ++prefEndInTrans;
    }
    if(prefEndInTrans<trans.size()) ++prefEndInTrans;
  }
      // Obtain the completion of the last prefix word if it is a
      // category and it is not completed
  if(lastPrefWordIsCategory)
  {
    if(unpreprocPref.size()>0 && unpreprocPref[unpreprocPref.size()-1]==' ')
    {
      lastPrefWordCompletion="";
    }
    else
    {
      std::string postprocPref=prePosProcessorPtr->postprocLine(preprocPref.c_str(),caseconv);
      std::string postprocPrefLastWord=StrProcUtils::getLastWord(postprocPref);
      std::string unpreprocPrefLastWord=StrProcUtils::getLastWord(unpreprocPref);
      lastPrefWordCompletion=getWordCompletion(unpreprocPrefLastWord,postprocPrefLastWord);
    }
  }

      // Obtain the prefix and the suffix in the preprocessed translation
  for(i=0;i<prefEndInTrans;++i)
    startStr.push_back(trans[i]);
  for(;i<trans.size();++i)
    endStr.push_back(trans[i]);

      // Introduce the special tag to mark the end of the prefix
  endPrefMarker=END_OF_PREF_MARK;
  if(endStr=="") modifiedTrans=startStr+" "+endPrefMarker;
  else modifiedTrans=startStr+endPrefMarker+" "+endStr;
  
      // Postprocess translation with the special tag
  postprocModifiedTrans=prePosProcessorPtr->postprocLine(modifiedTrans.c_str(),caseconv);
  
      // Search the special tag in the postprocessed translation and
      // retrieve the suffix given by the CAT system
  size_t startOfPrefMarker=postprocModifiedTrans.find(endPrefMarker,0);
      // Check if the postprocessing stage has remove the
      // end-of-pref. marker
  if(startOfPrefMarker>postprocModifiedTrans.size())
    startOfPrefMarker=postprocModifiedTrans.size();
  for(i=startOfPrefMarker+endPrefMarker.size();i<postprocModifiedTrans.size();++i)
    suffixInModifiedTrans.push_back(postprocModifiedTrans[i]);

      // Obtain the final postprocessed translation
  if(unpreprocPref.size()>0 && unpreprocPref[unpreprocPref.size()-1]==' ')
    unpreprocPref=StrProcUtils::removeLastBlank(unpreprocPref);
  result=unpreprocPref+lastPrefWordCompletion+suffixInModifiedTrans;
  
//   cerr<<"******************************"<<endl;
//   cerr<<"unpreprocPref: "<<unpreprocPref<<"|"<<endl; 
//   cerr<<"preprocPrefUnexpanded: "<<preprocPrefUnexpanded<<"|"<<endl;
//   cerr<<"trans: "<<trans<<"|"<<endl;
//   cerr<<"prefEndInTrans: "<<prefEndInTrans<<endl;
//   cerr<<"startStr: "<<startStr<<"|"<<endl;
//   cerr<<"endStr: "<<endStr<<"|"<<endl;
//   cerr<<"lastPrefWordCompletion: "<<lastPrefWordCompletion<<"|"<<endl;
//   cerr<<"modifiedTrans: "<<modifiedTrans<<"|"<<endl;
//   cerr<<"postprocModifiedTrans: "<<postprocModifiedTrans<<"|"<<endl;
//   cerr<<"suffixInModifiedTrans: "<<suffixInModifiedTrans<<"|"<<endl;
//   cerr<<"result: "<<result<<"|"<<endl;

  return result;
}

//--------------------------
std::string ThotDecoder::robustMergeTransWithUserPref(std::string trans,
                                                      std::string totalPrefix)
{
      // This function merges the translation of the CAT system with the
      // user prefix. This function works correctly even if there are
      // blank characters between words
  
      // Check if translation does not contain the prefix
  Vector<std::string> totalPrefixVec=StrProcUtils::stringToStringVector(totalPrefix);
  Vector<std::string> transVec=StrProcUtils::stringToStringVector(trans);
  if(!StrProcUtils::isPrefixStrVec(totalPrefixVec,transVec))
  {
    return totalPrefix;
  }
  else
  {
        // Process correct translation
    std::string catResult;
    unsigned int prefIdx=0;

        // Merge trans and totalPrefix taking into account blank
        // characters contained in totalPrefix
    for(unsigned int i=0;i<trans.size();++i)
    {
      if(prefIdx<totalPrefix.size())
      {       
        while(totalPrefix[prefIdx]==' ')
        {
          catResult.push_back(' ');
          ++prefIdx;
        }
        if(trans[i]!=' ')
        {
          catResult.push_back(trans[i]);
          ++prefIdx;
        }
      }
      else
      {
        catResult.push_back(trans[i]);
      }
    }
        // Add last blank characters contained in totalPrefix if any
    for(;prefIdx<totalPrefix.size();++prefIdx)
    {
      catResult.push_back(totalPrefix[prefIdx]);
    }
    
    return catResult;
  }
}

//--------------------------
std::string ThotDecoder::robustMergePostProcTransWithUserPref(std::string postproctrans,
                                                              std::string totalPrefix)
{
      // This function merges the post processed translation of the CAT
      // system with the user prefix. This function works correctly even
      // if the pre/post-process stage alters the number of words given
      // in the user prefix.

  std::string catResult;
  catResult=postproctrans;
  for(unsigned int i=0;i<totalPrefix.size();++i)
  {
    if(i<catResult.size()) catResult[i]=totalPrefix[i];
    else catResult.push_back(totalPrefix[i]);
  }
  return catResult;
}

//--------------------------
std::string ThotDecoder::expandLastWord(std::string& partialSent)
{
  std::string lastWord="";
  bool lastWordIsComplete=false;
 
  if(partialSent.size()>0)
  {
    unsigned int i=partialSent.size();
    if(partialSent[partialSent.size()-1]==' ') lastWordIsComplete=true;
    else
    {
          // Retrieve last word
      while(i!=0 && partialSent[i]!=' ')
      {
        --i;
      }
      if(partialSent[i]==' ') ++i;
      for(;i<partialSent.size();++i)
      {
        lastWord=lastWord+partialSent[i];
      }
    }
        // Check whether to predict the last word
    if(lastWordIsComplete || lastWord.size()<=MINIMUM_WORD_LENGTH_TO_EXPAND)
    {
          // do not predict last word if it is complete or if it is
          // complete but only its first MINIMUM_WORD_LENGTH_TO_EXPAND
          // characters have been introduced
      return lastWord;
    }
    else
    {
          // predict last word
      pair<Count,std::string> pcs;
      Vector<std::string> strVec=StrProcUtils::stringToStringVector(partialSent);
      Vector<std::string> hist;

      if(strVec.size()>=3) hist.push_back(strVec[strVec.size()-3]);
      if(strVec.size()>=2) hist.push_back(strVec[strVec.size()-2]);
      pcs=tdCommonVars.smtModelPtr->getBestSuffixGivenHist(hist,lastWord);
      if(pcs.second.size()>0) partialSent=partialSent+pcs.second+" ";
      else partialSent=partialSent+pcs.second;
      return lastWord+pcs.second;
    }
  }
  else
  {
    return lastWord;
  }
}

//--------------------------
std::string ThotDecoder::getWordCompletion(std::string uncompleteWord,
                                           std::string completeWord)

{
  if(uncompleteWord.size()>=completeWord.size()) return "";
  else
  {
    std::string completion;
    unsigned int l;
    
    for(l=0;l<uncompleteWord.size();++l)
    {
      if(uncompleteWord[l]!=completeWord[l]) break; 
    }
        // l is the length of the prefix shared by uncompleteWord and completeWord
    if(l>=1)
    {
      for(unsigned int i=uncompleteWord.size();i<completeWord.size();++i)
        completion.push_back(completeWord[i]);
    }    
    return completion;
  }
}

//--------------------------
ThotDecoder::~ThotDecoder()
{
      // Release server variables
  release();

      // Delete pointers
  delete tdCommonVars.wgHandlerPtr;
  delete tdCommonVars.smtModelPtr;
  delete tdCommonVars.ecModelPtr;

#ifdef THOT_ENABLE_UPDATE_LLWEIGHTS
  delete tdCommonVars.weightUpdateEnginePtr;
#endif
  
      // Destroy mutexes and conditions
  pthread_mutex_destroy(&user_id_to_idx_mut);
  pthread_mutex_destroy(&atomic_op_mut);
  pthread_mutex_destroy(&non_atomic_op_mut);
  pthread_mutex_destroy(&preproc_mut);
  pthread_cond_destroy(&non_atomic_op_cond);
}
