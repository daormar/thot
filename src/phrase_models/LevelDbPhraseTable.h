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

/**
 * @file LevelDbPhraseTable.h
 * 
 * @brief Implements a bilingual phrase table using leveldb.
 */

#ifndef _LevelDbPhraseTable
#define _LevelDbPhraseTable

#define WORD_INDEX_MODULO_BASE 254
#define WORD_INDEX_MODULO_BYTES 3

//--------------- Include files --------------------------------------

#include <math.h>
#include <sstream>

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "leveldb/cache.h"
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
    std::string dbName;

        // Converters
    virtual std::string vectorToString(const std::vector<WordIndex>& vec)const;
    virtual std::vector<WordIndex> stringToVector(const std::string s)const;

        // Read and write data
    virtual bool retrieveData(const std::vector<WordIndex>& phrase, int &count)const;
    virtual bool storeData(const std::vector<WordIndex>& phrase, int count)const;

  
  public:

    typedef std::map<std::vector<WordIndex>,PhrasePairInfo> SrcTableNode;
    typedef std::map<std::vector<WordIndex>,PhrasePairInfo> TrgTableNode;

      // Constructor
    LevelDbPhraseTable(void);

        // Key converters
    virtual std::string vectorToKey(const std::vector<WordIndex>& vec)const;
    virtual std::vector<WordIndex> keyToVector(const std::string key)const;

        // Returns s as (UNUSED_WORD, s)
    virtual std::vector<WordIndex> encodeSrc(const std::vector<WordIndex>& s);
        // Returns concatenated t and s as (t, UNUSED_WORD, s)
    virtual std::vector<WordIndex> encodeTrgSrc(const std::vector<WordIndex>& s,
                                                const std::vector<WordIndex>& t);

        // Wrapper for initializing levelDB
    virtual bool init(std::string levelDbPath);
        // Wrapper for removing levelDB
    virtual bool drop();
        // Wrapper for loading existing levelDB
    virtual bool load(std::string levelDbPath);
        // Abstract function definitions
    virtual void addTableEntry(const std::vector<WordIndex>& s,
                               const std::vector<WordIndex>& t,
                               PhrasePairInfo inf);
        // Adds an entry to the probability table
    virtual void addSrcInfo(const std::vector<WordIndex>& s,Count s_inf);
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
        // Returns information related to a given key.
    virtual Count getInfo(const std::vector<WordIndex>& key,bool &found);
        // Returns information related to a given s.
    virtual Count getSrcInfo(const std::vector<WordIndex>& s,bool &found);
        // Returns information related to a given t.
    virtual Count getTrgInfo(const std::vector<WordIndex>& t,bool &found);
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

        // Additional Functions
    bool nodeForTrgHasAtLeastOneTrans(const std::vector<WordIndex>& t);
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
        std::pair<std::vector<WordIndex>, int> dataItem;
           
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
        std::pair<std::vector<WordIndex>, int> operator*(void);
        const std::pair<std::vector<WordIndex>, int>* operator->(void);
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
