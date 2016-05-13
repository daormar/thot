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
 
#ifndef _BaseTranslationConstraints_h
#define _BaseTranslationConstraints_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "PositionIndex.h"
#include "myVector.h"
#include "ErrorDefs.h"
#include "StrProcUtils.h"
#include <utility>
#include <string>
#include <set>
#include <map>
#include <iostream>

//--------------- Constants ------------------------------------------


//--------------- Typedefs -------------------------------------------

class BaseTranslationConstraints
{
 public:

      // Declarations related to dynamic class loading
  typedef BaseTranslationConstraints* create_t(std::string);
  typedef std::string type_id_t(void);

      // Services
  virtual void obtainTransConstraints(std::string rawSrcSent,
                                      int verbosity=0)=0;
  virtual Vector<std::string> getSrcSentVec(void)const=0;
  virtual Vector<std::string> getTransForSrcPhr(pair<PositionIndex,PositionIndex> srcPhr)const=0;
  virtual std::set<pair<PositionIndex,PositionIndex> > getConstrainedSrcPhrases(void)const=0;
  virtual bool srcPhrAffectedByConstraint(pair<PositionIndex,PositionIndex> srcPhr)const=0;
  virtual bool translationSatisfiesConstraints(const Vector<std::string>& targetWordVec,
                                               const Vector<pair<PositionIndex,PositionIndex> >& alignedPositions)const=0;
      // NOTE: alignedPositions is a vector representing alignments
      // between source and target words.  The first index corresponds
      // to source word positions and the second one to target word
      // positions
  
  virtual void clear(void)=0;

      // Destructor
  virtual ~BaseTranslationConstraints(){};
};

#endif
