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
    options.filter_policy = leveldb::NewBloomFilterPolicy(16);
}

//-------------------------
string LevelDbPhraseTable::vectorToString(const Vector<WordIndex>& vec)const
{
    stringstream ss;
    for(size_t i = 0; i < vec.size(); ++i) {
        if(i != 0)
            ss << " ";
        ss << vec[i];
    }

    return ss.str();
}

//-------------------------
Vector<WordIndex> LevelDbPhraseTable::stringToVector(const string s)const
{
    istringstream iss(s);
    string word_index_str;
    Vector<WordIndex> vec;

    while(iss >> word_index_str) {
        vec.push_back(atoi(word_index_str.c_str()));
    }

    return vec;
}

//-------------------------
string LevelDbPhraseTable::vectorToKey(const Vector<WordIndex>& vec)const
{
    return vectorToString(vec);
}

//-------------------------
Vector<WordIndex> LevelDbPhraseTable::keyToVector(const string key)const
{
    return stringToVector(key);
}

//-------------------------
bool LevelDbPhraseTable::retrieveData(const Vector<WordIndex>& phrase, int &count)const
{
    string value_str;
    count = 0;
    string key = vectorToString(phrase);
    
    leveldb::Status result = db->Get(leveldb::ReadOptions(), key, &value_str);  // Read stored src value

    if (result.ok()) {
        count = atoi(value_str.c_str());
        return true;
    } else {
        return false;
    }
}

//-------------------------
bool LevelDbPhraseTable::storeData(const Vector<WordIndex>& phrase, int count)const
{
    stringstream ss;
    ss << count;
    string count_str = ss.str();

    leveldb::WriteBatch batch;
    batch.Put(vectorToString(phrase), count_str);
    leveldb::Status s = db->Write(leveldb::WriteOptions(), &batch);

    if(!s.ok())
        cerr << "Storing data status: " << s.ToString() << endl;

    return s.ok();
}

//-------------------------
bool LevelDbPhraseTable::init(string levelDbPath)
{
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
        cerr << "Dropping database status: " << status.ToString() << endl;
        
        return THOT_ERROR;
    }
}

//-------------------------
bool LevelDbPhraseTable::load(string levelDbPath)
{
    if(db != NULL)
    {
        delete db;
        db = NULL;
    }

    dbName = levelDbPath;
    leveldb::Status status = leveldb::DB::Open(options, dbName, &db);

    return (status.ok()) ? THOT_OK : THOT_ERROR;
}

//-------------------------
Vector<WordIndex> LevelDbPhraseTable::getSrc(const Vector<WordIndex>& s)
{
    // Prepare s vector as (UNUSED_WORD, s)
    Vector<WordIndex> uw_s_vec;
    uw_s_vec.push_back(UNUSED_WORD);
    uw_s_vec.insert(uw_s_vec.end(), s.begin(), s.end());

    return uw_s_vec;
}

//-------------------------
Vector<WordIndex> LevelDbPhraseTable::getSrcTrg(const Vector<WordIndex>& s,
                                               const Vector<WordIndex>& t)
{
    // Prepare (s,t) vector as (UNUSED_WORD, s, UNUSED_WORD, t)
    Vector<WordIndex> uw_s_uw_t_vec = getSrc(s);
    uw_s_uw_t_vec.push_back(UNUSED_WORD);
    uw_s_uw_t_vec.insert(uw_s_uw_t_vec.end(), t.begin(), t.end());

    return uw_s_uw_t_vec;
}

//-------------------------
Vector<WordIndex> LevelDbPhraseTable::getTrgSrc(const Vector<WordIndex>& s,
                                               const Vector<WordIndex>& t)
{
    // Prepare (t,s) vector as (t, UNUSED_WORD, s)
    Vector<WordIndex> t_uw_s_vec = t;
    t_uw_s_vec.push_back(UNUSED_WORD);
    t_uw_s_vec.insert(t_uw_s_vec.end(), s.begin(), s.end());

    return t_uw_s_vec;
}

//-------------------------
bool LevelDbPhraseTable::getNbestForSrc(const Vector<WordIndex>& s,
                                       NbestTableNode<PhraseTransTableNodeData>& nbt)
{
      // TO-BE-DONE (LOW PRIORITY)
}
//-------------------------
bool LevelDbPhraseTable::getNbestForTrg(const Vector<WordIndex>& t,
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
            Vector<WordIndex> s = iter->first;
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
void LevelDbPhraseTable::addTableEntry(const Vector<WordIndex>& s,
                                      const Vector<WordIndex>& t,
                                      PhrasePairInfo inf) 
{
    addSrcInfo(s, inf.first.get_c_s());  // (USUSED_WORD, s)
    storeData(t, (int) inf.second.get_c_s());  // (t)
    addSrcTrgInfo(s, t, (int) inf.second.get_c_st()); // (t, UNUSED_WORD, s)
}

//-------------------------
void LevelDbPhraseTable::addSrcInfo(const Vector<WordIndex>& s,
                                   Count s_inf)
{
    storeData(getSrc(s), (int) s_inf.get_c_s());
}

//-------------------------
void LevelDbPhraseTable::addSrcTrgInfo(const Vector<WordIndex>& s,
                                      const Vector<WordIndex>& t,
                                      Count st_inf)
{
    storeData(getTrgSrc(s, t), (int) st_inf.get_c_st());  // (t, UNUSED_WORD, s)
}

//-------------------------
void LevelDbPhraseTable::incrCountsOfEntry(const Vector<WordIndex>& s,
                                          const Vector<WordIndex>& t,
                                          Count c) 
{
    // Retrieve previous states
    Count s_count = cSrc(s);
    Count t_count = cTrg(t);
    Count src_trg_count = cSrcTrg(s, t);

    // Update counts
    addSrcInfo(s, s_count + c);  // (USUSED_WORD, s)
    storeData(t, (int) (t_count + c).get_c_s());  // (t)
    addSrcTrgInfo(s, t, (src_trg_count + c).get_c_st());
}

//-------------------------
PhrasePairInfo LevelDbPhraseTable::infSrcTrg(const Vector<WordIndex>& s,
                                            const Vector<WordIndex>& t,
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
Count LevelDbPhraseTable::getInfo(const Vector<WordIndex>& key,
                                    bool &found)
{
    int count;
    found = retrieveData(key, count);

    Count result = (found) ? Count((float) count) : Count();

    return result;
}

//-------------------------
Count LevelDbPhraseTable::getSrcInfo(const Vector<WordIndex>& s,
                                    bool &found)
{
    return getInfo(getSrc(s), found);
}

//-------------------------
Count LevelDbPhraseTable::getTrgInfo(const Vector<WordIndex>& t,
                                    bool &found)
{
    // Retrieve counter state
    return getInfo(t, found);
}

//-------------------------
Count LevelDbPhraseTable::getSrcTrgInfo(const Vector<WordIndex>& s,
                                       const Vector<WordIndex>& t,
                                       bool &found)
{
    // Retrieve counter state
    return getInfo(getTrgSrc(s, t), found);
}

//-------------------------
Prob LevelDbPhraseTable::pTrgGivenSrc(const Vector<WordIndex>& s,
                                     const Vector<WordIndex>& t)
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
LgProb LevelDbPhraseTable::logpTrgGivenSrc(const Vector<WordIndex>& s,
                                          const Vector<WordIndex>& t)
{
    return log((double) pTrgGivenSrc(s,t));
}

//-------------------------
Prob LevelDbPhraseTable::pSrcGivenTrg(const Vector<WordIndex>& s,
                                     const Vector<WordIndex>& t)
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
LgProb LevelDbPhraseTable::logpSrcGivenTrg(const Vector<WordIndex>& s,
                                          const Vector<WordIndex>& t)
{
    return log((double) pSrcGivenTrg(s,t));
}

//-------------------------
bool LevelDbPhraseTable::getEntriesForTarget(const Vector<WordIndex>& t,
                                            LevelDbPhraseTable::SrcTableNode& srctn) 
{
    bool found;

    Vector<WordIndex> start_vec = t;
    start_vec.push_back(UNUSED_WORD);

    Vector<WordIndex> end_vec(t);
    end_vec.push_back(3);

    string start_str = vectorToKey(start_vec);
    string end_str = vectorToKey(end_vec);

    leveldb::Slice start = start_str;
    leveldb::Slice end = end_str;

    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
    
    srctn.clear();  // Make sure that structure does not keep old values
    
    int i = 0;
    for(it->Seek(start); it->Valid() && it->key().ToString() < end.ToString(); it->Next(), i++) {
        Vector<WordIndex> vec = keyToVector(it->key().ToString());
        Vector<WordIndex> src(vec.begin() + start_vec.size(), vec.end());

        PhrasePairInfo ppi = infSrcTrg(src, t, found);
        if (!found || (int) ppi.first.get_c_s() == 0 || (int) ppi.second.get_c_s() == 0)
            continue;

        srctn.insert(pair<Vector<WordIndex>, PhrasePairInfo>(src, ppi));
    }

    delete it;

    return i > 0 && it->status().ok();
}

//-------------------------
bool LevelDbPhraseTable::getEntriesForSource(const Vector<WordIndex>& s,
                                            LevelDbPhraseTable::TrgTableNode& trgtn) 
{
    // TO-BE-DONE (LOW PRIORITY)
}

//-------------------------
Count LevelDbPhraseTable::cSrcTrg(const Vector<WordIndex>& s,
                                 const Vector<WordIndex>& t)
{
    bool found;
    return getSrcTrgInfo(s,t,found);
}

//-------------------------
Count LevelDbPhraseTable::cSrc(const Vector<WordIndex>& s)
{
    bool found;
    return getSrcInfo(s,found);
}

//-------------------------
Count LevelDbPhraseTable::cTrg(const Vector<WordIndex>& t)
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
    cout << "levelDB content:" << endl;   
    for(LevelDbPhraseTable::const_iterator iter = begin(); iter != end(); iter++)
    {
        pair<Vector<WordIndex>, int> x = *iter;
        if (printString) {
            cout << vectorToString(x.first);
        } else {
            cout << vectorToKey(x.first);
        }
        
        cout << "\t" << x.second << endl;
    }
}

//-------------------------
void LevelDbPhraseTable::clear(void)
{
    bool dropStatus = drop();

    if(dropStatus == THOT_ERROR)
    {
        exit(2);
    }

    leveldb::Status status = leveldb::DB::Open(options, dbName, &db);
    
    if(!status.ok())
    {
        cerr << "Cannot create new levelDB in " << dbName << endl;
        cerr << "Returned status: " << status.ToString() << endl;
        exit(3);
    }
}

//-------------------------
LevelDbPhraseTable::~LevelDbPhraseTable(void)
{
    if(db != NULL)
    {
        delete db;
        db = NULL;
    }
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
pair<Vector<WordIndex>, int> LevelDbPhraseTable::const_iterator::operator*(void)
{
    return *operator->();
}

//--------------------------
const pair<Vector<WordIndex>, int>*
LevelDbPhraseTable::const_iterator::operator->(void)
{
    string key = internalIter->key().ToString();
    Vector<WordIndex> key_vec = ptPtr->keyToVector(key);

    int count = atoi(internalIter->value().ToString().c_str());

    dataItem = make_pair(key_vec, count);

    return &dataItem;
}

//-------------------------