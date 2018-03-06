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

/**
 * @file FeatureHandler.cc
 * 
 * @brief Definitions file for FeatureHandler.h
 */

//--------------- Include files --------------------------------------

#include "FeatureHandler.h"

//--------------- FeatureHandler class functions

FeatureHandler::FeatureHandler()
{  
}

//---------------
int FeatureHandler::addWpFeat(int /*verbose*/)
{
  std::string featName="wp";
  std::cerr<<"** Creating word penalty feature ("<<featName<<" "<<wpSoFile<<")"<<std::endl;

      // Create model pointer if necessary
  if(wpModelInfo.wpModelPtr==NULL)
  {
    std::string initPars;
    wpModelInfo.wpModelPtr=wpModelInfo.classLoader.make_obj(initPars);
    if(wpModelInfo.wpModelPtr==NULL)
    {
      std::cerr<<"Error: BaseWordPenaltyModel pointer could not be instantiated"<<std::endl;    
      return THOT_ERROR;
    }
  }
  
      // Add feature
  WordPenaltyFeat<SmtModel::HypScoreInfo>* wordPenaltyFeatPtr=new WordPenaltyFeat<SmtModel::HypScoreInfo>;
  wordPenaltyFeatPtr->setFeatName(featName);
  wordPenaltyFeatPtr->link_wpm(wpModelInfo.wpModelPtr);
  featuresInfo.featPtrVec.push_back(wordPenaltyFeatPtr);

  return THOT_OK;
}

//---------------
int FeatureHandler::addLmFeats(std::string lmFileName,
                               int verbose)
{
  if(fileIsDescriptor(lmFileName))
    return process_lm_descriptor(lmFileName,verbose);
  else
    return process_lm_files_prefix(lmFileName,verbose);
}

//---------------
int FeatureHandler::addTmFeats(std::string tmFilesPrefix,
                               int verbose)
{
  if(fileIsDescriptor(tmFilesPrefix))
    return process_tm_descriptor(tmFilesPrefix,verbose);
  else
    return process_tm_files_prefix(tmFilesPrefix,verbose);
}

//---------------
FeaturesInfo<SmtModel::HypScoreInfo>* FeatureHandler::getFeatureInfoPtr(void)
{
  return &featuresInfo;
}

//---------------
int FeatureHandler::updateLmLinInterpWeights(std::string trgCorpusFileName,
                                             int verbose/*=0*/)
{
      // Iterate over language model features
  std::vector<unsigned int> featIndexVec;
  std::vector<LangModelFeat<SmtModel::HypScoreInfo>* > langModelFeatPtrs=featuresInfo.getLangModelFeatPtrs(featIndexVec);

  for(unsigned int i=0;i<langModelFeatPtrs.size();++i)
  {
    if(verbose)
      std::cerr<<"Updating linear interpolation weights for feature "<<langModelFeatPtrs[i]->getFeatName()<<std::endl;
    BaseNgramLM<LM_State>* lModelPtr=langModelFeatPtrs[i]->get_lmptr();
    int ret=updateLinWeightsForLmPtr(lModelPtr,trgCorpusFileName);
    if(ret==THOT_ERROR)
      return THOT_ERROR;
  }
  
  return THOT_OK;
}

//---------------
int FeatureHandler::updateLinWeightsForLmPtr(BaseNgramLM<LM_State>* lModelPtr,
                                             std::string trgCorpusFileName,
                                             int verbose/*=0*/)
{
      // Perform checkings to see if the model has weights to be updated
  _incrJelMerNgramLM<Count,Count>* incrJelMerLmPtr=dynamic_cast<_incrJelMerNgramLM<Count,Count>* >(lModelPtr);
  if(incrJelMerLmPtr)
  {
    int ret=incrJelMerLmPtr->updateModelWeights(trgCorpusFileName.c_str(),verbose);
    if(ret==THOT_ERROR)
      return THOT_ERROR;

    return THOT_OK;
  }
    
      // Model did not have weights to be updated
  if(verbose)
    std::cerr<<"Warning, current model does not have weights to be updated"<<std::endl;
  
  return THOT_OK;
}

//---------------
int FeatureHandler::updatePmLinInterpWeights(std::string srcCorpusFileName,
                                             std::string trgCorpusFileName,
                                             int verbose/*=0*/)
{
      // Update linear interpolation weights for all direct and inverse
      // phrase model feature pairs
  for(unsigned int i=0;i<swModelsInfo.swAligModelPtrVec.size();++i)
  {
    if(verbose)
      std::cerr<<"Updating linear interpolation weights for features "<<swModelsInfo.featNameVec[i]<<" and "<<swModelsInfo.invFeatNameVec[i]<<std::endl;

        // Obtain indices for features
    unsigned int dirPhrModelFeatIdx=getFeatureIdx(swModelsInfo.featNameVec[i]);
    unsigned int invPhrModelFeatIdx=getFeatureIdx(swModelsInfo.invFeatNameVec[i]);

        // Obtain specialized pointers
    DirectPhraseModelFeat<SmtModel::HypScoreInfo>* dirPmFeatPtr=dynamic_cast<DirectPhraseModelFeat<SmtModel::HypScoreInfo>* >(featuresInfo.featPtrVec[dirPhrModelFeatIdx]);
    InversePhraseModelFeat<SmtModel::HypScoreInfo>* invPmFeatPtr=dynamic_cast<InversePhraseModelFeat<SmtModel::HypScoreInfo>* >(featuresInfo.featPtrVec[invPhrModelFeatIdx]);
    
        // Update weights
    int ret=WeightUpdateUtils::updatePmLinInterpWeights(srcCorpusFileName,
                                                        trgCorpusFileName,
                                                        dirPmFeatPtr,
                                                        invPmFeatPtr,
                                                        verbose);
    if(ret==THOT_ERROR)
      return THOT_ERROR;
  }

  return THOT_OK;
}

//---------------
int FeatureHandler::onlineTrainFeats(OnlineTrainingPars onlineTrainingPars,
                                     std::string srcSent,
                                     std::string refSent,
                                     std::string /*sysSent*/,
                                     int verbose/*=0*/)
{
       // Check if input sentences are empty
  if(srcSent.size()==0 || refSent.size()==0)
  {
    std::cerr<<"Error: cannot process empty input sentences"<<std::endl;
    return THOT_ERROR;
  }

      // Train pair according to chosen algorithm
  switch(onlineTrainingPars.onlineLearningAlgorithm)
  {
    case BASIC_INCR_TRAINING:
      return incrTrainFeatsSentPair(onlineTrainingPars,srcSent,refSent,verbose);
      break;
    // case MINIBATCH_TRAINING:
    //   return minibatchTrainFeatsSentPair(srcSent,refSent,sysSent,verbose);
    //   break;
    // case BATCH_RETRAINING:
    //   return batchRetrainFeatsSentPair(srcSent,refSent,verbose);
    //   break;
    default:
      std::cerr<<"Warning: requested online learning algoritm with id="<<onlineTrainingPars.onlineLearningAlgorithm<<" is not implemented."<<std::endl;
      return THOT_ERROR;
      break;
  } 
}

//---------------------------------
int FeatureHandler::incrTrainFeatsSentPair(OnlineTrainingPars onlineTrainingPars,
                                           std::string srcSent,
                                           std::string refSent,
                                           int verbose/*=0*/)
{
      // Break input strings into word vectors
  std::vector<std::string> srcSentStrVec=StrProcUtils::stringToStringVector(srcSent);
  std::vector<std::string> refSentStrVec=StrProcUtils::stringToStringVector(refSent);

      // Train language model
  int ret=trainLangModel(langModelsInfo.lModelPtrVec[0],onlineTrainingPars.learnStepSize,refSentStrVec,verbose);
  if(ret==THOT_ERROR)
    return THOT_ERROR;
  
      // Train alignment model
  DirectPhraseModelFeat<SmtModel::HypScoreInfo>* dirPmFeatPtr=getDirectPhraseModelFeatPtr(swModelsInfo.featNameVec[0]);
  InversePhraseModelFeat<SmtModel::HypScoreInfo>* invPmFeatPtr=getInversePhraseModelFeatPtr(swModelsInfo.invFeatNameVec[0]);
  ret=trainAligModel(dirPmFeatPtr->get_pmptr(),
                     dirPmFeatPtr->get_swmptr(),
                     invPmFeatPtr->get_swmptr(),
                     onlineTrainingPars,
                     srcSentStrVec,
                     refSentStrVec,
                     verbose);
  if(ret==THOT_ERROR)
    return THOT_ERROR;

  return THOT_OK;
}

//---------------
DirectPhraseModelFeat<SmtModel::HypScoreInfo>*
FeatureHandler::getDirectPhraseModelFeatPtr(std::string directPhrModelFeatName)
{
      // Obtain index for feature
  unsigned int dirPhrModelFeatIdx=getFeatureIdx(directPhrModelFeatName);

      // Obtain specialized pointer
  DirectPhraseModelFeat<SmtModel::HypScoreInfo>* dirPmFeatPtr=dynamic_cast<DirectPhraseModelFeat<SmtModel::HypScoreInfo>* >(featuresInfo.featPtrVec[dirPhrModelFeatIdx]);

  return dirPmFeatPtr;
}

//---------------
InversePhraseModelFeat<SmtModel::HypScoreInfo>*
FeatureHandler::getInversePhraseModelFeatPtr(std::string invPhrModelFeatName)
{
      // Obtain indices for features
  unsigned int invPhrModelFeatIdx=getFeatureIdx(invPhrModelFeatName);

      // Obtain specialized pointers
  InversePhraseModelFeat<SmtModel::HypScoreInfo>* invPmFeatPtr=dynamic_cast<InversePhraseModelFeat<SmtModel::HypScoreInfo>* >(featuresInfo.featPtrVec[invPhrModelFeatIdx]);

  return invPmFeatPtr;
}

//---------------
int FeatureHandler::trainLangModel(BaseNgramLM<LM_State>* lModelPtr,
                                   float learnStepSize,
                                   std::vector<std::string> refSentStrVec,
                                   int verbose/*=0*/)
{
      // Train language model
  if(verbose) std::cerr<<"Training language model..."<<std::endl;
  int ret=lModelPtr->trainSentence(refSentStrVec,learnStepSize,0,verbose);
  if(ret==THOT_ERROR) return THOT_ERROR;

  return THOT_OK;
}

//---------------
int FeatureHandler::trainAligModel(BasePhraseModel* invPbModelPtr,
                                   BaseSwAligModel<PpInfo>* swAligModelPtr,
                                   BaseSwAligModel<PpInfo>* invSwAligModelPtr,
                                   OnlineTrainingPars onlineTrainingPars,
                                   std::vector<std::string> srcSentStrVec,
                                   std::vector<std::string> refSentStrVec,
                                   int verbose/*=0*/)
{
      // Revise vocabularies of the alignment models
  updateAligModelsSrcVoc(invPbModelPtr,swAligModelPtr,invSwAligModelPtr,srcSentStrVec);
  updateAligModelsTrgVoc(invPbModelPtr,swAligModelPtr,invSwAligModelPtr,refSentStrVec);

      // Add sentence pair to the single word models
  std::pair<unsigned int,unsigned int> sentRange;
  swAligModelPtr->addSentPair(srcSentStrVec,refSentStrVec,onlineTrainingPars.learnStepSize,sentRange);
  invSwAligModelPtr->addSentPair(refSentStrVec,srcSentStrVec,onlineTrainingPars.learnStepSize,sentRange);

      // Iterate over E_par interlaced samples
  int ret=THOT_OK;
  unsigned int curr_sample=sentRange.second;
  unsigned int oldest_sample=curr_sample-onlineTrainingPars.R_par;
  for(unsigned int i=1;i<=onlineTrainingPars.E_par;++i)
  {
    int n=oldest_sample+(i*(onlineTrainingPars.R_par/onlineTrainingPars.E_par));
    if(n>=0)
    {
      if(verbose)
        std::cerr<<"Alig. model training iteration over sample "<<n<<" ..."<<std::endl;

          // Train sw model
      if(verbose) std::cerr<<"Training single-word model..."<<std::endl;
      swAligModelPtr->trainSentPairRange(std::make_pair(n,n),verbose);

          // Train inverse sw model
      if(verbose) std::cerr<<"Training inverse single-word model..."<<std::endl;
      invSwAligModelPtr->trainSentPairRange(std::make_pair(n,n),verbose);

          // Add new translation options
      if(verbose) std::cerr<<"Adding new translation options..."<<std::endl;
      ret=addNewTransOpts(invPbModelPtr,swAligModelPtr,invSwAligModelPtr,n,verbose);
    }
  }

      // Discard unnecessary phrase-based model sufficient statistics
  int last_n=curr_sample-((onlineTrainingPars.E_par-1)*(onlineTrainingPars.R_par/onlineTrainingPars.E_par));
  if(last_n>=0)
  {
    int mapped_last_n=map_n_am_suff_stats(last_n);
    int idx_to_discard=mapped_last_n;
    if(idx_to_discard>0 && vecVecInvPhPair.size()>(unsigned int)idx_to_discard)
      vecVecInvPhPair[idx_to_discard].clear();
  }

  return ret;
}

//---------------
void FeatureHandler::updateAligModelsSrcVoc(BasePhraseModel* invPbModelPtr,
                                            BaseSwAligModel<PpInfo>* swAligModelPtr,
                                            BaseSwAligModel<PpInfo>* invSwAligModelPtr,
                                            std::vector<std::string> srcSentStrVec)
{
  for(unsigned int i=0;i<srcSentStrVec.size();++i)
  {
    addSrcSymbolToAligModels(invPbModelPtr,swAligModelPtr,invSwAligModelPtr,srcSentStrVec[i]);
  }
}

//---------------
void FeatureHandler::updateAligModelsTrgVoc(BasePhraseModel* invPbModelPtr,
                                            BaseSwAligModel<PpInfo>* swAligModelPtr,
                                            BaseSwAligModel<PpInfo>* invSwAligModelPtr,
                                            std::vector<std::string> trgSentStrVec)
{
  for(unsigned int i=0;i<trgSentStrVec.size();++i)
  {
    addTrgSymbolToAligModels(invPbModelPtr,swAligModelPtr,invSwAligModelPtr,trgSentStrVec[i]);
  }
}

//---------------
WordIndex FeatureHandler::addSrcSymbolToAligModels(BasePhraseModel* invPbModelPtr,
                                                   BaseSwAligModel<PpInfo>* swAligModelPtr,
                                                   BaseSwAligModel<PpInfo>* invSwAligModelPtr,
                                                   std::string s)
{
  WordIndex windex_ipbm=invPbModelPtr->addTrgSymbol(s);
  WordIndex windex_lex=swAligModelPtr->addSrcSymbol(s);
  WordIndex windex_ilex=invSwAligModelPtr->addTrgSymbol(s);
  if(windex_ipbm!=windex_lex || windex_ipbm!=windex_ilex)
  {
    std::cerr<<"Warning! phrase-based model vocabularies are now different from lexical model vocabularies."<<std::endl;
  }
  
  return windex_ipbm;
}

//---------------
WordIndex FeatureHandler::addTrgSymbolToAligModels(BasePhraseModel* invPbModelPtr,
                                                   BaseSwAligModel<PpInfo>* swAligModelPtr,
                                                   BaseSwAligModel<PpInfo>* invSwAligModelPtr,
                                                   std::string t)
{
  WordIndex windex_ipbm=invPbModelPtr->addSrcSymbol(t);
  WordIndex windex_lex=swAligModelPtr->addTrgSymbol(t);
  WordIndex windex_ilex=invSwAligModelPtr->addSrcSymbol(t);
  if(windex_ipbm!=windex_lex || windex_ipbm!=windex_ilex)
  {
    std::cerr<<"Warning! phrase-based model vocabularies are now different from lexical model vocabularies."<<std::endl;
  }
  
  return windex_ipbm;
}

//---------------
int FeatureHandler::addNewTransOpts(BasePhraseModel* invPbModelPtr,
                                    BaseSwAligModel<PpInfo>* swAligModelPtr,
                                    BaseSwAligModel<PpInfo>* invSwAligModelPtr,
                                    int n,
                                    int verbose/*=0*/)
{
      // Check if the phrase model is incremental (otherwise, it is not
      // possible to add new translation options)
  BaseIncrPhraseModel* baseIncrPhraseModelPtr=dynamic_cast<BaseIncrPhraseModel* >(invPbModelPtr);
  if(baseIncrPhraseModelPtr)
  {
        // Obtain sentence pair
    std::vector<std::string> srcSentStrVec;
    std::vector<std::string> refSentStrVec;
    Count c;
    swAligModelPtr->nthSentPair(n,srcSentStrVec,refSentStrVec,c);

        // Extract consistent phrase pairs
    std::vector<PhrasePair> vecUnfiltInvPhPair;
    PhraseExtractUtils::extractConsistentPhrasePairs(invSwAligModelPtr,swAligModelPtr,refSentStrVec,srcSentStrVec,vecUnfiltInvPhPair,verbose);

        // Filter phrase pairs
    std::vector<PhrasePair> vecInvPhPair;
    PhraseExtractUtils::filterPhrasePairs(vecUnfiltInvPhPair,vecInvPhPair);

        // Obtain mapped_n
    unsigned int mapped_n=map_n_am_suff_stats(n);
  
        // Grow vecVecInvPhPair if necessary
    std::vector<PhrasePair> vpp;
    while(vecVecInvPhPair.size()<=mapped_n) vecVecInvPhPair.push_back(vpp);
    
        // Subtract current phrase model sufficient statistics
    for(unsigned int i=0;i<vecVecInvPhPair[mapped_n].size();++i)
    {
      baseIncrPhraseModelPtr->strIncrCountsOfEntry(vecVecInvPhPair[mapped_n][i].s_,
                                                   vecVecInvPhPair[mapped_n][i].t_,
                                                   -1);
    }

        // Add new phrase model current sufficient statistics
    if(verbose) std::cerr<<"List of extracted consistent phrase pairs:"<<std::endl;
    for(unsigned int i=0;i<vecInvPhPair.size();++i)
    {
      baseIncrPhraseModelPtr->strIncrCountsOfEntry(vecInvPhPair[i].s_,
                                                   vecInvPhPair[i].t_,
                                                   1);
      if(verbose)
      {
        for(unsigned int j=0;j<vecInvPhPair[i].s_.size();++j) std::cerr<<vecInvPhPair[i].s_[j]<<" ";
        std::cerr<<"|||";
        for(unsigned int j=0;j<vecInvPhPair[i].t_.size();++j) std::cerr<<" "<<vecInvPhPair[i].t_[j];
        std::cerr<<std::endl;
      }
    }
  
        // Store new phrase model current sufficient statistics
    vecVecInvPhPair[mapped_n]=vecInvPhPair;

    return THOT_OK;
  }
  else
  {
    std::cerr<<"Warning: addition of new translation options not supported in this configuration!"<<std::endl;
    return THOT_ERROR;
  }
}

//---------------
unsigned int FeatureHandler::map_n_am_suff_stats(unsigned int n)
{
  return n;
}

//---------------
void FeatureHandler::trainWordPred(std::vector<std::string> strVec)
{
  langModelsInfo.wordPredictor.addSentence(strVec);
}


//---------------
int FeatureHandler::setWordPenSoFile(std::string soFileName)
{
  int verbosity=false;
  if(!wpModelInfo.classLoader.open_module(soFileName,verbosity))
  {
    std::cerr<<"Error: so file ("<<soFileName<<") could not be opened"<<std::endl;
    return THOT_ERROR;
  }
  else
  {
    wpSoFile=soFileName;
    return THOT_OK;
  }  
}

//---------------
int FeatureHandler::setDefaultLangSoFile(std::string soFileName)
{
  int verbosity=false;
  if(!langModelsInfo.defaultClassLoader.open_module(soFileName,verbosity))
  {
    std::cerr<<"Error: so file ("<<soFileName<<") could not be opened"<<std::endl;
    return THOT_ERROR;
  }
  else
  {
    defaultLangSoFile=soFileName;
    return THOT_OK;
  }
}

//---------------
int FeatureHandler::setDefaultTransSoFile(std::string soFileName)
{
  int verbosity=false;
  if(!phraseModelsInfo.defaultClassLoader.open_module(soFileName,verbosity))
  {
    std::cerr<<"Error: so file ("<<soFileName<<") could not be opened"<<std::endl;
    return THOT_ERROR;
  }
  else
  {
    defaultTransSoFile=soFileName;
    return THOT_OK;
  }
}

//---------------
int FeatureHandler::setDefaultSingleWordSoFile(std::string soFileName)
{
  int verbosity=false;
  if(!swModelsInfo.defaultClassLoader.open_module(soFileName,verbosity))
  {
    std::cerr<<"Error: so file ("<<soFileName<<") could not be opened"<<std::endl;
    return THOT_ERROR;
  }
  else
  {
    defaultSingleWordSoFile=soFileName;
    return THOT_OK;
  }
}

//--------------------------
BasePhraseModel* FeatureHandler::createPmPtr(std::string soFileName)
{
  if(soFileName.empty())
  {
    std::string initPars;
    BasePhraseModel* basePhrModelPtr=phraseModelsInfo.defaultClassLoader.make_obj(initPars);
    return basePhrModelPtr;
  }
  else
  {
        // Declare dynamic class loader instance
    SimpleDynClassLoader<BasePhraseModel> simpleDynClassLoader;
  
        // Open module
    bool verbosity=false;
    if(!simpleDynClassLoader.open_module(soFileName,verbosity))
    {
      std::cerr<<"Error: so file ("<<soFileName<<") could not be opened"<<std::endl;
      return NULL;
    }

        // Create tm file pointer
    BasePhraseModel* tmPtr=simpleDynClassLoader.make_obj("");

    if(tmPtr==NULL)
    {
      std::cerr<<"Error: BasePhraseModel pointer could not be instantiated"<<std::endl;    
      return NULL;
    }

        // Store loader
    phraseModelsInfo.simpleDynClassLoaderVec.push_back(simpleDynClassLoader);
        
    return tmPtr;
  }
}

//--------------------------
unsigned int FeatureHandler::getFeatureIdx(std::string featName)
{
  for(unsigned int i=0;i<featuresInfo.featPtrVec.size();++i)
  {
    if(featName==featuresInfo.featPtrVec[i]->getFeatName())
      return i;
  }
  return featuresInfo.featPtrVec.size();
}

//--------------------------
int FeatureHandler::createDirectPhrModelFeat(std::string featName,
                                             const ModelDescriptorEntry& modelDescEntry,
                                             DirectPhraseModelFeat<SmtModel::HypScoreInfo>** dirPmFeatPtrRef)
{
  std::cerr<<"** Creating direct phrase model feature ("<<featName<<" "<<modelDescEntry.modelInitInfo<<" "<<modelDescEntry.absolutizedModelFileName<<")"<<std::endl;

      // Display warning if so file is external
  if(soFileIsExternal(modelDescEntry.modelInitInfo))
  {
    std::cerr<<"Warning: so file ("<<modelDescEntry.modelInitInfo<<") is external to Thot package, to avoid execution problems ensure that the external file was compiled for the current version of the package"<<std::endl;
  }

      // Create feature pointer and set name
  (*dirPmFeatPtrRef)=new DirectPhraseModelFeat<SmtModel::HypScoreInfo>;
  DirectPhraseModelFeat<SmtModel::HypScoreInfo>* dirPmFeatPtr=*dirPmFeatPtrRef;
  dirPmFeatPtr->setFeatName(featName);

      // Add phrase model pointer
  BasePhraseModel* basePhraseModelPtr=createPmPtr(modelDescEntry.modelInitInfo);
  if(basePhraseModelPtr==NULL)
    return THOT_ERROR;
  phraseModelsInfo.invPbModelPtrVec.push_back(basePhraseModelPtr);

      // Add entry information
  phraseModelsInfo.modelDescEntryVec.push_back(modelDescEntry);

      // Add feature name
  phraseModelsInfo.featNameVec.push_back(featName);
  
      // Load phrase model
  std::cerr<<"* Loading phrase model..."<<std::endl;
  int ret=SmtModelUtils::loadPhrModel(basePhraseModelPtr,modelDescEntry.absolutizedModelFileName);
  if(ret==THOT_ERROR)
    return THOT_ERROR;
  
      // Link pointer to feature
  dirPmFeatPtr->link_pm(basePhraseModelPtr);  
  
      // Add direct swm pointer
  std::string initPars;
  BaseSwAligModel<PpInfo>* baseSwAligModelPtr=swModelsInfo.defaultClassLoader.make_obj(initPars);
  if(baseSwAligModelPtr==NULL)
  {
    std::cerr<<"Error: BaseSwAligModel pointer could not be instantiated"<<std::endl;
    return THOT_ERROR;
  }
  swModelsInfo.swAligModelPtrVec.push_back(baseSwAligModelPtr);

      // Add feature name
  swModelsInfo.featNameVec.push_back(featName);

      // Load direct single word model
  std::cerr<<"* Loading direct single word model..."<<std::endl;
  ret=SmtModelUtils::loadDirectSwModel(baseSwAligModelPtr,modelDescEntry.absolutizedModelFileName);
  if(ret==THOT_ERROR)
    return THOT_ERROR;
  
      // Link pointer to feature
  dirPmFeatPtr->link_swm(baseSwAligModelPtr);

      // Obtain lambda value
  std::cerr<<"* Reading lambda interpolation value..."<<std::endl;
  float lambda_swm;
  float lambda_invswm;
  std::string lambdaFileName=modelDescEntry.absolutizedModelFileName+".lambda";
  ret=SmtModelUtils::loadSwmLambdas(lambdaFileName,lambda_swm,lambda_invswm);
  if(ret==THOT_ERROR)
    return THOT_ERROR;
  std::cerr<<"lambda= "<<lambda_swm<<std::endl;
  
      // Set lambda value for feature
  dirPmFeatPtr->set_lambda(lambda_swm);
  
  return THOT_OK;
}

//--------------------------
int FeatureHandler::createInversePhrModelFeat(std::string featName,
                                              const ModelDescriptorEntry& modelDescEntry,
                                              BasePhraseModel* invPbModelPtr,
                                              InversePhraseModelFeat<SmtModel::HypScoreInfo>** invPmFeatPtrRef)
{
  std::cerr<<"** Creating inverse phrase model feature ("<<featName<<" "<<modelDescEntry.modelInitInfo<<" "<<modelDescEntry.absolutizedModelFileName<<")"<<std::endl;

      // Display warning if so file is external
  if(soFileIsExternal(modelDescEntry.modelInitInfo))
  {
    std::cerr<<"Warning: so file ("<<modelDescEntry.modelInitInfo<<") is external to Thot package, to avoid execution problems ensure that the external file was compiled for the current version of the package"<<std::endl;
  }

      // Create feature pointer and set name
  (*invPmFeatPtrRef)=new InversePhraseModelFeat<SmtModel::HypScoreInfo>;
  InversePhraseModelFeat<SmtModel::HypScoreInfo>* invPmFeatPtr=*invPmFeatPtrRef;
  invPmFeatPtr->setFeatName(featName);

      // Link pointer to feature
  std::cerr<<"* Linking phrase model previously loaded..."<<std::endl;
  invPmFeatPtr->link_pm(invPbModelPtr);

      // Add inverse swm pointer
  std::string initPars;
  BaseSwAligModel<PpInfo>* baseSwAligModelPtr=swModelsInfo.defaultClassLoader.make_obj(initPars);
  if(baseSwAligModelPtr==NULL)
  {
    std::cerr<<"Error: BaseSwAligModel pointer could not be instantiated"<<std::endl;
    return THOT_ERROR;
  }
  swModelsInfo.invSwAligModelPtrVec.push_back(baseSwAligModelPtr);

      // Add feature name
  swModelsInfo.invFeatNameVec.push_back(featName);

      // Load inverse single word model
  std::cerr<<"* Loading inverse single word model..."<<std::endl;
  int ret=SmtModelUtils::loadInverseSwModel(baseSwAligModelPtr,modelDescEntry.absolutizedModelFileName);
  if(ret==THOT_ERROR)
    return THOT_ERROR;
  
      // Link pointer to feature
  invPmFeatPtr->link_swm(baseSwAligModelPtr);

        // Obtain lambda value
  std::cerr<<"* Reading lambda interpolation value..."<<std::endl;
  float lambda_swm;
  float lambda_invswm;
  std::string lambdaFileName=modelDescEntry.absolutizedModelFileName+".lambda";
  ret=SmtModelUtils::loadSwmLambdas(lambdaFileName,lambda_swm,lambda_invswm);
  if(ret==THOT_ERROR)
    return THOT_ERROR;
  std::cerr<<"lambda= "<<lambda_invswm<<std::endl;
  
      // Set lambda value for feature
  invPmFeatPtr->set_lambda(lambda_invswm);

  return THOT_OK;
}

//--------------------------
int FeatureHandler::createSrcPhraseLenFeat(std::string featName,
                                           BasePhraseModel* basePhraseModelPtr,
                                           SrcPhraseLenFeat<SmtModel::HypScoreInfo>** srcPhraseLenFeatRef)
{
  std::cerr<<"** Creating source phrase length feature ("<<featName<<")"<<std::endl;

      // Create feature pointer and set name
  (*srcPhraseLenFeatRef)=new SrcPhraseLenFeat<SmtModel::HypScoreInfo>;
  SrcPhraseLenFeat<SmtModel::HypScoreInfo>* srcPhraseLenFeatPtr=*srcPhraseLenFeatRef;
  srcPhraseLenFeatPtr->setFeatName(featName);

      // Link pointer to feature
  srcPhraseLenFeatPtr->link_pm(basePhraseModelPtr);  
    
  return THOT_OK;
}

//--------------------------
int FeatureHandler::createTrgPhraseLenFeat(std::string featName,
                                           BasePhraseModel* basePhraseModelPtr,
                                           TrgPhraseLenFeat<SmtModel::HypScoreInfo>** trgPhraseLenFeatRef)
{
  std::cerr<<"** Creating target phrase length feature ("<<featName<<")"<<std::endl;

      // Create feature pointer and set name
  (*trgPhraseLenFeatRef)=new TrgPhraseLenFeat<SmtModel::HypScoreInfo>;
  TrgPhraseLenFeat<SmtModel::HypScoreInfo>* trgPhraseLenFeatPtr=*trgPhraseLenFeatRef;
  trgPhraseLenFeatPtr->setFeatName(featName);

      // Link pointer to feature
  trgPhraseLenFeatPtr->link_pm(basePhraseModelPtr);  
    
  return THOT_OK;
}

//--------------------------
int FeatureHandler::createSrcPosJumpFeat(std::string featName,
                                         BasePhraseModel* basePhraseModelPtr,
                                         SrcPosJumpFeat<SmtModel::HypScoreInfo>** srcPosJumpFeatRef)
{
  std::cerr<<"** Creating source position jump feature ("<<featName<<")"<<std::endl;

      // Create feature pointer and set name
  (*srcPosJumpFeatRef)=new SrcPosJumpFeat<SmtModel::HypScoreInfo>;
  SrcPosJumpFeat<SmtModel::HypScoreInfo>* srcPosJumpFeatPtr=*srcPosJumpFeatRef;
  srcPosJumpFeatPtr->setFeatName(featName);

      // Link pointer to feature
  srcPosJumpFeatPtr->link_pm(basePhraseModelPtr);  
    
  return THOT_OK;
}

//--------------------------
bool FeatureHandler::process_tm_descriptor(std::string tmDescFile,
                                           int verbose/*=0*/)
{
  if(verbose)
  {
    std::cerr<<"Processing translation model descriptor: "<<tmDescFile<<std::endl;
  }

      // Obtain info about translation model entries
  std::vector<ModelDescriptorEntry> modelDescEntryVec;
  if(extractModelEntryInfo(tmDescFile,modelDescEntryVec)==THOT_OK)
  {
        // Process descriptor entries
    for(unsigned int i=0;i<modelDescEntryVec.size();++i)
    {
          // Create direct phrase model feature
      std::string featName="pts_"+modelDescEntryVec[i].statusStr;
      DirectPhraseModelFeat<SmtModel::HypScoreInfo>* dirPmFeatPtr;
      int ret=createDirectPhrModelFeat(featName,modelDescEntryVec[i],&dirPmFeatPtr);
      if(ret==THOT_ERROR)
        return THOT_ERROR;
      featuresInfo.featPtrVec.push_back(dirPmFeatPtr);

          // Create inverse phrase model feature
      featName="pst_"+modelDescEntryVec[i].statusStr;
      InversePhraseModelFeat<SmtModel::HypScoreInfo>* invPmFeatPtr;
      ret=createInversePhrModelFeat(featName,modelDescEntryVec[i],dirPmFeatPtr->get_pmptr(),&invPmFeatPtr);
      if(ret==THOT_ERROR)
        return THOT_ERROR;
      featuresInfo.featPtrVec.push_back(invPmFeatPtr);
    }

        // Create source phrase length feature
    std::string featName="src_phr_len";
    SrcPhraseLenFeat<SmtModel::HypScoreInfo>* srcPhrLenFeatPtr;
    int ret=createSrcPhraseLenFeat(featName,phraseModelsInfo.invPbModelPtrVec[0],&srcPhrLenFeatPtr);
    if(ret==THOT_ERROR)
      return THOT_ERROR;
    featuresInfo.featPtrVec.push_back(srcPhrLenFeatPtr);

        // Create target phrase length feature
    featName="trg_phr_len";
    TrgPhraseLenFeat<SmtModel::HypScoreInfo>* trgPhrLenFeatPtr;
    ret=createTrgPhraseLenFeat(featName,phraseModelsInfo.invPbModelPtrVec[0],&trgPhrLenFeatPtr);
    if(ret==THOT_ERROR)
      return THOT_ERROR;
    featuresInfo.featPtrVec.push_back(trgPhrLenFeatPtr);

        // Create source position jump feature
    featName="src_pos_jump";
    SrcPosJumpFeat<SmtModel::HypScoreInfo>* srcPosJumpFeatPtr;
    ret=createSrcPosJumpFeat(featName,phraseModelsInfo.invPbModelPtrVec[0],&srcPosJumpFeatPtr);
    if(ret==THOT_ERROR)
      return THOT_ERROR;
    featuresInfo.featPtrVec.push_back(srcPosJumpFeatPtr);

    return THOT_OK;
  }
  else
  {
    return THOT_ERROR;
  }
}

//--------------------------
bool FeatureHandler::process_tm_files_prefix(std::string tmFilesPrefix,
                                             int verbose/*=0*/)
{
  if(verbose)
  {
    std::cerr<<"Processing translation model files prefix: "<<tmFilesPrefix<<std::endl;
  }

      // Create model descriptor entry
  ModelDescriptorEntry modelDescEntry;
  modelDescEntry.statusStr="main";
  modelDescEntry.modelInitInfo=defaultTransSoFile;
  modelDescEntry.modelFileName=tmFilesPrefix;
  modelDescEntry.absolutizedModelFileName=tmFilesPrefix;

      // Create direct phrase model feature
  std::string featName="pts";
  DirectPhraseModelFeat<SmtModel::HypScoreInfo>* dirPmFeatPtr;
  int ret=createDirectPhrModelFeat(featName,modelDescEntry,&dirPmFeatPtr);
  if(ret==THOT_ERROR)
    return THOT_ERROR;
  featuresInfo.featPtrVec.push_back(dirPmFeatPtr);

      // Create inverse phrase model feature
  featName="pst";
  InversePhraseModelFeat<SmtModel::HypScoreInfo>* invPmFeatPtr;
  ret=createInversePhrModelFeat(featName,modelDescEntry,dirPmFeatPtr->get_pmptr(),&invPmFeatPtr);
  if(ret==THOT_ERROR)
    return THOT_ERROR;
  featuresInfo.featPtrVec.push_back(invPmFeatPtr);

      // Create source phrase length feature
  featName="src_phr_len";
  SrcPhraseLenFeat<SmtModel::HypScoreInfo>* srcPhrLenFeatPtr;
  ret=createSrcPhraseLenFeat(featName,phraseModelsInfo.invPbModelPtrVec[0],&srcPhrLenFeatPtr);
  if(ret==THOT_ERROR)
    return THOT_ERROR;
  featuresInfo.featPtrVec.push_back(srcPhrLenFeatPtr);

      // Create target phrase length feature
  featName="trg_phr_len";
  TrgPhraseLenFeat<SmtModel::HypScoreInfo>* trgPhrLenFeatPtr;
  ret=createTrgPhraseLenFeat(featName,phraseModelsInfo.invPbModelPtrVec[0],&trgPhrLenFeatPtr);
  if(ret==THOT_ERROR)
    return THOT_ERROR;
  featuresInfo.featPtrVec.push_back(trgPhrLenFeatPtr);

      // Create source position jump feature
  featName="src_pos_jump";
  SrcPosJumpFeat<SmtModel::HypScoreInfo>* srcPosJumpFeatPtr;
  ret=createSrcPosJumpFeat(featName,phraseModelsInfo.invPbModelPtrVec[0],&srcPosJumpFeatPtr);
  if(ret==THOT_ERROR)
    return THOT_ERROR;
  featuresInfo.featPtrVec.push_back(srcPosJumpFeatPtr);
  
  return THOT_OK;
}

//---------------
BaseNgramLM<LM_State>* FeatureHandler::createLmPtr(std::string soFileName)
{
  if(soFileName.empty())
  {
    std::string initPars;
    BaseNgramLM<LM_State>* langModelPtr=langModelsInfo.defaultClassLoader.make_obj(initPars);
    return langModelPtr;
  }
  else
  {
        // Declare dynamic class loader instance
    SimpleDynClassLoader<BaseNgramLM<LM_State> > simpleDynClassLoader;
  
        // Open module
    bool verbosity=false;
    if(!simpleDynClassLoader.open_module(soFileName,verbosity))
    {
      std::cerr<<"Error: so file ("<<soFileName<<") could not be opened"<<std::endl;
      return NULL;
    }

        // Create lm file pointer
    BaseNgramLM<LM_State>* lmPtr=simpleDynClassLoader.make_obj("");

        // Store dynamic class loader
    langModelsInfo.simpleDynClassLoaderVec.push_back(simpleDynClassLoader);

    if(lmPtr==NULL)
    {
      std::cerr<<"Error: BaseNgramLM pointer could not be instantiated"<<std::endl;    
      return NULL;
    }
    
    return lmPtr;
  }
}

//---------------
int FeatureHandler::createLangModelFeat(std::string featName,
                                        const ModelDescriptorEntry& modelDescEntry,
                                        WordPredictor* wordPredPtr,
                                        LangModelFeat<SmtModel::HypScoreInfo>** langModelFeatPtrRef)
{
  std::cerr<<"** Creating language model feature ("<<featName<<" "<<modelDescEntry.modelInitInfo<<" "<<modelDescEntry.absolutizedModelFileName<<")"<<std::endl;

      // Display warning if so file is external
  if(soFileIsExternal(modelDescEntry.modelInitInfo))
  {
    std::cerr<<"Warning: so file ("<<modelDescEntry.modelInitInfo<<") is external to Thot package, to avoid execution problems ensure that the external file was compiled for the current version of the package"<<std::endl;
  }
  
      // Create feature pointer and set name
  (*langModelFeatPtrRef)=new LangModelFeat<SmtModel::HypScoreInfo>;
  LangModelFeat<SmtModel::HypScoreInfo>* langModelFeatPtr=*langModelFeatPtrRef;
  langModelFeatPtr->setFeatName(featName);

      // Add language model pointer
  BaseNgramLM<LM_State>* baseNgLmPtr=createLmPtr(modelDescEntry.modelInitInfo);
  if(baseNgLmPtr==NULL)
    return THOT_ERROR;
  langModelsInfo.lModelPtrVec.push_back(baseNgLmPtr);

      // Add entry information
  langModelsInfo.modelDescEntryVec.push_back(modelDescEntry);

      // Add feature name
  langModelsInfo.featNameVec.push_back(featName);

      // Load language model
  std::cerr<<"* Loading language model..."<<std::endl;
  int ret=SmtModelUtils::loadLangModel(baseNgLmPtr,modelDescEntry.absolutizedModelFileName);
  if(ret==THOT_ERROR)
    return THOT_ERROR;
  
      // Link pointer to feature
  langModelFeatPtr->link_lm(baseNgLmPtr);

      // Link word predictor to feature
  langModelFeatPtr->link_wp(wordPredPtr);
  
  return THOT_OK;
}

//---------------
bool FeatureHandler::process_lm_descriptor(std::string lmDescFile,
                                           int verbose/*=0*/)
{
  if(verbose)
  {
    std::cerr<<"Processing language model descriptor: "<<lmDescFile<<std::endl;
  }

      // Obtain info about language model entries
  std::vector<ModelDescriptorEntry> modelDescEntryVec;
  if(extractModelEntryInfo(lmDescFile,modelDescEntryVec)==THOT_OK)
  {
        // Process descriptor entries
    for(unsigned int i=0;i<modelDescEntryVec.size();++i)
    {
          // Create language model feature
      std::string featName="lm_"+modelDescEntryVec[i].statusStr;
      LangModelFeat<SmtModel::HypScoreInfo>* lmFeatPtr;
      int ret=createLangModelFeat(featName,modelDescEntryVec[i],&langModelsInfo.wordPredictor,&lmFeatPtr);
      if(ret==THOT_ERROR)
        return THOT_ERROR;
      featuresInfo.featPtrVec.push_back(lmFeatPtr);
    }

        // Load word predictor information
    std::cerr<<"* Loading word predictor information..."<<std::endl;
    int ret=loadWordPredInfo(lmDescFile);
    if(ret==THOT_ERROR)
      return THOT_ERROR;

    return THOT_OK;
  }
  else
  {
    return THOT_ERROR;
  }
}

//---------------
bool FeatureHandler::process_lm_files_prefix(std::string lmFilesPrefix,
                                             int verbose/*=0*/)
{
  if(verbose)
  {
    std::cerr<<"Processing language model files prefix: "<<lmFilesPrefix<<std::endl;
  }
  
        // Create model descriptor entry
  ModelDescriptorEntry modelDescEntry;
  modelDescEntry.statusStr="main";
  modelDescEntry.modelInitInfo=defaultLangSoFile;
  modelDescEntry.modelFileName=lmFilesPrefix;
  modelDescEntry.absolutizedModelFileName=lmFilesPrefix;

      // Create direct phrase model feature
  std::string featName="lm";
  
  LangModelFeat<SmtModel::HypScoreInfo>* lmFeatPtr;
  int ret=createLangModelFeat(featName,modelDescEntry,&langModelsInfo.wordPredictor,&lmFeatPtr);
  if(ret==THOT_ERROR)
    return THOT_ERROR;
  featuresInfo.featPtrVec.push_back(lmFeatPtr);

      // Load word predictor information
  std::cerr<<"** Loading word predictor information..."<<std::endl;
  ret=loadWordPredInfo(lmFilesPrefix);
  if(ret==THOT_ERROR)
    return THOT_ERROR;

  return THOT_OK;
}

//--------------------------
bool FeatureHandler::loadWordPredInfo(std::string lmFilesPrefix)
{
  std::string predFile=lmFilesPrefix;
  predFile=predFile+".wp";
  int err=langModelsInfo.wordPredictor.load(predFile.c_str());
  if(err==THOT_ERROR)
  {
    std::cerr<<"Warning: File for initializing the word predictor not provided!"<<std::endl;
  }
  return THOT_OK;
}

//--------------------------
bool FeatureHandler::print(std::string tmFileName,
                           std::string lmFileName,
                           int verbose/*=0*/)
{
      // Print language model parameters
  int ret=printLangModels(lmFileName,verbose);
  if(ret==THOT_ERROR)
    return THOT_ERROR;

      // Print alignment model parameters
  ret=printAligModels(tmFileName,verbose);
  if(ret==THOT_ERROR)
    return THOT_ERROR;
  
  return THOT_OK;
}

//--------------------------
bool FeatureHandler::printLambdasForModel(std::string modelFileName,
                                          std::string featName,
                                          std::string invFeatName,
                                          int /*verbose=0*/)
{
  std::string lambdaFileName=modelFileName+".lambda";

      // Obtain lambda for direct model
  unsigned int swmIdx=getFeatureIdx(featName);
  DirectPhraseModelFeat<SmtModel::HypScoreInfo>* dirPmFeatPtr=dynamic_cast<DirectPhraseModelFeat<SmtModel::HypScoreInfo>* >(featuresInfo.featPtrVec[swmIdx]);
  float lambda_swm=dirPmFeatPtr->get_lambda();

      // Obtain lambda for inverse model
  unsigned int invSwmIdx=getFeatureIdx(invFeatName);
  InversePhraseModelFeat<SmtModel::HypScoreInfo>* invPmFeatPtr=dynamic_cast<InversePhraseModelFeat<SmtModel::HypScoreInfo>* >(featuresInfo.featPtrVec[invSwmIdx]);
  float lambda_invswm=invPmFeatPtr->get_lambda();

  return SmtModelUtils::printSwmLambdas(lambdaFileName.c_str(),lambda_swm,lambda_invswm);
}

//--------------------------
bool FeatureHandler::printAligModels(std::string tmFileName,
                                     int verbose/*=0*/)
{
  if(fileIsDescriptor(tmFileName))
  {
        // Print phrase models
    for(unsigned int i=0;i<phraseModelsInfo.invPbModelPtrVec.size();++i)
    {
      int ret=SmtModelUtils::printPhrModel(phraseModelsInfo.invPbModelPtrVec[i],phraseModelsInfo.modelDescEntryVec[i].absolutizedModelFileName);
      if(ret==THOT_ERROR)
        return THOT_ERROR;
    }

        // Print direct single word models
    for(unsigned int i=0;i<swModelsInfo.featNameVec.size();++i)
    {
      std::string modelFileName=phraseModelsInfo.modelDescEntryVec[i].absolutizedModelFileName;
      int ret=SmtModelUtils::printDirectSwModel(swModelsInfo.swAligModelPtrVec[i],modelFileName);
      if(ret==THOT_ERROR)
        return THOT_ERROR;      
    }

        // Print inverse single word models
    for(unsigned int i=0;i<swModelsInfo.featNameVec.size();++i)
    {
      std::string modelFileName=phraseModelsInfo.modelDescEntryVec[i].absolutizedModelFileName;
      int ret=SmtModelUtils::printInverseSwModel(swModelsInfo.invSwAligModelPtrVec[i],modelFileName);
      if(ret==THOT_ERROR)
        return THOT_ERROR;      
    }

        // Print lambda files
    for(unsigned int i=0;i<swModelsInfo.featNameVec.size();++i)
    {
      std::string featName=swModelsInfo.featNameVec[i];
      std::string invFeatName=swModelsInfo.invFeatNameVec[i];
      std::string modelFileName=phraseModelsInfo.modelDescEntryVec[i].absolutizedModelFileName;

      int ret=printLambdasForModel(modelFileName,featName,invFeatName,verbose);
      if(ret==THOT_ERROR)
        return THOT_ERROR;      
    }
    
    return THOT_OK;
  }
  else
  {
    int ret=SmtModelUtils::printPhrModel(phraseModelsInfo.invPbModelPtrVec[0],tmFileName);
    if(ret==THOT_ERROR)
      return THOT_ERROR;
    
    ret=SmtModelUtils::printDirectSwModel(swModelsInfo.swAligModelPtrVec[0],tmFileName);
    if(ret==THOT_ERROR)
      return THOT_ERROR;
        
    ret=SmtModelUtils::printInverseSwModel(swModelsInfo.invSwAligModelPtrVec[0],tmFileName);
    if(ret==THOT_ERROR)
      return THOT_ERROR;

    std::string featName=swModelsInfo.featNameVec[0];
    std::string invFeatName=swModelsInfo.invFeatNameVec[0];    
    return printLambdasForModel(tmFileName,featName,invFeatName,verbose);
  }
}

//--------------------------
bool FeatureHandler::printAligModelLambdas(std::string tmFileName,
                                           int verbose/*=0*/)
{
  if(fileIsDescriptor(tmFileName))
  {
        // Print lambda files
    for(unsigned int i=0;i<swModelsInfo.featNameVec.size();++i)
    {
      std::string featName=swModelsInfo.featNameVec[i];
      std::string invFeatName=swModelsInfo.invFeatNameVec[i];
      std::string modelFileName=phraseModelsInfo.modelDescEntryVec[i].absolutizedModelFileName;

      int ret=printLambdasForModel(modelFileName,featName,invFeatName,verbose);
      if(ret==THOT_ERROR)
        return THOT_ERROR;      
    }
    
    return THOT_OK;
  }
  else
  {
    std::string featName=swModelsInfo.featNameVec[0];
    std::string invFeatName=swModelsInfo.invFeatNameVec[0];    
    return printLambdasForModel(tmFileName,featName,invFeatName,verbose);
  }
}

//--------------------------
bool FeatureHandler::printLangModels(std::string lmFileName,
                                     int /*verbose=0*/)
{
  if(fileIsDescriptor(lmFileName))
  {
    for(unsigned int i=0;i<langModelsInfo.lModelPtrVec.size();++i)
    {
      int ret=SmtModelUtils::printLangModel(langModelsInfo.lModelPtrVec[i],langModelsInfo.modelDescEntryVec[i].absolutizedModelFileName);
      if(ret==THOT_ERROR)
        return THOT_ERROR;
    }
    return THOT_OK;
  }
  else
  {
    return SmtModelUtils::printLangModel(langModelsInfo.lModelPtrVec[0],lmFileName);
  }
}

//--------------------------
void FeatureHandler::clear(void)
{
      // Clear training related data
  vecVecInvPhPair.clear();
  
      // Delete model pointers
  deleteWpModelPtr();
  deleteLangModelPtrs();
  deletePhrModelPtrs();
  deleteSwModelPtrs();

      // Delete feature pointers
  for(unsigned int i=0;i<featuresInfo.featPtrVec.size();++i)
  {
    delete featuresInfo.featPtrVec[i];
  }
  featuresInfo.featPtrVec.clear();
}

//--------------------------
void FeatureHandler::deleteWpModelPtr(void)
{
      // Release pointer
  if(wpModelInfo.wpModelPtr!=NULL)
  {
    delete wpModelInfo.wpModelPtr;
    wpModelInfo.wpModelPtr=NULL;
  }

      // Close modules
  int verbosity=false;
  wpModelInfo.classLoader.close_module(verbosity);
}

//--------------------------
void FeatureHandler::deleteLangModelPtrs(void)
{
      // Clear word predictor
  langModelsInfo.wordPredictor.clear();
  
      // Release pointers
  for(unsigned int i=0;i<langModelsInfo.lModelPtrVec.size();++i)
    delete langModelsInfo.lModelPtrVec[i];
  langModelsInfo.lModelPtrVec.clear();
  
      // Close modules
  int verbosity=false;
  for(unsigned int i=0;i<langModelsInfo.simpleDynClassLoaderVec.size();++i)
    langModelsInfo.simpleDynClassLoaderVec[i].close_module(verbosity);
  langModelsInfo.simpleDynClassLoaderVec.clear();
  
  langModelsInfo.defaultClassLoader.close_module(verbosity);

      // Clear model descriptor entries
  langModelsInfo.modelDescEntryVec.clear();

      // Clear feature names
  langModelsInfo.featNameVec.clear();

      // Clear dynamic class loader vector
  langModelsInfo.simpleDynClassLoaderVec.clear();
}

//--------------------------
void FeatureHandler::deletePhrModelPtrs(void)
{
      // Release pointers
  for(unsigned int i=0;i<phraseModelsInfo.invPbModelPtrVec.size();++i)
    delete phraseModelsInfo.invPbModelPtrVec[i];
  phraseModelsInfo.invPbModelPtrVec.clear();
  
      // Close modules
  int verbosity=false;
  for(unsigned int i=0;i<phraseModelsInfo.simpleDynClassLoaderVec.size();++i)
    phraseModelsInfo.simpleDynClassLoaderVec[i].close_module(verbosity);
  phraseModelsInfo.simpleDynClassLoaderVec.clear();
  
  phraseModelsInfo.defaultClassLoader.close_module(verbosity);
  
      // Clear model descriptor entries
  phraseModelsInfo.modelDescEntryVec.clear();

      // Clear feature names
  phraseModelsInfo.featNameVec.clear();
  
      // Clear dynamic class loader vector
  phraseModelsInfo.simpleDynClassLoaderVec.clear();
}

//--------------------------
void FeatureHandler::deleteSwModelPtrs(void)
{
  for(unsigned int i=0;i<swModelsInfo.swAligModelPtrVec.size();++i)
    delete swModelsInfo.swAligModelPtrVec[i];
  swModelsInfo.swAligModelPtrVec.clear();
  
  for(unsigned int i=0;i<swModelsInfo.invSwAligModelPtrVec.size();++i)
    delete swModelsInfo.invSwAligModelPtrVec[i];
  swModelsInfo.invSwAligModelPtrVec.clear();
  
  swModelsInfo.featNameVec.clear();
  swModelsInfo.invFeatNameVec.clear();

  int verbosity=false;
  swModelsInfo.defaultClassLoader.close_module(verbosity);
}

//--------------------------
FeatureHandler::~FeatureHandler()
{
  clear();
}
