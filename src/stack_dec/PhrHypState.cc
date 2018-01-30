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
 * @file PhrHypState.cc
 * 
 * @brief Definitions file for PhrHypState.h
 */

//--------------- Include files --------------------------------------

#include "PhrHypState.h"

//--------------- PhrHypState class functions

bool PhrHypState::operator< (const PhrHypState &right)const
{
  if(lmHist < right.lmHist) return 0; if(right.lmHist < lmHist) return 1;

  if(trglen < right.trglen) return 0; if(right.trglen < trglen) return 1;

  if(endLastSrcPhrase<right.endLastSrcPhrase) return 0;
  if(right.endLastSrcPhrase<endLastSrcPhrase) return 1;
  
  return sourceWordsAligned<right.sourceWordsAligned;
}
