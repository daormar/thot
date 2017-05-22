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
 
#ifndef _DynClassFactoryHandler
#define _DynClassFactoryHandler

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "WgUncoupledAssistedTrans.h"
#include "BaseAssistedTrans.h"
#include "BaseStackDecoder.h"
#include "BaseTranslationConstraints.h"
#include "BaseLogLinWeightUpdater.h"
#include "BaseScorer.h"
#include "BaseWgProcessorForAnlp.h"
#include "BaseEcModelForNbUcat.h"
#include "BaseErrorCorrectionModel.h"
#include "BasePhraseModel.h"
#include "BaseSwAligModel.h"
#include "BaseNgramLM.h"
#include "BaseWordPenaltyModel.h"
#include THOT_SMTMODEL_H // Define SmtModel type. It is set in
                              // configure by checking SMTMODEL_H
                              // variable (default value: SmtModel.h)
#include THOT_PPINFO_H // Define PpInfo type. It is set in
                            // configure by checking PPINFO_H variable
                            // (default value: PpInfo.h)
#include THOT_LM_STATE_H // Define LM_State type. It is set in
                              // configure by checking LM_STATE_H
                              // variable (default value: LM_State.h)

#include "DynClassFileHandler.h"
#include "SimpleDynClassLoader.h"
#include "ErrorDefs.h"

using namespace std;

//--------------- Structs --------------------------------------------

struct DynClassFactoryHandler
{
      // Data

      // Dynamic class file handler
  DynClassFileHandler dynClassFileHandler;
  
      // Loaders for the different classes
  SimpleDynClassLoader<BaseWordPenaltyModel> baseWordPenaltyModelDynClassLoader;
  std::string baseWordPenaltyModelInitPars;

  SimpleDynClassLoader<BaseNgramLM<LM_State> > baseNgramLMDynClassLoader;
  std::string baseNgramLMSoFileName;
  std::string baseNgramLMInitPars;

  SimpleDynClassLoader<BaseSwAligModel<PpInfo> > baseSwAligModelDynClassLoader;
  std::string baseSwAligModelInitPars;

  SimpleDynClassLoader<BasePhraseModel> basePhraseModelDynClassLoader;
  std::string basePhraseModelSoFileName;
  std::string basePhraseModelInitPars;

  SimpleDynClassLoader<BaseErrorCorrectionModel> baseErrorCorrectionModelDynClassLoader;
  std::string baseErrorCorrectionModelInitPars;

  SimpleDynClassLoader<BaseEcModelForNbUcat> baseEcModelForNbUcatDynClassLoader;
  std::string baseEcModelForNbUcatInitPars;
  
  SimpleDynClassLoader<BaseWgProcessorForAnlp> baseWgProcessorForAnlpDynClassLoader;
  std::string baseWgProcessorForAnlpInitPars;

  SimpleDynClassLoader<BaseScorer> baseScorerDynClassLoader;
  std::string baseScorerInitPars;

  SimpleDynClassLoader<BaseLogLinWeightUpdater> baseLogLinWeightUpdaterDynClassLoader;
  std::string baseLogLinWeightUpdaterInitPars;

  SimpleDynClassLoader<BaseTranslationConstraints> baseTranslationConstraintsDynClassLoader;
  std::string baseTranslationConstraintsInitPars;

  SimpleDynClassLoader<BaseStackDecoder<SmtModel> > baseStackDecoderDynClassLoader;
  std::string baseStackDecoderInitPars;

  SimpleDynClassLoader<BaseAssistedTrans<SmtModel> > baseAssistedTransDynClassLoader;
  std::string baseAssistedTransInitPars;
  
      // Functions
  DynClassFactoryHandler();
  int init_smt(std::string fileName,
               int verbose=1);
  void release_smt(int verbose=1);

  int init_smt_and_imt(std::string fileName,
                       int verbose=1);
  void release_smt_and_imt(int verbose=1);
};

#endif
