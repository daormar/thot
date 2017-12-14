/*
thot package for statistical machine translation
Copyright (C) 2017 Adam Harasimowicz
 
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
/* Module: LevelDbNgramTable                                        */
/*                                                                  */
/* Prototype file: LevelDbNgramTable                                */
/*                                                                  */
/* Description: Class to manage incremental conditional probability */
/*              tables of the form                                  */
/*              p(WordIndex|std::vector<WordIndex>)                 */
/*                                                                  */
/********************************************************************/

#ifndef _LevelDbNgramTable
#define _LevelDbNgramTable

#define WORD_INDEX_MODULO_BASE 254
#define WORD_INDEX_MODULO_BYTES 3

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "leveldb/cache.h"
#include "leveldb/db.h"
#include "leveldb/filter_policy.h"
#include "leveldb/write_batch.h"
#include <sstream>

#include "BaseIncrCondProbTable.h"
#include "ErrorDefs.h"
#include "MathDefs.h"

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------

//--------------- LevelDbNgramTable class

class LevelDbNgramTable: public BaseIncrCondProbTable<std::vector<WordIndex>, WordIndex, Count, Count>
{
        leveldb::DB* db;
        leveldb::Options options;
        std::string dbName;
        std::string dbNullKey;

            // Converters
        std::string vectorToString(const std::vector<WordIndex>& vec)const;
        std::vector<WordIndex> stringToVector(const std::string s)const;
        
            // Read and write data
        bool retrieveData(const std::string key, float &count)const;
        bool retrieveData(const std::vector<WordIndex>& phrase, float &count)const;
        bool storeData(const std::string key, float count);
        bool storeData(const std::vector<WordIndex>& phrase, float count);

            // Returns information related to a given key.
        Count getInfo(const std::vector<WordIndex>& key, bool &found);
        Count getTrgInfo(const WordIndex& t, bool &found);

    public:

        typedef BaseIncrCondProbTable<std::vector<WordIndex>, WordIndex, Count, Count>::SrcTableNode SrcTableNode;
        typedef BaseIncrCondProbTable<std::vector<WordIndex>, WordIndex, Count, Count>::TrgTableNode TrgTableNode;

          // Constructor
        LevelDbNgramTable(void);

            // Key converter and getter
        std::string vectorToKey(const std::vector<WordIndex>& vec)const;
        std::vector<WordIndex> keyToVector(const std::string key)const;

          // Wrapper for initializing levelDB
        bool init(std::string levelDbPath);
            // Wrapper for removing levelDB
        bool drop();
            // Wrapper for loading existing levelDB
        bool load(const char *fileName);
        //bool load(std::string fileName);

          // Basic functions
          // TODO Ordering by n-gram value

          // Concatenate s and t phrases
        std::vector<WordIndex> getSrcTrg(const std::vector<WordIndex>& s, const WordIndex& t)const;
        
        void addTableEntry(const std::vector<WordIndex>& s, const WordIndex& t, im_pair<Count,Count> inf);
        void addSrcInfo(const std::vector<WordIndex>& s, Count s_inf);
        void addSrcTrgInfo(const std::vector<WordIndex>& s, const WordIndex& t, Count st_inf);
        void incrCountsOfEntryLog(const std::vector<WordIndex>& s,
                                  const WordIndex& t,
                                  LogCount lc);
        im_pair<Count,Count> infSrcTrg(const std::vector<WordIndex>& s,
                                      const WordIndex& t,
                                      bool& found);
        Count getSrcInfo(const std::vector<WordIndex>& s, bool& found);
        Count getSrcTrgInfo(const std::vector<WordIndex>& s, const WordIndex& t, bool& found);
        Prob pTrgGivenSrc(const std::vector<WordIndex>& s, const WordIndex& t);
        LgProb logpTrgGivenSrc(const std::vector<WordIndex>& s, const WordIndex& t);
        Prob pSrcGivenTrg(const std::vector<WordIndex>& s, const WordIndex& t);
        LgProb logpSrcGivenTrg(const std::vector<WordIndex>& s, const WordIndex& t);
        bool getEntriesForSource(const std::vector<WordIndex>& s, TrgTableNode& trgtn);
        bool getEntriesForTarget(const WordIndex& t, SrcTableNode& tnode);
        bool getNbestForSrc(const std::vector<WordIndex>& s, NbestTableNode<WordIndex>& nbt);
        bool getNbestForTrg(const WordIndex& t, NbestTableNode<std::vector<WordIndex> >& nbt, int N = -1);

          // Count-related functions
        Count cSrcTrg(const std::vector<WordIndex>& s, const WordIndex& t);
        Count cSrc(const std::vector<WordIndex>& s);
        Count cTrg(const WordIndex& t);
        LogCount lcSrcTrg(const std::vector<WordIndex>& s, const WordIndex& t);
        LogCount lcSrc(const std::vector<WordIndex>& s);
        LogCount lcTrg(const WordIndex& t);

          // Size, clear, print functions
        size_t size(void);
        void clear(void);
        void print(bool printString = true);

          // Destructor
        ~LevelDbNgramTable(void);
  
          // const_iterator
        class const_iterator;
        friend class const_iterator;
        class const_iterator
        {
            protected:
                const LevelDbNgramTable* ptPtr;
                leveldb::Iterator* internalIter;
                std::pair<std::vector<WordIndex>, Count> dataItem;
              
            public:
                const_iterator(void)
                {
                    ptPtr = NULL;
                    internalIter = NULL;
                }
                const_iterator(const LevelDbNgramTable* _ptPtr,
                               leveldb::Iterator* iter
                               ):ptPtr(_ptPtr),internalIter(iter)
                {
                }
                bool operator++(void); //prefix
                bool operator++(int);  //postfix
                int operator==(const const_iterator& right); 
                int operator!=(const const_iterator& right);
                std::pair<std::vector<WordIndex>, Count> operator*(void);
                const std::pair<std::vector<WordIndex>, Count>* operator->(void);

                ~const_iterator()
                {
                    if(internalIter != NULL) {
                        delete internalIter;
                    }
                }
        };
          // const_iterator-related functions
        const_iterator begin(void)const;
        const_iterator end(void)const;
  
    protected:
        Count srcInfoNull;

        // Key and getter for nullInfo
        std::string getDbNullKey(void)const;
        std::vector<WordIndex> getVectorDbNullKey(void)const;

};

#endif
