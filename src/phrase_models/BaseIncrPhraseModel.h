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
 
/********************************************************************/
/*                                                                  */
/* Module: BaseIncrPhraseModel                                      */
/*                                                                  */
/* Prototype file: BaseIncrPhraseModel                              */
/*                                                                  */
/* Description: Defines the BaseIncrPhraseModel abstract base class.*/
/*              BaseIncrPhraseModel class derives from              */
/*              BasePhraseModel, and incorporates some extra        */
/*              functionality that allows to extend the phrase      */
/*              model.                                              */
/*                                                                  */
/********************************************************************/

#ifndef _BaseIncrPhraseModel_h
#define _BaseIncrPhraseModel_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseCountPhraseModel.h"

using namespace std;

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
    virtual void strAddTableEntry(const Vector<string>& s,
                                  const Vector<string>& t,
                                  PhrasePairInfo inf)=0;
    virtual void addTableEntry(const Vector<WordIndex>& s,
                               const Vector<WordIndex>& t,
                               PhrasePairInfo inf)=0;
	virtual void strIncrCountsOfEntry(const Vector<string>& s,
                                      const Vector<string>& t,
                                      Count count=1)=0;
	virtual void incrCountsOfEntry(const Vector<WordIndex>& s,
                                   const Vector<WordIndex>& t,
                                   Count count=1)=0;

        // Functions for extending the model
    virtual int trainBilPhrases(const Vector<Vector<std::string> >& srcPhrVec,
                                const Vector<Vector<std::string> >& trgPhrVec,
                                Count c=1,
                                Count lowerBound=0,
                                int verbose=0);

    virtual ~BaseIncrPhraseModel(){};
};

#endif
