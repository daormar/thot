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
/* Module: PhraseDictLog                                            */
/*                                                                  */
/* Prototype file: PhraseDictLog                                    */
/*                                                                  */
/* Description: Implements a phrase dictionary using logcounts.     */
/*                                                                  */
/********************************************************************/

#ifndef _PhraseDictLog
#define _PhraseDictLog

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BasePhraseTable.h"
#include "PhrasePairInfo.h"
#include "Cache_lct_.h"
#include "PhraseCountsLog.h"
#include "NbestTableNode.h"
#include "OrderedVector.h"
#include "TrieOfWords.h"
#include "TrieVecs.h"

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------
 
typedef OrderedVector<PhraseCountLogState,LogCount> PhraseTableNodeLog;
// each vector of WordIndex corresponds to one phrase
#ifdef USE_KNUTH_IMPL_FOR_TRIES
typedef TrieOfWords<PhraseTableNodeLog> DictLog;
#else
typedef TrieVecs<WordIndex,PhraseTableNodeLog> DictLog;
#endif

//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------


//--------------- PhraseDictLog class
class PhraseDictLog 
{
 public:
  
	PhraseDictLog(void);
     
    LogCount getLogCounts_t_(PhraseCountLogState pls_state,
                             const std::vector<WordIndex>& t,
                             bool& found);
    LogCount getLogCount_t(const std::vector<WordIndex>& f);
    void addSrcTrgPair(PhraseCountLogState pls_state,
                       const std::vector<WordIndex>& t,
                       LogCount logCount);
    void incrSrcTrgLogCount(PhraseCountLogState pls_state,
                            const std::vector<WordIndex>& t,
                            LogCount logCount);
    std::map<std::vector<WordIndex>,PhrasePairInfo>
      getEntriesFor_s(PhraseCountLogState pls_state);
    NbestTableNode<PhraseTransTableNodeData>
      getTranslationsFor_s_(PhraseCountLogState pls_state);
    PhraseTableNodeLog* getTranslationsFor_t_(const std::vector<WordIndex>& t);
    pair<bool,PhraseTableNodeLog*>
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
       const PhraseDictLog* phdPtr;
       DictLog::const_iterator trIter;
      public:
       const_iterator(void):phdPtr(0)
         {
         }
       const_iterator(const PhraseDictLog* _phdPtr,
                      DictLog::const_iterator _trIter):phdPtr(_phdPtr),trIter(_trIter)
         {
         }
       bool operator++(void); //prefix
       bool operator++(int);  //postfix
       int operator==(const const_iterator& right); 
       int operator!=(const const_iterator& right); 
       const DictLog::const_iterator& operator->(void)const;
    };

        // const iterator related functions
    PhraseDictLog::const_iterator begin(void)const;
    PhraseDictLog::const_iterator end(void)const;

  protected:
    DictLog dictLog;
    Cache_lct_ cache_lct_;
};

#endif
