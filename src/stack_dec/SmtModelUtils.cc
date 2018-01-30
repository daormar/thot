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
 * @file SmtModelUtils.cc
 * 
 * @brief Definitions file for SmtModelUtils.h
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
    if(ret==THOT_ERROR) return THOT_ERROR;
    
    ret=basePhraseModelPtr->loadTrgVocab(trgTrainVocabFileName.c_str());
    if(ret==THOT_ERROR) return THOT_ERROR;
    
        // load phrase model
    if(basePhraseModelPtr->load(modelFileName.c_str())!=0)
    {
      std::cerr<<"Error while reading phrase model file\n";
      return THOT_ERROR;
    }  
    
    return THOT_OK;
  }

  //--------------------------
  int printPhrModel(BasePhraseModel* basePhraseModelPtr,
                    std::string modelFileName)
  {
    bool ret=basePhraseModelPtr->print(modelFileName.c_str());
    if(ret==THOT_ERROR) return THOT_ERROR;

    return THOT_OK;
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
    if(ret==THOT_ERROR) return THOT_ERROR;
    
    return THOT_OK;
  }

  //--------------------------
  int printDirectSwModel(BaseSwAligModel<PpInfo>* baseSwAligModelPtr,
                         std::string modelFileName)
  {
    std::string invSwModelPrefix=modelFileName+"_invswm";
    int ret=baseSwAligModelPtr->print(invSwModelPrefix.c_str());
    if(ret==THOT_ERROR) return THOT_ERROR;

    return THOT_OK;
  }

  //--------------------------
  int loadInverseSwModel(BaseSwAligModel<PpInfo>* baseSwAligModelPtr,
                         std::string modelFileName)
  {
        // load inverse sw model (The inverse model is the one with the prefix
        // _swm)
    std::string invReadTablePrefix=modelFileName;
    invReadTablePrefix+="_swm";
    bool ret=baseSwAligModelPtr->load(invReadTablePrefix.c_str());
    if(ret==THOT_ERROR) return THOT_ERROR;
    
    return THOT_OK;
  }

  //--------------------------
  int printInverseSwModel(BaseSwAligModel<PpInfo>* baseSwAligModelPtr,
                          std::string modelFileName)
  {
    std::string invSwModelPrefix=modelFileName+"_swm";
    int ret=baseSwAligModelPtr->print(invSwModelPrefix.c_str());
    if(ret==THOT_ERROR) return THOT_ERROR;

    return THOT_OK;
  }

  //--------------------------
  int loadLangModel(BaseNgramLM<LM_State>* baseNgLmPtr,
                    std::string modelFileName)
  {
    if(baseNgLmPtr->load(modelFileName.c_str())==THOT_ERROR)
      return THOT_ERROR;
    else
      return THOT_OK;
  }

  //---------------------------------
  int printLangModel(BaseNgramLM<LM_State>* baseNgLmPtr,
                     std::string modelFileName)
  {
    bool ret=baseNgLmPtr->print(modelFileName.c_str());
    if(ret==THOT_ERROR) return THOT_ERROR;
    
    return THOT_OK;
  }

  //---------------------------------
  bool loadSwmLambdas(std::string lambdaFileName,
                      float& lambda_swm,
                      float& lambda_invswm)
  {
    AwkInputStream awk;
  
    if(awk.open(lambdaFileName.c_str())==THOT_ERROR)
    {
      std::cerr<<"Error in file containing the lambda value, file "<<lambdaFileName<<" does not exist."<<std::endl;
      return THOT_ERROR;
    }
    else
    {
      if(awk.getln())
      {
        if(awk.NF==1)
        {
          lambda_swm=atof(awk.dollar(1).c_str());
          lambda_invswm=atof(awk.dollar(1).c_str());
          return THOT_OK;
        }
        else
        {
          if(awk.NF==2)
          {
            lambda_swm=atof(awk.dollar(1).c_str());
            lambda_invswm=atof(awk.dollar(2).c_str());
            return THOT_OK;
          }
          else
          {
            std::cerr<<"Anomalous file with lambda values."<<std::endl;
            return THOT_ERROR;
          }
        }
      }
      else
      {
        std::cerr<<"Anomalous file with lambda values."<<std::endl;
        return THOT_ERROR;
      }
    }  
    return THOT_OK;
  }

  //---------------------------------
  bool printSwmLambdas(const char* lambdaFileName,
                       float lambda_swm,
                       float lambda_invswm)
  {
    std::ofstream outF;

    outF.open(lambdaFileName,std::ios::out);
    if(!outF)
    {
      std::cerr<<"Error while printing file with lambda values."<<std::endl;
      return THOT_ERROR;
    }
    else
    {
      outF<<lambda_swm<<" "<<lambda_invswm<<std::endl;
      outF.close();	
      return THOT_OK;
    }
  }

}
