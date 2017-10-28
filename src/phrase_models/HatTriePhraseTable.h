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
/* Module: HatTriePhraseTable                                       */
/*                                                                  */
/* Prototype file: HatTriePhraseTable                               */
/*                                                                  */
/* Description: Implements a bilingual phrase table.                */
/*                                                                  */
/********************************************************************/

#ifndef _HatTriePhraseTable
#define _HatTriePhraseTable

#define WORD_INDEX_MODULO_BASE 254
// Increase this parameter if you have more word in dictionary than
// WORD_INDEX_MODULO_BASE ^ WORD_INDEX_MODULO_BYTES (by default 254^3)
#define WORD_INDEX_MODULO_BYTES 3

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BasePhraseTable.h"
#include "hat_trie/htrie_map.h"

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------


//--------------- HatTriePhraseTable class

class HatTriePhraseTable: public BasePhraseTable
{
    public:

            // Define custom types for data structures and returned results

            // Returned result types during querying for entries for a given
            // source or target
        typedef std::map<std::vector<WordIndex>, PhrasePairInfo> SrcTableNode;
        typedef std::map<std::vector<WordIndex>, PhrasePairInfo> TrgTableNode;

            // Data structure for storing phrase counts
        typedef tsl::htrie_map<char, Count> PhraseTable;

            // Returned result types by iterator
        typedef std::pair<std::vector<WordIndex>, Count> PhraseInfoElement;

            // Constructor
        HatTriePhraseTable(void);

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

        // Get keys
        std::vector<WordIndex> getSrc(const std::vector<WordIndex> &s);
        std::vector<WordIndex> getSrcTrg(const std::vector<WordIndex> &s,
                                         const std::vector<WordIndex> &t);
        std::vector<WordIndex> getTrgSrc(const std::vector<WordIndex> &s,
                                         const std::vector<WordIndex> &t);

            // Destructor
        virtual ~HatTriePhraseTable();

            // const_iterator
        class const_iterator;
        friend class const_iterator;
        class const_iterator
        {
            protected:
                const HatTriePhraseTable* ptPtr;
                PhraseTable::const_iterator trgIter;

                HatTriePhraseTable::PhraseInfoElement dataItem;

            public:
                const_iterator(void) { ptPtr = NULL; }
                const_iterator(const HatTriePhraseTable* _ptPtr,
                               PhraseTable::const_iterator _trgIter
                            ) : ptPtr(_ptPtr), trgIter(_trgIter) {}
                bool operator++(void);  //prefix
                bool operator++(int);  //postfix
                int operator==(const const_iterator& right);
                int operator!=(const const_iterator& right);
                HatTriePhraseTable::PhraseInfoElement operator*(void);
                const HatTriePhraseTable::PhraseInfoElement* operator->(void);
        };

            // const_iterator related functions
        HatTriePhraseTable::const_iterator begin(void) const;
        HatTriePhraseTable::const_iterator end(void) const;

    protected:
        PhraseTable phraseTable;

            // Check type of phrase in vector
        bool isTargetPhrase(const std::vector<WordIndex>& vec) const;

            // Returns information related to a given t
        Count getTrgInfo(const std::vector<WordIndex>& t, bool &found);

            // Add target phrase count to the collection
        void addTrgInfo(const std::vector<WordIndex>& t, Count t_inf);

        void printVector(const std::vector<WordIndex>& vec) const;

            // Key converters
        virtual std::string vectorToKey(const std::vector<WordIndex>& vec)const;
        virtual std::vector<WordIndex> keyToVector(const std::string key)const;
        virtual std::string vectorToStdString(const std::vector<WordIndex>& vec)const;
        virtual std::vector<WordIndex> stringToVector(const std::string s)const;

};

#endif
