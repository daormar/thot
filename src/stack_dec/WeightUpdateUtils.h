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
 * @file WeightUpdateUtils.h
 * @brief Defines string processing utilities
 */

#ifndef _WeightUpdateUtils_h
#define _WeightUpdateUtils_h

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

extern "C" {
#include "step_by_step_dhs.h"
}

#include "InversePhraseModelFeat.h"
#include "DirectPhraseModelFeat.h"
#include "PhraseExtractUtils.h"
#include "WordGraph.h"
#include THOT_SMTMODEL_H // Define SmtModel type. It is set in
                         // configure by checking SMTMODEL_H
                         // variable (default value: SmtModel.h)
#include THOT_PPINFO_H // Define PpInfo type. It is set in
                       // configure by checking PPINFO_H variable
                       // (default value: PpInfo.h)
#include THOT_LM_STATE_H // Define LM_State type. It is set in
                         // configure by checking LM_STATE_H
                         // variable (default value: LM_State.h)
#include "BaseSwAligModel.h"
#include "BasePhraseModel.h"
#include "BaseNgramLM.h"
#include "PhrasePair.h"
#include "BaseLogLinWeightUpdater.h"
#include <stdio.h>
#include "myVector.h"
#include <string>

//--------------- Constants ------------------------------------------

#define NBLIST_SIZE_FOR_LLWEIGHT_UPDATE 1000
#define PHRSWLITM_DHS_FTOL                 0.001
#define PHRSWLITM_DHS_SCALE_PAR            1

namespace WeightUpdateUtils
{
  void updateLogLinearWeights(std::string refSent,
                              WordGraph* wgPtr,
                              BaseLogLinWeightUpdater* llWeightUpdaterPtr,
                              const Vector<pair<std::string,float> >& compWeights,
                              Vector<float>& newWeights,
                              int verbose=0);
  int updateLinInterpWeights(std::string srcCorpusFileName,
                             std::string trgCorpusFileName,
                             DirectPhraseModelFeat<SmtModel::HypScoreInfo>* dirPhrModelFeatPtr,
                             InversePhraseModelFeat<SmtModel::HypScoreInfo>* invPhrModelFeatPtr,
                             int verbose=0);  
}

#endif
