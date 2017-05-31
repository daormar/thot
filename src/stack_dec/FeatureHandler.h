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
 
#ifndef _FeatureHandler_h
#define _FeatureHandler_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include THOT_SMTMODEL_H // Define SmtModel type. It is set in
                         // configure by checking SMTMODEL_H
                         // variable (default value: SmtModel.h)
#include "SmtModelUtils.h"
#include "SrcPosJumpFeat.h"
#include "TrgPhraseLenFeat.h"
#include "SrcPhraseLenFeat.h"
#include "InversePhraseModelFeat.h"
#include "DirectPhraseModelFeat.h"
#include "LangModelFeat.h"
#include "WordPenaltyFeat.h"
#include "FeaturesInfo.h"
#include "SwModelsInfo.h"
#include "PhraseModelsInfo.h"
#include "LangModelsInfo.h"

//--------------- FeatureHandler class

class FeatureHandler
{
 public:

      // Functions to incorporate new features
  int addWpmFeat(int verbose);
  int loadLmFeats(std::string lmFileName,
                  int verbose);
  int loadTmFeats(std::string tmFilesPrefix,
                  int verbose);

      // Functions to print features
  bool print(std::string tmFileName,
             std::string lmFileName,
             int verbose=0);

      // Functions to specify default model types
  int setDefaultLangModelType(std::string modelType);
  int setDefaultTransModelType(std::string modelType);
  int setDefaultSingleWordModelType(std::string modelType);

      // Function to get pointer to feature information
  FeaturesInfo<SmtModel::HypScoreInfo>* getFeatureInfoPtr(void);
    
 private:

  std::string defaultLangModelType;
  std::string defaultTransModelType;
  std::string defaultSingleWordModelType;

  SwModelsInfo swModelsInfo;
  PhraseModelsInfo phraseModelsInfo;
  LangModelsInfo langModelsInfo;
  BaseWordPenaltyModel* wpModelPtr;
  FeaturesInfo<SmtModel::HypScoreInfo>* featuresInfoPtr;

      // Auxiliary functions

      // Language model-related functions
  BaseNgramLM<LM_State>* createLmPtr(std::string modelType);
  int createLangModelFeat(std::string featName,
                          const ModelDescriptorEntry& modelDescEntry,
                          LangModelFeat<SmtModel::HypScoreInfo>** langModelFeatPtrRef);
  bool process_lm_descriptor(std::string lmDescFile,
                             int verbose=0);
  bool process_lm_files_prefix(std::string lmFilesPrefix,
                               int verbose=0);

      // Phrase model-related functions
  BasePhraseModel* createPmPtr(std::string modelType);
  unsigned int getFeatureIdx(std::string featName);
  int createDirectPhrModelFeat(std::string featName,
                               const ModelDescriptorEntry& modelDescEntry,
                               DirectPhraseModelFeat<SmtModel::HypScoreInfo>** dirPmFeatPtrRef);
  int createInversePhrModelFeat(std::string featName,
                                const ModelDescriptorEntry& modelDescEntry,
                                BasePhraseModel* invPbModelPtr,
                                InversePhraseModelFeat<SmtModel::HypScoreInfo>** invPmFeatPtrRef);
  int createSrcPhraseLenFeat(std::string featName,
                             BasePhraseModel* basePhraseModelPtr,
                             SrcPhraseLenFeat<SmtModel::HypScoreInfo>** srcPhraseLenFeatRef);
  int createTrgPhraseLenFeat(std::string featName,
                             BasePhraseModel* basePhraseModelPtr,
                             TrgPhraseLenFeat<SmtModel::HypScoreInfo>** trgPhraseLenFeatRef);
  int createSrcPosJumpFeat(std::string featName,
                           BasePhraseModel* basePhraseModelPtr,
                           SrcPosJumpFeat<SmtModel::HypScoreInfo>** srcPosJumpFeatRef);
  bool process_tm_descriptor(std::string tmDescFile,
                             int verbose=0);
  bool process_tm_files_prefix(std::string tmFilesPrefix,
                               int verbose=0);

      // Auxiliary functions to print models
  bool printLambdas(std::string modelFileName,
                    std::string featName,
                    std::string invFeatName,
                    int verbose=0);
  bool printAligModels(std::string tmFileName,
                       int verbose=0);
  bool printLangModels(std::string lmFileName,
                       int verbose=0);
       
      // Memory management functions
  void deleteLangModelPtrs(void);
  void deletePhrModelPtrs(void);
  void deleteSwModelPtrs(void);
};
  
#endif
