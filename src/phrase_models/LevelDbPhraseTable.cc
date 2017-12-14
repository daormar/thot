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
/* Definitions file: LevelDbPhraseTable.cc                          */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "LevelDbPhraseTable.h"

//--------------- Function definitions

//-------------------------
LevelDbPhraseTable::LevelDbPhraseTable(void)
{
    options.create_if_missing = true;
    options.max_open_files = 4000;
    options.filter_policy = leveldb::NewBloomFilterPolicy(48);
    options.block_cache = leveldb::NewLRUCache(100 * 1048576);  // 100 MB for cache
    db = NULL;
    dbName = "";
}

//-------------------------
std::string LevelDbPhraseTable::vectorToString(const std::vector<WordIndex>& vec)const
{
    std::vector<WordIndex> str;
    for(size_t i = 0; i < vec.size(); i++) {
        // Use WORD_INDEX_MODULO_BYTES bytes to encode index
        for(int j = WORD_INDEX_MODULO_BYTES - 1; j >= 0; j--) {
            str.push_back(1 + (vec[i] / (unsigned int) pow(WORD_INDEX_MODULO_BASE, j) % WORD_INDEX_MODULO_BASE));
        }
    }

    std::string s(str.begin(), str.end());

    return s;
}

//-------------------------
std::vector<WordIndex> LevelDbPhraseTable::stringToVector(const std::string s)const
{
    std::vector<WordIndex> vec;

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
std::string LevelDbPhraseTable::vectorToKey(const std::vector<WordIndex>& vec)const
{
    return vectorToString(vec);
}

//-------------------------
std::vector<WordIndex> LevelDbPhraseTable::keyToVector(const std::string key)const
{
    return stringToVector(key);
}

//-------------------------
bool LevelDbPhraseTable::retrieveData(const std::vector<WordIndex>& phrase, int &count)const
{
    std::string value_str;
    count = 0;
    std::string key = vectorToString(phrase);
    
    leveldb::Status result = db->Get(leveldb::ReadOptions(), key, &value_str);  // Read stored src value

    if (result.ok()) {
        count = atoi(value_str.c_str());
        return true;
    } else {
        return false;
    }
}

//-------------------------
bool LevelDbPhraseTable::storeData(const std::vector<WordIndex>& phrase, int count)const
{
    std::stringstream ss;
    ss << count;
    std::string count_str = ss.str();

    leveldb::WriteBatch batch;
    batch.Put(vectorToString(phrase), count_str);
    leveldb::Status s = db->Write(leveldb::WriteOptions(), &batch);

    if(!s.ok())
        std::cerr << "Storing data status: " << s.ToString() << std::endl;

    return s.ok();
}

//-------------------------
bool LevelDbPhraseTable::init(std::string levelDbPath)
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
bool LevelDbPhraseTable::drop()
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
bool LevelDbPhraseTable::load(std::string levelDbPath)
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
std::vector<WordIndex> LevelDbPhraseTable::getSrc(const std::vector<WordIndex>& s)
{
    // Prepare s vector as (UNUSED_WORD, s)
    std::vector<WordIndex> uw_s_vec;
    uw_s_vec.push_back(UNUSED_WORD);
    uw_s_vec.insert(uw_s_vec.end(), s.begin(), s.end());

    return uw_s_vec;
}

//-------------------------
std::vector<WordIndex> LevelDbPhraseTable::getSrcTrg(const std::vector<WordIndex>& s,
                                                     const std::vector<WordIndex>& t)
{
    // Prepare (s,t) vector as (UNUSED_WORD, s, UNUSED_WORD, t)
    std::vector<WordIndex> uw_s_uw_t_vec = getSrc(s);
    uw_s_uw_t_vec.push_back(UNUSED_WORD);
    uw_s_uw_t_vec.insert(uw_s_uw_t_vec.end(), t.begin(), t.end());

    return uw_s_uw_t_vec;
}

//-------------------------
std::vector<WordIndex> LevelDbPhraseTable::getTrgSrc(const std::vector<WordIndex>& s,
                                                     const std::vector<WordIndex>& t)
{
    // Prepare (t,s) vector as (t, UNUSED_WORD, s)
    std::vector<WordIndex> t_uw_s_vec = t;
    t_uw_s_vec.push_back(UNUSED_WORD);
    t_uw_s_vec.insert(t_uw_s_vec.end(), s.begin(), s.end());

    return t_uw_s_vec;
}

//-------------------------
bool LevelDbPhraseTable::getNbestForSrc(const std::vector<WordIndex>& /*s*/,
                                        NbestTableNode<PhraseTransTableNodeData>& /*nbt*/)
{
      // TO-BE-DONE
  std::cerr<<"Warning: getNbestForSrc function not implemented"<<std::endl;
  return false;
}
//-------------------------
bool LevelDbPhraseTable::getNbestForTrg(const std::vector<WordIndex>& t,
                                        NbestTableNode<PhraseTransTableNodeData>& nbt,
                                        int N)
{
    LevelDbPhraseTable::SrcTableNode::iterator iter;	

    bool found;
    Count t_count;
    LevelDbPhraseTable::SrcTableNode node;
    LgProb lgProb;

    nbt.clear();

    found = getEntriesForTarget(t, node);
    t_count = cTrg(t);

    if(found) {
        // Generate transTableNode
        for(iter = node.begin(); iter != node.end(); iter++) 
        {
            std::vector<WordIndex> s = iter->first;
            PhrasePairInfo ppi = (PhrasePairInfo) iter->second;
            lgProb = log((float) ppi.second.get_c_st() / (float) t_count);
            nbt.insert(lgProb, s); // Insert pair <log probability, source phrase>
        }

#   ifdef DO_STABLE_SORT_ON_NBEST_TABLE
        // Performs stable sort on n-best table, this is done to ensure
        // that the n-best lists generated by cache models and
        // conventional models are identical. However this process is
        // time consuming and must be avoided if possible
        nbt.stableSort();
#   endif

        while(nbt.size() > (unsigned int) N && N >= 0)
        {
            // node contains N inverse translations, remove last element
            nbt.removeLastElement();
        }

        return true;
    }
    else
    {
        // Cannot find the target phrase
        return false;
    }
}

//-------------------------
void LevelDbPhraseTable::addTableEntry(const std::vector<WordIndex>& s,
                                       const std::vector<WordIndex>& t,
                                       PhrasePairInfo inf)
{
    addSrcInfo(s, inf.first.get_c_s());  // (USUSED_WORD, s)
    storeData(t, (int) round(inf.second.get_c_s()));  // (t)
    addSrcTrgInfo(s, t, (int) round(inf.second.get_c_st())); // (t, UNUSED_WORD, s)
}

//-------------------------
void LevelDbPhraseTable::addSrcInfo(const std::vector<WordIndex>& s,
                                    Count s_inf)
{
    storeData(getSrc(s), (int) round(s_inf.get_c_s()));
}

//-------------------------
void LevelDbPhraseTable::addSrcTrgInfo(const std::vector<WordIndex>& s,
                                       const std::vector<WordIndex>& t,
                                       Count st_inf)
{
    storeData(getTrgSrc(s, t), (int) round(st_inf.get_c_st()));  // (t, UNUSED_WORD, s)
}

//-------------------------
void LevelDbPhraseTable::incrCountsOfEntry(const std::vector<WordIndex>& s,
                                           const std::vector<WordIndex>& t,
                                           Count c)
{
    // Retrieve previous states
    Count s_count = cSrc(s);
    Count t_count = cTrg(t);
    Count src_trg_count = cSrcTrg(s, t);

    // Update counts
    addSrcInfo(s, s_count + c);  // (USUSED_WORD, s)
    storeData(t, (int) round((t_count + c).get_c_s()));  // (t)
    addSrcTrgInfo(s, t, (src_trg_count + c).get_c_st());
}

//-------------------------
PhrasePairInfo LevelDbPhraseTable::infSrcTrg(const std::vector<WordIndex>& s,
                                             const std::vector<WordIndex>& t,
                                             bool& found)
{
    PhrasePairInfo ppi;

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
Count LevelDbPhraseTable::getInfo(const std::vector<WordIndex>& key,
                                  bool &found)
{
    int count;
    found = retrieveData(key, count);

    Count result = (found) ? Count((float) count) : Count();

    return result;
}

//-------------------------
Count LevelDbPhraseTable::getSrcInfo(const std::vector<WordIndex>& s,
                                     bool &found)
{
    return getInfo(getSrc(s), found);
}

//-------------------------
Count LevelDbPhraseTable::getTrgInfo(const std::vector<WordIndex>& t,
                                     bool &found)
{
    // Retrieve counter state
    return getInfo(t, found);
}

//-------------------------
Count LevelDbPhraseTable::getSrcTrgInfo(const std::vector<WordIndex>& s,
                                        const std::vector<WordIndex>& t,
                                        bool &found)
{
    // Retrieve counter state
    return getInfo(getTrgSrc(s, t), found);
}

//-------------------------
Prob LevelDbPhraseTable::pTrgGivenSrc(const std::vector<WordIndex>& s,
                                      const std::vector<WordIndex>& t)
{
    // Calculates p(t|s)=count(s,t)/count(s)
    Count st_count = cSrcTrg(s, t);	
    if ((float) st_count > 0)
    {
        bool found;
        Count s_count = getSrcInfo(s, found);
        if ((float) s_count > 0)
            return (float) st_count / (float) s_count;
        else
            return PHRASE_PROB_SMOOTH;
    }
    else return PHRASE_PROB_SMOOTH;
}

//-------------------------
LgProb LevelDbPhraseTable::logpTrgGivenSrc(const std::vector<WordIndex>& s,
                                           const std::vector<WordIndex>& t)
{
    return log((double) pTrgGivenSrc(s,t));
}

//-------------------------
Prob LevelDbPhraseTable::pSrcGivenTrg(const std::vector<WordIndex>& s,
                                      const std::vector<WordIndex>& t)
{
    // p(s|t)=count(s,t)/count(t)
    Count st_count = cSrcTrg(s, t);
    if ((float) st_count > 0)
    {
        Count t_count = cTrg(t);
        if ((float) t_count > 0)
            return (float) st_count / (float) t_count;
        else
            return PHRASE_PROB_SMOOTH;
    }
    else return PHRASE_PROB_SMOOTH;
}

//-------------------------
LgProb LevelDbPhraseTable::logpSrcGivenTrg(const std::vector<WordIndex>& s,
                                           const std::vector<WordIndex>& t)
{
    return log((double) pSrcGivenTrg(s,t));
}

//-------------------------
bool LevelDbPhraseTable::getEntriesForTarget(const std::vector<WordIndex>& t,
                                             LevelDbPhraseTable::SrcTableNode& srctn)
{
    bool found;

    std::vector<WordIndex> start_vec = t;
    start_vec.push_back(UNUSED_WORD);

    std::vector<WordIndex> end_vec(t);
    end_vec.push_back(3);

    std::string start_str = vectorToKey(start_vec);
    std::string end_str = vectorToKey(end_vec);

    leveldb::Slice start = start_str;
    leveldb::Slice end = end_str;

    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
    
    srctn.clear();  // Make sure that structure does not keep old values
    
    int i = 0;
    for(it->Seek(start); it->Valid() && it->key().ToString() < end.ToString(); it->Next(), i++) {
        std::vector<WordIndex> vec = keyToVector(it->key().ToString());
        std::vector<WordIndex> src(vec.begin() + start_vec.size(), vec.end());

        PhrasePairInfo ppi = infSrcTrg(src, t, found);
        if (!found || fabs(ppi.first.get_c_s()) < FLT_EPSILON || fabs(ppi.second.get_c_s()) < FLT_EPSILON)
            continue;

        srctn.insert(std::pair<std::vector<WordIndex>, PhrasePairInfo>(src, ppi));
    }

    found = it->status().ok();

    delete it;

    return i > 0 && found;
}

//-------------------------
bool LevelDbPhraseTable::getEntriesForSource(const std::vector<WordIndex>& /*s*/,
                                             LevelDbPhraseTable::TrgTableNode& /*trgtn*/)
{
    // TO-BE-DONE
  std::cerr<<"Warning: getEntriesForSource function not implemented"<<std::endl;
  return false;
}

//-------------------------
Count LevelDbPhraseTable::cSrcTrg(const std::vector<WordIndex>& s,
                                  const std::vector<WordIndex>& t)
{
    bool found;
    return getSrcTrgInfo(s,t,found);
}

//-------------------------
Count LevelDbPhraseTable::cSrc(const std::vector<WordIndex>& s)
{
    bool found;
    return getSrcInfo(s,found);
}

//-------------------------
Count LevelDbPhraseTable::cTrg(const std::vector<WordIndex>& t)
{
    bool found;
    return getTrgInfo(t,found);
}

//-------------------------
size_t LevelDbPhraseTable::size(void)
{
    size_t len = 0;

    for(LevelDbPhraseTable::const_iterator iter = begin(); iter != end(); iter++, len++)
    {
        // Do nothing; iterates only over the elements in trie
    }

    return len;
}

//-------------------------
void LevelDbPhraseTable::print(bool printString)
{
    std::cout << "levelDB content:" << std::endl;   
    for(LevelDbPhraseTable::const_iterator iter = begin(); iter != end(); iter++)
    {
        std::pair<std::vector<WordIndex>, int> x = *iter;
        if (printString) {
            for(size_t i = 0; i < x.first.size(); i++)
                std::cout << x.first[i] << " ";
        } else {
            std::cout << vectorToKey(x.first);
        }
        
        std::cout << ":\t" << x.second << std::endl;
    }
}

//-------------------------
void LevelDbPhraseTable::clear(void)
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
LevelDbPhraseTable::~LevelDbPhraseTable(void)
{
    if(db != NULL)
        delete db;

    if(options.filter_policy != NULL)
        delete options.filter_policy;

    if(options.block_cache != NULL)
        delete options.block_cache;
}

//-------------------------
LevelDbPhraseTable::const_iterator LevelDbPhraseTable::begin(void)const
{
    leveldb::Iterator *local_iter = db->NewIterator(leveldb::ReadOptions());
    local_iter->SeekToFirst();

    if(!local_iter->Valid()) {
        delete local_iter;
        local_iter = NULL;
    }

    LevelDbPhraseTable::const_iterator iter(this, local_iter);

    return iter;
}

//-------------------------
LevelDbPhraseTable::const_iterator LevelDbPhraseTable::end(void)const
{
    LevelDbPhraseTable::const_iterator iter(this, NULL);

    return iter;
}

// const_iterator function definitions
//--------------------------
bool LevelDbPhraseTable::const_iterator::operator++(void) //prefix
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
bool LevelDbPhraseTable::const_iterator::operator++(int)  //postfix
{
    return operator++();
}

//--------------------------
int LevelDbPhraseTable::const_iterator::operator==(const const_iterator& right)
{
    return (ptPtr == right.ptPtr && internalIter == right.internalIter);
}

//--------------------------
int LevelDbPhraseTable::const_iterator::operator!=(const const_iterator& right)
{
    return !((*this) == right);
}

//--------------------------
std::pair<std::vector<WordIndex>, int> LevelDbPhraseTable::const_iterator::operator*(void)
{
    return *operator->();
}

//--------------------------
const std::pair<std::vector<WordIndex>, int>*
LevelDbPhraseTable::const_iterator::operator->(void)
{
    std::string key = internalIter->key().ToString();
    std::vector<WordIndex> key_vec = ptPtr->keyToVector(key);

    int count = atoi(internalIter->value().ToString().c_str());

    dataItem = std::make_pair(key_vec, count);

    return &dataItem;
}

//-------------------------
