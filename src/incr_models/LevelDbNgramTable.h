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
/*              tables of the form p(WordIndex|Vector<WordIndex>)   */
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

#include "BaseIncrCondProbTable.h"
#include "ErrorDefs.h"

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------

//--------------- LevelDbNgramTable class

class LevelDbNgramTable: public BaseIncrCondProbTable<Vector<WordIndex>, WordIndex, Count, Count>
{
        leveldb::DB* db;
        leveldb::Options options;
        string dbName;

            // Converters
        string vectorToString(const Vector<WordIndex>& vec)const;
        Vector<WordIndex> stringToVector(const string s)const;
        
            // Read and write data
        bool retrieveData(const Vector<WordIndex>& phrase, float &count)const;
        bool storeData(const Vector<WordIndex>& phrase, float count);

            // Returns information related to a given key.
        Count getInfo(const Vector<WordIndex>& key, bool &found);
        Count getTrgInfo(const WordIndex& t, bool &found);

    public:

        typedef BaseIncrCondProbTable<Vector<WordIndex>, WordIndex, Count, Count>::SrcTableNode SrcTableNode;
        typedef BaseIncrCondProbTable<Vector<WordIndex>, WordIndex, Count, Count>::TrgTableNode TrgTableNode;

          // Constructor
        LevelDbNgramTable(void);

            // Key converters
        string vectorToKey(const Vector<WordIndex>& vec)const;
        Vector<WordIndex> keyToVector(const string key)const;

          // Wrapper for initializing levelDB
        bool init(string levelDbPath);
            // Wrapper for removing levelDB
        bool drop();
            // Wrapper for loading existing levelDB
        bool load(string levelDbPath);

          // Basic functions
          // TODO Ordering by n-gram value

          // Concatenate s and t phrases
        Vector<WordIndex> getSrcTrg(const Vector<WordIndex>& s, const WordIndex& t)const;
        
        void addTableEntry(const Vector<WordIndex>& s, const WordIndex& t, im_pair<Count,Count> inf);
        void addSrcInfo(const Vector<WordIndex>& s, Count s_inf);
        void addSrcTrgInfo(const Vector<WordIndex>& s, const WordIndex& t, Count st_inf);
        void incrCountsOfEntryLog(const Vector<WordIndex>& s,
                                  const WordIndex& t,
                                  LogCount lc);
        im_pair<Count,Count> infSrcTrg(const Vector<WordIndex>& s,
                                      const WordIndex& t,
                                      bool& found);
        Count getSrcInfo(const Vector<WordIndex>& s, bool& found);
        Count getSrcTrgInfo(const Vector<WordIndex>& s, const WordIndex& t, bool& found);
        Prob pTrgGivenSrc(const Vector<WordIndex>& s, const WordIndex& t);
        LgProb logpTrgGivenSrc(const Vector<WordIndex>& s, const WordIndex& t);
        Prob pSrcGivenTrg(const Vector<WordIndex>& s, const WordIndex& t);
        LgProb logpSrcGivenTrg(const Vector<WordIndex>& s, const WordIndex& t);
        bool getEntriesForSource(const Vector<WordIndex>& s, TrgTableNode& trgtn);
        bool getEntriesForTarget(const WordIndex& t, SrcTableNode& tnode);
        bool getNbestForSrc(const Vector<WordIndex>& s, NbestTableNode<WordIndex>& nbt);
        bool getNbestForTrg(const WordIndex& t, NbestTableNode<Vector<WordIndex> >& nbt, int N = -1);

          // Count-related functions
        Count cSrcTrg(const Vector<WordIndex>& s, const WordIndex& t);
        Count cSrc(const Vector<WordIndex>& s);
        Count cTrg(const WordIndex& t);
        LogCount lcSrcTrg(const Vector<WordIndex>& s, const WordIndex& t);
        LogCount lcSrc(const Vector<WordIndex>& s);
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
                pair<Vector<WordIndex>, Count> dataItem;
              
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
                pair<Vector<WordIndex>, Count> operator*(void);
                const pair<Vector<WordIndex>, Count>* operator->(void);

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

};

#endif
