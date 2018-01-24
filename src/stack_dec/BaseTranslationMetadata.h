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
 
#ifndef _BaseTranslationMetadata_h
#define _BaseTranslationMetadata_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BasePbTransModelFeature.h"
#include "SourceSegmentation.h"
#include "PositionIndex.h"
#include "ErrorDefs.h"
#include "StrProcUtils.h"
#include <string>
#include <set>
#include <map>
#include <vector>
#include <utility>
#include <iostream>

//--------------- Constants ------------------------------------------


//--------------- Typedefs -------------------------------------------


//--------------- Classes --------------------------------------------

template<class SCORE_INFO>
class BaseTranslationMetadata
{
 public:

      // Declarations related to dynamic class loading
  typedef BaseTranslationMetadata* create_t(const char*);
  typedef const char* type_id_t(void);

      // Services

      // Initialization and source sentence extraction
  virtual void obtainTransConstraints(std::string rawSrcSent,
                                      int verbosity=0)=0;
  virtual std::vector<std::string> getSrcSentVec(void)const=0;

      // Constraint-related functions
  virtual std::vector<std::string> getTransForSrcPhr(std::pair<PositionIndex,PositionIndex> srcPhr)const=0;
  virtual std::set<std::pair<PositionIndex,PositionIndex> > getConstrainedSrcPhrases(void)const=0;
  virtual bool srcPhrAffectedByConstraint(std::pair<PositionIndex,PositionIndex> srcPhr)const=0;
  virtual bool translationSatisfiesConstraints(const SourceSegmentation& sourceSegmentation,
                                               const std::vector<PositionIndex>& targetSegmentCuts,
                                               const std::vector<std::string>& targetWordVec)const=0;
      // This function verifies if a sentence translation satisfies
      // constraints. It is applied over complete translations or
      // partial ones built from left to right.
  virtual bool phraseTranslationIsValid(const std::vector<std::string>& sourceWordVec,
                                        const std::vector<std::string>& targetWordVec)const=0;

      // Functions related to on-the-fly log-linear features
  virtual std::vector<BasePbTransModelFeature<SCORE_INFO>*> getOnTheFlyModelFeatures(void)
  {
    std::vector<BasePbTransModelFeature<SCORE_INFO>*> emptyVec;
    return emptyVec;
  }

  virtual void clear(void)=0;

      // Destructor
  virtual ~BaseTranslationMetadata(){};
};

#endif
