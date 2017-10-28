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
/* Module: PhraseDict                                               */
/*                                                                  */
/* Prototype file: PhraseDict                                       */
/*                                                                  */
/* Description: Implements a phrase dictionary.                     */
/*                                                                  */
/********************************************************************/

#ifndef _PhraseDict
#define _PhraseDict

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BasePhraseTable.h"
#include "PhrasePairInfo.h"
#include "Cache_ct_.h"
#include "PhraseCounts.h"
#include "NbestTableNode.h"
#include "OrderedVector.h"
#include "TrieOfWords.h"
#include "TrieVecs.h"

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------
 
typedef OrderedVector<PhraseCountState,Count> PhraseTableNode;
// each vector of WordIndex corresponds to one phrase
#ifdef USE_KNUTH_IMPL_FOR_TRIES
typedef TrieOfWords<PhraseTableNode> Dict;
#else
typedef TrieVecs<WordIndex,PhraseTableNode> Dict;
#endif

//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------


//--------------- PhraseDict class
class PhraseDict 
{
 public:
  
	PhraseDict(void);
     
    Count getCounts_t_(PhraseCountState ps_state,
                       const std::vector<WordIndex>& t,
                       bool& found);
    Count getCount_t(const std::vector<WordIndex>& f);
    void addSrcTrgPair(PhraseCountState ps_state,
                       const std::vector<WordIndex>& t,
                       Count count);
    void incrSrcTrgCount(PhraseCountState pe_state,
                         const std::vector<WordIndex>& t,
                         Count count);
    std::map<std::vector<WordIndex>,PhrasePairInfo>
      getEntriesFor_s(PhraseCountState ps_state);
    NbestTableNode<PhraseTransTableNodeData>
      getTranslationsFor_s_(PhraseCountState ps_state);
    PhraseTableNode* getTranslationsFor_t_(const std::vector<WordIndex>& t);
    std::pair<bool,PhraseTableNode*>
      nodeForTrgHasOneTransOrMore(const std::vector<WordIndex>& t);
    void addTrgIfNotExist(const std::vector<WordIndex>& t);
    
    size_t size(void);
    void clear(void);   

          // const_iterator
    class const_iterator;
    friend class const_iterator;
    class const_iterator
    {
      protected:
       const PhraseDict* phdPtr;
       Dict::const_iterator trIter;
      public:
       const_iterator(void):phdPtr(0)
         {
         }
       const_iterator(const PhraseDict* _phdPtr,
                      Dict::const_iterator _trIter):phdPtr(_phdPtr),trIter(_trIter)
         {
         }
       bool operator++(void); //prefix
       bool operator++(int);  //postfix
       int operator==(const const_iterator& right); 
       int operator!=(const const_iterator& right); 
       const Dict::const_iterator& operator->(void)const;
    };

        // const iterator related functions
    PhraseDict::const_iterator begin(void)const;
    PhraseDict::const_iterator end(void)const;

  protected:
    Dict dict;
    Cache_ct_ cache_ct_;
};

#endif
