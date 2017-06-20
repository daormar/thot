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
/* Module: FeatureHandler                                           */
/*                                                                  */
/* Definitions file: FeatureHandler.cc                              */
/*                                                                  */
/********************************************************************/


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
  cerr<<"** Creating word penalty feature ("<<featName<<" "<<wpModelType<<")"<<endl;

      // Create model pointer if necessary
  if(wpModelInfo.wpModelPtr==NULL)
  {
    std::string initPars;
    wpModelInfo.wpModelPtr=wpModelInfo.classLoader.make_obj(initPars);
    if(wpModelInfo.wpModelPtr==NULL)
    {
      cerr<<"Error: BaseWordPenaltyModel pointer could not be instantiated"<<endl;    
      return ERROR;
    }
  }
  
      // Add feature
  WordPenaltyFeat<SmtModel::HypScoreInfo>* wordPenaltyFeatPtr=new WordPenaltyFeat<SmtModel::HypScoreInfo>;
  wordPenaltyFeatPtr->setFeatName(featName);
  wordPenaltyFeatPtr->link_wpm(wpModelInfo.wpModelPtr);
  featuresInfo.featPtrVec.push_back(wordPenaltyFeatPtr);

  return OK;
}

//---------------
int FeatureHandler::addLmFeats(std::string lmFileName,
                               int verbose)
{
  std::string mainFileName;
  if(fileIsDescriptor(lmFileName,mainFileName))
    return process_lm_descriptor(lmFileName,verbose);
  else
    return process_lm_files_prefix(lmFileName,verbose);
}

//---------------
int FeatureHandler::addTmFeats(std::string tmFilesPrefix,
                               int verbose)
{
  std::string mainFileName;
  if(fileIsDescriptor(tmFilesPrefix,mainFileName))
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
int FeatureHandler::updateLinInterpWeights(std::string srcCorpusFileName,
                                           std::string trgCorpusFileName,
                                           int verbose/*=0*/)
{
      // Update linear interpolation weights for all direct and inverse
      // phrase model feature pairs
  for(unsigned int i=0;i<swModelsInfo.swAligModelPtrVec.size();++i)
  {
    if(verbose)
      cerr<<"Updating linear interpolation weights for features "<<swModelsInfo.featNameVec[i]<<" and "<<swModelsInfo.invFeatNameVec[i]<<endl;

        // Obtain indices for features
    unsigned int dirPhrModelFeatIdx=getFeatureIdx(swModelsInfo.featNameVec[i]);
    unsigned int invPhrModelFeatIdx=getFeatureIdx(swModelsInfo.invFeatNameVec[i]);

        // Obtain specialized pointers
    DirectPhraseModelFeat<SmtModel::HypScoreInfo>* dirPmFeatPtr=dynamic_cast<DirectPhraseModelFeat<SmtModel::HypScoreInfo>* >(featuresInfo.featPtrVec[dirPhrModelFeatIdx]);
    InversePhraseModelFeat<SmtModel::HypScoreInfo>* invPmFeatPtr=dynamic_cast<InversePhraseModelFeat<SmtModel::HypScoreInfo>* >(featuresInfo.featPtrVec[invPhrModelFeatIdx]);
    
        // Update weights
    int ret=WeightUpdateUtils::updateLinInterpWeights(srcCorpusFileName,
                                                      trgCorpusFileName,
                                                      dirPmFeatPtr,
                                                      invPmFeatPtr,
                                                      verbose);
    if(ret==ERROR)
      return ERROR;
  }

  return OK;
}

//---------------
int FeatureHandler::setWordPenModelType(std::string modelType)
{
  int verbosity=false;
  if(!wpModelInfo.classLoader.open_module(modelType,verbosity))
  {
    cerr<<"Error: so file ("<<modelType<<") could not be opened"<<endl;
    return ERROR;
  }
  else
  {
    wpModelType=modelType;
    return OK;
  }  
}

//---------------
int FeatureHandler::setDefaultLangModelType(std::string modelType)
{
  int verbosity=false;
  if(!langModelsInfo.defaultClassLoader.open_module(modelType,verbosity))
  {
    cerr<<"Error: so file ("<<modelType<<") could not be opened"<<endl;
    return ERROR;
  }
  else
  {
    defaultLangModelType=modelType;
    return OK;
  }
}

//---------------
int FeatureHandler::setDefaultTransModelType(std::string modelType)
{
  int verbosity=false;
  if(!phraseModelsInfo.defaultClassLoader.open_module(modelType,verbosity))
  {
    cerr<<"Error: so file ("<<modelType<<") could not be opened"<<endl;
    return ERROR;
  }
  else
  {
    defaultTransModelType=modelType;
    return OK;
  }
}

//---------------
int FeatureHandler::setDefaultSingleWordModelType(std::string modelType)
{
  int verbosity=false;
  if(!swModelsInfo.defaultClassLoader.open_module(modelType,verbosity))
  {
    cerr<<"Error: so file ("<<modelType<<") could not be opened"<<endl;
    return ERROR;
  }
  else
  {
    defaultSingleWordModelType=modelType;
    return OK;
  }
}

//--------------------------
BasePhraseModel* FeatureHandler::createPmPtr(std::string modelType)
{
  if(modelType.empty())
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
    if(!simpleDynClassLoader.open_module(modelType,verbosity))
    {
      cerr<<"Error: so file ("<<modelType<<") could not be opened"<<endl;
      return NULL;
    }

        // Create tm file pointer
    BasePhraseModel* tmPtr=simpleDynClassLoader.make_obj("");

    if(tmPtr==NULL)
    {
      cerr<<"Error: BasePhraseModel pointer could not be instantiated"<<endl;    
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
  cerr<<"** Creating direct phrase model feature ("<<featName<<" "<<modelDescEntry.modelType<<" "<<modelDescEntry.absolutizedModelFileName<<")"<<endl;

      // Create feature pointer and set name
  (*dirPmFeatPtrRef)=new DirectPhraseModelFeat<SmtModel::HypScoreInfo>;
  DirectPhraseModelFeat<SmtModel::HypScoreInfo>* dirPmFeatPtr=*dirPmFeatPtrRef;
  dirPmFeatPtr->setFeatName(featName);

      // Add phrase model pointer
  BasePhraseModel* basePhraseModelPtr=createPmPtr(modelDescEntry.modelType);
  if(basePhraseModelPtr==NULL)
    return ERROR;
  phraseModelsInfo.invPbModelPtrVec.push_back(basePhraseModelPtr);

      // Add entry information
  phraseModelsInfo.modelDescEntryVec.push_back(modelDescEntry);

      // Add feature name
  phraseModelsInfo.featNameVec.push_back(featName);
  
      // Load phrase model
  cerr<<"* Loading phrase model..."<<endl;
  int ret=SmtModelUtils::loadPhrModel(basePhraseModelPtr,modelDescEntry.absolutizedModelFileName);
  if(ret==ERROR)
    return ERROR;
  
      // Link pointer to feature
  dirPmFeatPtr->link_pm(basePhraseModelPtr);  
  
      // Add direct swm pointer
  std::string initPars;
  BaseSwAligModel<PpInfo>* baseSwAligModelPtr=swModelsInfo.defaultClassLoader.make_obj(initPars);
  if(baseSwAligModelPtr==NULL)
  {
    cerr<<"Error: BaseSwAligModel pointer could not be instantiated"<<endl;
    return ERROR;
  }
  swModelsInfo.swAligModelPtrVec.push_back(baseSwAligModelPtr);

      // Add feature name
  swModelsInfo.featNameVec.push_back(featName);

      // Load direct single word model
  cerr<<"* Loading direct single word model..."<<endl;
  ret=SmtModelUtils::loadDirectSwModel(baseSwAligModelPtr,modelDescEntry.absolutizedModelFileName);
  if(ret==ERROR)
    return ERROR;
  
      // Link pointer to feature
  dirPmFeatPtr->link_swm(baseSwAligModelPtr);

      // Obtain lambda value
  cerr<<"* Reading lambda interpolation value..."<<endl;
  float lambda_swm;
  float lambda_invswm;
  std::string lambdaFileName=modelDescEntry.absolutizedModelFileName+".lambda";
  ret=SmtModelUtils::loadSwmLambdas(lambdaFileName,lambda_swm,lambda_invswm);
  if(ret==ERROR)
    return ERROR;
  cerr<<"lambda= "<<lambda_swm<<endl;
  
      // Set lambda value for feature
  dirPmFeatPtr->set_lambda(lambda_swm);
  
  return OK;
}

//--------------------------
int FeatureHandler::createInversePhrModelFeat(std::string featName,
                                              const ModelDescriptorEntry& modelDescEntry,
                                              BasePhraseModel* invPbModelPtr,
                                              InversePhraseModelFeat<SmtModel::HypScoreInfo>** invPmFeatPtrRef)
{
  cerr<<"** Creating inverse phrase model feature ("<<featName<<" "<<modelDescEntry.modelType<<" "<<modelDescEntry.absolutizedModelFileName<<")"<<endl;

      // Create feature pointer and set name
  (*invPmFeatPtrRef)=new InversePhraseModelFeat<SmtModel::HypScoreInfo>;
  InversePhraseModelFeat<SmtModel::HypScoreInfo>* invPmFeatPtr=*invPmFeatPtrRef;
  invPmFeatPtr->setFeatName(featName);

      // Link pointer to feature
  cerr<<"* Linking phrase model previously loaded..."<<endl;
  invPmFeatPtr->link_pm(invPbModelPtr);

      // Add inverse swm pointer
  std::string initPars;
  BaseSwAligModel<PpInfo>* baseSwAligModelPtr=swModelsInfo.defaultClassLoader.make_obj(initPars);
  if(baseSwAligModelPtr==NULL)
  {
    cerr<<"Error: BaseSwAligModel pointer could not be instantiated"<<endl;
    return ERROR;
  }
  swModelsInfo.invSwAligModelPtrVec.push_back(baseSwAligModelPtr);

      // Add feature name
  swModelsInfo.invFeatNameVec.push_back(featName);

      // Load inverse single word model
  cerr<<"* Loading inverse single word model..."<<endl;
  int ret=SmtModelUtils::loadInverseSwModel(baseSwAligModelPtr,modelDescEntry.absolutizedModelFileName);
  if(ret==ERROR)
    return ERROR;
  
      // Link pointer to feature
  invPmFeatPtr->link_swm(baseSwAligModelPtr);

        // Obtain lambda value
  cerr<<"* Reading lambda interpolation value..."<<endl;
  float lambda_swm;
  float lambda_invswm;
  std::string lambdaFileName=modelDescEntry.absolutizedModelFileName+".lambda";
  ret=SmtModelUtils::loadSwmLambdas(lambdaFileName,lambda_swm,lambda_invswm);
  if(ret==ERROR)
    return ERROR;
  cerr<<"lambda= "<<lambda_invswm<<endl;
  
      // Set lambda value for feature
  invPmFeatPtr->set_lambda(lambda_invswm);

  return OK;
}

//--------------------------
int FeatureHandler::createSrcPhraseLenFeat(std::string featName,
                                           BasePhraseModel* basePhraseModelPtr,
                                           SrcPhraseLenFeat<SmtModel::HypScoreInfo>** srcPhraseLenFeatRef)
{
  cerr<<"** Creating source phrase length feature ("<<featName<<")"<<endl;

      // Create feature pointer and set name
  (*srcPhraseLenFeatRef)=new SrcPhraseLenFeat<SmtModel::HypScoreInfo>;
  SrcPhraseLenFeat<SmtModel::HypScoreInfo>* srcPhraseLenFeatPtr=*srcPhraseLenFeatRef;
  srcPhraseLenFeatPtr->setFeatName(featName);

      // Link pointer to feature
  srcPhraseLenFeatPtr->link_pm(basePhraseModelPtr);  
    
  return OK;
}

//--------------------------
int FeatureHandler::createTrgPhraseLenFeat(std::string featName,
                                           BasePhraseModel* basePhraseModelPtr,
                                           TrgPhraseLenFeat<SmtModel::HypScoreInfo>** trgPhraseLenFeatRef)
{
  cerr<<"** Creating target phrase length feature ("<<featName<<")"<<endl;

      // Create feature pointer and set name
  (*trgPhraseLenFeatRef)=new TrgPhraseLenFeat<SmtModel::HypScoreInfo>;
  TrgPhraseLenFeat<SmtModel::HypScoreInfo>* trgPhraseLenFeatPtr=*trgPhraseLenFeatRef;
  trgPhraseLenFeatPtr->setFeatName(featName);

      // Link pointer to feature
  trgPhraseLenFeatPtr->link_pm(basePhraseModelPtr);  
    
  return OK;
}

//--------------------------
int FeatureHandler::createSrcPosJumpFeat(std::string featName,
                                         BasePhraseModel* basePhraseModelPtr,
                                         SrcPosJumpFeat<SmtModel::HypScoreInfo>** srcPosJumpFeatRef)
{
  cerr<<"** Creating source position jump feature ("<<featName<<")"<<endl;

      // Create feature pointer and set name
  (*srcPosJumpFeatRef)=new SrcPosJumpFeat<SmtModel::HypScoreInfo>;
  SrcPosJumpFeat<SmtModel::HypScoreInfo>* srcPosJumpFeatPtr=*srcPosJumpFeatRef;
  srcPosJumpFeatPtr->setFeatName(featName);

      // Link pointer to feature
  srcPosJumpFeatPtr->link_pm(basePhraseModelPtr);  
    
  return OK;
}

//--------------------------
bool FeatureHandler::process_tm_descriptor(std::string tmDescFile,
                                           int verbose/*=0*/)
{
  if(verbose)
  {
    cerr<<"Processing translation model descriptor: "<<tmDescFile<<endl;
  }

      // Obtain info about translation model entries
  Vector<ModelDescriptorEntry> modelDescEntryVec;
  if(extractModelEntryInfo(tmDescFile,modelDescEntryVec)==OK)
  {
        // Process descriptor entries
    for(unsigned int i=0;i<modelDescEntryVec.size();++i)
    {
          // Create direct phrase model feature
      std::string featName="pts_"+modelDescEntryVec[i].statusStr;
      DirectPhraseModelFeat<SmtModel::HypScoreInfo>* dirPmFeatPtr;
      int ret=createDirectPhrModelFeat(featName,modelDescEntryVec[i],&dirPmFeatPtr);
      if(ret==ERROR)
        return ERROR;
      featuresInfo.featPtrVec.push_back(dirPmFeatPtr);

          // Create inverse phrase model feature
      featName="pst_"+modelDescEntryVec[i].statusStr;
      InversePhraseModelFeat<SmtModel::HypScoreInfo>* invPmFeatPtr;
      ret=createInversePhrModelFeat(featName,modelDescEntryVec[i],dirPmFeatPtr->get_pmptr(),&invPmFeatPtr);
      if(ret==ERROR)
        return ERROR;
      featuresInfo.featPtrVec.push_back(invPmFeatPtr);
    }

        // Create source phrase length feature
    std::string featName="src_phr_len";
    SrcPhraseLenFeat<SmtModel::HypScoreInfo>* srcPhrLenFeatPtr;
    int ret=createSrcPhraseLenFeat(featName,phraseModelsInfo.invPbModelPtrVec[0],&srcPhrLenFeatPtr);
    if(ret==ERROR)
      return ERROR;
    featuresInfo.featPtrVec.push_back(srcPhrLenFeatPtr);

        // Create target phrase length feature
    featName="trg_phr_len";
    TrgPhraseLenFeat<SmtModel::HypScoreInfo>* trgPhrLenFeatPtr;
    ret=createTrgPhraseLenFeat(featName,phraseModelsInfo.invPbModelPtrVec[0],&trgPhrLenFeatPtr);
    if(ret==ERROR)
      return ERROR;
    featuresInfo.featPtrVec.push_back(trgPhrLenFeatPtr);

        // Create source position jump feature
    featName="src_pos_jump";
    SrcPosJumpFeat<SmtModel::HypScoreInfo>* srcPosJumpFeatPtr;
    ret=createSrcPosJumpFeat(featName,phraseModelsInfo.invPbModelPtrVec[0],&srcPosJumpFeatPtr);
    if(ret==ERROR)
      return ERROR;
    featuresInfo.featPtrVec.push_back(srcPosJumpFeatPtr);

    return OK;
  }
  else
  {
    return ERROR;
  }
}

//--------------------------
bool FeatureHandler::process_tm_files_prefix(std::string tmFilesPrefix,
                                             int verbose/*=0*/)
{
  if(verbose)
  {
    cerr<<"Processing translation model files prefix: "<<tmFilesPrefix<<endl;
  }

      // Create model descriptor entry
  ModelDescriptorEntry modelDescEntry;
  modelDescEntry.statusStr="main";
  modelDescEntry.modelType=defaultTransModelType;
  modelDescEntry.modelFileName=tmFilesPrefix;
  modelDescEntry.absolutizedModelFileName=tmFilesPrefix;

      // Create direct phrase model feature
  std::string featName="pts";
  DirectPhraseModelFeat<SmtModel::HypScoreInfo>* dirPmFeatPtr;
  int ret=createDirectPhrModelFeat(featName,modelDescEntry,&dirPmFeatPtr);
  if(ret==ERROR)
    return ERROR;
  featuresInfo.featPtrVec.push_back(dirPmFeatPtr);

      // Create inverse phrase model feature
  featName="pst";
  InversePhraseModelFeat<SmtModel::HypScoreInfo>* invPmFeatPtr;
  ret=createInversePhrModelFeat(featName,modelDescEntry,dirPmFeatPtr->get_pmptr(),&invPmFeatPtr);
  if(ret==ERROR)
    return ERROR;
  featuresInfo.featPtrVec.push_back(invPmFeatPtr);

      // Create source phrase length feature
  featName="src_phr_len";
  SrcPhraseLenFeat<SmtModel::HypScoreInfo>* srcPhrLenFeatPtr;
  ret=createSrcPhraseLenFeat(featName,phraseModelsInfo.invPbModelPtrVec[0],&srcPhrLenFeatPtr);
  if(ret==ERROR)
    return ERROR;
  featuresInfo.featPtrVec.push_back(srcPhrLenFeatPtr);

      // Create target phrase length feature
  featName="trg_phr_len";
  TrgPhraseLenFeat<SmtModel::HypScoreInfo>* trgPhrLenFeatPtr;
  ret=createTrgPhraseLenFeat(featName,phraseModelsInfo.invPbModelPtrVec[0],&trgPhrLenFeatPtr);
  if(ret==ERROR)
    return ERROR;
  featuresInfo.featPtrVec.push_back(trgPhrLenFeatPtr);

      // Create source position jump feature
  featName="src_pos_jump";
  SrcPosJumpFeat<SmtModel::HypScoreInfo>* srcPosJumpFeatPtr;
  ret=createSrcPosJumpFeat(featName,phraseModelsInfo.invPbModelPtrVec[0],&srcPosJumpFeatPtr);
  if(ret==ERROR)
    return ERROR;
  featuresInfo.featPtrVec.push_back(srcPosJumpFeatPtr);
  
  return OK;
}

//---------------
BaseNgramLM<LM_State>* FeatureHandler::createLmPtr(std::string modelType)
{
  if(modelType.empty())
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
    if(!simpleDynClassLoader.open_module(modelType,verbosity))
    {
      cerr<<"Error: so file ("<<modelType<<") could not be opened"<<endl;
      return NULL;
    }

        // Create lm file pointer
    BaseNgramLM<LM_State>* lmPtr=simpleDynClassLoader.make_obj("");

        // Store dynamic class loader
    langModelsInfo.simpleDynClassLoaderVec.push_back(simpleDynClassLoader);

    if(lmPtr==NULL)
    {
      cerr<<"Error: BaseNgramLM pointer could not be instantiated"<<endl;    
      return NULL;
    }
    
    return lmPtr;
  }
}

//---------------
int FeatureHandler::createLangModelFeat(std::string featName,
                                        const ModelDescriptorEntry& modelDescEntry,
                                        LangModelFeat<SmtModel::HypScoreInfo>** langModelFeatPtrRef)
{
  cerr<<"** Creating language model feature ("<<featName<<" "<<modelDescEntry.modelType<<" "<<modelDescEntry.absolutizedModelFileName<<")"<<endl;

      // Create feature pointer and set name
  (*langModelFeatPtrRef)=new LangModelFeat<SmtModel::HypScoreInfo>;
  LangModelFeat<SmtModel::HypScoreInfo>* langModelFeatPtr=*langModelFeatPtrRef;
  langModelFeatPtr->setFeatName(featName);

      // Add language model pointer
  BaseNgramLM<LM_State>* baseNgLmPtr=createLmPtr(modelDescEntry.modelType);
  if(baseNgLmPtr==NULL)
    return ERROR;
  langModelsInfo.lModelPtrVec.push_back(baseNgLmPtr);

      // Add entry information
  langModelsInfo.modelDescEntryVec.push_back(modelDescEntry);

      // Add feature name
  langModelsInfo.featNameVec.push_back(featName);

      // Load language model
  cerr<<"* Loading language model..."<<endl;
  int ret=SmtModelUtils::loadLangModel(baseNgLmPtr,modelDescEntry.absolutizedModelFileName);
  if(ret==ERROR)
    return ERROR;
  
      // Link pointer to feature
  langModelFeatPtr->link_lm(baseNgLmPtr);
  
  return OK;
}

//---------------
bool FeatureHandler::process_lm_descriptor(std::string lmDescFile,
                                           int verbose/*=0*/)
{
  if(verbose)
  {
    cerr<<"Processing language model descriptor: "<<lmDescFile<<endl;
  }

      // Obtain info about translation model entries
  Vector<ModelDescriptorEntry> modelDescEntryVec;
  if(extractModelEntryInfo(lmDescFile,modelDescEntryVec)==OK)
  {
        // Process descriptor entries
    for(unsigned int i=0;i<modelDescEntryVec.size();++i)
    {
          // Create direct phrase model feature
      std::string featName="lm_"+modelDescEntryVec[i].statusStr;
      LangModelFeat<SmtModel::HypScoreInfo>* lmFeatPtr;
      int ret=createLangModelFeat(featName,modelDescEntryVec[i],&lmFeatPtr);
      if(ret==ERROR)
        return ERROR;
      featuresInfo.featPtrVec.push_back(lmFeatPtr);
    }
    
    return OK;
  }
  else
  {
    return ERROR;
  }
}

//---------------
bool FeatureHandler::process_lm_files_prefix(std::string lmFilesPrefix,
                                             int verbose/*=0*/)
{
  if(verbose)
  {
    cerr<<"Processing language model files prefix: "<<lmFilesPrefix<<endl;
  }

        // Create model descriptor entry
  ModelDescriptorEntry modelDescEntry;
  modelDescEntry.statusStr="main";
  modelDescEntry.modelType=defaultLangModelType;
  modelDescEntry.modelFileName=lmFilesPrefix;
  modelDescEntry.absolutizedModelFileName=lmFilesPrefix;

      // Create direct phrase model feature
  std::string featName="pts";
  
  LangModelFeat<SmtModel::HypScoreInfo>* lmFeatPtr;
  int ret=createLangModelFeat(featName,modelDescEntry,&lmFeatPtr);
  if(ret==ERROR)
    return ERROR;
  featuresInfo.featPtrVec.push_back(lmFeatPtr);
  
  return OK;
}

//--------------------------
bool FeatureHandler::print(std::string tmFileName,
                           std::string lmFileName,
                           int verbose/*=0*/)
{
      // Print language model parameters
  int ret=printLangModels(lmFileName,verbose);
  if(ret==ERROR)
    return ERROR;

      // Print alignment model parameters
  ret=printAligModels(tmFileName,verbose);
  if(ret==ERROR)
    return ERROR;
  
  return OK;
}

//--------------------------
bool FeatureHandler::printLambdas(std::string modelFileName,
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
  std::string mainFileName;
  if(fileIsDescriptor(tmFileName,mainFileName))
  {
    for(unsigned int i=0;i<phraseModelsInfo.invPbModelPtrVec.size();++i)
    {
      int ret=SmtModelUtils::printPhrModel(phraseModelsInfo.invPbModelPtrVec[i],phraseModelsInfo.modelDescEntryVec[i].absolutizedModelFileName);
      if(ret==ERROR)
        return ERROR;
    }

    for(unsigned int i=0;i<swModelsInfo.swAligModelPtrVec.size();++i)
    {
      int ret=SmtModelUtils::printDirectSwModel(swModelsInfo.swAligModelPtrVec[i],tmFileName);
      if(ret==ERROR)
        return ERROR;      
    }

    for(unsigned int i=0;i<swModelsInfo.invSwAligModelPtrVec.size();++i)
    {
      int ret=SmtModelUtils::printDirectSwModel(swModelsInfo.invSwAligModelPtrVec[i],tmFileName);
      if(ret==ERROR)
        return ERROR;      
    }

    for(unsigned int i=0;i<swModelsInfo.featNameVec.size();++i)
    {
      std::string featName=swModelsInfo.featNameVec[i];
      std::string invFeatName=swModelsInfo.invFeatNameVec[i];
      std::string modelFileName=phraseModelsInfo.modelDescEntryVec[i].absolutizedModelFileName;
      int ret=printLambdas(modelFileName,featName,invFeatName,verbose);
      if(ret==ERROR)
        return ERROR;      
    }
        
    return OK;
  }
  else
  {
    int ret=SmtModelUtils::printPhrModel(phraseModelsInfo.invPbModelPtrVec[0],tmFileName);
    if(ret==ERROR)
      return ERROR;
    
    ret=SmtModelUtils::printDirectSwModel(swModelsInfo.swAligModelPtrVec[0],tmFileName);
    if(ret==ERROR)
      return ERROR;
        
    ret=SmtModelUtils::printInverseSwModel(swModelsInfo.invSwAligModelPtrVec[0],tmFileName);
    if(ret==ERROR)
      return ERROR;

    std::string featName=swModelsInfo.featNameVec[0];
    std::string invFeatName=swModelsInfo.invFeatNameVec[0];    
    return printLambdas(tmFileName,featName,invFeatName,verbose);
  }
}

//--------------------------
bool FeatureHandler::printLangModels(std::string lmFileName,
                                     int /*verbose=0*/)
{
  std::string mainFileName;
  if(fileIsDescriptor(lmFileName,mainFileName))
  {
    for(unsigned int i=0;i<langModelsInfo.lModelPtrVec.size();++i)
    {
      int ret=SmtModelUtils::printLangModel(langModelsInfo.lModelPtrVec[i],langModelsInfo.modelDescEntryVec[i].absolutizedModelFileName);
      if(ret==ERROR)
        return ERROR;
    }
    return OK;
  }
  else
  {
    return SmtModelUtils::printLangModel(langModelsInfo.lModelPtrVec[0],lmFileName);
  }
}

//--------------------------
void FeatureHandler::clear(void)
{
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
