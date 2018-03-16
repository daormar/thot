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
 * @file CustomFeatureHandler.h
 * 
 * @brief Class to handle custom log-linear model features.
 */

#ifndef _CustomFeatureHandler_h
#define _CustomFeatureHandler_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include THOT_SMTMODEL_H // Define SmtModel type. It is set in
                         // configure by checking SMTMODEL_H
                         // variable (default value: SmtModel.h)
#include "FeaturesInfo.h"
#include "SimpleDynClassLoader.h"
#include "ModelDescriptorUtils.h"
#include "StrProcUtils.h"

//--------------- CustomFeatureHandler class

class CustomFeatureHandler
{
 public:

      // Constructor
  CustomFeatureHandler();
  
      // Functions to incorporate features
  int addCustomFeats(std::string customFeatDescFile,
                     int verbose);

      // Function to get pointers to features
  FeaturesInfo<SmtModel::HypScoreInfo>* getFeatureInfoPtr(void);
  
      // Clear function
  void clear(void);

      // Destructor
  ~CustomFeatureHandler();
  
 private:

  FeaturesInfo<SmtModel::HypScoreInfo> featuresInfo;
  std::vector<SimpleDynClassLoader<BasePbTransModelFeature<SmtModel::HypScoreInfo> > > simpleDynClassLoaderVec;
  
  int createCustomFeat(std::string featName,
                       const ModelDescriptorEntry& modelDescEntry,
                       BasePbTransModelFeature<SmtModel::HypScoreInfo>** featPtrRef);
  BasePbTransModelFeature<SmtModel::HypScoreInfo>* createFeatPtr(std::string soFileName);

};
  
#endif
