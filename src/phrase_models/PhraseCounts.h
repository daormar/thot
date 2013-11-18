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
/* Module: PhraseCounts                                             */
/*                                                                  */
/* Prototype file: PhraseCounts                                     */
/*                                                                  */
/* Description: PhraseCounts stores phrase counts using a trie.     */
/*                                                                  */
/********************************************************************/

#ifndef _PhraseCounts
#define _PhraseCounts

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "PhraseDefs.h"
#include "TrieBidOfWords.h"

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------

typedef TrieBid<WordIndex,Count>* PhraseCountState;
typedef TrieBidOfWords<Count> PcTable;

//--------------- function declarations ------------------------------

//--------------- Classes --------------------------------------------

//--------------- PhraseCounts class
class PhraseCounts
{
 public:
  PhraseCounts(void);
  PhraseCountState addPhrase(const Vector<WordIndex>& phrase,
                             Count count=1);
  PhraseCountState incrCountOfPhrase(const Vector<WordIndex>& phrase,
                                     Count count);	
  Count getCount(const Vector<WordIndex>& phrase,
                 bool& found);
  PhraseCountState getState(const Vector<WordIndex>& phrase);
  void getPhraseGivenState(PhraseCountState pState,
                           Vector<WordIndex>& phrase);
  Count getCountGivenState(PhraseCountState pState);
  unsigned int size(void);
  unsigned int trieSize(void);
  void clear(void);
  
 protected:	
  PcTable pcTable;
  unsigned int numberOfPhrasesStored;
  
  Vector<WordIndex> invertVectorElements(const Vector<WordIndex>& v);
};

#endif
