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
 
#ifndef _ThotDecoderCommonVars_h
#define _ThotDecoderCommonVars_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseTranslationMetadata.h"
#include "OnlineTrainingPars.h"
#include "FeatureHandler.h"
#include "BaseLogLinWeightUpdater.h"
#include "BaseScorer.h"
#include "BasePbTransModel.h"
#include "BaseErrorCorrectionModel.h"
#include "SwModelInfo.h"
#include "PhraseModelInfo.h"
#include "LangModelInfo.h"
#include "BaseWordPenaltyModel.h"
#include "WgHandler.h"
#include "DynClassFactoryHandler.h"
#include THOT_SMTMODEL_H // Define SmtModel type. It is set in
                         // configure by checking SMTMODEL_H
                         // variable (default value: SmtModel.h)

//--------------- Classes --------------------------------------------

class ThotDecoderCommonVars
{
 public:

      // Variables related to decoder elements
  SwModelInfo* swModelInfoPtr;
  PhraseModelInfo* phrModelInfoPtr;
  LangModelInfo* langModelInfoPtr;
  WgHandler* wgHandlerPtr;
  BaseErrorCorrectionModel* ecModelPtr;
  bool curr_ecm_valid_for_wg;
  BaseScorer* scorerPtr;
  BaseLogLinWeightUpdater* llWeightUpdaterPtr;

      // Auxiliary decoder variables
  BasePbTransModel<SmtModel::Hypothesis>* smtModelPtr;
  BaseTranslationMetadata<SmtModel::HypScoreInfo>* trMetadataPtr;

      // Variables related to feature-based implementation
  bool featureBasedImplEnabled;
  FeatureHandler featureHandler;

      // Handler of dynamic classes
  DynClassFactoryHandler dynClassFactoryHandler;

      // Parameters for online training
  OnlineTrainingPars onlineTrainingPars;
};

#endif
