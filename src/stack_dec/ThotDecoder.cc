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
      // Determine which implementation is being used
  tdCommonVars.featureBasedImplEnabled=featureBasedImplIsEnabled();

      // Call the appropriate initialization for current implementation
  if(tdCommonVars.featureBasedImplEnabled)
    init_translator_feat_impl();
  else
    init_translator_legacy_impl();
}

//--------------------------
bool ThotDecoder::featureBasedImplIsEnabled(void)
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

//--------------------------
void ThotDecoder::init_translator_legacy_impl(void)
{
      // Show static types
  std::cerr<<"Static types:"<<std::endl;
  std::cerr<<"- SMT model type (SmtModel): "<<SMT_MODEL_TYPE_NAME<<" ("<<THOT_SMTMODEL_H<<")"<<std::endl;
  std::cerr<<"- Language model state (LM_Hist): "<<LM_STATE_TYPE_NAME<<" ("<<THOT_LM_STATE_H<<")"<<std::endl;
  std::cerr<<"- Partial probability information for single word models (PpInfo): "<<PPINFO_TYPE_NAME<<" ("<<THOT_PPINFO_H<<")"<<std::endl;

      // Initialize class factories
  int err=tdCommonVars.dynClassFactoryHandler.init_smt_and_imt(THOT_MASTER_INI_PATH);
  if(err==THOT_ERROR)
    exit(THOT_ERROR);
  
      // Create server variables  
  tdCommonVars.langModelInfoPtr=new LangModelInfo;

  tdCommonVars.langModelInfoPtr->wpModelPtr=tdCommonVars.dynClassFactoryHandler.baseWordPenaltyModelDynClassLoader.make_obj(tdCommonVars.dynClassFactoryHandler.baseWordPenaltyModelInitPars);
  if(tdCommonVars.langModelInfoPtr->wpModelPtr==NULL)
  {
    std::cerr<<"Error: BaseWordPenaltyModel pointer could not be instantiated"<<std::endl;
    exit(THOT_ERROR);
  }
  
  tdCommonVars.langModelInfoPtr->lModelPtr=tdCommonVars.dynClassFactoryHandler.baseNgramLMDynClassLoader.make_obj(tdCommonVars.dynClassFactoryHandler.baseNgramLMInitPars);
  if(tdCommonVars.langModelInfoPtr->lModelPtr==NULL)
  {
    std::cerr<<"Error: BaseNgramLM pointer could not be instantiated"<<std::endl;
    exit(THOT_ERROR);
  }
  
  tdCommonVars.phrModelInfoPtr=new PhraseModelInfo;
  tdCommonVars.phrModelInfoPtr->invPbModelPtr=tdCommonVars.dynClassFactoryHandler.basePhraseModelDynClassLoader.make_obj(tdCommonVars.dynClassFactoryHandler.basePhraseModelInitPars);
  if(tdCommonVars.phrModelInfoPtr->invPbModelPtr==NULL)
  {
    std::cerr<<"Error: BasePhraseModel pointer could not be instantiated"<<std::endl;
    exit(THOT_ERROR);
  }

  tdCommonVars.swModelInfoPtr=new SwModelInfo;

  tdCommonVars.wgHandlerPtr=new WgHandler;

  tdCommonVars.ecModelPtr=tdCommonVars.dynClassFactoryHandler.baseErrorCorrectionModelDynClassLoader.make_obj(tdCommonVars.dynClassFactoryHandler.baseErrorCorrectionModelInitPars);
  if(tdCommonVars.ecModelPtr==NULL)
  {
    std::cerr<<"Error: BaseErrorCorrectionModel pointer could not be instantiated"<<std::endl;
    exit(THOT_ERROR);
  }

      // Check if error correction model is valid for word graphs
  BaseWgProcessorForAnlp* wgpPtr;
  wgpPtr=tdCommonVars.dynClassFactoryHandler.baseWgProcessorForAnlpDynClassLoader.make_obj(tdCommonVars.dynClassFactoryHandler.baseWgProcessorForAnlpInitPars);
  if(wgpPtr==NULL)
  {
    std::cerr<<"Error: BaseWgProcessorForAnlp pointer could not be instantiated"<<std::endl;
    exit(THOT_ERROR);
  }
  else
  {
    if(!wgpPtr->link_ecm_wg(tdCommonVars.ecModelPtr))
      tdCommonVars.curr_ecm_valid_for_wg=false;
    else
      tdCommonVars.curr_ecm_valid_for_wg=true;
        // Delete temporary pointer
    delete wgpPtr;
  }

  tdCommonVars.scorerPtr=tdCommonVars.dynClassFactoryHandler.baseScorerDynClassLoader.make_obj(tdCommonVars.dynClassFactoryHandler.baseScorerInitPars);
  if(tdCommonVars.scorerPtr==NULL)
  {
    std::cerr<<"Error: BaseScorer pointer could not be instantiated"<<std::endl;
    exit(THOT_ERROR);
  }

  tdCommonVars.llWeightUpdaterPtr=tdCommonVars.dynClassFactoryHandler.baseLogLinWeightUpdaterDynClassLoader.make_obj(tdCommonVars.dynClassFactoryHandler.baseLogLinWeightUpdaterInitPars);
  if(tdCommonVars.llWeightUpdaterPtr==NULL)
  {
    std::cerr<<"Error: BaseLogLinWeightUpdater pointer could not be instantiated"<<std::endl;
    exit(THOT_ERROR);
  }

      // Link scorer to weight updater
  if(!tdCommonVars.llWeightUpdaterPtr->link_scorer(tdCommonVars.scorerPtr))
  {
    std::cerr<<"Error: Scorer class could not be linked to log-linear weight updater"<<std::endl;
    exit(THOT_ERROR);
  }

      // Instantiate smt model
  tdCommonVars.smtModelPtr=new SmtModel();
  
      // Link language model, phrase model and single word model if
      // appliable
  _phraseBasedTransModel<SmtModel::Hypothesis>* phrbtm_ptr=dynamic_cast<_phraseBasedTransModel<SmtModel::Hypothesis>* >(tdCommonVars.smtModelPtr);
  if(phrbtm_ptr)
  {
    phrbtm_ptr->link_lm_info(tdCommonVars.langModelInfoPtr);
    phrbtm_ptr->link_pm_info(tdCommonVars.phrModelInfoPtr);
  }
  _phrSwTransModel<SmtModel::Hypothesis>* base_pbswtm_ptr=dynamic_cast<_phrSwTransModel<SmtModel::Hypothesis>* >(tdCommonVars.smtModelPtr);
  if(base_pbswtm_ptr)
    base_pbswtm_ptr->link_swm_info(tdCommonVars.swModelInfoPtr);
    
      // Initialize mutexes and conditions
  pthread_mutex_init(&user_id_to_idx_mut,NULL);
  pthread_mutex_init(&atomic_op_mut,NULL);
  pthread_mutex_init(&non_atomic_op_mut,NULL);
  pthread_mutex_init(&preproc_mut,NULL);
  pthread_cond_init(&non_atomic_op_cond,NULL);
  non_atomic_ops_running=0;
}

//--------------------------
void ThotDecoder::init_translator_feat_impl(void)
{
      // Show static types
  std::cerr<<"Static types:"<<std::endl;
  std::cerr<<"- SMT model type (SmtModel): "<<SMT_MODEL_TYPE_NAME<<" ("<<THOT_SMTMODEL_H<<")"<<std::endl;
  std::cerr<<"- Language model state (LM_Hist): "<<LM_STATE_TYPE_NAME<<" ("<<THOT_LM_STATE_H<<")"<<std::endl;
  std::cerr<<"- Partial probability information for single word models (PpInfo): "<<PPINFO_TYPE_NAME<<" ("<<THOT_PPINFO_H<<")"<<std::endl;

      // Initialize class factories
  int err=tdCommonVars.dynClassFactoryHandler.init_smt_and_imt(THOT_MASTER_INI_PATH);
  if(err==THOT_ERROR)
    exit(THOT_ERROR);
  
      // Create server variables
  tdCommonVars.wgHandlerPtr=new WgHandler;

  tdCommonVars.ecModelPtr=tdCommonVars.dynClassFactoryHandler.baseErrorCorrectionModelDynClassLoader.make_obj(tdCommonVars.dynClassFactoryHandler.baseErrorCorrectionModelInitPars);
  if(tdCommonVars.ecModelPtr==NULL)
  {
    std::cerr<<"Error: BaseErrorCorrectionModel pointer could not be instantiated"<<std::endl;
    exit(THOT_ERROR);
  }

      // Check if error correction model is valid for word graphs
  BaseWgProcessorForAnlp* wgpPtr;
  wgpPtr=tdCommonVars.dynClassFactoryHandler.baseWgProcessorForAnlpDynClassLoader.make_obj(tdCommonVars.dynClassFactoryHandler.baseWgProcessorForAnlpInitPars);
  if(wgpPtr==NULL)
  {
    std::cerr<<"Error: BaseWgProcessorForAnlp pointer could not be instantiated"<<std::endl;
    exit(THOT_ERROR);
  }
  else
  {
    if(!wgpPtr->link_ecm_wg(tdCommonVars.ecModelPtr))
      tdCommonVars.curr_ecm_valid_for_wg=false;
    else
      tdCommonVars.curr_ecm_valid_for_wg=true;
        // Delete temporary pointer
    delete wgpPtr;
  }

  tdCommonVars.scorerPtr=tdCommonVars.dynClassFactoryHandler.baseScorerDynClassLoader.make_obj(tdCommonVars.dynClassFactoryHandler.baseScorerInitPars);
  if(tdCommonVars.scorerPtr==NULL)
  {
    std::cerr<<"Error: BaseScorer pointer could not be instantiated"<<std::endl;
    exit(THOT_ERROR);
  }

  tdCommonVars.llWeightUpdaterPtr=tdCommonVars.dynClassFactoryHandler.baseLogLinWeightUpdaterDynClassLoader.make_obj(tdCommonVars.dynClassFactoryHandler.baseLogLinWeightUpdaterInitPars);
  if(tdCommonVars.llWeightUpdaterPtr==NULL)
  {
    std::cerr<<"Error: BaseLogLinWeightUpdater pointer could not be instantiated"<<std::endl;
    exit(THOT_ERROR);
  }

      // Link scorer to weight updater
  if(!tdCommonVars.llWeightUpdaterPtr->link_scorer(tdCommonVars.scorerPtr))
  {
    std::cerr<<"Error: Scorer class could not be linked to log-linear weight updater"<<std::endl;
    exit(THOT_ERROR);
  }

      // Instantiate smt model
  tdCommonVars.smtModelPtr=new SmtModel();
  
      // Link features information if appliable 
  _pbTransModel<SmtModel::Hypothesis>* pbtm_ptr=dynamic_cast<_pbTransModel<SmtModel::Hypothesis>* >(tdCommonVars.smtModelPtr);
  if(pbtm_ptr)
    pbtm_ptr->link_feats_info(tdCommonVars.featureHandler.getFeatureInfoPtr());

      // Define feature handler class loaders
  tdCommonVars.featureHandler.setWordPenSoFile(tdCommonVars.dynClassFactoryHandler.baseWordPenaltyModelSoFileName);
  tdCommonVars.featureHandler.setDefaultLangSoFile(tdCommonVars.dynClassFactoryHandler.baseNgramLMSoFileName);
  tdCommonVars.featureHandler.setDefaultTransSoFile(tdCommonVars.dynClassFactoryHandler.basePhraseModelSoFileName);
  tdCommonVars.featureHandler.setDefaultSingleWordSoFile(tdCommonVars.dynClassFactoryHandler.baseSwAligModelSoFileName);
    
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
int ThotDecoder::init_idx_data(size_t idx)
{    
      // Create a translator instance
  tdPerUserVarsVec[idx].stackDecoderPtr=tdCommonVars.dynClassFactoryHandler.baseStackDecoderDynClassLoader.make_obj(tdCommonVars.dynClassFactoryHandler.baseStackDecoderInitPars);
  if(tdPerUserVarsVec[idx].stackDecoderPtr==NULL)
  {
    std::cerr<<"Error: BaseStackDecoder pointer could not be instantiated"<<std::endl;
    return THOT_ERROR;
  }

      // Set breadthFirst flag
  tdPerUserVarsVec[idx].stackDecoderPtr->set_breadthFirst(false);

      // Create statistical machine translation model instance (it is
      // cloned from the main one)
  BaseSmtModel<SmtModel::Hypothesis>* baseSmtModelPtr=tdCommonVars.smtModelPtr->clone();
  tdPerUserVarsVec[idx].smtModelPtr=dynamic_cast<BasePbTransModel<SmtModel::Hypothesis>* >(baseSmtModelPtr);

      // Create translation constraints object
  tdPerUserVarsVec[idx].trConstraintsPtr=tdCommonVars.dynClassFactoryHandler.baseTranslationConstraintsDynClassLoader.make_obj(tdCommonVars.dynClassFactoryHandler.baseTranslationConstraintsInitPars);
  if(tdPerUserVarsVec[idx].trConstraintsPtr==NULL)
  {
    std::cerr<<"Error: BaseTranslationConstraints pointer could not be instantiated"<<std::endl;
    return THOT_ERROR;
  }

      // Link translation constraints
  tdPerUserVarsVec[idx].smtModelPtr->link_trans_constraints(tdPerUserVarsVec[idx].trConstraintsPtr);

      // Link statistical machine translation model
  int ret=tdPerUserVarsVec[idx].stackDecoderPtr->link_smt_model(tdPerUserVarsVec[idx].smtModelPtr);
  if(ret==THOT_ERROR)
  {
    std::cerr<<"Error while linking smt model to decoder, revise master.ini file"<<std::endl;
    return THOT_ERROR;
  }

      // Enable best score pruning
  tdPerUserVarsVec[idx].stackDecoderPtr->useBestScorePruning(true);

      // Determine if the translator incorporates hypotheses recombination
  tdPerUserVarsVec[idx].stackDecoderRecPtr=dynamic_cast<_stackDecoderRec<SmtModel>*>(tdPerUserVarsVec[idx].stackDecoderPtr);
  
      // Create error correcting model for uncoupled cat instance
  tdPerUserVarsVec[idx].ecModelForNbUcatPtr=tdCommonVars.dynClassFactoryHandler.baseEcModelForNbUcatDynClassLoader.make_obj(tdCommonVars.dynClassFactoryHandler.baseEcModelForNbUcatInitPars);
  if(tdPerUserVarsVec[idx].ecModelForNbUcatPtr==NULL)
  {
    std::cerr<<"Error: BaseEcModelForNbUcat pointer could not be instantiated"<<std::endl;
    return THOT_ERROR;
  }
  
      // Link ecm for ucat with ecm
  tdPerUserVarsVec[idx].ecModelForNbUcatPtr->link_ecm(tdCommonVars.ecModelPtr);

      // Create assisted translator instance
  tdPerUserVarsVec[idx].assistedTransPtr=tdCommonVars.dynClassFactoryHandler.baseAssistedTransDynClassLoader.make_obj(tdCommonVars.dynClassFactoryHandler.baseAssistedTransInitPars);
  if(tdPerUserVarsVec[idx].assistedTransPtr==NULL)
  {
    std::cerr<<"Error: BaseAssistedTrans pointer could not be instantiated"<<std::endl;
    return THOT_ERROR;
  }
  
      // Link translator with the assisted translator
  ret=tdPerUserVarsVec[idx].assistedTransPtr->link_stack_trans(tdPerUserVarsVec[idx].stackDecoderPtr);

      // Check if assistedTransPtr points to an uncoupled assisted
      // translator
  tdPerUserVarsVec[idx]._nbUncoupledAssistedTransPtr=dynamic_cast<_nbUncoupledAssistedTrans<SmtModel>*>(tdPerUserVarsVec[idx].assistedTransPtr);
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
  if(tdCommonVars.curr_ecm_valid_for_wg)
  {
        // Create word-graph processor instance
    tdPerUserVarsVec[idx].wgpPtr=tdCommonVars.dynClassFactoryHandler.baseWgProcessorForAnlpDynClassLoader.make_obj(tdCommonVars.dynClassFactoryHandler.baseWgProcessorForAnlpInitPars);
    if(tdPerUserVarsVec[idx].wgpPtr==NULL)
    {
      std::cerr<<"Error: BaseWgProcessorForAnlp pointer could not be instantiated"<<std::endl;
      return THOT_ERROR;
    }
    
    tdPerUserVarsVec[idx].wgUncoupledAssistedTransPtr=dynamic_cast<WgUncoupledAssistedTrans<SmtModel>*>(tdPerUserVarsVec[idx].assistedTransPtr);
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
  }
      // Initialize prePosProcessorPtr for idx
  tdPerUserVarsVec[idx].prePosProcessorPtr=NULL;

  return THOT_OK;
}

//--------------------------
void ThotDecoder::release_idx_data(size_t idx)
{
      // Check if data is already released
  if(!idxDataReleased[idx])
  {
    delete tdPerUserVarsVec[idx].smtModelPtr;
    delete tdPerUserVarsVec[idx].stackDecoderPtr;
    delete tdPerUserVarsVec[idx].ecModelForNbUcatPtr;
    if(tdCommonVars.curr_ecm_valid_for_wg)
    {
      delete tdPerUserVarsVec[idx].wgpPtr;
    }
    delete tdPerUserVarsVec[idx].assistedTransPtr;

    if(tdPerUserVarsVec[idx].prePosProcessorPtr!=NULL)
      delete tdPerUserVarsVec[idx].prePosProcessorPtr;
    tdPerUserVarsVec[idx].prePosProcessorPtr=NULL;
    delete tdPerUserVarsVec[idx].trConstraintsPtr;

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

      // Initialize per user variables
  while(tdPerUserVarsVec.size()<=idx)
  {
    idxDataReleased.push_back(false);
    ThotDecoderPerUserVars tdPerUserVars;
    tdPerUserVarsVec.push_back(tdPerUserVars);
    int ret=init_idx_data(tdPerUserVarsVec.size()-1);
    if(ret==THOT_ERROR)
      exit(1);
    
    std::string totalPrefix;
    totalPrefixVec.push_back(totalPrefix);
  }

      // Initialize per user mutexes
  while(per_user_mut.size()<=idx)
  {
    pthread_mutex_t user_mut;
    pthread_mutex_init(&user_mut,NULL);
    per_user_mut.push_back(user_mut);
  }
  
  /////////// end of mutex 
  pthread_mutex_unlock(&user_id_to_idx_mut);
   
  return idx;
}

//--------------------------
int ThotDecoder::initUsingCfgFile(std::string cfgFile,
                                  ThotDecoderUserPars& tdup,
                                  int verbose)
{
  int ret;
  int argc;
  std::vector<std::string> argv_stl;
  std::string comment="#";

      // Extract parameters from file
  ret=extractParsFromFile(cfgFile.c_str(),argc,argv_stl,comment);
  if(ret==THOT_ERROR)
  {
    return THOT_ERROR;
  }
  std::cerr<<"Processing configuration file ("<<cfgFile<<")..."<<std::endl;
  
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
  std::vector<float> olParsVec;
  std::vector<float> tmWeightsVec;
  std::vector<float> ecWeightsVec;
  
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
        std::cerr<<"Error: no value for -tm parameter."<<std::endl;
        return THOT_ERROR;
      }
      else
      {
        std::cerr<<"-tm parameter changed from \""<<tm_str<<"\" to \""<<argv_stl[i+1]<<"\""<<std::endl;
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
        std::cerr<<"Error: no value for -lm parameter."<<std::endl;
        return THOT_ERROR;
      }
      else
      {
        std::cerr<<"-lm parameter changed from \""<<lm_str<<"\" to \""<<argv_stl[i+1]<<"\""<<std::endl;
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
        std::cerr<<"Error: no value for -W parameter."<<std::endl;
        return THOT_ERROR;
      }
      else
      {
        std::cerr<<"-W parameter changed from \""<<W<<"\" to \""<<argv_stl[i+1]<<"\""<<std::endl;
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
        std::cerr<<"Error: no value for -S parameter."<<std::endl;
        return THOT_ERROR;
      }
      else
      {
        std::cerr<<"-S parameter changed from \""<<tdup.S<<"\" to \""<<argv_stl[i+1]<<"\""<<std::endl;
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
        std::cerr<<"Error: no value for -A parameter."<<std::endl;
        return THOT_ERROR;
      }
      else
      {
        std::cerr<<"-A parameter changed from \""<<A<<"\" to \""<<argv_stl[i+1]<<"\""<<std::endl;
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
        std::cerr<<"Error: no value for -E parameter."<<std::endl;
        return THOT_ERROR;
      }
      else
      {
        std::cerr<<"-E parameter changed from \""<<E<<"\" to \""<<argv_stl[i+1]<<"\""<<std::endl;
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
        std::cerr<<"Error: no value for -nomon parameter."<<std::endl;
        return THOT_ERROR;
      }
      else
      {
        std::cerr<<"-nomon parameter changed from \""<<nomon<<"\" to \""<<argv_stl[i+1]<<"\""<<std::endl;
        nomon=atoi(argv_stl[i+1].c_str());
        ++matched;
        ++i;
      }
    }

        // -be parameter
    if(argv_stl[i]=="-be" && !matched)
    {
      std::cerr<<"-be parameter given (not given by default)"<<std::endl;
      tdup.be=true;
      ++matched;
    }

        // -G parameter
    if(argv_stl[i]=="-G" && !matched)
    {
      if(i==argc-1)
      {
        std::cerr<<"Error: no value for -G parameter."<<std::endl;
        return THOT_ERROR;
      }
      else
      {
        std::cerr<<"-G parameter changed from \""<<tdup.G<<"\" to \""<<argv_stl[i+1]<<"\""<<std::endl;
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
        std::cerr<<"Error: no value for -h parameter."<<std::endl;
        return THOT_ERROR;
      }
      else
      {
        std::cerr<<"-h parameter changed from \""<<h<<"\" to \""<<argv_stl[i+1]<<"\""<<std::endl;
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
        std::cerr<<"Error: no values for -olp parameter."<<std::endl;
        return THOT_ERROR;
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

        std::cerr<<"-olp parameter given:";
        for(unsigned int i=0;i<olParsVec.size();++i)
          std::cerr<<" "<<olParsVec[i];
        std::cerr<<std::endl;
      }
    }

        // -tmw parameter
    if(argv_stl[i]=="-tmw" && !matched)
    {
      if(i==argc-1)
      {
        std::cerr<<"Error: no values for -tmw parameter."<<std::endl;
        return THOT_ERROR;
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
        std::cerr<<"-tmw parameter given:";
        for(unsigned int i=0;i<tmWeightsVec.size();++i)
          std::cerr<<" "<<tmWeightsVec[i];
        std::cerr<<std::endl;
      }
    }

        // -ecw parameter
    if(argv_stl[i]=="-ecw" && !matched)
    {
      if(i==argc-1)
      {
        std::cerr<<"Error: no values for -ecw parameter."<<std::endl;
        return THOT_ERROR;
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
        std::cerr<<"-ecw parameter given:";
        for(unsigned int i=0;i<ecWeightsVec.size();++i)
          std::cerr<<" "<<ecWeightsVec[i];
        std::cerr<<std::endl;
      }
    }

        // -catw parameter
    if(argv_stl[i]=="-catw" && !matched)
    {
      if(i==argc-1)
      {
        std::cerr<<"Error: no values for -catw parameter."<<std::endl;
        return THOT_ERROR;
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
        std::cerr<<"-catw parameter given:";
        for(unsigned int i=0;i<tdup.catWeightsVec.size();++i)
          std::cerr<<" "<<tdup.catWeightsVec[i];
        std::cerr<<std::endl;
      }
    }

        // -np parameter
    if(argv_stl[i]=="-np" && !matched)
    {
      if(i==argc-1)
      {
        std::cerr<<"Error: no value for -np parameter."<<std::endl;
        return THOT_ERROR;
      }
      else
      {
        std::cerr<<"-np parameter changed from \""<<tdup.np<<"\" to \""<<argv_stl[i+1]<<"\""<<std::endl;
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
        std::cerr<<"Error: no value for -wgp parameter."<<std::endl;
        return THOT_ERROR;
      }
      else
      {
        std::cerr<<"-wgp parameter changed from \""<<tdup.wgp<<"\" to \""<<argv_stl[i+1]<<"\""<<std::endl;
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
        std::cerr<<"Error: no value for -wgh parameter."<<std::endl;
        return THOT_ERROR;
      }
      else
      {
        std::cerr<<"-wgh parameter changed from \""<<tdup.wgh_str<<"\" to \""<<argv_stl[i+1]<<"\""<<std::endl;
        tdup.wgh_str=argv_stl[i+1];
        ++matched;
        ++i;
      }
    }

        // -sp option
    if(argv_stl[i]=="-sp" && !matched)
    {
      std::cerr<<"-sp parameter changed from \""<<false<<"\" to \""<<argv_stl[i+1]<<"\""<<std::endl;
      tdup.sp=atoi(argv_stl[i+1].c_str());
      ++matched;
      ++i;
    }

        // -uc parameter
    if(argv_stl[i]=="-uc" && !matched)
    {
      if(i==argc-1)
      {
        std::cerr<<"Error: no value for -uc parameter."<<std::endl;
        return THOT_ERROR;
      }
      else
      {
        std::cerr<<"-uc parameter changed from \""<<tdup.uc_str<<"\" to \""<<argv_stl[i+1]<<"\""<<std::endl;
        tdup.uc_str=argv_stl[i+1];
        ++matched;
        ++i;
      }
    }
    ++i;
  }

  // Initialize server

      // Add word penalty model feature
  if(tdCommonVars.featureBasedImplEnabled)
  {
    ret=tdCommonVars.featureHandler.addWpFeat(verbose);
    if(ret==THOT_ERROR)
      return THOT_ERROR;
  }

      // Load language model
  if(tdCommonVars.featureBasedImplEnabled)
  {
    ret=load_lm_feat_impl(lm_str.c_str(),verbose);
    if(ret==THOT_ERROR) return THOT_ERROR;
  }
  else
  {
    ret=load_lm_legacy_impl(lm_str.c_str(),verbose);
    if(ret==THOT_ERROR) return THOT_ERROR;
  }

      // Load translation model
  if(tdCommonVars.featureBasedImplEnabled)
  {
    ret=load_tm_feat_impl(tm_str.c_str(),verbose);
    if(ret==THOT_ERROR) return THOT_ERROR;
  }
  else
  {
    ret=load_tm_legacy_impl(tm_str.c_str(),verbose);
    if(ret==THOT_ERROR) return THOT_ERROR;
  }

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
  if(ret==THOT_ERROR) return THOT_ERROR;

  return THOT_OK;
}

//--------------------------
int ThotDecoder::initUserPars(int user_id,
                              const ThotDecoderUserPars& tdup,
                              int verbose)
{
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

      // Wait until all non-atomic operations have finished
  wait_on_non_atomic_op_cond();

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
  if(ret==THOT_ERROR) return THOT_ERROR;

      // Set cat weights
  set_catw(user_id,tdup.catWeightsVec,verbose);

      // Unlock non_atomic_op_cond mutex
  pthread_mutex_unlock(&non_atomic_op_mut);

  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);

  return THOT_OK;
}

//--------------------------
void ThotDecoder::setNonMonotonicity(int nomon,
                                     int verbose/*=0*/)
{
  if(verbose)
  {
    std::cerr<<"Non-monotonicity is now set to "<<nomon<<std::endl;
  }

      // Set appropriate model parameters
  tdCommonVars.smtModelPtr->set_U_par(nomon);
}

//--------------------------
void ThotDecoder::set_W(float W_par,
                        int verbose/*=0*/)
{
  if(verbose)
  {
    std::cerr<<"W parameter is set to "<<W_par<<std::endl;
  }
  tdCommonVars.smtModelPtr->set_W_par(W_par);
}
  

//--------------------------
void ThotDecoder::set_S(int user_id,
                        unsigned int S_par,
                        int verbose/*=0*/)
{
      // Obtain index vector given user_id
  size_t idx=get_vecidx_for_user_id(user_id);
  if(verbose) std::cerr<<"user_id: "<<user_id<<", idx: "<<idx<<std::endl;

  if(verbose)
  {
    std::cerr<<"S parameter is set to "<<S_par<<std::endl;
  }
  tdPerUserVarsVec[idx].stackDecoderPtr->set_S_par(S_par);
}
  
//--------------------------
void ThotDecoder::set_A(unsigned int A_par,
                        int verbose/*=0*/)
{
  if(verbose)
  {
    std::cerr<<"A parameter is set to "<<A_par<<std::endl;
  }
  tdCommonVars.smtModelPtr->set_A_par(A_par);
}
  
//--------------------------
void ThotDecoder::set_E(unsigned int E_par,
                        int verbose/*=0*/)
{
  if(verbose)
  {
    std::cerr<<"E parameter is set to "<<E_par<<std::endl;
  }
  tdCommonVars.smtModelPtr->set_E_par(E_par);
}

//--------------------------
void ThotDecoder::set_be(int user_id,
                         int be_par,
                         int verbose/*=0*/)
{
      // Obtain index vector given user_id
  size_t idx=get_vecidx_for_user_id(user_id);
  if(verbose) std::cerr<<"user_id: "<<user_id<<", idx: "<<idx<<std::endl;

  if(verbose)
  {
    std::cerr<<"be parameter is set to "<<be_par<<std::endl;
  }
  tdPerUserVarsVec[idx].stackDecoderPtr->set_breadthFirst(!be_par);
}

//--------------------------
bool ThotDecoder::set_G(int user_id,
                        unsigned int G_par,
                        int verbose/*=0*/)
{
      // Obtain index vector given user_id
  size_t idx=get_vecidx_for_user_id(user_id);
  if(verbose) std::cerr<<"user_id: "<<user_id<<", idx: "<<idx<<std::endl;

  if(verbose)
  {
    std::cerr<<"G parameter is set to "<<G_par<<std::endl;
  }
  tdPerUserVarsVec[idx].stackDecoderPtr->set_G_par(G_par);

  return THOT_OK;
}
  
//--------------------------
void ThotDecoder::set_h(unsigned int h_par,
                        int verbose/*=0*/)
{
  if(verbose)
  {
    std::cerr<<"h parameter is set to "<<h_par<<std::endl;
  }
      // Set heuristic
  tdCommonVars.smtModelPtr->setHeuristic(h_par);
}
  
//--------------------------
bool ThotDecoder::set_np(int user_id,
                         unsigned int np_par,
                         int verbose/*=0*/)
{
      // Obtain index vector given user_id
  size_t idx=get_vecidx_for_user_id(user_id);
  if(verbose) std::cerr<<"user_id: "<<user_id<<", idx: "<<idx<<std::endl;

  if(verbose)
  {
    std::cerr<<"np parameter is set to "<<np_par<<std::endl;
  }
      // Set np value
  bool b;
  if(tdPerUserVarsVec[idx]._nbUncoupledAssistedTransPtr)
  {
    tdPerUserVarsVec[idx]._nbUncoupledAssistedTransPtr->set_n(np_par);
    b=THOT_OK;
  }
  else
  {
    if(verbose)
      std::cerr<<"warning! np parameter cannot be applied to coupled translators."<<std::endl;
    b=THOT_ERROR;
  }

  return b;  
}
  
//--------------------------
bool ThotDecoder::set_wgp(int user_id,
                          float wgp_par,
                          int verbose/*=0*/)
{
      // Check if ECM can be used to process word graphs
  if(!tdCommonVars.curr_ecm_valid_for_wg)
  {
    std::cerr<<"Error: EC model is not valid for word-graphs"<<std::endl;
    return THOT_ERROR;
  }
  
      // Obtain index vector given user_id
  size_t idx=get_vecidx_for_user_id(user_id);
  if(verbose) std::cerr<<"user_id: "<<user_id<<", idx: "<<idx<<std::endl;

  if(verbose)
  {
    std::cerr<<"wgp parameter is set to "<<wgp_par<<std::endl;
  }
      // Set wgp value
  if(tdPerUserVarsVec[idx].wgUncoupledAssistedTransPtr)
    tdPerUserVarsVec[idx].wgUncoupledAssistedTransPtr->set_wgp(wgp_par);
  else
  {
    std::cerr<<"warning! wgp parameter cannot be applied to translators that do not use word-graphs."<<std::endl;
  }

  return THOT_OK;
}
  
//--------------------------
void ThotDecoder::set_preproc(int user_id,
                              unsigned int preprocId_par,
                              int verbose/*=0*/)
{
      // Obtain index vector given user_id
  size_t idx=get_vecidx_for_user_id(user_id);
  if(verbose) std::cerr<<"user_id: "<<user_id<<", idx: "<<idx<<std::endl;

  tdState.preprocId=preprocId_par;
  if(tdPerUserVarsVec[idx].prePosProcessorPtr!=0)
    delete tdPerUserVarsVec[idx].prePosProcessorPtr;
  
  switch(preprocId_par)
  {
    case DISABLE_PREPROC:
      tdPerUserVarsVec[idx].prePosProcessorPtr=0;
      if(verbose)
        std::cerr<<"The Pre/pos-processing steps are disabled."<<std::endl;
      break;
#ifndef THOT_DISABLE_PREPROC_CODE
    case XRCE_PREPROC1: tdPerUserVarsVec[idx].prePosProcessorPtr=new XRCE_PrePosProcessor1();
      if(verbose)
        std::cerr<<"Pre/pos-processing steps enabled for the XRCE corpus, version 1."<<std::endl;
      break;
    case XRCE_PREPROC2: tdPerUserVarsVec[idx].prePosProcessorPtr=new XRCE_PrePosProcessor2();
      if(verbose)
        std::cerr<<"Pre/pos-processing steps enabled for the XRCE corpus, version 2."<<std::endl;
      break;
    case XRCE_PREPROC3: tdPerUserVarsVec[idx].prePosProcessorPtr=new XRCE_PrePosProcessor3();
      if(verbose)
        std::cerr<<"Pre/pos-processing steps enabled for the XRCE corpus, version 3."<<std::endl;
      break;
    case XRCE_PREPROC4: tdPerUserVarsVec[idx].prePosProcessorPtr=new XRCE_PrePosProcessor4();
      if(verbose)
        std::cerr<<"Pre/pos-processing steps enabled for the XRCE corpus, version 4."<<std::endl;
      break;
    case EU_PREPROC1: tdPerUserVarsVec[idx].prePosProcessorPtr=new EU_PrePosProcessor1();
      if(verbose)
        std::cerr<<"Pre/pos-processing steps enabled for the EU corpus, version 1."<<std::endl;
      break;
    case EU_PREPROC2: tdPerUserVarsVec[idx].prePosProcessorPtr=new EU_PrePosProcessor2();
      if(verbose)
        std::cerr<<"Pre/pos-processing steps enabled for the EU corpus, version 2."<<std::endl;
      break;
#endif
    default: tdPerUserVarsVec[idx].prePosProcessorPtr=0;
      if(verbose)
        std::cerr<<"Warning! invalid preprocId, the pre/pos-processing steps are disabled"<<std::endl;
      break;
  }
}
  
//--------------------------
void ThotDecoder::set_tmw(std::vector<float> tmwVec_par,
                          int verbose/*=0*/)
{
      // Set translation model weights
  tdCommonVars.smtModelPtr->setWeights(tmwVec_par);
    
  if(verbose)
  {
    tdCommonVars.smtModelPtr->printWeights(std::cerr);
    std::cerr<<std::endl;
  }
}
  
//--------------------------
void ThotDecoder::set_ecw(std::vector<float> ecwVec_par,
                          int verbose/*=0*/)
{
      // Set error correcting model weights
  tdCommonVars.ecModelPtr->setWeights(ecwVec_par);
    
  if(verbose)
  {
    tdCommonVars.ecModelPtr->printWeights(std::cerr);
    std::cerr<<std::endl;
  }
}
  
//--------------------------
void ThotDecoder::set_catw(int user_id,
                           std::vector<float> catwVec_par,
                           int verbose/*=0*/)
{
      // Obtain index vector given user_id
  size_t idx=get_vecidx_for_user_id(user_id);
  if(verbose) std::cerr<<"user_id: "<<user_id<<", idx: "<<idx<<std::endl;

      // Set cat weights
  tdPerUserVarsVec[idx].assistedTransPtr->setWeights(catwVec_par);
    
  if(verbose)
  {
    tdPerUserVarsVec[idx].assistedTransPtr->printWeights(std::cerr);
    std::cerr<<std::endl;
  }
}

//--------------------------
bool ThotDecoder::set_wgh(const char *wgHandlerFileName,
                          int verbose/*=0*/)
{
  if(verbose)
    std::cerr<<"Loading worgraph handler information from file "<<wgHandlerFileName<<std::endl;
  
  bool ret=tdCommonVars.wgHandlerPtr->load(wgHandlerFileName);
  
  return ret;
}

//--------------------------
bool ThotDecoder::instantiate_swm_info(const char* tmFilesPrefix,
                                       int /*verbose=0*/)
{
      // Return if current translation model does not use sw models
  _phrSwTransModel<SmtModel::Hypothesis>* base_pbswtm_ptr=dynamic_cast<_phrSwTransModel<SmtModel::Hypothesis>* >(tdCommonVars.smtModelPtr);
  if(base_pbswtm_ptr==NULL)
    return THOT_OK;

      // Delete previous instantiation
  deleteSwModelPtrs();

      // Obtain info about translation model entries
  unsigned int numTransModelEntries;
  std::vector<ModelDescriptorEntry> modelDescEntryVec;
  if(extractModelEntryInfo(tmFilesPrefix,modelDescEntryVec)==THOT_OK)
  {
    numTransModelEntries=modelDescEntryVec.size();
  }
  else
  {
    numTransModelEntries=1;
  }

      // Add one swm pointer per each translation model entry
  for(unsigned int i=0;i<numTransModelEntries;++i)
  {
    tdCommonVars.swModelInfoPtr->swAligModelPtrVec.push_back(tdCommonVars.dynClassFactoryHandler.baseSwAligModelDynClassLoader.make_obj(tdCommonVars.dynClassFactoryHandler.baseSwAligModelInitPars));
    if(tdCommonVars.swModelInfoPtr->swAligModelPtrVec[i]==NULL)
    {
      std::cerr<<"Error: BaseSwAligModel pointer could not be instantiated"<<std::endl;
      return THOT_ERROR;
    }
  }

      // Add one inverse swm pointer per each translation model entry
  for(unsigned int i=0;i<numTransModelEntries;++i)
  {
    tdCommonVars.swModelInfoPtr->invSwAligModelPtrVec.push_back(tdCommonVars.dynClassFactoryHandler.baseSwAligModelDynClassLoader.make_obj(tdCommonVars.dynClassFactoryHandler.baseSwAligModelInitPars));
    if(tdCommonVars.swModelInfoPtr->invSwAligModelPtrVec[i]==NULL)
    {
      std::cerr<<"Error: BaseSwAligModel pointer could not be instantiated"<<std::endl;
      return THOT_ERROR;
    }
  }

  return THOT_OK;
}

//--------------------------
bool ThotDecoder::load_tm_legacy_impl(const char* tmFilesPrefix,
                                      int verbose/*=0*/)
{
  int ret;
    
  if(strcmp(tdState.tmFilesPrefixGiven.c_str(),tmFilesPrefix)==0)
  {
    if(verbose) std::cerr<<"Translation model already loaded"<<std::endl;
    ret=THOT_OK;
  }
  else
  {
    if(verbose)
    {
      std::cerr<<"Loading translation model given prefix: "<<tmFilesPrefix<<std::endl;
    }
        // Instantiate single word model information
    ret=instantiate_swm_info(tmFilesPrefix,verbose);

    if(ret==THOT_OK)
    {
        // Load alignment model
      _phraseBasedTransModel<SmtModel::Hypothesis>* phrbtm_ptr=dynamic_cast<_phraseBasedTransModel<SmtModel::Hypothesis>* >(tdCommonVars.smtModelPtr);
      if(phrbtm_ptr)
      {
        ret=phrbtm_ptr->loadAligModel(tmFilesPrefix);
        if(ret==THOT_OK)
        {
          tdState.tmFilesPrefixGiven=tmFilesPrefix;
        }
      }
    }
  }

  return ret;
}

//--------------------------
bool ThotDecoder::load_tm_feat_impl(const char* tmFilesPrefix,
                                    int verbose/*=0*/)
{
  int ret;
    
  if(strcmp(tdState.tmFilesPrefixGiven.c_str(),tmFilesPrefix)==0)
  {
    if(verbose) std::cerr<<"Translation model already loaded"<<std::endl;
    ret=THOT_OK;
  }
  else
  {
    ret=tdCommonVars.featureHandler.addTmFeats(tmFilesPrefix,verbose);
        // Store tm information
    if(ret==THOT_OK)
      tdState.tmFilesPrefixGiven=tmFilesPrefix;
  }  

  return ret;
}

//--------------------------
bool ThotDecoder::load_lm_legacy_impl(const char* lmFileName,
                                      int verbose/*=0*/)
{
  int ret;

  if(strcmp(tdState.lmfileLoaded.c_str(),lmFileName)==0)
  {
    if(verbose) std::cerr<<"Language model already loaded"<<std::endl;
    ret=THOT_OK;
  }
  else
  {
    if(verbose)
    {
      std::cerr<<"Loading language model from file: "<<lmFileName<<std::endl;
    }
    _phraseBasedTransModel<SmtModel::Hypothesis>* phrbtm_ptr=dynamic_cast<_phraseBasedTransModel<SmtModel::Hypothesis>* >(tdCommonVars.smtModelPtr);
    if(phrbtm_ptr)
    {
      ret=phrbtm_ptr->loadLangModel(lmFileName);
      if(ret==THOT_OK)
      {
        tdState.lmfileLoaded=lmFileName;
      }
    }
    else
      ret=THOT_ERROR;
  }
  
  return ret;
}

//--------------------------
bool ThotDecoder::load_lm_feat_impl(const char* lmFileName,
                                    int verbose/*=0*/)
{
  int ret;

  if(strcmp(tdState.lmfileLoaded.c_str(),lmFileName)==0)
  {
    if(verbose) std::cerr<<"Language model already loaded"<<std::endl;
    ret=THOT_OK;
  }
  else
  {
    ret=tdCommonVars.featureHandler.addLmFeats(lmFileName,verbose);
    if(ret==THOT_OK)
      tdState.lmfileLoaded=lmFileName;
  }
  
  return ret;  
}

//--------------------------
bool ThotDecoder::load_ecm(const char* ecmFilesPrefix,
                           int verbose/*=0*/)
{
  int ret;

  if(strcmp(tdState.ecmFilesPrefixGiven.c_str(),ecmFilesPrefix)==0)
  {
    if(verbose) std::cerr<<"Error correcting model already loaded"<<std::endl;
    ret=THOT_OK;
  }
  else
  {
    if(verbose)
    {
      std::cerr<<"Loading error correcting model given the prefix: "<<ecmFilesPrefix<<std::endl;
    }
    
    ret=tdCommonVars.ecModelPtr->load(ecmFilesPrefix);
    if(ret==THOT_OK)
    {
      tdState.ecmFilesPrefixGiven=ecmFilesPrefix;
    }
  }

  return ret;
}

//--------------------------
bool ThotDecoder::onlineTrainSentPair(int user_id,
                                      const char *srcSent,
                                      const char *refSent,
                                      int verbose/*=0*/)
{
  int ret;

      // Check if input sentences are empty
  if(strlen(srcSent)==0 || strlen(refSent)==0)
  {
    std::cerr<<"Error: one or both of the input sentences to be trained are empty"<<std::endl;
    return THOT_ERROR;
  }
    
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

      // Wait until all non-atomic operations have finished
  wait_on_non_atomic_op_cond();

      // Obtain index vector given user_id
  size_t idx=get_vecidx_for_user_id(user_id);
  if(verbose) std::cerr<<"user_id: "<<user_id<<", idx: "<<idx<<std::endl;

  if(verbose)
  {
    std::cerr<<"Training sentence pair:"<<std::endl;
    std::cerr<<" - source: "<<srcSent<<std::endl;
    std::cerr<<" - reference: "<<refSent<<std::endl;
  }

      // Check if pre/post processing is enabled
  if(tdState.preprocId)
  {
        // Pre/post processing enabled
    std::string preprocSrcSent=preprocLine(tdPerUserVarsVec[idx].prePosProcessorPtr,srcSent,tdState.caseconv,false);
    std::string preprocRefSent=preprocLine(tdPerUserVarsVec[idx].prePosProcessorPtr,refSent,tdState.caseconv,false);

        // Obtain system translation
    SmtModel::Hypothesis hyp=tdPerUserVarsVec[idx].stackDecoderPtr->translate(preprocSrcSent.c_str());
    std::string preprocSysSent=tdPerUserVarsVec[idx].smtModelPtr->getTransInPlainText(hyp);

    if(verbose)
    {
      std::cerr<<" - preproc. source: "<<preprocSrcSent<<std::endl;
      std::cerr<<" - preproc. reference: "<<preprocRefSent<<std::endl;
      std::cerr<<" - preproc. sys translation: "<<preprocSysSent<<std::endl;
    }
        // Add sentence to word-predictor
    addSentenceToWordPred(preprocRefSent,verbose);

    if(verbose) std::cerr<<"Training models..."<<std::endl;

        // Measure training time
    double prevElapsedTime,elapsedTime,ucpu,scpu;
    ctimer(&prevElapsedTime,&ucpu,&scpu);
    
        // Train generative models
    ret=onlineTrainFeats(preprocSrcSent,preprocRefSent,preprocSysSent,verbose);

    ctimer(&elapsedTime,&ucpu,&scpu);
    if(verbose)
    {
      std::cerr<<"Training process ended."<<std::endl;
      std::cerr<<"Training time: "<<elapsedTime-prevElapsedTime<<std::endl;
    }
  }
  else
  {
        // Pre/post processing disabled

        // Obtain system translation
    if(tdPerUserVarsVec[idx].stackDecoderRecPtr)
      tdPerUserVarsVec[idx].stackDecoderRecPtr->enableWordGraph();

    SmtModel::Hypothesis hyp=tdPerUserVarsVec[idx].stackDecoderPtr->translate(srcSent);
    std::string sysSent=tdPerUserVarsVec[idx].smtModelPtr->getTransInPlainText(hyp);

        // Add sentence to word-predictor
    addSentenceToWordPred(refSent,verbose);

    if(verbose) std::cerr<<"Training models..."<<std::endl;

        // Measure training time
    double prevElapsedTime,elapsedTime,ucpu,scpu;
    ctimer(&prevElapsedTime,&ucpu,&scpu);

#ifdef THOT_ENABLE_UPDATE_LLWEIGHTS

    onlineTrainLogLinWeights(srcSent,refSent,verbose);
  
#endif

        // Train generative models
    ret=onlineTrainFeats(srcSent,refSent,sysSent,verbose);
   
    ctimer(&elapsedTime,&ucpu,&scpu);
    if(verbose) std::cerr<<"Training time: "<<elapsedTime-prevElapsedTime<<std::endl;
  }

      // Unlock non_atomic_op_cond mutex
  pthread_mutex_unlock(&non_atomic_op_mut);

  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);

  return ret;
}

//--------------------------
void ThotDecoder::addSentenceToWordPred(std::string sentence,
                                        int verbose/*=0*/)
{
  if(tdCommonVars.featureBasedImplEnabled)
  {
    tdCommonVars.featureHandler.trainWordPred(StrProcUtils::stringToStringVector(sentence));
  }
  else
  {
    tdCommonVars.smtModelPtr->addSentenceToWordPred(StrProcUtils::stringToStringVector(sentence),verbose);
  }
}

//--------------------------
int ThotDecoder::onlineTrainFeats(std::string srcSent,
                                  std::string refSent,
                                  std::string sysSent,
                                  int verbose/*=0*/)
{
  if(tdCommonVars.featureBasedImplEnabled)
  {
    return tdCommonVars.featureHandler.onlineTrainFeats(tdCommonVars.onlineTrainingPars,
                                                        srcSent,
                                                        refSent,
                                                        sysSent,
                                                        verbose);
  }
  else
  {
    return tdCommonVars.smtModelPtr->onlineTrainFeatsSentPair(srcSent.c_str(),refSent.c_str(),sysSent.c_str(),verbose);    
  }  
}

//--------------------------
void ThotDecoder::onlineTrainLogLinWeights(size_t idx,
                                           const char *srcSent,
                                           const char *refSent,
                                           int verbose/*=0*/)
{
  if(tdPerUserVarsVec[idx].stackDecoderRecPtr)
  {
        // Retrieve pointer to wordgraph (use word-graph provided by the
        // word-graph handler if available)
    std::vector<std::string> sentStrVec=StrProcUtils::stringToStringVector(srcSent);
    bool found;
    std::string wgPathStr=tdCommonVars.wgHandlerPtr->pathAssociatedToSentence(sentStrVec,found);
    if(found)
    {
          // Obtain new weights
      WordGraph wg;
      wg.load(wgPathStr.c_str());
      std::vector<pair<std::string,float> > compWeights;
      tdCommonVars.smtModelPtr->getWeights(compWeights);
      std::vector<float> newWeights;
      WeightUpdateUtils::updateLogLinearWeights(refSent,
                                                &wg,
                                                tdCommonVars.llWeightUpdaterPtr,
                                                compWeights,
                                                newWeights,
                                                verbose);
          // Set new weights
      tdCommonVars.smtModelPtr->setWeights(newWeights);        
    }
    else
    {
          // Obtain new weights
      std::vector<pair<std::string,float> > compWeights;
      tdCommonVars.smtModelPtr->getWeights(compWeights);
      std::vector<float> newWeights;
      WordGraph* wgPtr=tdPerUserVarsVec[idx].stackDecoderRecPtr->getWordGraphPtr();
      WeightUpdateUtils::updateLogLinearWeights(refSent,
                                                wgPtr,
                                                tdCommonVars.llWeightUpdaterPtr,
                                                compWeights,
                                                newWeights,
                                                verbose);
          // Set new weights
      tdCommonVars.smtModelPtr->setWeights(newWeights);
    }    
    tdPerUserVarsVec[idx].stackDecoderRecPtr->disableWordGraph();
  }
}

//--------------------------
void ThotDecoder::setOnlineTrainPars(OnlineTrainingPars onlineTrainingPars,
                                     int verbose/*=0*/)
{
  if(verbose)
  {
    std::cerr<<"Setting online training pars..."<<std::endl;

    std::cerr<<"onlineLearningAlgorithm= "<<onlineTrainingPars.onlineLearningAlgorithm<<" ; ";
    std::cerr<<"learningRatePolicy= "<<onlineTrainingPars.learningRatePolicy<<" ; ";
    std::cerr<<"learnStepSize= "<<onlineTrainingPars.learnStepSize<<" ; ";
    std::cerr<<"emIters= "<<onlineTrainingPars.emIters<<" ; ";
    std::cerr<<"E_par= "<<onlineTrainingPars.E_par<<" ; ";
    std::cerr<<"R_par= "<<onlineTrainingPars.R_par<<std::endl;
  }

  tdCommonVars.onlineTrainingPars=onlineTrainingPars;

      // Handle online training parameters in legacy implementation
  _phraseBasedTransModel<SmtModel::Hypothesis>* pbtm_ptr=dynamic_cast<_phraseBasedTransModel<SmtModel::Hypothesis>* >(tdCommonVars.smtModelPtr);
  if(pbtm_ptr)
    pbtm_ptr->setOnlineTrainingPars(onlineTrainingPars,verbose);
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

      // Wait until all non-atomic operations have finished
  wait_on_non_atomic_op_cond();

      // Obtain index vector given user_id
  size_t idx=get_vecidx_for_user_id(user_id);
  if(verbose) std::cerr<<"user_id: "<<user_id<<", idx: "<<idx<<std::endl;

  
  if(verbose)
  {
    std::cerr<<"Training ecm for string pair:"<<std::endl;
    std::cerr<<" - string x: "<<strx<<std::endl;
    std::cerr<<" - string y: "<<stry<<std::endl;
  }
    
  if(tdState.preprocId)
  {
    std::string preprocx=preprocLine(tdPerUserVarsVec[idx].prePosProcessorPtr,strx,tdState.caseconv,false);
    std::string preprocy=preprocLine(tdPerUserVarsVec[idx].prePosProcessorPtr,stry,tdState.caseconv,false);
    if(verbose)
    {
      std::cerr<<" - preproc. string x: "<<preprocx<<std::endl;
      std::cerr<<" - preproc. string y: "<<preprocy<<std::endl;
    }
    ret=tdCommonVars.ecModelPtr->trainStrPair(preprocx.c_str(),preprocy.c_str(),verbose);
  }
  else
  {
    ret=tdCommonVars.ecModelPtr->trainStrPair(strx,stry,verbose);
  }

      // Unlock non_atomic_op_cond mutex
  pthread_mutex_unlock(&non_atomic_op_mut);

  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);

  return ret;
}

//--------------------------
bool ThotDecoder::translateSentence(int user_id,
                                    const char *sentenceToTranslate,
                                    std::string& result,
                                    std::string& bestHypInfo,
                                    int verbose/*=0*/)
{
      // Increase non_atomic_ops_running variable
  increase_non_atomic_ops_running();
  
      // Obtain index vector given user_id
  size_t idx=get_vecidx_for_user_id(user_id);
  if(verbose) std::cerr<<"user_id: "<<user_id<<", idx: "<<idx<<std::endl;

  pthread_mutex_lock(&per_user_mut[idx]);
  /////////// begin of user mutex

  if(verbose)
  {
    std::cerr<<"Translating sentence: "<<sentenceToTranslate<<std::endl;
  }
  if(tdState.preprocId)
  {
    std::string preprocSrcSent=preprocLine(tdPerUserVarsVec[idx].prePosProcessorPtr,sentenceToTranslate,tdState.caseconv,true);
    if(verbose)
    {
      std::cerr<<" - preproc. source: "<<preprocSrcSent<<std::endl;
    }

        // Obtain translation using precalculated word-graph or translator
    std::string aux=translateSentenceAux(idx,preprocSrcSent,bestHypInfo);
    
    result=postprocLine(tdPerUserVarsVec[idx].prePosProcessorPtr,aux.c_str(),tdState.caseconv);
    if(verbose)
    {
      std::cerr<<" - preproc. target: "<<aux<<std::endl;
      std::cerr<<" - posproc. target: "<<result<<std::endl;     
    }
  }
  else
  {
    result=translateSentenceAux(idx,sentenceToTranslate,bestHypInfo,verbose);
    if(verbose)
    {
      std::cerr<<"- target translation: "<<result<<std::endl;
    }
  }

  /////////// end of user mutex 
  pthread_mutex_unlock(&per_user_mut[idx]);

      // Decrease non_atomic_ops_running variable
  decrease_non_atomic_ops_running();

  return THOT_OK;
}

//--------------------------
std::string ThotDecoder::translateSentenceAux(size_t idx,
                                              std::string sentenceToTranslate,
                                              std::string& bestHypInfo,
                                              int verbose/*=0*/)
{
      // Obtain translation using precalculated word-graph or translator
  bool found;

      // Initialize variables
  bestHypInfo.clear();
  
  std::vector<std::string> sentStrVec=StrProcUtils::stringToStringVector(sentenceToTranslate);
  std::string wgPathStr=tdCommonVars.wgHandlerPtr->pathAssociatedToSentence(sentStrVec,found);
  if(found)
  {
        // Use word graph
    WordGraph wg;

        // Load word graph
    wg.load(wgPathStr.c_str());

        // Obtain original word graph component weights
    std::vector<pair<std::string,float> > originalWgCompWeights;
    wg.getCompWeights(originalWgCompWeights);

        // Print component weight info to the error output
    if(verbose)
    {
      std::cerr<<"Original word graph component vector:";
      for(unsigned int i=0;i<originalWgCompWeights.size();++i)
        std::cerr<<" "<<originalWgCompWeights[i].first<<": "<<originalWgCompWeights[i].second<<";";
      std::cerr<<std::endl;
    }

        // Set current component weights (this operation causes a
        // complete re-scoring of the word graph arcs if there exist
        // score component information for them)
    std::vector<pair<std::string,float> > currCompWeights;
    tdCommonVars.smtModelPtr->getWeights(currCompWeights);
    wg.setCompWeights(currCompWeights);

        // Print component weight info to the error output
    if(verbose)
    {
      std::cerr<<"New word graph component vector:";
      for(unsigned int i=0;i<currCompWeights.size();++i)
        std::cerr<<" "<<currCompWeights[i].first<<": "<<currCompWeights[i].second<<";";
      std::cerr<<std::endl;
    }
    
        // Obtain best path
    std::set<WordGraphArcId> emptyExcludedArcsSet;
    std::vector<WordGraphArc> arcVec;
    wg.bestPathFromFinalStateToIdx(INITIAL_STATE,emptyExcludedArcsSet,arcVec);

        // Obtain translation
    std::vector<std::string> resultVec;
    for(std::vector<WordGraphArc>::reverse_iterator riter=arcVec.rbegin();riter!=arcVec.rend();++riter)
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
    SmtModel::Hypothesis hyp=tdPerUserVarsVec[idx].stackDecoderPtr->translate(sentenceToTranslate.c_str());
    if(verbose)
    {
      std::cerr<<"- source sentence without constraint information: ";
      std::cerr<<tdPerUserVarsVec[idx].smtModelPtr->getCurrentSrcSent()<<std::endl;
      std::cerr<<"- best hypothesis: "<<std::endl;
      tdPerUserVarsVec[idx].smtModelPtr->printHyp(hyp,std::cerr);
    }
    std::string result=tdPerUserVarsVec[idx].smtModelPtr->getTransInPlainText(hyp);
    std::ostringstream stream;
    tdPerUserVarsVec[idx].smtModelPtr->printHyp(hyp,stream);
    bestHypInfo=stream.str();
    bestHypInfo.erase(std::remove(bestHypInfo.begin(), bestHypInfo.end(), '\n'), bestHypInfo.end());
      
    return result;
  }
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
  if(verbose) std::cerr<<"user_id: "<<user_id<<", idx: "<<idx<<std::endl;

  if(verbose)
  {
    std::cerr<<"Verifying model coverage for sentence pair: "<<srcSent<<" ||| "<<refSent<<std::endl;
  }
  SmtModel::Hypothesis hyp;
  if(tdState.preprocId)
  {
    std::string preprocSrcSent=preprocLine(tdPerUserVarsVec[idx].prePosProcessorPtr,srcSent,tdState.caseconv,false);
    std::string preprocRefSent=preprocLine(tdPerUserVarsVec[idx].prePosProcessorPtr,refSent,tdState.caseconv,false);
    if(verbose)
    {
      std::cerr<<" - preproc. source: "<<preprocSrcSent<<std::endl;
      std::cerr<<" - preproc. reference: "<<preprocRefSent<<std::endl;
    }
    hyp=tdPerUserVarsVec[idx].stackDecoderPtr->verifyCoverageForRef(preprocSrcSent.c_str(),preprocRefSent.c_str());
    std::string aux=tdPerUserVarsVec[idx].smtModelPtr->getTransInPlainText(hyp);
    result=postprocLine(tdPerUserVarsVec[idx].prePosProcessorPtr,aux.c_str(),tdState.caseconv);
  }
  else
  {
    hyp=tdPerUserVarsVec[idx].stackDecoderPtr->verifyCoverageForRef(srcSent,refSent);
    result=tdPerUserVarsVec[idx].smtModelPtr->getTransInPlainText(hyp);
  }
  if(verbose)
  {
    tdPerUserVarsVec[idx].smtModelPtr->printHyp(hyp,std::cerr);
    if(!tdPerUserVarsVec[idx].smtModelPtr->isComplete(hyp))
      std::cerr<<"No coverage for sentence pair!"<<std::endl;
  }

  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);

  if(!tdPerUserVarsVec[idx].smtModelPtr->isComplete(hyp))
    return THOT_OK;
  else return THOT_ERROR;
}

//--------------------------
bool ThotDecoder::startCat(int user_id,
                           const char *sentenceToTranslate,
                           std::string &catResult,
                           int verbose/*=0*/)
{
      // Increase non_atomic_ops_running variable
  increase_non_atomic_ops_running();

      // Obtain index vector given user_id
  size_t idx=get_vecidx_for_user_id(user_id);
  if(verbose) std::cerr<<"user_id: "<<user_id<<", idx: "<<idx<<std::endl;

  pthread_mutex_lock(&per_user_mut[idx]);
  /////////// begin of user mutex

  if(tdPerUserVarsVec[idx]._nbUncoupledAssistedTransPtr)
  {
        // Execute specific actions for uncoupled assisted translators
        // Disable best score pruning
    tdPerUserVarsVec[idx].stackDecoderPtr->useBestScorePruning(false);
  }

  totalPrefixVec[idx]="";
  if(verbose)
  {
    std::cerr<<"***** Translating sentence: "<<sentenceToTranslate<<std::endl;
  }
  
  if(tdState.preprocId)
  {
    std::string aux;
    std::string preprocSent=preprocLine(tdPerUserVarsVec[idx].prePosProcessorPtr,sentenceToTranslate,tdState.caseconv,true);
    RejectedWordsSet emptyRejWordsSet;
    aux=tdPerUserVarsVec[idx].assistedTransPtr->translateWithPrefix(preprocSent,"",emptyRejWordsSet,verbose);
    catResult=postprocLine(tdPerUserVarsVec[idx].prePosProcessorPtr,aux.c_str(),tdState.caseconv);
    if(verbose)
    {
      std::cerr<<"* preprocessed sentence: "<<preprocSent<<std::endl;
      std::cerr<<"* translation: "<<aux<<std::endl;
      std::cerr<<"* postprocessed translation: "<<catResult<<std::endl;
    }
  }
  else
  {
        // No pre/post-processing steps are applied
    RejectedWordsSet emptyRejWordsSet;
    catResult=tdPerUserVarsVec[idx].assistedTransPtr->translateWithPrefix(sentenceToTranslate,"",emptyRejWordsSet,verbose);
    if(verbose)
    {
      std::cerr<<"* translation: "<<catResult<<std::endl;
    }
  }

  if(tdPerUserVarsVec[idx]._nbUncoupledAssistedTransPtr)
  {
        // Execute specific actions for uncoupled assisted translators
        // Enable best score pruning
    tdPerUserVarsVec[idx].stackDecoderPtr->useBestScorePruning(true);
  }

  /////////// end of user mutex 
  pthread_mutex_unlock(&per_user_mut[idx]);

      // Decrease non_atomic_ops_running variable
  decrease_non_atomic_ops_running();

  return THOT_OK;
}
  
//--------------------------
void ThotDecoder::addStrToPref(int user_id,
                               const char *strToAddToPref,
                               const RejectedWordsSet& rejectedWords,
                               std::string &catResult,
                               int verbose/*=0*/)
{
      // Increase non_atomic_ops_running variable
  increase_non_atomic_ops_running();

      // Obtain index vector given user_id
  size_t idx=get_vecidx_for_user_id(user_id);
  if(verbose) std::cerr<<"user_id: "<<user_id<<", idx: "<<idx<<std::endl;

  pthread_mutex_lock(&per_user_mut[idx]);
  /////////// begin of user mutex
  
  addStrToPrefAux(idx,strToAddToPref,rejectedWords,catResult,verbose);
  
  /////////// end of user mutex 
  pthread_mutex_unlock(&per_user_mut[idx]);

      // Decrease non_atomic_ops_running variable
  decrease_non_atomic_ops_running();
}

//--------------------------
void ThotDecoder::addStrToPrefAux(size_t idx,
                                  const char *strToAddToPref,
                                  const RejectedWordsSet& rejectedWords,
                                  std::string &catResult,
                                  int verbose/*=0*/)
{
  if(tdPerUserVarsVec[idx]._nbUncoupledAssistedTransPtr)
  {
        // Execute specific actions for uncoupled assisted translators
        // Disable best score pruning
    tdPerUserVarsVec[idx].stackDecoderPtr->useBestScorePruning(false);
  }
  
  if(totalPrefixVec[idx]=="") totalPrefixVec[idx]=strToAddToPref;
  else totalPrefixVec[idx]=totalPrefixVec[idx]+strToAddToPref;
  
  if(verbose)
  {
    std::cerr<<"Add string to prefix: "<<strToAddToPref<<"|"<<std::endl;
    std::cerr<<"Total prefix (not preprocessed): "<<totalPrefixVec[idx]<<"|"<<std::endl;
  }
  if(tdState.preprocId)
  {    
    std::string trans;
    std::string expLastWord;
    std::string preprocPrefUnexpanded=preprocLine(tdPerUserVarsVec[idx].prePosProcessorPtr,totalPrefixVec[idx],tdState.caseconv,false);    
    std::string preprocPref=preprocPrefUnexpanded;
    
    expLastWord=expandLastWord(preprocPref);
    tdPerUserVarsVec[idx].assistedTransPtr->resetPrefix();
    trans=tdPerUserVarsVec[idx].assistedTransPtr->addStrToPrefix(preprocPref,
                                                                 rejectedWords,
                                                                 verbose);
    catResult=robustObtainFinalOutput(tdPerUserVarsVec[idx].prePosProcessorPtr,
                                      totalPrefixVec[idx],
                                      preprocPrefUnexpanded,
                                      preprocPref,
                                      trans.c_str(),
                                      tdState.caseconv);
    
    if(verbose)
    {
      std::cerr<<"Preprocessed prefix: "<<preprocPrefUnexpanded<<"|"<<std::endl;
      std::cerr<<"Expanded preprocessed prefix: "<<preprocPref<<"|"<<std::endl;
      std::cerr<<"Translation: "<<trans<<"|"<<std::endl;
      std::cerr<<"Postproc. trans. with user pref.: "<<catResult<<"|"<<std::endl;
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
      std::cerr<<"Expanded prefix: "<<expPref<<"|"<<std::endl;
      std::cerr<<"Translation: "<<trans<<"|"<<std::endl;
      std::cerr<<"Final output: "<<catResult<<"|"<<std::endl;
    }
  }

  if(tdPerUserVarsVec[idx]._nbUncoupledAssistedTransPtr)
  {
        // Execute specific actions for uncoupled assisted translators
        // Enable best score pruning
    tdPerUserVarsVec[idx].stackDecoderPtr->useBestScorePruning(true);
  }

      // Decrease non_atomic_ops_running variable
  decrease_non_atomic_ops_running();

  /////////// end of user mutex 
  pthread_mutex_unlock(&per_user_mut[idx]);
}

//--------------------------
void ThotDecoder::setPref(int user_id,
                          const char *prefStr,
                          const RejectedWordsSet& rejectedWords,
                          std::string &catResult,
                          int verbose/*=0*/)
{
      // Increase non_atomic_ops_running variable
  increase_non_atomic_ops_running();

      // Obtain index vector given user_id
  size_t idx=get_vecidx_for_user_id(user_id);

  pthread_mutex_lock(&per_user_mut[idx]);
  /////////// begin of user mutex

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
        // Old prefix is not a prefix of the new one, reset it
    resetPrefixAux(idx);
    strToAddToPref=prefStr;
  }

  addStrToPrefAux(idx,strToAddToPref.c_str(),rejectedWords,catResult,verbose);

  /////////// end of user mutex 
  pthread_mutex_unlock(&per_user_mut[idx]);
  
      // Decrease non_atomic_ops_running variable
  decrease_non_atomic_ops_running();
}

//--------------------------
void ThotDecoder::resetPrefix(int user_id,
                              int verbose/*=0*/)
{
      // Increase non_atomic_ops_running variable
  increase_non_atomic_ops_running();

      // Obtain index vector given user_id
  size_t idx=get_vecidx_for_user_id(user_id);
  if(verbose) std::cerr<<"user_id: "<<user_id<<", idx: "<<idx<<std::endl;

  pthread_mutex_lock(&per_user_mut[idx]);
  /////////// begin of user mutex

  if(verbose)
  {
    std::cerr<<"Reset prefix"<<std::endl;
  }

  resetPrefixAux(idx);

  /////////// end of user mutex 
  pthread_mutex_unlock(&per_user_mut[idx]);

      // Decrease non_atomic_ops_running variable
  decrease_non_atomic_ops_running();
}

//--------------------------
void ThotDecoder::resetPrefixAux(size_t idx)
{
  totalPrefixVec[idx]="";
  tdPerUserVarsVec[idx].assistedTransPtr->resetPrefix();
}

//--------------------------
bool ThotDecoder::use_caseconv(int user_id,
                               const char *caseConvFile,
                               int verbose/*=0*/)
{
  int ret;
  
      // Obtain index vector given user_id
  size_t idx=get_vecidx_for_user_id(user_id);
  if(verbose) std::cerr<<"user_id: "<<user_id<<", idx: "<<idx<<std::endl;

  if(tdState.preprocId)
  {
    ret=tdPerUserVarsVec[idx].prePosProcessorPtr->loadCapitInfo(caseConvFile);
    if(ret==THOT_OK)
    {
      tdState.caseconv=true;
    }
  }
  else
  {
    std::cerr<<"Warning! case conversion cannot be activated because pre/pos-processing steps are disabled."<<std::endl;
    ret=THOT_OK;
  }

  return ret;
}

//--------------------------
void ThotDecoder::clearTrans(int /*verbose*//*=0*/)
{
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

      // Wait until all non-atomic operations have finished
  wait_on_non_atomic_op_cond();

  tdCommonVars.wgHandlerPtr->clear();
  tdCommonVars.smtModelPtr->clear();
  tdCommonVars.ecModelPtr->clear();
  tdCommonVars.featureHandler.clear();
  for(size_t i=0;i<tdPerUserVarsVec.size();++i)
  {
    release_idx_data(i);
  }
  tdPerUserVarsVec.clear();
  tdState.default_values();
  totalPrefixVec.clear();
  userIdToIdx.clear();
  idxDataReleased.clear();

      // Unlock non_atomic_op_cond mutex
  pthread_mutex_unlock(&non_atomic_op_mut);

  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);
}

//--------------------------
bool ThotDecoder::printModels(int verbose/*=0*/)
{
  if(tdCommonVars.featureBasedImplEnabled)
    return printModelsFeatImpl(verbose);
  else
    return printModelsLegacyImpl(verbose);
}

//--------------------------
bool ThotDecoder::printModelsFeatImpl(int verbose/*=0*/)
{
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

  if(verbose)
  {
    std::cerr<<"Printing models stored by the translator (tm files prefix: "<<tdState.tmFilesPrefixGiven<<" , lm files prefix: "<<tdState.lmfileLoaded<<" , ecm files prefix: "<<tdState.lmfileLoaded<<")"<<std::endl;
  }

  int ret;

      // Print alignment model parameters
  ret=tdCommonVars.featureHandler.print(tdState.tmFilesPrefixGiven,tdState.lmfileLoaded);
  
  if(ret==THOT_OK)
  {
        // Print error correcting model parameters
    ret=tdCommonVars.ecModelPtr->print(tdState.ecmFilesPrefixGiven.c_str());
  }

  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);

  return ret;
}

//--------------------------
bool ThotDecoder::printModelsLegacyImpl(int verbose/*=0*/)
{
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

  if(verbose)
  {
    std::cerr<<"Printing models stored by the translator (tm files prefix: "<<tdState.tmFilesPrefixGiven<<" , lm files prefix: "<<tdState.lmfileLoaded<<" , ecm files prefix: "<<tdState.lmfileLoaded<<")"<<std::endl;
  }

  int ret;
  _phraseBasedTransModel<SmtModel::Hypothesis>* phrbtm_ptr=dynamic_cast<_phraseBasedTransModel<SmtModel::Hypothesis>* >(tdCommonVars.smtModelPtr);
  if(phrbtm_ptr)
  {
        // Print alignment model parameters
    ret=phrbtm_ptr->printAligModel(tdState.tmFilesPrefixGiven);
    if(ret==THOT_OK)
    {
          // Print language model parameters
      ret=phrbtm_ptr->printLangModel(tdState.lmfileLoaded);
      if(ret==THOT_OK)
      {
            // Print error correcting model parameters
        ret=tdCommonVars.ecModelPtr->print(tdState.ecmFilesPrefixGiven.c_str()); 
      }
    }
  }
  else
    ret=THOT_ERROR;
  
  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);

  return ret;
}

//--------------------------
int ThotDecoder::printModelWeights(void)
{
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

      // Print smt model weights
  cout<<"- SMT model weights= ";
  tdCommonVars.smtModelPtr->printWeights(cout);
  cout<<std::endl;

      // Print assisted translator weights
  BaseAssistedTrans<SmtModel>* assistedTransPtr=tdCommonVars.dynClassFactoryHandler.baseAssistedTransDynClassLoader.make_obj(tdCommonVars.dynClassFactoryHandler.baseAssistedTransInitPars);
  if(assistedTransPtr==NULL)
  {
    std::cerr<<"Error: BaseAssistedTrans pointer could not be instantiated"<<std::endl;
    return THOT_ERROR;
  }

  WgUncoupledAssistedTrans<SmtModel>* wgUncoupledAssistedTransPtr=dynamic_cast<WgUncoupledAssistedTrans<SmtModel>*>(assistedTransPtr);
  if(!wgUncoupledAssistedTransPtr)
  {
    cout<<"- Assisted translator weights= ";
    assistedTransPtr->printWeights(cout);
    cout << std::endl;
  }
  else
  {
    if(tdCommonVars.curr_ecm_valid_for_wg)
    {
      cout<<"- Assisted translator weights= ";
      assistedTransPtr->printWeights(cout);
      cout << std::endl;
    }
    else
    {
      cout<<"Warning: current error correcting model cannot be combined with word-graph based assisted translators"<<std::endl;
    }
  }

      // Release memory
  delete assistedTransPtr;
  
      // Print error correction model weights
  cout<<"- Error correction model weights= ";
  tdCommonVars.ecModelPtr->printWeights(cout);
  cout<<std::endl;

  
  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);

  return THOT_OK;
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

      // NOTE 2: this function does intentionally not release mutex
}

//--------------------------
void ThotDecoder::increase_non_atomic_ops_running(void)
{
  pthread_mutex_lock(&non_atomic_op_mut);
  /////////// begin of mutex
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
std::string ThotDecoder::robustObtainFinalOutput(BasePrePosProcessor* prePosProcessorPtr,
                                                 std::string unpreprocPref,
                                                 std::string preprocPrefUnexpanded,
                                                 std::string preprocPref,
                                                 std::string trans,
                                                 bool caseconv)
{
  std::vector<std::string> preprocPrefVec=StrProcUtils::stringToStringVector(preprocPref);
  std::vector<std::string> transVec=StrProcUtils::stringToStringVector(trans);
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
      std::string postprocPref=postprocLine(prePosProcessorPtr,preprocPref.c_str(),caseconv);
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
  postprocModifiedTrans=postprocLine(prePosProcessorPtr,modifiedTrans.c_str(),caseconv);
  
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
  
//   std::cerr<<"******************************"<<std::endl;
//   std::cerr<<"unpreprocPref: "<<unpreprocPref<<"|"<<std::endl; 
//   std::cerr<<"preprocPrefUnexpanded: "<<preprocPrefUnexpanded<<"|"<<std::endl;
//   std::cerr<<"trans: "<<trans<<"|"<<std::endl;
//   std::cerr<<"prefEndInTrans: "<<prefEndInTrans<<std::endl;
//   std::cerr<<"startStr: "<<startStr<<"|"<<std::endl;
//   std::cerr<<"endStr: "<<endStr<<"|"<<std::endl;
//   std::cerr<<"lastPrefWordCompletion: "<<lastPrefWordCompletion<<"|"<<std::endl;
//   std::cerr<<"modifiedTrans: "<<modifiedTrans<<"|"<<std::endl;
//   std::cerr<<"postprocModifiedTrans: "<<postprocModifiedTrans<<"|"<<std::endl;
//   std::cerr<<"suffixInModifiedTrans: "<<suffixInModifiedTrans<<"|"<<std::endl;
//   std::cerr<<"result: "<<result<<"|"<<std::endl;

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
  std::vector<std::string> totalPrefixVec=StrProcUtils::stringToStringVector(totalPrefix);
  std::vector<std::string> transVec=StrProcUtils::stringToStringVector(trans);
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
      std::vector<std::string> strVec=StrProcUtils::stringToStringVector(partialSent);
      std::vector<std::string> hist;

      if(strVec.size()>=3) hist.push_back(strVec[strVec.size()-3]);
      if(strVec.size()>=2) hist.push_back(strVec[strVec.size()-2]);
      pcs=getBestSuffixGivenHist(hist,lastWord);
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
pair<Count,std::string> ThotDecoder::getBestSuffixGivenHist(std::vector<std::string> hist,
                                                            std::string input)
{
  if(tdCommonVars.featureBasedImplEnabled)
  {
    return getBestSuffixGivenHistFeatImpl(hist,input);
  }
  else
  {
    return tdCommonVars.smtModelPtr->getBestSuffixGivenHist(hist,input);
  }
}

//--------------------------
pair<Count,std::string> ThotDecoder::getBestSuffixGivenHistFeatImpl(std::vector<std::string> hist,
                                                                    std::string input)
{
      // Obtain pointer to features info
  FeaturesInfo<SmtModel::HypScoreInfo>* featsInfoPtr=tdCommonVars.featureHandler.getFeatureInfoPtr();

      // Obtain pointers to language model features
  std::vector<LangModelFeat<SmtModel::HypScoreInfo>* > langModelFeatsVec=featsInfoPtr->getLangModelFeatPtrs();
  
      // Obtain best suffix using first available language model feature
  if(langModelFeatsVec.empty())
  {
    pair<Count,std::string> pcs;
    pcs.first=0;
    return pcs;
  }
  else
    return langModelFeatsVec[0]->getBestSuffixGivenHist(hist,input);
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
std::string ThotDecoder::preprocLine(BasePrePosProcessor* prePosProcessorPtr,
                                     std::string str,
                                     bool caseconv,
                                     bool keepPreprocInfo)
{
  pthread_mutex_lock(&preproc_mut);
  /////////// begin of preproc mutex 
  std::string result=prePosProcessorPtr->preprocLine(str,caseconv,keepPreprocInfo);
  /////////// end of preproc mutex 
  pthread_mutex_unlock(&preproc_mut);

  return result;
}

//--------------------------
std::string ThotDecoder::postprocLine(BasePrePosProcessor* prePosProcessorPtr,
                                      std::string str,
                                      bool caseconv)
{
  pthread_mutex_lock(&preproc_mut);
  /////////// begin of preproc mutex
  std::string result=prePosProcessorPtr->postprocLine(str,caseconv);
  /////////// end of preproc mutex 
  pthread_mutex_unlock(&preproc_mut);

  return result;
}

//--------------------------
void ThotDecoder::deleteSwModelPtrs(void)
{
  for(unsigned int i=0;i<tdCommonVars.swModelInfoPtr->swAligModelPtrVec.size();++i)
    delete tdCommonVars.swModelInfoPtr->swAligModelPtrVec[i];
  for(unsigned int i=0;i<tdCommonVars.swModelInfoPtr->invSwAligModelPtrVec.size();++i)
    delete tdCommonVars.swModelInfoPtr->invSwAligModelPtrVec[i];
}

//--------------------------
void ThotDecoder::destroy_feat_impl(void)
{
        // Release server variables
  clearTrans();

      // Delete pointers
  delete tdCommonVars.wgHandlerPtr;
  delete tdCommonVars.smtModelPtr;
  delete tdCommonVars.ecModelPtr;
  delete tdCommonVars.llWeightUpdaterPtr;
  delete tdCommonVars.scorerPtr;
  
      // Release class factory handler
  tdCommonVars.dynClassFactoryHandler.release_smt_and_imt();
  
      // Destroy mutexes and conditions
  pthread_mutex_destroy(&user_id_to_idx_mut);
  pthread_mutex_destroy(&atomic_op_mut);
  pthread_mutex_destroy(&non_atomic_op_mut);
  pthread_mutex_destroy(&preproc_mut);
  pthread_cond_destroy(&non_atomic_op_cond);
  for(unsigned int i=0;i<per_user_mut.size();++i)
    pthread_mutex_destroy(&per_user_mut[i]);
}

//--------------------------
void ThotDecoder::destroy_legacy_impl(void)
{
      // Release server variables
  clearTrans();

      // Delete pointers
  delete tdCommonVars.langModelInfoPtr->wpModelPtr;
  delete tdCommonVars.langModelInfoPtr->lModelPtr;
  delete tdCommonVars.langModelInfoPtr;
  delete tdCommonVars.phrModelInfoPtr->invPbModelPtr;
  delete tdCommonVars.phrModelInfoPtr;
  deleteSwModelPtrs();
  delete tdCommonVars.swModelInfoPtr;
  delete tdCommonVars.wgHandlerPtr;
  delete tdCommonVars.smtModelPtr;
  delete tdCommonVars.ecModelPtr;
  delete tdCommonVars.llWeightUpdaterPtr;
  delete tdCommonVars.scorerPtr;

      // Release class factory handler
  tdCommonVars.dynClassFactoryHandler.release_smt_and_imt();

      // Destroy mutexes and conditions
  pthread_mutex_destroy(&user_id_to_idx_mut);
  pthread_mutex_destroy(&atomic_op_mut);
  pthread_mutex_destroy(&non_atomic_op_mut);
  pthread_mutex_destroy(&preproc_mut);
  pthread_cond_destroy(&non_atomic_op_cond);
  for(unsigned int i=0;i<per_user_mut.size();++i)
    pthread_mutex_destroy(&per_user_mut[i]);
}

//--------------------------
ThotDecoder::~ThotDecoder()
{
  if(tdCommonVars.featureBasedImplEnabled)
    destroy_feat_impl();
  else
    destroy_legacy_impl();
}
