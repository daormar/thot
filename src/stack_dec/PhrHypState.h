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
 * @file PhrHypState.h
 * 
 * @brief Declares the PhrHypState class. This class represents the
 * state of a hypothesis for phrase-based translation.
 */

#ifndef _PhrHypState_h
#define _PhrHypState_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "LM_State.h"
#include "PositionIndex.h"
#include "SmtDefs.h"
#include "BaseHypState.h"
#include "Bitset.h"

//--------------- Classes --------------------------------------------

/**
 * @brief The PhrHypState class represents the state of a hypothesis for
 * phrase-based translation.
 */

class PhrHypState: public BaseHypState
{
  public:

       // Language model info
   LM_State lmHist;

       // Target length
   unsigned int trglen;

       // End position of the last covered source phrase
   PositionIndex endLastSrcPhrase;

       // Coverage info
   Bitset<MAX_SENTENCE_LENGTH_ALLOWED> sourceWordsAligned;	
       
       // Ordering
   bool operator< (const PhrHypState &right)const;
};

#endif
