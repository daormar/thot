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
/* Module: BdbPhraseTable                                           */
/*                                                                  */
/* Definitions file: BdbPhraseTable.cc                              */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "BdbPhraseTable.h"

//--------------- Function definitions

//-------------------------
BdbPhraseTable::BdbPhraseTable(void)
{
  envPtr=NULL;
  phrDictDb=NULL;
}

//-------------------------
int phr_dict_cmp_func(Db* /*db*/,
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
int phr_dict_cmp_func(Db* db,
                      const Dbt* dbt1,
                      const Dbt* dbt2,
                      size_t* /*locp*/)
{
  return phr_dict_cmp_func(db,dbt1,dbt2);
}

//-------------------------
bool BdbPhraseTable::init(const char *fileName)
{
  cerr<<"Initializing BDB phrase table"<<endl;

      // clear object
  clear();

  std::string outputFilesPrefix=fileName;

#ifdef THOT_ENABLE_BDBENV
      // create environment
  envPtr=new DbEnv(0);
  std::string envName=extractDirName(outputFilesPrefix);
  u_int32_t env_o_flags = DB_CREATE|DB_INIT_MPOOL;
  u_int32_t env_cachesize=256 *1024;
  envPtr->open(envName.c_str(),env_o_flags,0);
  envPtr->set_cachesize(0,env_cachesize,1);
#else
  envPtr=NULL;
#endif
  
      // open databases
  u_int32_t o_flags = DB_CREATE; // Open flags
  
  std::string phrDictDbName=outputFilesPrefix+".bdb_phrdict";
  phrDictDb=new Db(envPtr,0);
      // Set comparison function for phrDictDb
  int ret=phrDictDb->set_bt_compare(phr_dict_cmp_func);
  if(ret)
    return ERROR;
  ret=phrDictDb->open(NULL,phrDictDbName.c_str(),NULL,DB_BTREE,o_flags,0);
  if(ret)
    return ERROR;
  
  return THOT_OK;
}

//-------------------------
void BdbPhraseTable::encodeKeyDataForPhrDictDb(PhrDictKey& phrDictKey,
                                               PhrDictValue& phrDictValue,
                                               Dbt& key,
                                               Dbt& data)
{  
  key.set_data(&phrDictKey);
  key.set_size(phrDictKey.getSize());
  data.set_data(&phrDictValue);
  data.set_size(sizeof(PhrDictValue));
}

//-------------------------
void BdbPhraseTable::decodeKeyDataForPhrDictDb(PhrDictKey& phrDictKey,
                                               PhrDictValue& phrDictValue,
                                               Dbt& key,
                                               Dbt& data)
{
      // Decode key
  PhrDictKey* keyPtr=(PhrDictKey*) key.get_data();
  u_int32_t keySize=key.get_size();
  unsigned int numWords=keySize/sizeof(WordIndex);
  phrDictKey.clear();
  for(unsigned int i=0;i<numWords;++i)
    phrDictKey.words[i]=keyPtr->words[i];

      // Decode data
  PhrDictValue* valPtr=(PhrDictValue*) data.get_data();
  phrDictValue.count=valPtr->count;
}

//-------------------------
int BdbPhraseTable::retrieveDataForPhrDict(const Vector<WordIndex>& s,
                                           const Vector<WordIndex>& t,
                                           PhrDictValue& phrDictValue)
{
      // Obtain source phrase index
  PhrDictKey phrDictKey;
  int ret=phrDictKey.setPhrPair(s,t);
  if(ret==ERROR)
    return ERROR;
  Dbt key;
  Dbt data;
  encodeKeyDataForPhrDictDb(phrDictKey,phrDictValue,key,data);
  
      // Retrieve key/data pair from database
  ret=phrDictDb->get(NULL,&key,&data,0);
  if(ret)
    return ERROR;
  else
  {
    decodeKeyDataForPhrDictDb(phrDictKey,phrDictValue,key,data);
    return THOT_OK;
  }
}

//-------------------------
int BdbPhraseTable::putDataForPhrDict(const Vector<WordIndex>& s,
                                      const Vector<WordIndex>& t,
                                      Count c)
{
      // Encode key/value pair
  PhrDictKey phrDictKey;
  int ret=phrDictKey.setPhrPair(s,t);
  if(ret==ERROR)
    return ERROR;
  PhrDictValue phrDictValue;
  phrDictValue.count=c;
  Dbt key;
  Dbt data;
  encodeKeyDataForPhrDictDb(phrDictKey,phrDictValue,key,data);
  
      // Put record
  ret=phrDictDb->put(NULL,&key,&data,0);
  if(ret)
    return ERROR;
  else
    return THOT_OK;
}

//-------------------------
int BdbPhraseTable::incrPhrDictCount(const Vector<WordIndex>& s,
                                     const Vector<WordIndex>& t,
                                     Count c)
{
  PhrDictValue phrDictValue;
  int ret=retrieveDataForPhrDict(s,t,phrDictValue);
  if(ret==ERROR)
  {
        // Entry was not found

    ret=putDataForPhrDict(s,t,c);
    if(ret)
      return ERROR;
    else
      return THOT_OK;
  }
  else
  {
        // Entry was found

    ret=putDataForPhrDict(s,t,phrDictValue.count+c);
    if(ret)
      return ERROR;
    else
      return THOT_OK;
  }
}

//-------------------------
void BdbPhraseTable::incrCountsOfEntry(const Vector<WordIndex>& s,
                                       const Vector<WordIndex>& t,
                                       Count c)
{
  Vector<WordIndex> emptyPhrase;
  incrPhrDictCount(s,t,c);
  incrPhrDictCount(s,emptyPhrase,c);
  incrPhrDictCount(emptyPhrase,t,c);
}

//-------------------------
Count BdbPhraseTable::getSrcInfo(const Vector<WordIndex>& s,
                                 bool& found)
{
  PhrDictValue phrDictValue;
  Vector<WordIndex> emptyPhrase;
  int ret=retrieveDataForPhrDict(s,emptyPhrase,phrDictValue);
  if(ret==ERROR)
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
Count BdbPhraseTable::getTrgInfo(const Vector<WordIndex>& t,
                                 bool& found)
{
  PhrDictValue phrDictValue;
  Vector<WordIndex> emptyPhrase;
  int ret=retrieveDataForPhrDict(emptyPhrase,t,phrDictValue);
  if(ret==ERROR)
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
Count BdbPhraseTable::getSrcTrgInfo(const Vector<WordIndex>& s,
                                    const Vector<WordIndex>& t,
                                    bool &found)
{
  PhrDictValue phrDictValue;
  int ret=retrieveDataForPhrDict(s,t,phrDictValue);
  if(ret==ERROR)
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
bool BdbPhraseTable::getEntriesForTarget(const Vector<WordIndex>& t,
                                         SrcTableNode& srctn)
{
      // Find translations for t
  
      // Define cursor
  Dbc* cursorPtr;
  phrDictDb->cursor(NULL,&cursorPtr,0);

      // Position the cursor in the first ocurrence of t in the
      // phrase dictionary
  srctn.clear();
  PhrDictKey phrDictKey;
  Vector<WordIndex> emptyPhrase;
  phrDictKey.setPhrPair(emptyPhrase,t);
  Dbt key(&phrDictKey,phrDictKey.getSize());
  Dbt data;
  int ret=cursorPtr->get(&key, &data, DB_SET_RANGE);
  if(ret)
  {
    srctn.clear();
    return false;
  }

      // Store count for t
  PhrDictValue* phrDictValuePtr=(PhrDictValue*) data.get_data();
  Count trgPhrCount=phrDictValuePtr->count;
  
      // Use the cursor to iterate over translations for t
  do
  {
    PhrDictValue phrDictValue;
    decodeKeyDataForPhrDictDb(phrDictKey,phrDictValue,key,data);
    Vector<WordIndex> curr_t;
    Vector<WordIndex> curr_s;
    phrDictKey.getPhrPair(curr_s,curr_t);
    if(curr_t==t)
    {
      if(!curr_s.empty())
      {
            // Store translation option
        pair<Vector<WordIndex>,PhrasePairInfo> pVecPhinfo;
        pVecPhinfo.first=curr_s;
        pVecPhinfo.second.first=trgPhrCount;
        pVecPhinfo.second.second=phrDictValue.count;
        srctn.insert(pVecPhinfo);
      }
    }
    else
      break;
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
Prob BdbPhraseTable::pTrgGivenSrc(const Vector<WordIndex>& s,
                                  const Vector<WordIndex>& t)
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

//-------------------------
LgProb BdbPhraseTable::logpTrgGivenSrc(const Vector<WordIndex>& s,
                                       const Vector<WordIndex>& t)
{
  return log((double)pTrgGivenSrc(s,t));
}

//-------------------------
Prob BdbPhraseTable::pSrcGivenTrg(const Vector<WordIndex>& s,
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
LgProb BdbPhraseTable::logpSrcGivenTrg(const Vector<WordIndex>& s,
                                       const Vector<WordIndex>& t)
{
  return log((double)pSrcGivenTrg(s,t));  
}

//-------------------------
bool BdbPhraseTable::getNbestForTrg(const Vector<WordIndex>& t,
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
size_t BdbPhraseTable::size(void)
{
  cerr<<"Warning: size() function not implemented in BdbPhraseTable class"<<endl;

  return 0;
}

//-------------------------
void BdbPhraseTable::clear(void)
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
std::string BdbPhraseTable::extractDirName(std::string filePath)
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
BdbPhraseTable::~BdbPhraseTable()
{
  clear();
}

//-------------------------
