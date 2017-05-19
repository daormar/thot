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
 

#include "SmtModelUtils.h"

namespace SmtModelUtils
{

  //--------------------------
  int loadPhrModel(BasePhraseModel* basePhraseModelPtr,
                   std::string modelFileName)
  {
        // Load phrase model vocabularies 
    std::string srcTrainVocabFileName=modelFileName;
    srcTrainVocabFileName+="_swm.svcb";
    std::string trgTrainVocabFileName=modelFileName;
    trgTrainVocabFileName+="_swm.tvcb";
    
    int ret=basePhraseModelPtr->loadSrcVocab(srcTrainVocabFileName.c_str());
    if(ret==ERROR) return ERROR;
    
    ret=basePhraseModelPtr->loadTrgVocab(trgTrainVocabFileName.c_str());
    if(ret==ERROR) return ERROR;
    
        // load phrase model
    if(basePhraseModelPtr->load(modelFileName.c_str())!=0)
    {
      cerr<<"Error while reading phrase model file\n";
      return ERROR;
    }  
    
    return OK;
  }
  
  //--------------------------
  int loadDirectSwModel(BaseSwAligModel<PpInfo>* baseSwAligModelPtr,
                        std::string modelFileName)
  {
        // load sw model (The direct model is the one with the prefix
        // _invswm)
    std::string invReadTablePrefix=modelFileName;
    invReadTablePrefix+="_invswm";
    bool ret=baseSwAligModelPtr->load(invReadTablePrefix.c_str());
    if(ret==ERROR) return ERROR;
    
    return OK;
  }

  //--------------------------
  int loadLangModel(BaseNgramLM<LM_State>* baseNgLmPtr,
                    std::string modelFileName)
  {
    if(baseNgLmPtr->load(modelFileName.c_str())==ERROR)
      return ERROR;
    else
      return OK;
  }

}
