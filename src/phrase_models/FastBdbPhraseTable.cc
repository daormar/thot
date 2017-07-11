/*
thot package for statistical machine translation
Copyright (C) 2013 Daniel Ortiz-Mart\'inez
 
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
/* Module: FastBdbPhraseTable                                       */
/*                                                                  */
/* Definitions file: FastBdbPhraseTable.cc                          */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "FastBdbPhraseTable.h"

//--------------- Function definitions

//-------------------------
FastBdbPhraseTable::FastBdbPhraseTable(void)
{
  envPtr=NULL;
  phrDictDb=NULL;
}

//-------------------------
int static_phr_dict_cmp_func(Db* /*db*/,
                             const Dbt* dbt1,
                             const Dbt* dbt2)
{
      // Decode keys
  PhrDictKey* keyPtr1=(PhrDictKey*) dbt1->get_data();
  PhrDictKey key1;
  key1.clear();
  unsigned int numWordsKey1=dbt1->get_size()/sizeof(WordIndex);
  for(unsigned int i=0;i<numWordsKey1;++i)
    key1.words[i]=keyPtr1->words[i];
    
  PhrDictKey* keyPtr2=(PhrDictKey*) dbt2->get_data();
  PhrDictKey key2;
  key2.clear();
  unsigned int numWordsKey2=dbt2->get_size()/sizeof(WordIndex);
  for(unsigned int i=0;i<numWordsKey2;++i)
    key2.words[i]=keyPtr2->words[i];

      // Obtain phrases
  Vector<WordIndex> srcPhr1, srcPhr2;
  Vector<WordIndex> trgPhr1, trgPhr2;
  key1.getPhrPair(srcPhr1,trgPhr1);
  key2.getPhrPair(srcPhr2,trgPhr2);

      // Compare target phrases
  for(unsigned int i=0;i<min(trgPhr1.size(),trgPhr2.size());++i)
  {
    if(trgPhr1[i]<trgPhr2[i])
      return -1;
    if(trgPhr2[i]<trgPhr1[i])
      return 1;
  }
  if(trgPhr1.size()<trgPhr2.size())
    return -1;
  if(trgPhr2.size()<trgPhr1.size())
    return 1;

      // Compare source phrases
  for(unsigned int i=0;i<min(srcPhr1.size(),srcPhr2.size());++i)
  {
    if(srcPhr1[i]<srcPhr2[i])
      return -1;
    if(srcPhr2[i]<srcPhr1[i])
      return 1;
  }
  if(srcPhr1.size()<srcPhr2.size())
    return -1;
  if(srcPhr2.size()<srcPhr1.size())
    return 1;
    
  return 0;
}

//-------------------------
int static_phr_dict_cmp_func(Db* db,
                             const Dbt* dbt1,
                             const Dbt* dbt2,
                             size_t* /*locp*/)
{
  return static_phr_dict_cmp_func(db,dbt1,dbt2);
}

//-------------------------
bool FastBdbPhraseTable::init(const char *fileName)
{
  cerr<<"Initializing fast BDB phrase table"<<endl;
  
      // clear object
  clear();

  std::string outputFilesPrefix=fileName;

#ifdef THOT_ENABLE_BDBENV
      // create environment
  envPtr=new DbEnv(0);
  std::string envName=extractDirName(outputFilesPrefix);
  u_int32_t env_o_flags = DB_CREATE|DB_INIT_MPOOL;
  u_int32_t env_cachesize=8*1024;
  envPtr->open(envName.c_str(),env_o_flags,0);
  envPtr->set_cachesize(0,env_cachesize,1);
#else
  envPtr=NULL;
#endif
      // open databases
  u_int32_t o_flags = DB_CREATE|DB_NOMMAP; // Open flags
  
  std::string phrDictDbName=outputFilesPrefix+".fbdb_phrdict";
  phrDictDb=new Db(envPtr,0);
      // Set comparison function for phrDictDb
  int ret=phrDictDb->set_bt_compare(static_phr_dict_cmp_func);
  if(ret)
    return THOT_ERROR;
  
  ret=phrDictDb->open(NULL,phrDictDbName.c_str(),NULL,DB_BTREE,o_flags,0);
  if(ret)
    return THOT_ERROR;

      // Verify existence of fast search availability flag (if it does
      // not exist, create and put its value to zero)
  if(!fastSearchAvailableFlagIsDefined())
    resetFastSearchAvailableFlag();
    
  return THOT_OK;
}

//-------------------------
void FastBdbPhraseTable::initDbtKey(Dbt& key,
                                    PhrDictKey& phrDictKey)
{
  DBT* dbtStructPtr=key.get_DBT();
  memset(dbtStructPtr,0,sizeof(DBT));
  dbtStructPtr->data=phrDictKey.words;
  dbtStructPtr->size=phrDictKey.getSize();
  dbtStructPtr->ulen=phrDictKey.getSize();
  dbtStructPtr->flags=DB_DBT_USERMEM;
}

//-------------------------
void FastBdbPhraseTable::initDbtKeyCursor(Dbt& key,
                                          PhrDictKey& phrDictKey)
{
  DBT* dbtStructPtr=key.get_DBT();
  memset(dbtStructPtr,0,sizeof(DBT));
  dbtStructPtr->data=phrDictKey.words;
  size_t keysize=sizeof(PhrDictKey);
  dbtStructPtr->size=keysize;
  dbtStructPtr->ulen=keysize;
  dbtStructPtr->flags=DB_DBT_USERMEM;
}

//-------------------------
void FastBdbPhraseTable::initDbtData(Dbt& data,
                                     PhrDictValue& phrDictValue)
{
  DBT* dbtStructPtr=data.get_DBT();
  memset(dbtStructPtr,0,sizeof(DBT));
  dbtStructPtr->data=&phrDictValue;
  size_t datasize=sizeof(phrDictValue);
  dbtStructPtr->size=datasize;
  dbtStructPtr->ulen=datasize;
  dbtStructPtr->flags=DB_DBT_USERMEM;
}

//-------------------------
int FastBdbPhraseTable::retrieveDataForPhrDict(const Vector<WordIndex>& s,
                                               const Vector<WordIndex>& t,
                                               PhrDictValue& phrDictValue)
{  
      // Initialize key/data pair
  PhrDictKey phrDictKey;
  int ret=phrDictKey.setPhrPair(s,t);
  if(ret==THOT_ERROR)
    return THOT_ERROR;
  Dbt key;
  initDbtKey(key,phrDictKey);
  
  Dbt data;
  initDbtData(data,phrDictValue);
    
      // Retrieve key/data pair from database
  ret=phrDictDb->get(NULL,&key,&data,0);
  if(ret)
  {
    return THOT_ERROR;
  }
  else
  {
    return THOT_OK;
  }
}

//-------------------------
int FastBdbPhraseTable::putDataForPhrDict(const Vector<WordIndex>& s,
                                          const Vector<WordIndex>& t,
                                          Count c)
{
      // Initialize key/data pair
  PhrDictKey phrDictKey;
  int ret=phrDictKey.setPhrPair(s,t);
  if(ret==THOT_ERROR)
    return THOT_ERROR;
  Dbt key;
  initDbtKey(key,phrDictKey);

  PhrDictValue phrDictValue;
  phrDictValue.count=c;
  Dbt data;
  initDbtData(data,phrDictValue);

      // Put record
  ret=phrDictDb->put(NULL,&key,&data,0);
  if(ret)
    return THOT_ERROR;
  else
    return THOT_OK;
}

//-------------------------
int FastBdbPhraseTable::incrPhrDictCount(const Vector<WordIndex>& s,
                                         const Vector<WordIndex>& t,
                                         Count c)
{
  PhrDictValue phrDictValue;
  int ret=retrieveDataForPhrDict(s,t,phrDictValue);
  if(ret==THOT_ERROR)
  {
        // Entry was not found
    ret=putDataForPhrDict(s,t,c);
    if(ret)
      return THOT_ERROR;
    else
      return THOT_OK;
  }
  else
  {
        // Entry was found
    ret=putDataForPhrDict(s,t,phrDictValue.count+c);
    if(ret)
      return THOT_ERROR;
    else
      return THOT_OK;
  }
}

//-------------------------
void FastBdbPhraseTable::incrCountsOfEntry(const Vector<WordIndex>& s,
                                           const Vector<WordIndex>& t,
                                           Count c)
{
      // Fast search is no longer available
  resetFastSearchAvailableFlag();

  Vector<WordIndex> emptyPhrase;
  incrPhrDictCount(s,t,c);
  incrPhrDictCount(s,emptyPhrase,c);
  incrPhrDictCount(emptyPhrase,t,c);
}

//-------------------------
void FastBdbPhraseTable::enableFastSearch(void)
{
      // Verify if fast search is already enabled
  if(!getFastSearchAvailableFlag())
  {
        // Define cursor
    Dbc* cursorPtr;
    phrDictDb->cursor(NULL,&cursorPtr,0);

        // Initialize key/data pair
    PhrDictKey phrDictKey;
    Dbt key;
    initDbtKeyCursor(key,phrDictKey);
    
    PhrDictValue phrDictValue;
    Dbt data;
    initDbtData(data,phrDictValue);

        // Iterate over dictionary entries
    while(cursorPtr->get(&key, &data, DB_NEXT)==0)
    {
          // Retrieve key and data for entry
      Vector<WordIndex> curr_s;
      Vector<WordIndex> curr_t;
      phrDictKey.getPhrPair(curr_s,curr_t);
      
      if(!curr_s.empty() && !curr_t.empty())
      {
            // Obtain count of source phrase
        bool found;
        Count srcCount=getSrcInfo(curr_s,found);
      
        if(found)
        {
              // Update source phrase count for entry
          Dbt keyToBeUpdated;
          initDbtKey(keyToBeUpdated,phrDictKey);
          
          PhrDictValue newPhrDictValue;
          newPhrDictValue.count=phrDictValue.count;
          newPhrDictValue.auxCount=srcCount;
          Dbt updatedData;
          initDbtData(updatedData,newPhrDictValue);
          
          cursorPtr->put(&keyToBeUpdated, &updatedData, DB_CURRENT);        
        }
      }
          // Reset key
      phrDictKey.clear();
    }
        // Close cursor
    if(cursorPtr!=NULL)
      cursorPtr->close();

        // After finishing, the database is ready for fast search
    setFastSearchAvailableFlag();
  }
}

//-------------------------
Count FastBdbPhraseTable::getSrcInfo(const Vector<WordIndex>& s,
                                     bool& found)
{
  PhrDictValue phrDictValue;
  Vector<WordIndex> emptyPhrase;
  int ret=retrieveDataForPhrDict(s,emptyPhrase,phrDictValue);
  if(ret==THOT_ERROR)
  {
        // Entry was not found
    found=false;
    return 0;
  }
  else
  {
    found=true;
    return phrDictValue.count;
  }
}

//-------------------------
Count FastBdbPhraseTable::getTrgInfo(const Vector<WordIndex>& t,
                                     bool& found)
{
  PhrDictValue phrDictValue;
  Vector<WordIndex> emptyPhrase;
  int ret=retrieveDataForPhrDict(emptyPhrase,t,phrDictValue);
  if(ret==THOT_ERROR)
  {
        // Entry was not found
    found=false;
    return 0;
  }
  else
  {
    found=true;
    return phrDictValue.count;
  }
}

//-------------------------
Count FastBdbPhraseTable::getSrcTrgInfo(const Vector<WordIndex>& s,
                                        const Vector<WordIndex>& t,
                                        bool &found)
{
  PhrDictValue phrDictValue;
  int ret=retrieveDataForPhrDict(s,t,phrDictValue);
  if(ret==THOT_ERROR)
  {
        // Entry was not found
    found=false;
    return 0;
  }
  else
  {
    found=true;
    return phrDictValue.count;
  }
}

//-------------------------
bool FastBdbPhraseTable::getEntriesForTarget(const Vector<WordIndex>& t,
                                             SrcTableNode& srctn)
{
      // Define cursor
  Dbc* cursorPtr;
  phrDictDb->cursor(NULL,&cursorPtr,0);

      // Position the cursor in the first ocurrence of t in the
      // phrase dictionary
  srctn.clear();

      // Initialize key/data pair
  PhrDictKey phrDictKey;
  Vector<WordIndex> emptyPhrase;
  phrDictKey.setPhrPair(emptyPhrase,t);
  Dbt key;
  initDbtKeyCursor(key,phrDictKey);

  PhrDictValue phrDictValue;
  Dbt data;
  initDbtData(data,phrDictValue);
  
  int ret=cursorPtr->get(&key, &data, DB_SET_RANGE);
  if(ret)
  {
        // Close cursor
    if(cursorPtr!=NULL)
      cursorPtr->close();
    srctn.clear();
    return false;
  }

      // Store count for t
  Count trgPhrCount=phrDictValue.count;
  
      // Use the cursor to iterate over translations for t
  do
  {
    Vector<WordIndex> curr_t;
    Vector<WordIndex> curr_s;
    phrDictKey.getPhrPair(curr_s,curr_t);
    if(curr_t==t)
    {
      if(!curr_s.empty())
      {
            // Store translation option
        std::pair<Vector<WordIndex>,PhrasePairInfo> pVecPhinfo;
        pVecPhinfo.first=curr_s;
        pVecPhinfo.second.first=trgPhrCount;
        pVecPhinfo.second.second=phrDictValue.count;
        srctn.insert(pVecPhinfo);
      }
    }
    else
      break;
        // Reset key data
    phrDictKey.clear();
  } while(cursorPtr->get(&key, &data, DB_NEXT)==0);


      // Close cursor
  if(cursorPtr!=NULL)
    cursorPtr->close();

  if(srctn.empty())
    return false;
  else
    return true;
}

//-------------------------
Prob FastBdbPhraseTable::pTrgGivenSrc(const Vector<WordIndex>& s,
                                      const Vector<WordIndex>& t)
{
  if(getFastSearchAvailableFlag())
  {
    PhrDictValue phrDictValue;
    int ret=retrieveDataForPhrDict(s,t,phrDictValue);
    if(ret==THOT_ERROR)
      return PHRASE_PROB_SMOOTH;
    else
      return (float) phrDictValue.count/(float) phrDictValue.auxCount;
  }
  else
  {
    bool found;
    Count count_s_t_=getSrcTrgInfo(s,t,found);
    if((float)count_s_t_>0)
    {
      Count count_s=getSrcInfo(s,found);
      if((float)count_s>0) 
      {
        return (float) count_s_t_/(float)count_s;
      }
      else
        return PHRASE_PROB_SMOOTH;
    }
    else
      return PHRASE_PROB_SMOOTH;
  }
}

//-------------------------
LgProb FastBdbPhraseTable::logpTrgGivenSrc(const Vector<WordIndex>& s,
                                           const Vector<WordIndex>& t)
{
  return log((double)pTrgGivenSrc(s,t));
}

//-------------------------
Prob FastBdbPhraseTable::pSrcGivenTrg(const Vector<WordIndex>& s,
                                      const Vector<WordIndex>& t)
{
  bool found;
  Count count_s_t_=getSrcTrgInfo(s,t,found);
  if((float)count_s_t_>0)
  {
    Count count_t_=getTrgInfo(t,found);
	if((float)count_t_>0) 
    {
      return (float) count_s_t_/(float)count_t_;
    }
	else
      return PHRASE_PROB_SMOOTH;
  }
  else
    return PHRASE_PROB_SMOOTH;
}

//-------------------------
LgProb FastBdbPhraseTable::logpSrcGivenTrg(const Vector<WordIndex>& s,
                                           const Vector<WordIndex>& t)
{
  return log((double)pSrcGivenTrg(s,t));  
}

//-------------------------
bool FastBdbPhraseTable::getNbestForTrg(const Vector<WordIndex>& t,
                                        NbestTableNode<PhraseTransTableNodeData>& nbt,
                                        int N)
{
      // Obtain entries for target phrase
  SrcTableNode srctn;
  bool found=getEntriesForTarget(t,srctn);
  if(!found)
    return false;

      // Put entries in NbestTableNode object
  nbt.clear();
  for(SrcTableNode::const_iterator citer=srctn.begin();citer!=srctn.end();++citer)
  {
    LgProb lgProb=log((float)citer->second.second/(float)citer->second.first);
    nbt.insert(lgProb,citer->first);
  }
  
      // Keep N-best entries
  while(nbt.size()>(unsigned int)N && N>=0)
  {
        // node contains N inverse translations, remove last element
    nbt.removeLastElement();
  }

  return true;
}

//-------------------------
size_t FastBdbPhraseTable::size(void)
{
  cerr<<"Warning: size() function not implemented in FastBdbPhraseTable class"<<endl;

  return 0;
}

//-------------------------
void FastBdbPhraseTable::clear(void)
{
  if(phrDictDb!=NULL)
  {
    phrDictDb->close(0);
    delete phrDictDb;
    phrDictDb=NULL;
  }

  if(envPtr!=NULL)
  {
    envPtr->close(0);
    delete envPtr;
    envPtr=NULL;
  }
}

//---------------
void FastBdbPhraseTable::setFastSearchAvailableFlag(void)
{
  Vector<WordIndex> emptyWordVec;
  putDataForPhrDict(emptyWordVec,emptyWordVec,1);
}

//---------------
void FastBdbPhraseTable::resetFastSearchAvailableFlag(void)
{
  Vector<WordIndex> emptyWordVec;
  putDataForPhrDict(emptyWordVec,emptyWordVec,0);
}

//---------------
bool FastBdbPhraseTable::getFastSearchAvailableFlag(void)
{
  Vector<WordIndex> emptyWordVec;
  PhrDictValue phrDictValue;
  int ret=retrieveDataForPhrDict(emptyWordVec,emptyWordVec,phrDictValue);
  if(ret==THOT_OK)
  {
    if((double)phrDictValue.count>0.0)
      return true;
    else
      return false;
  }
  else
    return false;
}

//---------------
bool FastBdbPhraseTable::fastSearchAvailableFlagIsDefined(void)
{
  Vector<WordIndex> emptyWordVec;
  PhrDictValue phrDictValue;
  int ret=retrieveDataForPhrDict(emptyWordVec,emptyWordVec,phrDictValue);
  if(ret==THOT_OK)
    return true;
  else
    return false;
}

//---------------
std::string FastBdbPhraseTable::extractDirName(std::string filePath)
{
  if(filePath.empty())
  {
    std::string dirName;
    return dirName;
  }
  else
  {
        // Provided file path is not empty
    int last_slash_pos=-1;

        // Find last position of slash symbol
    for(unsigned int i=0;i<filePath.size();++i)
      if(filePath[i]=='/')
        last_slash_pos=i;

        // Check if any slash symbols were found
    if(last_slash_pos==-1)
    {
      std::string dirName;
      return dirName;
    }
    else
    {
          // The last slash symbol was found at "last_slash_pos"
      return filePath.substr(0,last_slash_pos+1);
    }
  }
}

//-------------------------
FastBdbPhraseTable::~FastBdbPhraseTable()
{
  clear();
}

//-------------------------
