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
 
#ifndef _LangModelsInfo_h
#define _LangModelsInfo_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include THOT_LM_STATE_H // Define LM_State type. It is set in
                         // configure by checking LM_STATE_H
                         // variable (default value: LM_State.h)
#include "ModelDescriptorUtils.h"
#include "BaseNgramLM.h"
#include "BaseWordPenaltyModel.h"
#include "WordIndex.h"
#include "myVector.h"
#include "SimpleDynClassLoader.h"

//--------------- LangModelsInfo struct

struct LangModelsInfo
{
  Vector<BaseNgramLM<LM_State>*> lModelPtrVec;
  Vector<ModelDescriptorEntry> modelDescEntryVec;
  Vector<SimpleDynClassLoader<BaseNgramLM<LM_State> > > simpleDynClassLoaderVec;
  BaseWordPenaltyModel* wpModelPtr;
};

#endif
