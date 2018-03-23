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
 * @file LevelDbDict.cc
 * 
 * @brief Definitions file for LevelDbDict.h
 */

//--------------- Include files --------------------------------------

#include "LevelDbDict.h"

//--------------- Function definitions

//-------------------------
LevelDbDict::LevelDbDict(void)
{
    options.create_if_missing = true;
    options.max_open_files = 4000;
    options.filter_policy = leveldb::NewBloomFilterPolicy(48);
    options.block_cache = leveldb::NewLRUCache(100 * 1048576);  // 100 MB for cache
    db = NULL;
    dbName = "";
}

//-------------------------
bool LevelDbDict::retrieveData(const std::vector<std::string>& phrase, Score &score)const
{
    std::string value_str;
    score = 0;
    std::string key = StrProcUtils::stringVectorToString(phrase);
    
    leveldb::Status result = db->Get(leveldb::ReadOptions(), key, &value_str);  // Read stored src value

    if (result.ok())
    {
        score = atof(value_str.c_str());
        return true;
    }
    else
    {
        return false;
    }
}

//-------------------------
bool LevelDbDict::storeData(const std::vector<std::string>& phrase, Score score)const
{
    std::stringstream ss;
    ss << score;
    std::string score_str = ss.str();

    leveldb::WriteBatch batch;
    batch.Put(StrProcUtils::stringVectorToString(phrase), score_str);
    leveldb::Status s = db->Write(leveldb::WriteOptions(), &batch);

    if(!s.ok())
        std::cerr << "Storing data status: " << s.ToString() << std::endl;

    return s.ok();
}

//-------------------------
bool LevelDbDict::init(std::string levelDbPath)
{
    std::cerr << "Initializing LevelDB phrase table" << std::endl;

    if(db != NULL)
    {
        delete db;
        db = NULL;
    }

    if(load(levelDbPath) != THOT_OK)
        return THOT_ERROR;

    clear();

    return THOT_OK;
}

//-------------------------
bool LevelDbDict::drop()
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
bool LevelDbDict::load(std::string levelDbPath)
{
    if(db != NULL)
    {
        delete db;
        db = NULL;
    }

    dbName = levelDbPath;
    leveldb::Status status = leveldb::DB::Open(options, dbName, &db);

    if (status.ok())
    {
        return THOT_OK;
    }
    else
    {
        std::cerr << status.ToString() << std::endl;
        return THOT_ERROR;
    }
}

//-------------------------
size_t LevelDbDict::size(void)
{
    size_t len = 0;

    for(LevelDbDict::const_iterator iter = begin(); iter != end(); iter++, len++)
    {
        // Do nothing; iterates only over the elements in trie
    }

    return len;
}

//-------------------------
void LevelDbDict::clear(void)
{
    if(dbName.size() > 0)
    {
        bool dropStatus = drop();

        if(dropStatus == THOT_ERROR)
        {
            exit(2);
        }

        leveldb::Status status = leveldb::DB::Open(options, dbName, &db);
        
        if(!status.ok())
        {
            std::cerr << "Cannot create new levelDB in " << dbName << std::endl;
            std::cerr << "Returned status: " << status.ToString() << std::endl;
            exit(3);
        }
    }
}

//-------------------------
std::vector<std::string> LevelDbDict::encodePhrase(const std::vector<std::string>& p)
{
    // Encode phrase p as (LEVELDBDICT_PHR_SEP, p)
    std::vector<std::string> uw_s_vec;
    uw_s_vec.push_back(LEVELDBDICT_PHR_SEP);
    uw_s_vec.insert(uw_s_vec.end(), p.begin(), p.end());

    return uw_s_vec;
}

//-------------------------
std::vector<std::string> LevelDbDict::encodeSrcTrgPhrases(const std::vector<std::string>& s,
                                                          const std::vector<std::string>& t)
{
    // Encode (t,s) phrases as (t, LEVELDBDICT_PHR_SEP, s)
    std::vector<std::string> t_uw_s_vec = t;
    t_uw_s_vec.push_back(LEVELDBDICT_PHR_SEP);
    t_uw_s_vec.insert(t_uw_s_vec.end(), s.begin(), s.end());

    return t_uw_s_vec;
}

//-------------------------
void LevelDbDict::addDictEntry(const std::vector<std::string>& s,
                               const std::vector<std::string>& t,
                               Score score)
{
  storeData(encodeSrcTrgPhrases(s, t), score);  // (t, LEVELDBDICT_PHR_SEP, s)
}

//-------------------------
Score LevelDbDict::getScore(const std::vector<std::string>& s,
                            const std::vector<std::string>& t,
                            bool &found)
{
  std::vector<std::string> key=encodeSrcTrgPhrases(s, t);
  Score score;
  found = retrieveData(key, score);

  Score result = (found) ? Score((float) score) : Score();
  
  return result;
}

//-------------------------
bool LevelDbDict::getTransOptsForSource(const std::vector<std::string>& s,
                                        std::vector<std::vector<std::string> >& transOptVec)
{
      // TO-BE-DONE
  bool found;

  std::vector<std::string> start_vec = s;
  start_vec.push_back(LEVELDBDICT_PHR_SEP);

  std::vector<std::string> end_vec(s);
  end_vec.push_back(LEVELDBDICT_PHR_SEP_NEXT);

  std::string start_str = StrProcUtils::stringVectorToString(start_vec);
  std::string end_str = StrProcUtils::stringVectorToString(end_vec);

  leveldb::Slice start = start_str;
  leveldb::Slice end = end_str;

  leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
    
  transOptVec.clear();  // Make sure that structure does not keep old values
    
  int i = 0;
  for(it->Seek(start); it->Valid() && it->key().ToString() < end.ToString(); it->Next(), i++)
  {
    std::vector<std::string> vec = StrProcUtils::stringToStringVector(it->key().ToString());
    std::vector<std::string> trg(vec.begin() + start_vec.size(), vec.end());
    transOptVec.push_back(trg);
  }

  found = it->status().ok();
  
  delete it;
  
  return i > 0 && found;
}

//-------------------------
LevelDbDict::~LevelDbDict(void)
{
    if(db != NULL)
        delete db;

    if(options.filter_policy != NULL)
        delete options.filter_policy;

    if(options.block_cache != NULL)
        delete options.block_cache;
}

//-------------------------
LevelDbDict::const_iterator LevelDbDict::begin(void)const
{
    leveldb::Iterator *local_iter = db->NewIterator(leveldb::ReadOptions());
    local_iter->SeekToFirst();

    if(!local_iter->Valid()) {
        delete local_iter;
        local_iter = NULL;
    }

    LevelDbDict::const_iterator iter(this, local_iter);

    return iter;
}

//-------------------------
LevelDbDict::const_iterator LevelDbDict::end(void)const
{
    LevelDbDict::const_iterator iter(this, NULL);

    return iter;
}

// const_iterator function definitions
//--------------------------
bool LevelDbDict::const_iterator::operator++(void) //prefix
{
    internalIter->Next();

    bool isValid = internalIter->Valid();

    if(!isValid)
    {
        delete internalIter;
        internalIter = NULL;
    }

    return isValid;
}

//--------------------------
bool LevelDbDict::const_iterator::operator++(int)  //postfix
{
    return operator++();
}

//--------------------------
int LevelDbDict::const_iterator::operator==(const const_iterator& right)
{
    return (ptPtr == right.ptPtr && internalIter == right.internalIter);
}

//--------------------------
int LevelDbDict::const_iterator::operator!=(const const_iterator& right)
{
    return !((*this) == right);
}

//--------------------------
std::pair<std::vector<std::string>, int> LevelDbDict::const_iterator::operator*(void)
{
    return *operator->();
}

//--------------------------
const std::pair<std::vector<std::string>, int>*
LevelDbDict::const_iterator::operator->(void)
{
    std::string key = internalIter->key().ToString();
    std::vector<std::string> key_vec = StrProcUtils::stringToStringVector(key);

    int score = atoi(internalIter->value().ToString().c_str());

    dataItem = std::make_pair(key_vec, score);

    return &dataItem;
}

//-------------------------
