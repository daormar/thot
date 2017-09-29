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
 
#ifndef _ThotDecoderPerUserVars_h
#define _ThotDecoderPerUserVars_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseTranslationConstraints.h"
#include "CatDefs.h"
#include "_nbUncoupledAssistedTrans.h"
#include "WgUncoupledAssistedTrans.h"
#include "BaseAssistedTrans.h"
#include "_stackDecoderRec.h"
#include "BaseStackDecoder.h"
#include THOT_SMTMODEL_H // Define SmtModel type. It is set in
                              // configure by checking SMTMODEL_H
                              // variable (default value: SmtModel.h)
#include "BasePrePosProcessor.h"

//--------------- Classes --------------------------------------------

class ThotDecoderPerUserVars
{
 public:
  BasePrePosProcessor* prePosProcessorPtr;
  BasePbTransModel<SmtModel::Hypothesis>* smtModelPtr;
  BaseStackDecoder<SmtModel>* stackDecoderPtr;
  _stackDecoderRec<SmtModel>* stackDecoderRecPtr;
  BaseEcModelForNbUcat* ecModelForNbUcatPtr;
  BaseAssistedTrans<SmtModel>* assistedTransPtr;
  _nbUncoupledAssistedTrans<SmtModel>* _nbUncoupledAssistedTransPtr;
  WgUncoupledAssistedTrans<SmtModel>* wgUncoupledAssistedTransPtr;
  BaseWgProcessorForAnlp* wgpPtr;
  BaseTranslationConstraints* trConstraintsPtr;
};

#endif
