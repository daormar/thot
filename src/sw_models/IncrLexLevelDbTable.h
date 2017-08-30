/*
thot package for statistical machine translation
Copyright (C) 2017 Daniel Ortiz-Mart\'inez, Adam Harasimowicz

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
/* Module: IncrLexLevelDbTable                                      */
/*                                                                  */
/* Prototype file: IncrLexLevelDbTable.h                            */
/*                                                                  */
/* Description: Defines the IncrLexLevelDbTable class.              */
/*              IncrLexLevelDbTable class stores an incremental     */
/*              lexical table.                                      */
/*                                                                  */
/********************************************************************/

#ifndef _IncrLexLevelDbTable_h
#define _IncrLexLevelDbTable_h

#define WORD_INDEX_MODULO_BASE 254
#define WORD_INDEX_MODULO_BYTES 3

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <_incrLexTable.h>
#include <myVector.h>
#include <ErrorDefs.h>
#include <StatModelDefs.h>

#include "leveldb/cache.h"
#include "leveldb/db.h"
#include "leveldb/filter_policy.h"
#include "leveldb/write_batch.h"

#include <fstream>
#include <set>
#include <stdlib.h>

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------

//--------------- function declarations ------------------------------

//--------------- Classes --------------------------------------------

//--------------- IncrLexLevelDbTable class

class IncrLexLevelDbTable : public _incrLexTable
{
    leveldb::DB* db;
    leveldb::Options options;
    string dbName;

        // Converters
    string vectorToString(const Vector<WordIndex>& vec)const;
    Vector<WordIndex> stringToVector(const string s)const;
    bool stringToFloat(const string value_str, float &value)const;
    string floatToString(const float value)const;

        // Read and write data
    bool retrieveData(const Vector<WordIndex>& phrase, float &value)const;
    bool storeData(const Vector<WordIndex>& phrase, float value)const;

        // Key converters
    string vectorToKey(const Vector<WordIndex>& vec)const;
    Vector<WordIndex> keyToVector(const string key)const;

    public:

            // Constructor and destructor
        IncrLexLevelDbTable(void);
        ~IncrLexLevelDbTable(void);

            // Function for initializing and removing LevelDB
        bool init(string levelDbPath);
        bool drop();

            // Functions to handle lexNumer
        void setLexNumer(WordIndex s,
                         WordIndex t,
                         float f);
        float getLexNumer(WordIndex s,
                          WordIndex t,
                          bool& found);

            // Functions to handle lexDenom
        void setLexDenom(WordIndex s,
                         float f);
        float getLexDenom(WordIndex s,
                          bool& found);

            // Function to set lexical numerator and denominator
        void setLexNumDen(WordIndex s,
                          WordIndex t,
                          float num,
                          float den);

            // Functions to get translations for word
        bool getTransForTarget(WordIndex t,
                               std::set<WordIndex>& transSet);

            // load function
        bool load(const char* lexNumDenFile);

            // print function
        bool print(const char* lexNumDenFile);

            // clear() function
        void clear(void);
};

#endif
