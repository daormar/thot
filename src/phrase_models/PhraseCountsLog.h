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
/* Module: PhraseCountsLog                                          */
/*                                                                  */
/* Prototype file: PhraseCountsLog                                  */
/*                                                                  */
/* Description: PhraseCountsLog stores phrase logcounts using a     */
/*              trie.                                               */
/*                                                                  */
/********************************************************************/

#ifndef _PhraseCountsLog
#define _PhraseCountsLog

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "PhraseDefs.h"
#include "TrieBidOfWords.h"
#include <MathFuncs.h>

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------

typedef TrieBid<WordIndex,LogCount>* PhraseCountLogState;
typedef TrieBidOfWords<LogCount> PlcTable;

//--------------- function declarations ------------------------------

//--------------- Classes --------------------------------------------

//--------------- PhraseCountsLog class
class PhraseCountsLog
{
 public:
  PhraseCountsLog(void);
  PhraseCountLogState addPhrase(const Vector<WordIndex>& phrase,
                                LogCount logCount);
  PhraseCountLogState incrLogCountOfPhrase(const Vector<WordIndex>& phrase,
                                           LogCount logCount);	
  LogCount getLogCount(const Vector<WordIndex>& phrase,
                       bool& found);
  PhraseCountLogState getState(const Vector<WordIndex>& phrase);
  void getPhraseGivenState(PhraseCountLogState plState,
                           Vector<WordIndex>& phrase);
  LogCount getLogCountGivenState(PhraseCountLogState plState);
  unsigned int size(void);
  unsigned int trieSize(void);
  void clear(void);
  
 protected:	
  PlcTable plcTable;
  unsigned int numberOfPhrasesStored;
  
  Vector<WordIndex> invertVectorElements(const Vector<WordIndex>& v);
};

#endif
