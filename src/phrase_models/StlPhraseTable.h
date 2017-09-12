/*
thot package for statistical machine translation
Copyright (C) 2017 Adam Harasimowicz, Daniel Ortiz-Mart\'inez

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
/* Module: StlPhraseTable                                           */
/*                                                                  */
/* Prototype file: StlPhraseTable                                   */
/*                                                                  */
/* Description: Implements a bilingual phrase table.                */
/*                                                                  */
/********************************************************************/

#ifndef _StlPhraseTable
#define _StlPhraseTable

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BasePhraseTable.h"

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------


//--------------- StlPhraseTable class

class StlPhraseTable: public BasePhraseTable
{
    public:

            // Define custom types for data structures and returned results

            // Returned result types during querying for entries for a given
            // source or target
        typedef std::map<std::vector<WordIndex>, PhrasePairInfo> SrcTableNode;
        typedef std::map<std::vector<WordIndex>, PhrasePairInfo> TrgTableNode;

            // Data structure for source phrases
        typedef std::map<std::vector<WordIndex>, Count> SrcPhraseInfo;
            // Data structure for target phrases
        typedef std::map<std::vector<WordIndex>, Count> TrgPhraseInfo;

            // Returned result types by iterator
        typedef std::pair<std::vector<WordIndex>, std::vector<WordIndex> > PhraseInfoElementKey;
        typedef std::pair<PhraseInfoElementKey, int> PhraseInfoElement;

            // Define source-target data structure and its elements
        typedef pair<SrcPhraseInfo::iterator, TrgPhraseInfo::iterator> SrcTrgKey;

        struct SrcTrgKeyComparator
        {
            /* Implements comparator for keys in (s, t) data structure.
               The comparator first checks target phrase and the source
               phrase during detrmining the order. */

            bool operator()(const SrcTrgKey &left, const SrcTrgKey& right)const
            {
                /* Compare at the beginning trg phrase and then src phrase
                   as looking for src phrases for give trg is more common
                   operation. */
                if (left.second->first == right.second->first)
                {
                    return left.first->first < right.first->first;
                }
                else
                {
                    return left.second->first < right.second->first;
                }
            }
        };

        typedef std::map<SrcTrgKey, Count, SrcTrgKeyComparator> SrcTrgPhraseInfo;

            // Constructor
        StlPhraseTable(void);

            // Abstract function definitions
        virtual void addTableEntry(const std::vector<WordIndex>& s,
                                   const std::vector<WordIndex>& t,
                                   PhrasePairInfo inf);
            // Adds an entry to the probability table
        virtual void addSrcInfo(const std::vector<WordIndex>& s, Count s_inf);
        virtual void addSrcTrgInfo(const std::vector<WordIndex>& s,
                                   const std::vector<WordIndex>& t,
                                   Count st_inf);
        virtual void incrCountsOfEntry(const std::vector<WordIndex>& s,
                                       const std::vector<WordIndex>& t,
                                       Count c);
            // Increase the counts of a given phrase pair
        virtual PhrasePairInfo infSrcTrg(const std::vector<WordIndex>& s,
                                         const std::vector<WordIndex>& t,
                                         bool& found);
            // Returns information related to a given s
        virtual Count getSrcInfo(const std::vector<WordIndex>& s, bool &found);
            // Returns information related to a given s and t.
        virtual Count getSrcTrgInfo(const std::vector<WordIndex>& s,
                                    const std::vector<WordIndex>& t,
                                    bool &found);
            // Returns information related to a given s and t.
        virtual Prob pTrgGivenSrc(const std::vector<WordIndex>& s,
                                  const std::vector<WordIndex>& t);
        virtual LgProb logpTrgGivenSrc(const std::vector<WordIndex>& s,
                                       const std::vector<WordIndex>& t);
        virtual Prob pSrcGivenTrg(const std::vector<WordIndex>& s,
                                  const std::vector<WordIndex>& t);
        virtual LgProb logpSrcGivenTrg(const std::vector<WordIndex>& s,
                                       const std::vector<WordIndex>& t);
        virtual bool getEntriesForTarget(const std::vector<WordIndex>& t,
                                         SrcTableNode& srctn);
            // Stores in srctn the entries associated to a given target
            // phrase t, returns true if there are one or more entries
        virtual bool getEntriesForSource(const std::vector<WordIndex>& s,
                                         TrgTableNode& trgtn);
            // Stores in trgtn the entries associated to a given source
            // phrase s, returns true if there are one or more entries
        virtual bool getNbestForSrc(const std::vector<WordIndex>& s,
                                    NbestTableNode<PhraseTransTableNodeData>& nbt);
        virtual bool getNbestForTrg(const std::vector<WordIndex>& t,
                                    NbestTableNode<PhraseTransTableNodeData>& nbt,
                                    int N=-1);

            // Counts-related functions
        virtual Count cSrcTrg(const std::vector<WordIndex>& s,
                              const std::vector<WordIndex>& t);
        virtual Count cSrc(const std::vector<WordIndex>& s);
        virtual Count cTrg(const std::vector<WordIndex>& t);

            // print function
        virtual void print(void);

            // size and clear functions
        virtual size_t size(void);
        virtual void clear(void);

            // Destructor
        virtual ~StlPhraseTable();

            // const_iterator
        class const_iterator;
        friend class const_iterator;
        class const_iterator
        {
            protected:
                const StlPhraseTable* ptPtr;
                SrcPhraseInfo::const_iterator srcIter;
                TrgPhraseInfo::const_iterator trgIter;
                SrcTrgPhraseInfo::const_iterator srcTrgIter;

                StlPhraseTable::PhraseInfoElement dataItem;

            public:
                const_iterator(void) { ptPtr = NULL; }
                const_iterator(const StlPhraseTable* _ptPtr,
                               SrcPhraseInfo::const_iterator _srcIter,
                               TrgPhraseInfo::const_iterator _trgIter,
                               SrcTrgPhraseInfo::const_iterator _srcTrgIter
                            ) : ptPtr(_ptPtr), srcIter(_srcIter), trgIter(_trgIter), srcTrgIter(_srcTrgIter) {}
                bool operator++(void);  //prefix
                bool operator++(int);  //postfix
                int operator==(const const_iterator& right);
                int operator!=(const const_iterator& right);
                StlPhraseTable::PhraseInfoElement operator*(void);
                const StlPhraseTable::PhraseInfoElement* operator->(void);
        };

            // const_iterator related functions
        StlPhraseTable::const_iterator begin(void) const;
        StlPhraseTable::const_iterator end(void) const;

            // const_iterator related functions for targets
        StlPhraseTable::TrgPhraseInfo::const_iterator beginTrg(void) const;
        StlPhraseTable::TrgPhraseInfo::const_iterator endTrg(void) const;

    protected:
        SrcPhraseInfo srcPhraseInfo;
        TrgPhraseInfo trgPhraseInfo;
        SrcTrgPhraseInfo srcTrgPhraseInfo;

            // Restore iterators from src and trg collections to create key object for (src, trg)
        StlPhraseTable::SrcTrgKey getSrcTrgKey(const std::vector<WordIndex>& s,
                                               const std::vector<WordIndex>& t,
                                               bool &found);

            // Returns information related to a given t
        Count getTrgInfo(const std::vector<WordIndex>& t, bool &found);

            // Add target phrase count to the collection
        void addTrgInfo(const std::vector<WordIndex>& t, Count t_inf);

        void printVector(const std::vector<WordIndex>& vec) const;

};

#endif
