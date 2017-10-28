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
/* Module: _sentLengthModel                                         */
/*                                                                  */
/* Prototype file: _sentLengthModel.h                               */
/*                                                                  */
/* Description: Defines the _sentLengthModel class.                 */
/*              _sentLengthModel is a predecessor class for         */
/*              derivating sentence length models.                  */
/*                                                                  */
/********************************************************************/

#ifndef __sentLengthModel_h
#define __sentLengthModel_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseSentLengthModel.h"
#include "SingleWordVocab.h"
#include "BaseSentenceHandler.h"

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- Function declarations ------------------------------


//--------------- Classes --------------------------------------------


//--------------- _sentLengthModel class

class _sentLengthModel: public BaseSentLengthModel
{
 public:

    void linkVocabPtr(SingleWordVocab* _swVocabPtr);
    void linkSentPairInfo(BaseSentenceHandler* _sentenceHandlerPtr);
    void trainSentPairRange(std::pair<unsigned int,unsigned int> sentPairRange,
                            int verbosity=0);
    int nthSentPair(unsigned int n,
                    std::vector<std::string>& srcSentStr,
                    std::vector<std::string>& trgSentStr,
                    Count& c);
	
 protected:
	
	SingleWordVocab* swVocabPtr;

    BaseSentenceHandler* sentenceHandlerPtr;
};

#endif
