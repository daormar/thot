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
/* Definitions file: IncrLexLevelDbTable.cc                         */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "IncrLexLevelDbTable.h"

//--------------- Global variables -----------------------------------


//--------------- Function declarations


//--------------- Constants


//--------------- Classes --------------------------------------------


//--------------- IncrLexLevelDbTable class function definitions

//-------------------------
IncrLexLevelDbTable::IncrLexLevelDbTable(void)
{
    options.create_if_missing = true;
    options.filter_policy = leveldb::NewBloomFilterPolicy(10);
    options.block_cache = leveldb::NewLRUCache(10 * 1048576);  // 10 MB for cache
    db = NULL;
    dbName = "";
}

//-------------------------
bool IncrLexLevelDbTable::init(string levelDbPath)
{
    cerr << "Initializing LevelDB phrase table" << endl;

    if(db != NULL)
    {
        delete db;
        db = NULL;
    }

    if(load(levelDbPath.c_str()) != THOT_OK)
        return THOT_ERROR;

    clear();

    return THOT_OK;
}

//-------------------------
bool IncrLexLevelDbTable::drop()
{
    if(db != NULL)
    {
        delete db;
        db = NULL;
    }

    leveldb::Status status = leveldb::DestroyDB(dbName, options);

    if(status.ok())
    {
        return THOT_OK;
    }
    else
    {
        cerr << "Dropping database status: " << status.ToString() << endl;

        return THOT_ERROR;
    }
}

//-------------------------
string IncrLexLevelDbTable::vectorToString(const Vector<WordIndex>& vec)const
{
    Vector<WordIndex> str;
    for(size_t i = 0; i < vec.size(); i++) {
        // Use WORD_INDEX_MODULO_BYTES bytes to encode index
        for(int j = WORD_INDEX_MODULO_BYTES - 1; j >= 0; j--) {
            str.push_back(1 + (vec[i] / (unsigned int) pow(WORD_INDEX_MODULO_BASE, j) % WORD_INDEX_MODULO_BASE));
        }
    }

    string s(str.begin(), str.end());

    return s;
}

//-------------------------
Vector<WordIndex> IncrLexLevelDbTable::stringToVector(const string s)const
{
    Vector<WordIndex> vec;

    for(size_t i = 0; i < s.size();)  // A string length is WORD_INDEX_MODULO_BYTES * n + 1
    {
        unsigned int wi = 0;
        for(int j = WORD_INDEX_MODULO_BYTES - 1; j >= 0; j--, i++) {
            wi += (((unsigned char) s[i]) - 1) * (unsigned int) pow(WORD_INDEX_MODULO_BASE, j);
        }

        vec.push_back(wi);
    }

    return vec;
}

//-------------------------
string IncrLexLevelDbTable::vectorToKey(const Vector<WordIndex>& vec)const
{
    return vectorToString(vec);
}

//-------------------------
Vector<WordIndex> IncrLexLevelDbTable::keyToVector(const string key)const
{
    return stringToVector(key);
}

//-------------------------
bool IncrLexLevelDbTable::stringToFloat(const string value_str, float &value)const
{
    // Decode string representation to float without loosing precision
    Vector<WordIndex> vec = stringToVector(value_str);
    unsigned char *p = reinterpret_cast<unsigned char*>(&value);

    // Cannot retireve value or format is incorrect
    if (vec.size() != sizeof(value)) return false;

    for (size_t i = 0; i < sizeof(value); i++)
    {
        p[i] = vec[i];
    }

    return true;
}

//-------------------------
string IncrLexLevelDbTable::floatToString(const float value)const
{
    // Encode float as a string without loosing precision
    unsigned char const *p = reinterpret_cast<unsigned char const*>(&value);
    Vector<WordIndex> vec;
    for (size_t i = 0; i < sizeof(value); i++)
    {
        vec.push_back((WordIndex) p[i]);
    }

    return vectorToString(vec);
}

//-------------------------
bool IncrLexLevelDbTable::retrieveData(const Vector<WordIndex>& phrase, float &value)const
{
    string value_str;
    value = 0;
    string key = vectorToString(phrase);

    leveldb::Status result = db->Get(leveldb::ReadOptions(), key, &value_str);

    if (result.ok()) {
        return stringToFloat(value_str, value);
    } else {
        return false;
    }
}

//-------------------------
bool IncrLexLevelDbTable::storeData(const Vector<WordIndex>& phrase, float value)const
{
    string value_str = floatToString(value);

    leveldb::WriteBatch batch;
    batch.Put(vectorToString(phrase), value_str);
    leveldb::Status s = db->Write(leveldb::WriteOptions(), &batch);

    if(!s.ok())
        cerr << "Storing data status: " << s.ToString() << endl;

    return s.ok();
}

//-------------------------
void IncrLexLevelDbTable::setLexNumer(WordIndex s,
                                      WordIndex t,
                                      float f)
{
    // Insert lexNumer for pair (s,t)
    // Due to performance of getTransForTarget method,
    // pair (s,t) is stored as (t,s) in DB
    Vector<WordIndex> st_vec;
    st_vec.push_back(t);
    st_vec.push_back(s);

    storeData(st_vec, f);
}

//-------------------------
float IncrLexLevelDbTable::getLexNumer(WordIndex s,
                                       WordIndex t,
                                       bool& found)
{
    float lexNumber;
    Vector<WordIndex> st_vec;
    st_vec.push_back(t);
    st_vec.push_back(s);

    found = retrieveData(st_vec, lexNumber);

    return lexNumber;
}

//-------------------------
void IncrLexLevelDbTable::setLexDenom(WordIndex s,
                                      float d)
{
    Vector<WordIndex> s_vec;
    s_vec.push_back(s);

    storeData(s_vec, d);
}

//-------------------------
float IncrLexLevelDbTable::getLexDenom(WordIndex s,
                                       bool& found)
{
    float lexDenom;
    Vector<WordIndex> s_vec;
    s_vec.push_back(s);

    found = retrieveData(s_vec, lexDenom);

    return lexDenom;
}

//-------------------------
bool IncrLexLevelDbTable::getTransForTarget(WordIndex t,
                                            std::set<WordIndex>& transSet)
{
    bool found;

    Vector<WordIndex> start_vec;
    start_vec.push_back(t);
    Vector<WordIndex> end_vec;
    end_vec.push_back(t + 1);

    string start_str = vectorToKey(start_vec);
    string end_str = vectorToKey(end_vec);

    leveldb::Slice start = start_str;
    leveldb::Slice end = end_str;

    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());

    transSet.clear();

    for(it->Seek(start); it->Valid() && it->key().ToString() < end.ToString(); it->Next()) {
        Vector<WordIndex> vec = keyToVector(it->key().ToString());

        if(vec.size() > 1) {  // Skip single word entry, e.g. (t)
            WordIndex s = vec[vec.size() - 1];
            transSet.insert(s);
        }
    }

    found = it->status().ok();

    delete it;

    return transSet.size() > 0 && found;
}

//-------------------------
void IncrLexLevelDbTable::setLexNumDen(WordIndex s,
                                       WordIndex t,
                                       float num,
                                       float den)
{
    setLexDenom(s, den);
    setLexNumer(s, t, num);
}

//-------------------------
bool IncrLexLevelDbTable::load(const char* lexNumDenFile)
{
    if(db != NULL)
    {
        delete db;
        db = NULL;
    }

    cerr << "Loading lexnd in LevelDB format from " << lexNumDenFile << endl;

    dbName = lexNumDenFile;
    leveldb::Status status = leveldb::DB::Open(options, dbName, &db);

    if (status.ok())
    {
        return THOT_OK;
    }
    else
    {
        cerr << "Cannot open DB: " << status.ToString() << endl;

        return THOT_ERROR;
    }
}

//-------------------------
bool IncrLexLevelDbTable::print(const char* lexNumDenFile)
{
    ofstream outF;
    outF.open(lexNumDenFile, ios::out);

    if(!outF)
    {
        cerr << "Error while printing lexical nd file." << endl;

        return THOT_ERROR;
    }
    else
    {
        // Print file with lexical nd values
        // Disable cache for bulk load
        leveldb::ReadOptions options;
        options.fill_cache = false;

        leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions(options));

        for(it->SeekToFirst(); it->Valid(); it->Next()) {
            Vector<WordIndex> vec = keyToVector(it->key().ToString());

            for(size_t i = 0; i < vec.size(); i++) {
                outF << vec[i] << " ";
            }

            outF << it->value().ToString() << endl;
        }

        delete it;

        return THOT_OK;
    }
}

//-------------------------
void IncrLexLevelDbTable::clear(void)
{
    if(dbName.size() > 0)  // Database name is mandatory
    {
        // Remove old database
        bool dropStatus = drop();

        if(dropStatus == THOT_ERROR)
        {
            exit(2);
        }

        // Create empty database
        leveldb::Status status = leveldb::DB::Open(options, dbName, &db);

        if(!status.ok())
        {
            cerr << "Cannot create new levelDB in " << dbName << endl;
            cerr << "Returned status: " << status.ToString() << endl;
            exit(3);
        }
    }
}

//-------------------------
IncrLexLevelDbTable::~IncrLexLevelDbTable(void)
{
    if(db != NULL)
        delete db;

    if(options.filter_policy != NULL)
        delete options.filter_policy;

    if(options.block_cache != NULL)
        delete options.block_cache;
}
