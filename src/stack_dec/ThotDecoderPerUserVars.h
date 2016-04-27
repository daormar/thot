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

#include "CatDefs.h"
#include "_nbUncoupledAssistedTrans.h"
#include "WgUncoupledAssistedTrans.h"
#include "BaseAssistedTrans.h"
#include "_stackDecoderRec.h"
#include "BaseStackDecoder.h"
#include "SmtModelTypes.h"
#include "BasePrePosProcessor.h"

using namespace std;

//--------------- Classes --------------------------------------------

class ThotDecoderPerUserVars
{
 public:
  BasePrePosProcessor* prePosProcessorPtr;
  BasePbTransModel<CURR_MODEL_TYPE::Hypothesis>* smtModelPtr;
  BaseStackDecoder<CURR_MODEL_TYPE>* stackDecoderPtr;
  _stackDecoderRec<CURR_MODEL_TYPE>* stackDecoderRecPtr;
  BaseEcModelForNbUcat* ecModelForNbUcatPtr;
  BaseAssistedTrans<CURR_MODEL_TYPE>* assistedTransPtr;
  _nbUncoupledAssistedTrans<CURR_MODEL_TYPE>* _nbUncoupledAssistedTransPtr;
  WgUncoupledAssistedTrans<CURR_MODEL_TYPE>* wgUncoupledAssistedTransPtr;
  BaseWgProcessorForAnlp* wgpPtr;
};

#endif
