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
#define WORD_INDEX_MODULO_BYTES 3  // TODO 3 or 4 bytes?

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
    bool retrieveData(const Vector<WordIndex>& phrase, int &count)const;
    bool storeData(const Vector<WordIndex>& phrase, int count);

      // Concatenate s and t phrases
    Vector<WordIndex> getSrcTrg(const Vector<WordIndex>& s, const WordIndex& t)const;

        // Returns information related to a given key.
    Count getInfo(const Vector<WordIndex>& key,bool &found);
    Count getTrgInfo(const WordIndex& t, bool &found);

 public:

    typedef typename BaseIncrCondProbTable<Vector<WordIndex>,WordIndex,Count,Count>::SrcTableNode SrcTableNode;
    typedef typename BaseIncrCondProbTable<Vector<WordIndex>,WordIndex,Count,Count>::TrgTableNode TrgTableNode;

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
      // TODO t: Vector<WordIndex> ===> WordIndex
      // TODO Add empty key to size, iter etc.
      // TODO Ordering by n-gram value
    void addTableEntry(const Vector<WordIndex>& s, const WordIndex& t, im_pair<Count,Count> inf);
    void addSrcInfo(const Vector<WordIndex>& s, Count s_inf);
    void addSrcTrgInfo(const Vector<WordIndex>& s, const WordIndex& t, Count st_inf);
    void incrCountsOfEntryLog(const Vector<WordIndex>& s,
                              const WordIndex& t,
                              LogCount lc);
    im_pair<Count,Count> infSrcTrg(const Vector<WordIndex>& s,
                                   const WordIndex& t,
                                   bool& found);
    Count getSrcInfo(const Vector<WordIndex>& s,bool& found);
    Count getSrcTrgInfo(const Vector<WordIndex>& s,const WordIndex& t,bool& found);
    Prob pTrgGivenSrc(const Vector<WordIndex>& s,const WordIndex& t);
    LgProb logpTrgGivenSrc(const Vector<WordIndex>& s,const WordIndex& t);
    Prob pSrcGivenTrg(const Vector<WordIndex>& s,const WordIndex& t);
    LgProb logpSrcGivenTrg(const Vector<WordIndex>& s,const WordIndex& t);
  bool getEntriesForSource(const Vector<WordIndex>& s,TrgTableNode& trgtn);  // TODO
  bool getEntriesForTarget(const WordIndex& t,SrcTableNode& tnode);  // TODO
  bool getNbestForSrc(const Vector<WordIndex>& s,NbestTableNode<WordIndex>& nbt);  // TODO
  bool getNbestForTrg(const WordIndex& t,NbestTableNode<Vector<WordIndex> >& nbt,int N=-1);  // TODO

      // Count-related functions
  Count cSrcTrg(const Vector<WordIndex>& s,const WordIndex& t);
  Count cSrc(const Vector<WordIndex>& s);
  Count cTrg(const WordIndex& t);  // TODO
  LogCount lcSrcTrg(const Vector<WordIndex>& s,const WordIndex& t);  // TODO
  LogCount lcSrc(const Vector<WordIndex>& s);  // TODO
  LogCount lcTrg(const WordIndex& t);  // TODO

      // Size, clear functions
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

//--------------- Template function definitions
/*
//-------------------------
template<class WordIndex,class Count,class Count>
bool LevelDbNgramTable<WordIndex,Count,Count>::getEntriesForSource(const Vector<WordIndex>& s,
                                                                      TrgTableNode& trgtn)
{
  typename SrcTrgInfo::const_iterator titer;
  unsigned int i;
  pair<WordIndex,im_pair<Count,Count> > pdp;
  Vector<WordIndex> vecx;
  Count* siPtr;

  siPtr=srcInfo.find(s);
  
  trgtn.clear();
  for(titer=srcTrgInfo.begin();titer!=srcTrgInfo.end();++titer)
  {
    if((double)titer->second.get_c_st()!=0)
    {
      if(titer->first.size()>1)
      {
        vecx.clear();
        for(i=0;i<titer->first.size()-1;++i)
        {
          vecx.push_back(titer->first[i]);
        }
        if(vecx==s)
        {
          pdp.first=titer->first.back();
          pdp.second.first=*siPtr;
          pdp.second.second=titer->second;
          trgtn.insert(pdp);
        }
      }
    }
  }
  if(trgtn.size()>0) return true;
  else return false;  
}

//-------------------------
template<class WordIndex,class Count,class Count>
bool
LevelDbNgramTable<WordIndex,Count,Count>::getEntriesForTarget(const WordIndex& t,typename LevelDbNgramTable<WordIndex,Count,Count>::SrcTableNode& tnode)
{
  typename SrcTrgInfo::const_iterator titer;
  unsigned int i;
  pair<Vector<WordIndex>,im_pair<Count,Count> > pdp;

  tnode.clear();
  for(titer=srcTrgInfo.begin();titer!=srcTrgInfo.end();++titer)
  {
    if((double)titer->second.get_c_st()!=0)
    {
      if(titer->first.size()>1)
      {
        if(titer->first.back()==t)
        {
          pdp.first.clear();
          for(i=0;i<titer->first.size()-1;++i)
          {
            pdp.first.push_back(titer->first[i]);
          }
          pdp.second.first=*srcInfo.find(pdp.first);
          pdp.second.second=titer->second;
          tnode.insert(pdp);
        }
      }
    }
  }
  if(tnode.size()>0) return true;
  else return false;
}
//-------------------------
template<class WordIndex,class Count,class Count>
bool LevelDbNgramTable<WordIndex,Count,Count>::getNbestForSrc(const Vector<WordIndex>& s,
                                                                 NbestTableNode<WordIndex>& nbt)
{
  TrgTableNode tnode;
  typename TrgTableNode::iterator tNodeIter;
  Count* siPtr;
  bool ret;

  nbt.clear();
  ret=getEntriesForSource(s,tnode);

  siPtr=srcInfo.find(s);

  for(tNodeIter=tnode.begin();tNodeIter!=tnode.end();++tNodeIter)
  {
    nbt.insert((float)tNodeIter->second.second.get_lc_st()-(float)siPtr->get_lc_s(),tNodeIter->first);
  }
  return ret;  
}

//-------------------------
template<class WordIndex,class Count,class Count>
bool LevelDbNgramTable<WordIndex,Count,Count>::getNbestForTrg(const WordIndex& t,
                                                                 NbestTableNode<Vector<WordIndex> >& nbt,
                                                                 int N)
{
  SrcTableNode tnode;
  typename SrcTableNode::iterator tNodeIter;
  bool ret;
  
  nbt.clear();
  ret=getEntriesForTarget(t,tnode);
  for(tNodeIter=tnode.begin();tNodeIter!=tnode.end();++tNodeIter)
  {
    Count* siPtr;

    siPtr=srcInfo.find(tNodeIter->first);
    nbt.insert((float)tNodeIter->second.second.get_lc_st()-(float)siPtr->get_lc_s(),tNodeIter->first);
  }

  if(N>=0)
    while(nbt.size()>(unsigned int) N) nbt.removeLastElement();
  
  return ret;
}


//-------------------------
template<class WordIndex,class Count,class Count>
Count LevelDbNgramTable<WordIndex,Count,Count>::cTrg(const WordIndex& t)
{
  typename SrcTrgInfo::const_iterator titer;
  Count c_t=SMALL_LG_NUM;
  
  for(titer=srcTrgInfo.begin();titer!=srcTrgInfo.end();++titer)
  {
    if((double)titer->second.get_c_st()>0)
    {
      if(titer->first.size()>1)
      {
        if(titer->first[0]==t)
        {
          c_t=(float)c_t+(float)titer->second.get_c_st();
        }
      }
    }
  }
  return c_t;
}

//-------------------------
template<class WordIndex,class Count,class Count>
LogCount LevelDbNgramTable<WordIndex,Count,Count>::lcSrcTrg(const Vector<WordIndex>& s,
                                                               const WordIndex& t)
{
  Count* stiPtr;
  Vector<WordIndex> vecx;
  unsigned int i;
  
  for(i=0;i<s.size();++i)
  {
    vecx.push_back(s[i]);
  }
  vecx.push_back(t);

  stiPtr=srcTrgInfo.find(vecx);

  if(stiPtr==NULL)
  {
    return SMALL_LG_NUM;
  }
  else
  {
    return stiPtr->get_lc_st();
  }
}

//-------------------------
template<class WordIndex,class Count,class Count>
LogCount LevelDbNgramTable<WordIndex,Count,Count>::lcSrc(const Vector<WordIndex>& s)
{
  Count* siPtr;

  if(s.size()==0)
  {
    return srcInfoNull.get_lc_s();
  }
  else
  {
    siPtr=srcInfo.find(s);
    if(siPtr==NULL)
    {
      return SMALL_LG_NUM;
    }
    else return siPtr->get_lc_s();
  }
}

//-------------------------
template<class WordIndex,class Count,class Count>
LogCount LevelDbNgramTable<WordIndex,Count,Count>::lcTrg(const WordIndex& t)
{
  typename SrcTrgInfo::const_iterator titer;
  LogCount lc_t=SMALL_LG_NUM;
  
  for(titer=srcTrgInfo.begin();titer!=srcTrgInfo.end();++titer)
  {
    if((double)titer->second.get_lc_st()>SMALL_LG_NUM)
    {
      if(titer->first.size()>1)
      {
        if(titer->first[0]==t)
        {
          lc_t=MathFuncs::lns_sumlog(lc_t,titer->second.get_lc_st());
        }
      }
    }
  }
  return lc_t;
}*/


#endif
