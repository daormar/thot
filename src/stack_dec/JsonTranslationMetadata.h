/*
thot package for statistical machine translation
Copyright (C) 2017 Adam Harasimowicz

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

#ifndef _JsonTranslationMetadata_h
#define _JsonTranslationMetadata_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseTranslationMetadata.h"
#include "picojson.h"

//--------------- Constants ------------------------------------------

#define JSON_TEX_SEGMENTATION "tex_segmentation"

//--------------- Typedefs -------------------------------------------

//--------------- Classes --------------------------------------------

class JsonTranslationMetadata: public BaseTranslationMetadata
{
 public:

      // Constructor
  JsonTranslationMetadata(void);

      // Services

      // Initialization and source sentence extraction
  void obtainTransConstraints(std::string rawSrcSent,
                              int verbosity=0);
  std::vector<std::string> getSrcSentVec(void)const;

      // Constraint-related functions
  std::vector<std::string> getTransForSrcPhr(std::pair<PositionIndex, PositionIndex> srcPhr)const;
  std::set<std::pair<PositionIndex,PositionIndex> > getConstrainedSrcPhrases(void)const;
  bool srcPhrAffectedByConstraint(std::pair<PositionIndex, PositionIndex> srcPhr)const;
  bool translationSatisfiesConstraints(const SourceSegmentation& sourceSegmentation,
                                       const std::vector<PositionIndex>& targetSegmentCuts,
                                       const std::vector<std::string>& targetWordVec)const;
      // This function verifies if a sentence translation satisfies
      // constraints. It is applied over complete translations or
      // partial ones built from left to right.
  bool phraseTranslationIsValid(const std::vector<std::string>& sourceWordVec,
                                const std::vector<std::string>& targetWordVec)const;

  void clear(void);

      // Destructor
  ~JsonTranslationMetadata(){};

 private:

      // Data members
  std::vector<std::string> srcSentVec;
  std::map<std::pair<PositionIndex,PositionIndex>,std::vector<std::string> > srcPhrTransMap;

      // Check if string is empty or contains only whitespace characters
  bool containsOnlyWhitespaces(std::string phrase);
      // Check if phrase is in source sentence at given position
  bool isPhraseInSentence(std::vector<std::string>& phrase, unsigned int startPosition);
      // Map translation on their positions in source sentence
  void addTranslations(std::vector<std::pair<std::string, std::string> >& translations);
      // Check if translation violates constraints
  bool transViolatesSrcPhrConstraints(const SourceSegmentation& sourceSegmentation,
                                      const std::vector<PositionIndex>& targetSegmentCuts,
                                      const std::vector<std::string>& targetWordVec)const;
  bool transViolatesSrcPhrConstraint(std::pair<PositionIndex,PositionIndex> constrainedSrcSegm,
                                     std::vector<std::string> constrainedTrans,
                                     const SourceSegmentation& sourceSegmentation,
                                     const std::vector<PositionIndex>& targetSegmentCuts,
                                     const std::vector<std::string>& targetWordVec)const;
};

#endif
