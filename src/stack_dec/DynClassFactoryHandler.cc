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
 * @file DynClassFactoryHandler.cc
 * 
 * @brief Definitions file for DynClassFactoryHandler.h
 */

//--------------- Include files --------------------------------------

#include "DynClassFactoryHandler.h"

//--------------- DynClassFactoryHandler struct functions

//--------------------------
DynClassFactoryHandler::DynClassFactoryHandler()
{
}

//--------------------------
int DynClassFactoryHandler::init_smt(std::string fileName,
                                     int verbose/*=1*/)
{
      // Release data structure
  release_smt();

      // Initialize smt dynamic classes...
  
  if(dynClassFileHandler.load(fileName,verbose)==THOT_ERROR)
  {
    std::cerr<<"Error while loading ini file"<<std::endl;
    return THOT_ERROR;
  }
      // Define variables to obtain base class infomation
  std::string baseClassName;
  std::string soFileName;
  std::string initPars;

      ///////////// Obtain info for BaseWordPenaltyModel class
  baseClassName="BaseWordPenaltyModel";
  if(dynClassFileHandler.getInfoForBaseClass(baseClassName,soFileName,initPars)==THOT_ERROR)
  {
    std::cerr<<"Error: ini file does not contain information about "<<baseClassName<<" class"<<std::endl;
    std::cerr<<"Please check content of master.ini file or execute \"thot_handle_ini_files -r\" to reset it"<<std::endl;
    return THOT_ERROR;
  }
      // Load class derived from BaseWordPenaltyModel dynamically
  if(!baseWordPenaltyModelDynClassLoader.open_module(soFileName,verbose))
  {
    std::cerr<<"Error: so file ("<<soFileName<<") could not be opened"<<std::endl;
    return THOT_ERROR;
  }
      // Store so file name
  baseWordPenaltyModelSoFileName=soFileName;
    
      // Store init parameters for BaseWordPenaltyModel
  baseWordPenaltyModelInitPars=initPars;

      ///////////// Obtain info for BaseNgramLM class
  baseClassName="BaseNgramLM";
  if(dynClassFileHandler.getInfoForBaseClass(baseClassName,soFileName,initPars)==THOT_ERROR)
  {
    std::cerr<<"Error: ini file does not contain information about "<<baseClassName<<" class"<<std::endl;
    std::cerr<<"Please check content of master.ini file or execute \"thot_handle_ini_files -r\" to reset it"<<std::endl;
    return THOT_ERROR;
  }   
      // Load class derived from BaseNgramLM dynamically
  if(!baseNgramLMDynClassLoader.open_module(soFileName,verbose))
  {
    std::cerr<<"Error: so file ("<<soFileName<<") could not be opened"<<std::endl;
    return THOT_ERROR;
  }
      // Store so file name
  baseNgramLMSoFileName=soFileName;
  
      // Store init parameters for BaseNgramLM
  baseNgramLMInitPars=initPars;

      ///////////// Obtain info for BaseSwAligModel class
  baseClassName="BaseSwAligModel";
  if(dynClassFileHandler.getInfoForBaseClass(baseClassName,soFileName,initPars)==THOT_ERROR)
  {
    std::cerr<<"Error: ini file does not contain information about "<<baseClassName<<" class"<<std::endl;
    std::cerr<<"Please check content of master.ini file or execute \"thot_handle_ini_files -r\" to reset it"<<std::endl;
    return THOT_ERROR;
  }   
      // Load class derived from BaseSwAligModel dynamically
  if(!baseSwAligModelDynClassLoader.open_module(soFileName,verbose))
  {
    std::cerr<<"Error: so file ("<<soFileName<<") could not be opened"<<std::endl;
    return THOT_ERROR;
  }

      // Store so file name
  baseSwAligModelSoFileName=soFileName;

      // Store init parameters for BaseSwAligModel
  baseSwAligModelInitPars=initPars;

      ///////////// Obtain info for BasePhraseModel class
  baseClassName="BasePhraseModel";
  if(dynClassFileHandler.getInfoForBaseClass(baseClassName,soFileName,initPars)==THOT_ERROR)
  {
    std::cerr<<"Error: ini file does not contain information about "<<baseClassName<<" class"<<std::endl;
    std::cerr<<"Please check content of master.ini file or execute \"thot_handle_ini_files -r\" to reset it"<<std::endl;
    return THOT_ERROR;
  }   
      // Load class derived from BasePhraseModel dynamically
  if(!basePhraseModelDynClassLoader.open_module(soFileName,verbose))
  {
    std::cerr<<"Error: so file ("<<soFileName<<") could not be opened"<<std::endl;
    return THOT_ERROR;
  }
      // Store so file name
  basePhraseModelSoFileName=soFileName;
  
      // Store init parameters for BasePhraseModel
  basePhraseModelInitPars=initPars;

      ///////////// Obtain info for BaseScorer class
  baseClassName="BaseScorer";
  if(dynClassFileHandler.getInfoForBaseClass(baseClassName,soFileName,initPars)==THOT_ERROR)
  {
    std::cerr<<"Error: ini file does not contain information about "<<baseClassName<<" class"<<std::endl;
    std::cerr<<"Please check content of master.ini file or execute \"thot_handle_ini_files -r\" to reset it"<<std::endl;
    return THOT_ERROR;
  }   
      // Load class derived from BaseScorer dynamically
  if(!baseScorerDynClassLoader.open_module(soFileName,verbose))
  {
    std::cerr<<"Error: so file ("<<soFileName<<") could not be opened"<<std::endl;
    return THOT_ERROR;
  }     

      ///////////// Obtain info for BaseLogLinWeightUpdater class
  baseClassName="BaseLogLinWeightUpdater";
  if(dynClassFileHandler.getInfoForBaseClass(baseClassName,soFileName,initPars)==THOT_ERROR)
  {
    std::cerr<<"Error: ini file does not contain information about "<<baseClassName<<" class"<<std::endl;
    std::cerr<<"Please check content of master.ini file or execute \"thot_handle_ini_files -r\" to reset it"<<std::endl;
    return THOT_ERROR;
  }   
      // Load class derived from BaseLogLinWeightUpdater dynamically
  if(!baseLogLinWeightUpdaterDynClassLoader.open_module(soFileName,verbose))
  {
    std::cerr<<"Error: so file ("<<soFileName<<") could not be opened"<<std::endl;
    return THOT_ERROR;
  }     
      // Store init parameters for BaseLogLinWeightUpdater
  baseLogLinWeightUpdaterInitPars=initPars;

      ///////////// Obtain info for BaseTranslationMetadata class
  baseClassName="BaseTranslationMetadata";
  if(dynClassFileHandler.getInfoForBaseClass(baseClassName,soFileName,initPars)==THOT_ERROR)
  {
    std::cerr<<"Error: ini file does not contain information about "<<baseClassName<<" class"<<std::endl;
    std::cerr<<"Please check content of master.ini file or execute \"thot_handle_ini_files -r\" to reset it"<<std::endl;
    return THOT_ERROR;
  }   
      // Load class derived from BaseTranslationMetadata dynamically
  if(!baseTranslationMetadataDynClassLoader.open_module(soFileName,verbose))
  {
    std::cerr<<"Error: so file ("<<soFileName<<") could not be opened"<<std::endl;
    return THOT_ERROR;
  }     
      // Store init parameters for BaseTranslationMetadata
  baseTranslationMetadataInitPars=initPars;

      ///////////// Obtain info for BaseStackDecoder class
  baseClassName="BaseStackDecoder";
  if(dynClassFileHandler.getInfoForBaseClass(baseClassName,soFileName,initPars)==THOT_ERROR)
  {
    std::cerr<<"Error: ini file does not contain information about "<<baseClassName<<" class"<<std::endl;
    std::cerr<<"Please check content of master.ini file or execute \"thot_handle_ini_files -r\" to reset it"<<std::endl;
    return THOT_ERROR;
  }   
      // Load class derived from BaseStackDecoder dynamically
  if(!baseStackDecoderDynClassLoader.open_module(soFileName,verbose))
  {
    std::cerr<<"Error: so file ("<<soFileName<<") could not be opened"<<std::endl;
    return THOT_ERROR;
  }     
      // Store init parameters for BaseStackDecoder
  baseStackDecoderInitPars=initPars;

  return THOT_OK;
}

//--------------------------
void DynClassFactoryHandler::release_smt(int verbose/*=1*/)
{
      // Close smt modules
  baseWordPenaltyModelDynClassLoader.close_module(verbose);
  baseNgramLMDynClassLoader.close_module(verbose);
  baseSwAligModelDynClassLoader.close_module(verbose);
  basePhraseModelDynClassLoader.close_module(verbose);
  baseScorerDynClassLoader.close_module(verbose);
  baseLogLinWeightUpdaterDynClassLoader.close_module(verbose);
  baseTranslationMetadataDynClassLoader.close_module(verbose);
  baseStackDecoderDynClassLoader.close_module(verbose);
}

//--------------------------
int DynClassFactoryHandler::init_smt_and_imt(std::string fileName,
                                             int verbose/*=1*/)
{
      // Initialize smt modules
  int ret=init_smt(fileName,verbose);
  if(ret==THOT_ERROR)
    return THOT_ERROR;

      // Define variables to obtain base class infomation
  std::string baseClassName;
  std::string soFileName;
  std::string initPars;

        ///////////// Obtain info for BaseErrorCorrectionModel class
  baseClassName="BaseErrorCorrectionModel";
  if(dynClassFileHandler.getInfoForBaseClass(baseClassName,soFileName,initPars)==THOT_ERROR)
  {
    std::cerr<<"Error: ini file does not contain information about "<<baseClassName<<" class"<<std::endl;
    std::cerr<<"Please check content of master.ini file or execute \"thot_handle_ini_files -r\" to reset it"<<std::endl;
    return THOT_ERROR;
  }   
      // Load class derived from BaseErrorCorrectionModel dynamically
  if(!baseErrorCorrectionModelDynClassLoader.open_module(soFileName,verbose))
  {
    std::cerr<<"Error: so file ("<<soFileName<<") could not be opened"<<std::endl;
    return THOT_ERROR;
  }     
      // Store init parameters for BaseErrorCorrectionModel
  baseErrorCorrectionModelInitPars=initPars;

      ///////////// Obtain info for BaseEcModelForNbUcat class
  baseClassName="BaseEcModelForNbUcat";
  if(dynClassFileHandler.getInfoForBaseClass(baseClassName,soFileName,initPars)==THOT_ERROR)
  {
    std::cerr<<"Error: ini file does not contain information about "<<baseClassName<<" class"<<std::endl;
    std::cerr<<"Please check content of master.ini file or execute \"thot_handle_ini_files -r\" to reset it"<<std::endl;
    return THOT_ERROR;
  }   
      // Load class derived from BaseEcModelForNbUcat dynamically
  if(!baseEcModelForNbUcatDynClassLoader.open_module(soFileName,verbose))
  {
    std::cerr<<"Error: so file ("<<soFileName<<") could not be opened"<<std::endl;
    return THOT_ERROR;
  }     
      // Store init parameters for BaseEcModelForNbUcat
  baseEcModelForNbUcatInitPars=initPars;

      ///////////// Obtain info for BaseWgProcessorForAnlp class
  baseClassName="BaseWgProcessorForAnlp";
  if(dynClassFileHandler.getInfoForBaseClass(baseClassName,soFileName,initPars)==THOT_ERROR)
  {
    std::cerr<<"Error: ini file does not contain information about "<<baseClassName<<" class"<<std::endl;
    std::cerr<<"Please check content of master.ini file or execute \"thot_handle_ini_files -r\" to reset it"<<std::endl;
    return THOT_ERROR;
  }   
      // Load class derived from BaseWgProcessorForAnlp dynamically
  if(!baseWgProcessorForAnlpDynClassLoader.open_module(soFileName,verbose))
  {
    std::cerr<<"Error: so file ("<<soFileName<<") could not be opened"<<std::endl;
    return THOT_ERROR;
  }     
      // Store init parameters for BaseWgProcessorForAnlp
  baseWgProcessorForAnlpInitPars=initPars;

      ///////////// Obtain info for BaseAssistedTrans class
  baseClassName="BaseAssistedTrans";
  if(dynClassFileHandler.getInfoForBaseClass(baseClassName,soFileName,initPars)==THOT_ERROR)
  {
    std::cerr<<"Error: ini file does not contain information about "<<baseClassName<<" class"<<std::endl;
    std::cerr<<"Please check content of master.ini file or execute \"thot_handle_ini_files -r\" to reset it"<<std::endl;
    return THOT_ERROR;
  }   
      // Load class derived from BaseAssistedTrans dynamically
  if(!baseAssistedTransDynClassLoader.open_module(soFileName,verbose))
  {
    std::cerr<<"Error: so file ("<<soFileName<<") could not be opened"<<std::endl;
    return THOT_ERROR;
  }     
      // Store init parameters for BaseAssistedTrans
  baseAssistedTransInitPars=initPars;

  return THOT_OK;
}

//--------------------------
void DynClassFactoryHandler::release_smt_and_imt(int verbose/*=1*/)
{
      // Close smt modules
  release_smt(verbose);

      // Close imt modules
  baseErrorCorrectionModelDynClassLoader.close_module(verbose);
  baseEcModelForNbUcatDynClassLoader.close_module(verbose);
  baseWgProcessorForAnlpDynClassLoader.close_module(verbose);
  baseAssistedTransDynClassLoader.close_module(verbose);
}
