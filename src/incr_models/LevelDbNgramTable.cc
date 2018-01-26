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

/**
 * @file LevelDbNgramTable.cc
 * 
 * @brief Definitions file for LevelDbNgramTable.h
 */

//--------------- Include files --------------------------------------

#include "LevelDbNgramTable.h"

//--------------- Function definitions

//-------------------------
LevelDbNgramTable::LevelDbNgramTable(void)
{
    options.create_if_missing = true;
    options.max_open_files = 4000;
    options.filter_policy = leveldb::NewBloomFilterPolicy(48);  // Use index store in memory to reduce number of disk operations
    options.block_cache = leveldb::NewLRUCache(100 * 1048576);  // 100 MB for cache
    db = NULL;
    dbName = "";
    // Key for null info - use the maximum allowed value as the key
    std::vector<WordIndex> null_vec;
    for (unsigned int i = 0; i < WORD_INDEX_MODULO_BYTES; i++)
    {
        null_vec.push_back(WORD_INDEX_MODULO_BASE);
    }

    std::string null_str(null_vec.begin(), null_vec.end());
    dbNullKey = null_str;
}

//-------------------------
std::string LevelDbNgramTable::vectorToString(const std::vector<WordIndex>& vec)const
{
    std::vector<WordIndex> str;
    str.push_back(vec.size() + 1);  // Add 1 to avoid string with leading \0

    for(size_t i = 0; i < vec.size(); i++) {
        // Use WORD_INDEX_MODULO_BYTES bytes to encode index
        for(int j = WORD_INDEX_MODULO_BYTES - 1; j >= 0; j--)
        {
            str.push_back(1 + (vec[i] / (unsigned int) pow(WORD_INDEX_MODULO_BASE, j) % WORD_INDEX_MODULO_BASE));
        }
    }

    std::string s(str.begin(), str.end());

    return s;
}

//-------------------------
std::vector<WordIndex> LevelDbNgramTable::stringToVector(const std::string s)const
{
    std::vector<WordIndex> vec;

    // A string length is WORD_INDEX_MODULO_BYTES * n + 1
    // Count from 1 to skip n value (technically, n+1)
    for(size_t i = 1; i < s.size();)
    {
        unsigned int wi = 0;
        for(int j = WORD_INDEX_MODULO_BYTES - 1; j >= 0; j--, i++)
        {
            wi += (((unsigned char) s[i]) - 1) * (unsigned int) pow(WORD_INDEX_MODULO_BASE, j);
        }

        vec.push_back(wi);
    }

    return vec;
}
//-------------------------
std::string LevelDbNgramTable::getDbNullKey(void)const
{
    return dbNullKey;
}

//-------------------------
std::vector<WordIndex> LevelDbNgramTable::getVectorDbNullKey(void)const
{
    return stringToVector(dbNullKey);
}

//-------------------------
std::string LevelDbNgramTable::vectorToKey(const std::vector<WordIndex>& vec)const
{
    return vectorToString(vec);
}

//-------------------------
std::vector<WordIndex> LevelDbNgramTable::keyToVector(const std::string key)const
{
    return stringToVector(key);
}

//-------------------------
bool LevelDbNgramTable::retrieveData(const std::string key, float &count)const
{
    std::string value_str;
    count = 0;

    leveldb::Status result = db->Get(leveldb::ReadOptions(), key, &value_str);  // Read stored src value

    if (result.ok())
    {
        count = atof(value_str.c_str());
        return true;
    }
    else
    {
        return false;
    }
}

//-------------------------
bool LevelDbNgramTable::retrieveData(const std::vector<WordIndex>& phrase, float &count)const
{
    if (phrase.size() == 0)
    {
        count = srcInfoNull.get_c_s();

        return true;
    }
    else
    {
        std::string key = vectorToString(phrase);
        return retrieveData(key, count);
    }
}

//-------------------------
bool LevelDbNgramTable::storeData(const std::string key, float count)
{
    std::stringstream ss;
    ss << count;
    std::string count_str = ss.str();

    leveldb::WriteBatch batch;
    batch.Put(key, count_str);
    leveldb::Status s = db->Write(leveldb::WriteOptions(), &batch);

    if(!s.ok())
        std::cerr << "Storing data status: " << s.ToString() << std::endl;

    return s.ok();
}

//-------------------------
bool LevelDbNgramTable::storeData(const std::vector<WordIndex>& phrase, float count)
{
    std::string key;

    if (phrase.size() == 0)
    {
        srcInfoNull = count;
        key = dbNullKey;
    }
    else
    {
        key = vectorToString(phrase);
    }

    return storeData(key, count);
}

//-------------------------
bool LevelDbNgramTable::init(std::string levelDbPath)
{
    std::cerr << "Initializing LevelDB phrase table" << std::endl;

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
bool LevelDbNgramTable::drop()
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
        std::cerr << "Dropping database status: " << status.ToString() << std::endl;
        
        return THOT_ERROR;
    }
}

//-------------------------
bool LevelDbNgramTable::load(const char *fileName)
{
    std::string levelDbPath(fileName);

    if(db != NULL)
    {
        delete db;
        db = NULL;
    }

    dbName = levelDbPath;
    leveldb::Status status = leveldb::DB::Open(options, dbName, &db);

    if(status.ok())
    {
        // Restore null count
        float null_count;
        retrieveData(dbNullKey, null_count);
        if (null_count == 0)  // Add key if it doesn't exist
            storeData(dbNullKey, null_count);
        srcInfoNull = null_count;

        return THOT_OK;
    }
    else
    {
        std::cerr << status.ToString() << std::endl;

        return THOT_ERROR;
    }
}

//-------------------------
std::vector<WordIndex> LevelDbNgramTable::getSrcTrg(const std::vector<WordIndex>& s,
                                               const WordIndex& t)const
{
    // Concatenate vectors s and t
    std::vector<WordIndex> st = s;
    st.push_back(t);

    return st;
}


//-------------------------
bool LevelDbNgramTable::getNbestForSrc(const std::vector<WordIndex>& s,
                                       NbestTableNode<WordIndex>& nbt)
{
    TrgTableNode tnode;
    TrgTableNode::iterator tNodeIter;
    bool found;
    Count s_count = cSrc(s);
    
    nbt.clear();
    found = getEntriesForSource(s, tnode);

    for(tNodeIter = tnode.begin(); tNodeIter != tnode.end(); tNodeIter++)
    {
        nbt.insert((float) tNodeIter->second.second.get_lc_st() - (float) s_count.get_lc_s(), tNodeIter->first);
    }

    return found;
}
//-------------------------
bool LevelDbNgramTable::getNbestForTrg(const WordIndex& t,
                                       NbestTableNode<std::vector<WordIndex> >& nbt,
                                       int N)
{
    SrcTableNode tnode;
    SrcTableNode::iterator tNodeIter;
    bool found;
    Count s_count;
    
    nbt.clear();
    found = getEntriesForTarget(t, tnode);
    
    for(tNodeIter = tnode.begin(); tNodeIter != tnode.end(); tNodeIter++)
    {
        s_count = tNodeIter->second.first;
        nbt.insert((float) tNodeIter->second.second.get_lc_st() - (float) s_count.get_lc_s(), tNodeIter->first);
    }

    if(N >= 0)
        while(nbt.size() > (unsigned int) N)
            nbt.removeLastElement();
    
    return found;
}

//-------------------------
void LevelDbNgramTable::addTableEntry(const std::vector<WordIndex>& s,
                                      const WordIndex& t,
                                      im_pair<Count, Count> inf) 
{
    addSrcInfo(s, inf.first);  // (s)
    addSrcTrgInfo(s, t, inf.second);  // (s, t)
}

//-------------------------
void LevelDbNgramTable::addSrcInfo(const std::vector<WordIndex>& s,
                                   Count s_inf)
{
    storeData(s, s_inf.get_c_s());
}

//-------------------------
void LevelDbNgramTable::addSrcTrgInfo(const std::vector<WordIndex>& s,
                                      const WordIndex& t,
                                      Count st_inf)
{
    storeData(getSrcTrg(s, t), st_inf.get_c_st());  // (s, t)
}

//-------------------------
void LevelDbNgramTable::incrCountsOfEntryLog(const std::vector<WordIndex>& s,
                                             const WordIndex& t,
                                             LogCount lc) 
{
    // Retrieve previous states
    Count src_trg_count = cSrcTrg(s, t);
    src_trg_count.incr_logcount((float) lc);

    // Update counts
    addSrcTrgInfo(s, t, src_trg_count);  // (s, t)
}

//-------------------------
im_pair<Count, Count> LevelDbNgramTable::infSrcTrg(const std::vector<WordIndex>& s,
                                                   const WordIndex& t,
                                                   bool& found) 
{
    im_pair<Count, Count> ppi;

    ppi.first = getSrcInfo(s, found);
    if (!found)
    {
        ppi.second = 0;
        return ppi;
    }
    else
    {
        ppi.second = getSrcTrgInfo(s, t, found);
        return ppi;
    }
}

//-------------------------
Count LevelDbNgramTable::getInfo(const std::vector<WordIndex>& key,
                                 bool &found)
{
    float count;
    found = retrieveData(key, count);

    Count result = (found) ? Count(count) : Count();

    return result;
}

//-------------------------
Count LevelDbNgramTable::getSrcInfo(const std::vector<WordIndex>& s,
                                    bool &found)
{
    return getInfo(s, found);
}

//-------------------------
Count LevelDbNgramTable::getTrgInfo(const WordIndex& t,
                                    bool &found)
{
    // Retrieve counter state
    std::vector<WordIndex> t_vec;
    t_vec.push_back(t);

    return getInfo(t_vec, found);
}

//-------------------------
Count LevelDbNgramTable::getSrcTrgInfo(const std::vector<WordIndex>& s,
                                       const WordIndex& t,
                                       bool &found)
{
    // Retrieve counter state
    return getInfo(getSrcTrg(s, t), found);
}

//-------------------------
Prob LevelDbNgramTable::pTrgGivenSrc(const std::vector<WordIndex>& s,
                                     const WordIndex& t)
{
    // Calculates P(t|s) = count(s,t) / count(s)
    Count st_count = cSrcTrg(s, t);
    if ((float) st_count > 0)
    {
        bool found;
        Count s_count = getSrcInfo(s, found);
        if ((float) s_count > 0)
            return (float) st_count.get_c_st() / (float) s_count.get_c_s();
        else
            return 0;
    }
    else return 0;
}

//-------------------------
LgProb LevelDbNgramTable::logpTrgGivenSrc(const std::vector<WordIndex>& s,
                                          const WordIndex& t)
{
    Prob p = pTrgGivenSrc(s, t);

    if ((double) p == 0.0)
        return SMALL_LG_NUM;
    
    return p.get_lp();
}

//-------------------------
Prob LevelDbNgramTable::pSrcGivenTrg(const std::vector<WordIndex>& s,
                                     const WordIndex& t)
{
    // P(s|t) = count(s, t) / count(t)
    Count st_count = cSrcTrg(s, t);
    if ((float) st_count > 0)
    {
        Count t_count = cTrg(t);
        if ((float) t_count > 0)
            return (float) st_count.get_c_st() / (float) t_count.get_c_s();
        else
            return SMALL_LG_NUM;
    }
    else return SMALL_LG_NUM;
}

//-------------------------
LgProb LevelDbNgramTable::logpSrcGivenTrg(const std::vector<WordIndex>& s,
                                          const WordIndex& t)
{
    return pSrcGivenTrg(s,t).get_lp();
}

//-------------------------
bool LevelDbNgramTable::getEntriesForTarget(const WordIndex& t,
                                            LevelDbNgramTable::SrcTableNode& tnode)
{
    // Method shouldn't be widely used as it requires to
    // iterate over the whole database
    std::pair<std::vector<WordIndex>, im_pair<Count, Count> > pdp;

    tnode.clear();  // Make sure that structure does not keep old values
    
    for(LevelDbNgramTable::const_iterator iter = begin(); iter != end(); iter++)
    {
        std::pair<std::vector<WordIndex>, Count> x = *iter;

        if (x.first.size() > 1 && x.first.back() == t)
        {
            std::vector<WordIndex> src(x.first.begin(), x.first.end() - 1);

            pdp.first = src;
            pdp.second.first = cSrc(src);
            pdp.second.second = x.second;

            tnode.insert(pdp);
        }
    }

    return tnode.size() > 0;
}

//-------------------------
bool LevelDbNgramTable::getEntriesForSource(const std::vector<WordIndex>& s,
                                            LevelDbNgramTable::TrgTableNode& trgtn) 
{
    bool found;
    std::pair<WordIndex, im_pair<Count, Count> > pdp;  // Data structure format: (t, (count(s), count(s, t)))

    Count s_count = cSrc(s);  // Retrieve count(s)

    // Define key range in which we look for target phrases
    std::vector<WordIndex> start_vec = s;
    start_vec.push_back(0);

    std::vector<WordIndex> end_vec = s;
    end_vec[end_vec.size() - 1] += 1;
    end_vec.push_back(0);

    std::string start_str = vectorToKey(start_vec);
    std::string end_str = vectorToKey(end_vec);

    leveldb::Slice start = start_str;
    leveldb::Slice end = end_str;

    // Iterate over the defined key range and populate valid results
    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
    
    trgtn.clear();  // Make sure that structure does not keep old values
    
    for(it->Seek(start); it->Valid() && it->key().ToString() < end.ToString(); it->Next())
    {
        std::vector<WordIndex> vec = keyToVector(it->key().ToString());

        if (s.size() == vec.size() - 1 && vec.size() > 1)
        {
            pdp.first = vec.back();  // t
            pdp.second.first = s_count;  // count(s)
            pdp.second.second = Count(atof(it->value().ToString().c_str()));  // sount(s, t)

            if (fabs(pdp.second.second.get_c_st()) < EPSILON)  // Compare to 0
                continue;

            trgtn.insert(pdp);
        }
        
    }

    found = it->status().ok();

    delete it;

    return trgtn.size() > 0 && found;
}

//-------------------------
Count LevelDbNgramTable::cSrcTrg(const std::vector<WordIndex>& s,
                                 const WordIndex& t)
{
    bool found;
    return getSrcTrgInfo(s, t, found);
}

//-------------------------
Count LevelDbNgramTable::cSrc(const std::vector<WordIndex>& s)
{
    bool found;
    return getSrcInfo(s, found);
}

//-------------------------
Count LevelDbNgramTable::cTrg(const WordIndex& t)
{
    // Method shouldn't be widely used as it requires to
    // iterate over the whole database
    Count t_count = Count(0);

    for(LevelDbNgramTable::const_iterator iter = begin(); iter != end(); iter++)
    {
        std::pair<std::vector<WordIndex>, Count> x = *iter;

        if (x.first.size() > 1 && x.first.back() == t)
        {
            t_count += x.second;
        }
    }

    return t_count;
}

//-------------------------
LogCount LevelDbNgramTable::lcSrcTrg(const std::vector<WordIndex>& s, const WordIndex& t)
{
    return LogCount(cSrcTrg(s, t).get_lc_st());
}

//-------------------------
LogCount LevelDbNgramTable::lcSrc(const std::vector<WordIndex>& s)
{
    return LogCount(cSrc(s).get_lc_s());
}

//-------------------------
LogCount LevelDbNgramTable::lcTrg(const WordIndex& t)
{
    // Method shouldn't be widely used as it requires to
    // iterate over the whole database
    return LogCount(cTrg(t).get_lc_s());
}

//-------------------------
size_t LevelDbNgramTable::size(void)
{
    size_t len = 0;  // It counts also null info entry

    for(LevelDbNgramTable::const_iterator iter = begin(); iter != end(); iter++, len++)
    {
        // Do nothing; iterates only over the elements in DB
    }

    return len;
}

//-------------------------
void LevelDbNgramTable::print(bool printString)
{
    std::cout << "levelDB content:" << std::endl;
    for(LevelDbNgramTable::const_iterator iter = begin(); iter != end(); iter++)
    {
        std::pair<std::vector<WordIndex>, Count> x = *iter;
        if (printString) {
            for(size_t i = 0; i < x.first.size(); i++)
              std::cout << x.first[i] << " ";
        } else {
          std::cout << vectorToKey(x.first);
        }
        
        std::cout << ":\t" << x.second.get_c_s() << std::endl;
    }
}

//-------------------------
void LevelDbNgramTable::clear(void)
{
    if(dbName.size() > 0)
    {
        // Remove old database - faster than removing single keys
        bool dropStatus = drop();

        if(dropStatus == THOT_ERROR)
        {
            exit(2);
        }

        // Create empty DB
        leveldb::Status status = leveldb::DB::Open(options, dbName, &db);
        
        if(!status.ok())
        {
            std::cerr << "Cannot create new levelDB in " << dbName << std::endl;
            std::cerr << "Returned status: " << status.ToString() << std::endl;
            exit(3);
        }

        // Clear empty key counter
        storeData(dbNullKey, 0);
        srcInfoNull = Count();
    }
}

//-------------------------
LevelDbNgramTable::~LevelDbNgramTable(void)
{
    if(db != NULL)
        delete db;

    if(options.filter_policy != NULL)
        delete options.filter_policy;

    if(options.block_cache != NULL)
        delete options.block_cache;
}

//-------------------------
LevelDbNgramTable::const_iterator LevelDbNgramTable::begin(void)const
{
    leveldb::Iterator *local_iter = db->NewIterator(leveldb::ReadOptions());
    local_iter->SeekToFirst();

    // Skip item, if it stores nullInfo, to be compatible with other implementations
    if (local_iter->Valid() && local_iter->key().ToString() == getDbNullKey())
    {
        local_iter->Next();
    }

    // Check if iterator is ready to read data from it
    if(!local_iter->Valid())
    {
        delete local_iter;
        local_iter = NULL;
    }

    LevelDbNgramTable::const_iterator iter(this, local_iter);

    return iter;
}

//-------------------------
LevelDbNgramTable::const_iterator LevelDbNgramTable::end(void)const
{
    LevelDbNgramTable::const_iterator iter(this, NULL);

    return iter;
}

// const_iterator function definitions
//--------------------------
bool LevelDbNgramTable::const_iterator::operator++(void) //prefix
{
    internalIter->Next();

    // Skip item, if it stores nullInfo, to be compatible with other implementations
    if (internalIter->Valid() && internalIter->key().ToString() == ptPtr->getDbNullKey())
    {
        internalIter->Next();
    }

    bool isValid = internalIter->Valid();

    if(!isValid)
    {
        delete internalIter;
        internalIter = NULL;
    }

    return isValid;
}

//--------------------------
bool LevelDbNgramTable::const_iterator::operator++(int)  //postfix
{
    return operator++();
}

//--------------------------
int LevelDbNgramTable::const_iterator::operator==(const const_iterator& right)
{
    return (ptPtr == right.ptPtr && internalIter == right.internalIter);
}

//--------------------------
int LevelDbNgramTable::const_iterator::operator!=(const const_iterator& right)
{
    return !((*this) == right);
}

//--------------------------
std::pair<std::vector<WordIndex>, Count> LevelDbNgramTable::const_iterator::operator*(void)
{
    return *operator->();
}

//--------------------------
const std::pair<std::vector<WordIndex>, Count>*
LevelDbNgramTable::const_iterator::operator->(void)
{
    std::string key = internalIter->key().ToString();
    std::vector<WordIndex> key_vec = ptPtr->keyToVector(key);

    float count = atof(internalIter->value().ToString().c_str());

    dataItem = make_pair(key_vec, Count(count));

    return &dataItem;
}

//-------------------------
