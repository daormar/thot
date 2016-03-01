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
 
#ifndef _TranslationConstraints_h
#define _TranslationConstraints_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <utility>
#include <string>
#include <set>
#include <map>
#include <iostream>
#include "StrProcUtils.h"
#include "PositionIndex.h"
#include "myVector.h"
#include "ErrorDefs.h"

//--------------- Constants ------------------------------------------

#define XML_PHR_ANNOT_TAG_NAME "phr_pair_annot"
#define XML_SRC_SEGM_TAG_NAME "src_segm"
#define XML_TRG_SEGM_TAG_NAME "trg_segm"

//--------------- Typedefs -------------------------------------------

class TranslationConstraints
{
 public:

      // Constructor
  TranslationConstraints(void);

      // Services
  void obtainTransConstraints(std::string rawSrcSent,
                              int verbosity=0);
  Vector<std::string> getSrcSentVec(void)const;
  Vector<std::string> getTransForSrcPhr(pair<PositionIndex,PositionIndex> srcPhr)const;
  std::set<pair<PositionIndex,PositionIndex> > getConstrainedSrcPhrases(void)const;
  bool srcPhrAffectedByConstraint(pair<PositionIndex,PositionIndex> srcPhr)const;
  
 private:

      // Data members
  std::set<std::string> xmlTagSet;
  Vector<std::string> srcSentVec;
  std::map<pair<PositionIndex,PositionIndex>,Vector<std::string> > srcPhrTransMap;

      // Auxiliary functions
  std::string tokenizeSrcSentence(std::string srcSent)const;
  std::string obtainStartTag(std::string tagName)const;
  std::string obtainEndTag(std::string tagName)const;
  bool xmlTag(std::string srcSent,
              unsigned int initialPos,
              unsigned int& endTagPos)const;
  bool constraintFound(Vector<std::string> tokRawSrcSentVec,
                       unsigned int currPos,
                       Vector<std::string>& srcPhrase,
                       Vector<std::string>& trgPhrase,
                       unsigned int& finalPos)const;
};

#endif
