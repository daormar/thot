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
 
#ifndef _PhraseExtractParameters
#define _PhraseExtractParameters

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "PhraseDefs.h"

//--------------- Constants ------------------------------------------

#define DEFAULT_MAX_TRG_PHRASE_LENGTH     7

//--------------- PhraseExtractParameters struct
struct PhraseExtractParameters
{
  bool monotone;
  int maxTrgPhraseLength;
  bool constraintSrcLen;
  bool countSpurious;
  int maxNumbOfCombsInTable;	

  PhraseExtractParameters(void)
    {
      monotone=false;
      maxTrgPhraseLength=DEFAULT_MAX_TRG_PHRASE_LENGTH;
      constraintSrcLen=false;
      countSpurious=true;
    }
};

#endif
