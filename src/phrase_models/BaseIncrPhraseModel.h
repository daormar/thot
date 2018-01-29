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
 * @file BaseIncrPhraseModel.h
 * 
 * @brief Defines the BaseIncrPhraseModel abstract base class.
 * BaseIncrPhraseModel class derives from BasePhraseModel, and
 * incorporates some extra functionality that allows to extend the
 * phrase model.
 */

#ifndef _BaseIncrPhraseModel_h
#define _BaseIncrPhraseModel_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseCountPhraseModel.h"


//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- Classes --------------------------------------------


//--------------- BaseIncrPhraseModel class

class BaseIncrPhraseModel: public BaseCountPhraseModel
{
 public:

    typedef BaseCountPhraseModel::SrcTableNode SrcTableNode;
    typedef BaseCountPhraseModel::TrgTableNode TrgTableNode;

        // Functions to extend or modify the model
    virtual void strAddTableEntry(const std::vector<std::string>& s,
                                  const std::vector<std::string>& t,
                                  PhrasePairInfo inf)=0;
    virtual void addTableEntry(const std::vector<WordIndex>& s,
                               const std::vector<WordIndex>& t,
                               PhrasePairInfo inf)=0;
	virtual void strIncrCountsOfEntry(const std::vector<std::string>& s,
                                      const std::vector<std::string>& t,
                                      Count count=1)=0;
	virtual void incrCountsOfEntry(const std::vector<WordIndex>& s,
                                   const std::vector<WordIndex>& t,
                                   Count count=1)=0;

        // Functions for extending the model
    virtual int trainBilPhrases(const std::vector<std::vector<std::string> >& srcPhrVec,
                                const std::vector<std::vector<std::string> >& trgPhrVec,
                                Count c=1,
                                Count lowerBound=0,
                                int verbose=0);

    virtual ~BaseIncrPhraseModel(){};
};

#endif
