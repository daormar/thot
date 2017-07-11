/*
thot package for statistical machine translation
 
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
/* Module: LevelDbPhraseTable                                       */
/*                                                                  */
/* Prototype file: LevelDbPhraseTable                               */
/*                                                                  */
/* Description: Implements a bilingual phrase table using a double  */
/*              array trie.                                         */
/*                                                                  */
/********************************************************************/

#ifndef _LevelDbPhraseTable
#define _LevelDbPhraseTable

#define WORD_INDEX_MODULO_BASE 254
#define WORD_INDEX_MODULO_BYTES 3
#define TRIE_NUM 10

//--------------- Include files --------------------------------------

#include <math.h>
#include <sstream>

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "leveldb/db.h"
#include "leveldb/filter_policy.h"
#include "leveldb/write_batch.h"

#include "BasePhraseTable.h"
#include "ErrorDefs.h"


//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------


//--------------- LevelDbPhraseTable class

class LevelDbPhraseTable: public BasePhraseTable
{
    leveldb::DB* db;
    leveldb::Options options;
    string dbName = "/tmp/thot_leveldb";

        // Converters
    virtual string vectorToString(const Vector<WordIndex>& vec)const;
    virtual Vector<WordIndex> stringToVector(const string s)const;
    virtual string vectorToKey(const Vector<WordIndex>& vec)const;
    virtual Vector<WordIndex> keyToVector(const string key)const;
    
        // Read and write data
    virtual bool retrieveData(const Vector<WordIndex>& phrase, int &count)const;
    virtual bool storeData(const Vector<WordIndex>& phrase, int count)const;

  
  public:

    typedef std::map<Vector<WordIndex>,PhrasePairInfo> SrcTableNode;
    typedef std::map<Vector<WordIndex>,PhrasePairInfo> TrgTableNode;

      // Constructor
    LevelDbPhraseTable(void);

        // Wrapper for saving trie structure
    virtual bool save(const char *path);
        // Wrapper for loading trie structure
    virtual bool load(const char *path);
        // Returns s as (UNUSED_WORD, s)
    virtual Vector<WordIndex> getSrc(const Vector<WordIndex>& s);
        // Returns concatenated s and t as (UNUSED_WORD, s, UNUSED_WORD, t)
    virtual Vector<WordIndex> getSrcTrg(const Vector<WordIndex>& s,
                                        const Vector<WordIndex>& t);
        // Returns concatenated t and s as (t, UNUSED_WORD, s)
    virtual Vector<WordIndex> getTrgSrc(const Vector<WordIndex>& s,
                                        const Vector<WordIndex>& t);
        // Abstract function definitions
    virtual void addTableEntry(const Vector<WordIndex>& s,
                               const Vector<WordIndex>& t,
                               PhrasePairInfo inf);
        // Adds an entry to the probability table
    virtual void addSrcInfo(const Vector<WordIndex>& s,Count s_inf);
    virtual void addSrcTrgInfo(const Vector<WordIndex>& s,
                               const Vector<WordIndex>& t,
                               Count st_inf);
    virtual void incrCountsOfEntry(const Vector<WordIndex>& s,
                                   const Vector<WordIndex>& t,
                                   Count c);
        // Increase the counts of a given phrase pair
    virtual PhrasePairInfo infSrcTrg(const Vector<WordIndex>& s,
                                     const Vector<WordIndex>& t,
                                     bool& found);
        // Returns information related to a given key.
    virtual Count getInfo(const Vector<WordIndex>& key,bool &found);
        // Returns information related to a given s.
    virtual Count getSrcInfo(const Vector<WordIndex>& s,bool &found);
        // Returns information related to a given t.
    virtual Count getTrgInfo(const Vector<WordIndex>& t,bool &found);
        // Returns information related to a given s and t.
    virtual Count getSrcTrgInfo(const Vector<WordIndex>& s,
                                const Vector<WordIndex>& t,
                                bool &found);
        // Returns information related to a given s and t.
    virtual Prob pTrgGivenSrc(const Vector<WordIndex>& s,
                              const Vector<WordIndex>& t);
    virtual LgProb logpTrgGivenSrc(const Vector<WordIndex>& s,
                                   const Vector<WordIndex>& t);
    virtual Prob pSrcGivenTrg(const Vector<WordIndex>& s,
                              const Vector<WordIndex>& t);
    virtual LgProb logpSrcGivenTrg(const Vector<WordIndex>& s,
                                   const Vector<WordIndex>& t);
    virtual bool getEntriesForTarget(const Vector<WordIndex>& t,
                                     SrcTableNode& srctn);
        // Stores in srctn the entries associated to a given target
        // phrase t, returns true if there are one or more entries
    virtual bool getEntriesForSource(const Vector<WordIndex>& s,
                                     TrgTableNode& trgtn);
        // Stores in trgtn the entries associated to a given source
        // phrase s, returns true if there are one or more entries
    virtual bool getNbestForSrc(const Vector<WordIndex>& s,
                                NbestTableNode<PhraseTransTableNodeData>& nbt);
    virtual bool getNbestForTrg(const Vector<WordIndex>& t,
                                NbestTableNode<PhraseTransTableNodeData>& nbt,
                                int N=-1);

       // Counts-related functions
    virtual Count cSrcTrg(const Vector<WordIndex>& s,
                          const Vector<WordIndex>& t);
    virtual Count cSrc(const Vector<WordIndex>& s);
    virtual Count cTrg(const Vector<WordIndex>& t);

        // Additional Functions
    bool nodeForTrgHasAtLeastOneTrans(const Vector<WordIndex>& t);
        // Returns true if t has one translation or more
    
        // size and clear functions
    virtual size_t size(void);
    virtual void print(bool printString = true);
    virtual void clear(void);   

        // Destructor
    virtual ~LevelDbPhraseTable();

      // const_iterator
    class const_iterator;
    friend class const_iterator;
    class const_iterator
    {
      protected:
        const LevelDbPhraseTable* ptPtr;
        leveldb::Iterator* internalIter;
        pair<Vector<WordIndex>, int> dataItem;
           
      public:
        const_iterator(void)
        {
          ptPtr = NULL;
          internalIter = NULL;
        }
        const_iterator(const LevelDbPhraseTable* _ptPtr,
                       leveldb::Iterator* iter
                       ):ptPtr(_ptPtr),internalIter(iter)
        {
        }
        bool operator++(void); //prefix
        bool operator++(int);  //postfix
        int operator==(const const_iterator& right); 
        int operator!=(const const_iterator& right);
        pair<Vector<WordIndex>, int> operator*(void);
        const pair<Vector<WordIndex>, int>* operator->(void);
        /* const PhraseDict::const_iterator& operator->(void)const; */
        ~const_iterator()
        {
          if(internalIter != NULL) {
            delete internalIter;
          }
        }
    };

        // const_iterator related functions
    LevelDbPhraseTable::const_iterator begin(void)const;
    LevelDbPhraseTable::const_iterator end(void)const;

};

#endif