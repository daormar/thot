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
/* Module: BaseCountPhraseModel                                     */
/*                                                                  */
/* Prototype file: BaseCountPhraseModel                             */
/*                                                                  */
/* Description: Defines the BaseCountPhraseModel abstract base      */
/*              class. BaseCountPhraseModel class derives from      */
/*              BasePhraseModel, and incorporates some extra        */
/*              functionality that allows to work with phrase       */
/*              counts.                                             */
/*                                                                  */
/********************************************************************/

#ifndef _BaseCountPhraseModel_h
#define _BaseCountPhraseModel_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BasePhraseModel.h"

using namespace std;

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- Classes --------------------------------------------


//--------------- BaseCountPhraseModel class

class BaseCountPhraseModel: public BasePhraseModel
{
 public:

    typedef BasePhraseModel::SrcTableNode SrcTableNode;
    typedef BasePhraseModel::TrgTableNode TrgTableNode;

        // Counts-related functions
    virtual Count cSrcTrg(const Vector<WordIndex>& s,
                          const Vector<WordIndex>& t)=0;
    virtual Count cSrc(const Vector<WordIndex>& s)=0;
    virtual Count cTrg(const Vector<WordIndex>& t)=0;

    virtual Count cHSrcHTrg(const Vector<std::string>& s,
                            const Vector<std::string>& t)=0;
    virtual Count cHSrc(const Vector<std::string>& s)=0;
    virtual Count cHTrg(const Vector<std::string>& t)=0;
    
        // Functions to access model probabilities
    virtual PhrasePairInfo infSrcTrg(const Vector<WordIndex>& s,
                                     const Vector<WordIndex>& t,
                                     bool& found)=0;

    virtual ~BaseCountPhraseModel(){};
};

#endif
