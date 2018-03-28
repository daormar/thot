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
 * @file LevelDbDict.h
 * 
 * @brief Implements a bilingual dictionary with scores using leveldb.
 */

#ifndef _LevelDbDict
#define _LevelDbDict

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

#include "StatModelDefs.h"
#include "StrProcUtils.h"
#include "ErrorDefs.h"
#include <iostream>
#include <map>
#include <vector>
#include <stdlib.h>

//--------------- Constants ------------------------------------------

#define LEVELDBDICT_PHR_SEP      "\x01"
#define LEVELDBDICT_PHR_SEP_NEXT "\x02"

//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------


//--------------- LevelDbDict class

class LevelDbDict
{
    leveldb::DB* db;
    leveldb::Options options;
    std::string dbName;

        // Read and write data
    virtual bool retrieveData(const std::vector<std::string>& phrase, Score &score)const;
    virtual bool storeData(const std::vector<std::string>& phrase, Score score)const;

        // Returns s as (LEVELDBDICT_PHR_SEP, s)
    virtual std::vector<std::string> encodePhrase(const std::vector<std::string>& p);
        // Returns concatenated t and s as (t, LEVELDBDICT_PHR_SEP, s)
    virtual std::vector<std::string> encodeSrcTrgPhrases(const std::vector<std::string>& s,
                                                         const std::vector<std::string>& t);

  public:

      // Constructor
    LevelDbDict(void);

        // Wrapper for initializing levelDB
    virtual bool init(std::string levelDbPath);
        // Wrapper for removing levelDB
    virtual bool drop();
        // Wrapper for loading existing levelDB
    virtual bool load(std::string levelDbPath);
    
        // Abstract function definitions
    virtual void addDictEntry(const std::vector<std::string>& s,
                              const std::vector<std::string>& t,
                              Score score);
        // Returns information related to a given s and t.
    virtual Score getScore(const std::vector<std::string>& s,
                           const std::vector<std::string>& t,
                           bool &found);
    virtual bool getTransOptsForSource(const std::vector<std::string>& s,
                                       std::vector<std::vector<std::string> >& transOptVec);

        // size and clear functions
    virtual size_t size(void);
    virtual void clear(void);

        // Destructor
    virtual ~LevelDbDict();

      // const_iterator
    class const_iterator;
    friend class const_iterator;
    class const_iterator
    {
      protected:
        const LevelDbDict* ptPtr;
        leveldb::Iterator* internalIter;
        std::pair<std::vector<std::string>, int> dataItem;
           
      public:
        const_iterator(void)
        {
          ptPtr = NULL;
          internalIter = NULL;
        }
        const_iterator(const LevelDbDict* _ptPtr,
                       leveldb::Iterator* iter
                       ):ptPtr(_ptPtr),internalIter(iter)
        {
        }
        bool operator++(void); //prefix
        bool operator++(int);  //postfix
        int operator==(const const_iterator& right); 
        int operator!=(const const_iterator& right);
        std::pair<std::vector<std::string>, int> operator*(void);
        const std::pair<std::vector<std::string>, int>* operator->(void);
        ~const_iterator()
        {
          if(internalIter != NULL) {
            delete internalIter;
          }
        }
    };

        // const_iterator related functions
    LevelDbDict::const_iterator begin(void)const;
    LevelDbDict::const_iterator end(void)const;

};

#endif
