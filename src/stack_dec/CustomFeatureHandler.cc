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
 * @file CustomFeatureHandler.cc
 * 
 * @brief Definitions file for CustomFeatureHandler.h
 */

//--------------- Include files --------------------------------------

#include "CustomFeatureHandler.h"

//--------------- CustomFeatureHandler class functions

CustomFeatureHandler::CustomFeatureHandler()
{  
}

//---------------
int CustomFeatureHandler::addCustomFeats(std::string customFeatDescFile,
                                         int verbose)
{
  if(customFeatDescFile==NONE_DESCRIPTOR)
  {
    return THOT_OK;
  }
  
  if(verbose)
  {
    std::cerr<<"Processing custom feature descriptor: "<<customFeatDescFile<<std::endl;
  }

      // Obtain info about custom feature entries
  std::vector<ModelDescriptorEntry> modelDescEntryVec;
  if(extractModelEntryInfo(customFeatDescFile,modelDescEntryVec)==THOT_OK)
  {
        // Process descriptor entries
    for(unsigned int i=0;i<modelDescEntryVec.size();++i)
    {
      std::string featName="custom_"+modelDescEntryVec[i].statusStr;
      BasePbTransModelFeature<SmtModel::HypScoreInfo>* featPtr;
      int ret=createCustomFeat(featName,modelDescEntryVec[i],&featPtr);
      if(ret==THOT_ERROR)
        return THOT_ERROR;
      featuresInfo.featPtrVec.push_back(featPtr);
    }
    return THOT_OK;
  }
  else
  {
    return THOT_ERROR;
  }
}

//---------------
int CustomFeatureHandler::createCustomFeat(std::string featName,
                                           const ModelDescriptorEntry& modelDescEntry,
                                           BasePbTransModelFeature<SmtModel::HypScoreInfo>** featPtrRef)
{
   std::cerr<<"** Creating custom feature ("<<featName<<" "<<modelDescEntry.modelInitInfo<<" "<<modelDescEntry.absolutizedModelFileName<<")"<<std::endl;

      // Display warning if so file is external
  if(soFileIsExternal(modelDescEntry.modelInitInfo))
  {
    std::cerr<<"Warning: so file ("<<modelDescEntry.modelInitInfo<<") is external to Thot package, to avoid execution problems ensure that the external file was compiled for the current version of the package"<<std::endl;
  }

      // Create feature pointer and set name
  (*featPtrRef)=createFeatPtr(modelDescEntry.modelInitInfo,modelDescEntry.absolutizedModelFileName);
  BasePbTransModelFeature<SmtModel::HypScoreInfo>* featPtr=*featPtrRef;
  if(featPtr==NULL)
    return THOT_ERROR;

      // Set feature name
  featPtr->setFeatName(featName);
  
  return THOT_OK; 
}

//--------------------------
BasePbTransModelFeature<SmtModel::HypScoreInfo>* CustomFeatureHandler::createFeatPtr(std::string soFileName,
                                                                                     std::string modelFileName)
{
      // Declare dynamic class loader instance
  SimpleDynClassLoader<BasePbTransModelFeature<SmtModel::HypScoreInfo> > simpleDynClassLoader;
  
      // Open module
  bool verbosity=false;
  if(!simpleDynClassLoader.open_module(soFileName,verbosity))
  {
    std::cerr<<"Error: so file ("<<soFileName<<") could not be opened"<<std::endl;
    return NULL;
  }

      // Create model feature pointer
  BasePbTransModelFeature<SmtModel::HypScoreInfo>* featPtr=simpleDynClassLoader.make_obj(modelFileName);

  if(featPtr==NULL)
  {
    std::cerr<<"Error: BasePbTransModelFeature pointer could not be instantiated"<<std::endl;    
    return NULL;
  }

      // Store loader
  simpleDynClassLoaderVec.push_back(simpleDynClassLoader);
  
  return featPtr;  
}

//---------------
FeaturesInfo<SmtModel::HypScoreInfo>* CustomFeatureHandler::getFeatureInfoPtr(void)
{
  return &featuresInfo;
}

//--------------------------
void CustomFeatureHandler::clear(void)
{
      // Delete feature pointers
  for(unsigned int i=0;i<featuresInfo.featPtrVec.size();++i)
  {
    delete featuresInfo.featPtrVec[i];
  }
  featuresInfo.featPtrVec.clear();

      // Clear class loaders
  simpleDynClassLoaderVec.clear();
}

//--------------------------
CustomFeatureHandler::~CustomFeatureHandler()
{
  clear();
}
