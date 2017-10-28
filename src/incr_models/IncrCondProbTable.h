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
/* Module: IncrCondProbTable                                        */
/*                                                                  */
/* Prototype file: IncrCondProbTable                                */
/*                                                                  */
/* Description: Generic class to manage incremental conditional     */
/*              probability tables p(t|s).                          */
/*                                                                  */
/********************************************************************/

#ifndef _IncrCondProbTable
#define _IncrCondProbTable

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseIncrCondProbTable.h"

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------

//--------------- IncrCondProbTable class

template<class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
class IncrCondProbTable: public BaseIncrCondProbTable<SRCDATA,TRGDATA,SRCTRG_INFO,SRC_INFO>
{
 public:
  
  typedef typename BaseIncrCondProbTable<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::SrcTableNode SrcTableNode;
  typedef typename BaseIncrCondProbTable<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::TrgTableNode TrgTableNode;
  typedef std::map<SRCDATA,SRCTRG_INFO> TrgSrcInfoNode;
  typedef std::map<TRGDATA,TrgSrcInfoNode> TrgSrcInfo;
  typedef std::map<SRCDATA,SRC_INFO> SrcDataInfo;
  
      // Basic functions
  void addTableEntry(const SRCDATA& s,
                     const TRGDATA& t,
                     im_pair<SRC_INFO,SRCTRG_INFO> inf);
  void addSrcInfo(const SRCDATA& s,SRC_INFO s_inf);
  void addSrcTrgInfo(const SRCDATA& s,const TRGDATA& t,SRCTRG_INFO st_inf);
  void incrCountsOfEntryLog(const SRCDATA& s,
                            const TRGDATA& t,
                            LogCount lc);
  im_pair<SRC_INFO,SRCTRG_INFO> infSrcTrg(const SRCDATA& s,
                                          const TRGDATA& t,
                                          bool& found);
  SRC_INFO getSrcInfo(const SRCDATA& s,
                      bool& found);
  SRCTRG_INFO getSrcTrgInfo(const SRCDATA& s,
                            const TRGDATA& t,
                            bool& found);
  Prob pTrgGivenSrc(const SRCDATA& s,
                    const TRGDATA& t);
  LgProb logpTrgGivenSrc(const SRCDATA& s,
                         const TRGDATA& t);
  Prob pSrcGivenTrg(const SRCDATA& s,
                    const TRGDATA& t);
  LgProb logpSrcGivenTrg(const SRCDATA& s,
                         const TRGDATA& t);
  bool getEntriesForTarget(const TRGDATA& t,
                           SrcTableNode& tnode);
  bool getEntriesForSource(const SRCDATA& s,
                           TrgTableNode& trgtn);
  bool getNbestForSrc(const SRCDATA& s,
                      NbestTableNode<TRGDATA>& nbt);
  bool getNbestForTrg(const TRGDATA& t,
                      NbestTableNode<SRCDATA>& nbt,
                      int N=-1);

       // Count-related functions
  LogCount lcSrcTrg(const SRCDATA& s,const TRGDATA& t);
  LogCount lcSrc(const SRCDATA& s);
  LogCount lcTrg(const TRGDATA& t);

      // size, clear functions
  size_t size(void);
  void clear(void);

      // Destructor
  ~IncrCondProbTable(){}
   
      // const_iterator
  class const_iterator;
  friend class const_iterator;
  class const_iterator
  {
   protected:
    typename TrgSrcInfo::const_iterator tmIter;
    
   public:
    const_iterator(void){}
    const_iterator(typename TrgSrcInfo::const_iterator _tmIter)
    {
      tmIter=_tmIter;
    }
    bool operator++(void) //prefix
    {
      typename TrgSrcInfo::const_iterator tmIterAux;
      
      tmIterAux=tmIter;
      ++tmIter;
      return tmIterAux!=tmIter;
    }
    bool operator++(int)  //postfix
    {
      return operator++();
    }
    int operator==(const const_iterator& right)
    {
      return this->tmIter==right.tmIter;
    }
    int operator!=(const const_iterator& right)
    {
      return !(*this==right);
    }
    const typename TrgSrcInfo::const_iterator& operator->(void)const
    {
      return tmIter;
    }
    std::pair<TRGDATA,TrgSrcInfoNode> operator*(void)const
    {
      return *tmIter;
    }
    ~const_iterator(){}
  };
      // const_iterator-related functions
  const_iterator begin(void)const;
  const_iterator end(void)const;
  
 protected:
  TrgSrcInfo table;
  SrcDataInfo srcDataInfo;
};

//--------------- Template function definitions

//-------------------------
template<class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
size_t IncrCondProbTable<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::size(void)
{
 return table.size();
}
//-------------------------
template<class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
void IncrCondProbTable<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::clear(void)
{
 table.clear();
 srcDataInfo.clear();
}
//-------------------------
template<class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
void IncrCondProbTable<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::addTableEntry(const SRCDATA& s,
                                                                            const TRGDATA& t,
                                                                            im_pair<SRC_INFO,SRCTRG_INFO> inf)
{
  addSrcInfo(s,inf.first);
  addSrcTrgInfo(s,t,inf.second);
}

//-------------------------
template<class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
void IncrCondProbTable<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::addSrcInfo(const SRCDATA& s,
                                                                         SRC_INFO s_inf)
{
  srcDataInfo[s]=s_inf;  
}

//-------------------------
template<class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
void IncrCondProbTable<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::addSrcTrgInfo(const SRCDATA& s,
                                                                            const TRGDATA& t,
                                                                            SRCTRG_INFO st_inf)
{
  typename TrgSrcInfo::iterator trgSrcInfoIter;
  typename TrgSrcInfoNode::iterator tNodeIter;
  TrgSrcInfoNode tNode;
  std::pair<SRCDATA,SRCTRG_INFO> tNodePair;	
  
  trgSrcInfoIter=table.find(t);
  if(trgSrcInfoIter!=table.end())
  {
    tNodeIter=trgSrcInfoIter->second.find(s);
    if(tNodeIter!=trgSrcInfoIter->second.end()) 
    {
      tNodeIter->second=st_inf;
    }
    else 
    {
      tNodePair.first=s;
      tNodePair.second=st_inf;
      trgSrcInfoIter->second.insert(tNodePair);
    }
  }
  else
  {
    tNodePair.first=s;
    tNodePair.second=st_inf;
    tNode.insert(tNodePair);
    table[t]=tNode;  
  }
}

//-------------------------
template<class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
void IncrCondProbTable<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::incrCountsOfEntryLog(const SRCDATA& s,
                                                                                   const TRGDATA& t,
                                                                                   LogCount lc)
{
  typename TrgSrcInfoNode::iterator trgSrcNodeIter;
  typename SrcDataInfo::iterator srcDataInfoIter;
  typename TrgSrcInfo::iterator trgSrcInfoIter;

      // Increase source counts
  srcDataInfoIter=srcDataInfo.find(s);
  if(srcDataInfoIter==srcDataInfo.end())
  {
        // s not present
    SRC_INFO srci;
    srci.set_logcount((float)lc);
    srcDataInfo[s]=srci;
  }
  else
  {
        // s is already stored
    srcDataInfoIter->second.incr_logcount((float)lc);
    srcDataInfo[s]=srcDataInfoIter->second;
  }

      // Increase source-target counts
  trgSrcInfoIter=table.find(t);
  if(trgSrcInfoIter==table.end())
  {
        // t not present
    SRCTRG_INFO srctrgi;
    TrgSrcInfoNode trgSrcInfoNode;
    
    srctrgi.set_logcount((float)lc);
    trgSrcInfoNode[s]=srctrgi;
    table[t]=trgSrcInfoNode;
  }
  else
  {
        // t is already stored
        // find source within the table node for t...
    trgSrcNodeIter=trgSrcInfoIter->second.find(s);
    if(trgSrcNodeIter==trgSrcInfoIter->second.end())
    {
          // s not found
      SRCTRG_INFO srctrgi;
      srctrgi.set_logcount((float)lc);
      trgSrcInfoIter->second[s]=srctrgi;
    }
    else
    {
          // s found
      trgSrcNodeIter->second.incr_logcount((float)lc);
    }
  }  
}

//-------------------------
template<class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
im_pair<SRC_INFO,SRCTRG_INFO> IncrCondProbTable<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::infSrcTrg(const SRCDATA& s,
                                                                                                 const TRGDATA& t,
                                                                                                 bool& found)
{
 typename TrgSrcInfo::const_iterator trgSrcInfoIter;
 typename TrgSrcInfoNode::const_iterator trgSrcNodeIter;
 im_pair<SRC_INFO,SRCTRG_INFO> psst;
 typename SrcDataInfo::const_iterator sdcIter;
 
 found=false;
 trgSrcInfoIter=table.find(t);
 if(trgSrcInfoIter!=table.end())
 {
   trgSrcNodeIter=trgSrcInfoIter->second.find(s);
   if(trgSrcNodeIter!=trgSrcInfoIter->second.end())
   {
     psst.second=trgSrcNodeIter->second;
     sdcIter=srcDataInfo.find(s);
     psst.first=sdcIter->second;
     found=true;
     return psst;
   }
   else return psst; 
 }
 else return psst;  
}

//-------------------------
template<class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
SRC_INFO IncrCondProbTable<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::getSrcInfo(const SRCDATA& s,
                                                                             bool& found)
{
 SRC_INFO sinfo;
 typename SrcDataInfo::const_iterator sdcIter;
 
 found=false;

 sdcIter=srcDataInfo.find(s);
 if(sdcIter!=srcDataInfo.end())
 {
   sinfo=sdcIter->second;
   found=true;
   return sinfo;
 }
 else
 {
   found=false;
   return sinfo;
 }
}

//-------------------------
template<class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
SRCTRG_INFO IncrCondProbTable<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::getSrcTrgInfo(const SRCDATA& s,
                                                                                   const TRGDATA& t,
                                                                                   bool& found)
{
 typename TrgSrcInfo::const_iterator trgSrcInfoIter;
 typename TrgSrcInfoNode::const_iterator trgSrcNodeIter;
 
 trgSrcInfoIter=table.find(t);
 if(trgSrcInfoIter!=table.end())
 {
   trgSrcNodeIter=trgSrcInfoIter->second.find(s);
   if(trgSrcNodeIter!=trgSrcInfoIter->second.end())
   {
     found=true;
     return trgSrcNodeIter->second;
   }
   else
   {
     SRCTRG_INFO sti;

     found=false;
     return sti;     
   }
 }
 else
 {
   SRCTRG_INFO sti;

   found=false;
   return sti;
 }
}

//-------------------------
template<class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
Prob IncrCondProbTable<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::pTrgGivenSrc(const SRCDATA& s,
                                                                           const TRGDATA& t)
{
  return logpTrgGivenSrc(s,t).get_p();
}
//-------------------------
template<class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
LgProb IncrCondProbTable<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::logpTrgGivenSrc(const SRCDATA& s,
                                                                                const TRGDATA& t)
{
  im_pair<SRC_INFO,SRCTRG_INFO> psst;
  bool found;
  
  psst=infSrcTrg(s,t,found);
  if(!found)
  {
    return SMALL_LG_NUM;
  }
  else
  {
    if((float)psst.first<=SMALL_LG_NUM) return SMALL_LG_NUM;
    else
    {
      return (float)psst.second.get_lc_st()-(float)psst.first.get_lc_s();
    }
  }
}

//-------------------------
template<class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
Prob IncrCondProbTable<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::pSrcGivenTrg(const SRCDATA& s,
                                                                           const TRGDATA& t)
{
  return logpSrcGivenTrg(s,t).get_p();    
}

//-------------------------
template<class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
LgProb IncrCondProbTable<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::logpSrcGivenTrg(const SRCDATA& s,
                                                                                const TRGDATA& t)
{
  LogCount lc_st;
  LogCount lc_t;
  
  lc_t=lcTrg(t);
  if((float)lc_t<=SMALL_LG_NUM)
  {
    return SMALL_LG_NUM;
  }
  else
  {
    lc_st=lcSrcTrg(s,t);
    return (float)lc_st-(float)lc_t;
  }
}

//-------------------------
template<class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
bool
IncrCondProbTable<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::getEntriesForTarget(const TRGDATA& t,
                                                                             typename IncrCondProbTable<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::SrcTableNode& tnode)
{
 typename TrgSrcInfo::const_iterator trgSrcInfoIter;
 TrgSrcInfoNode trgSrcInfoNode;
 typename TrgSrcInfoNode::const_iterator trgSrcInfoNodeIter;
 
 trgSrcInfoIter=table.find(t);
 if(trgSrcInfoIter!=table.end())
 {
   trgSrcInfoNode=trgSrcInfoIter->second;
   for(trgSrcInfoNodeIter=trgSrcInfoNode.begin();trgSrcInfoNodeIter!=trgSrcInfoNode.end();++trgSrcInfoNodeIter)
   {
     im_pair<SRC_INFO,SRCTRG_INFO> psst;
     typename SrcDataInfo::const_iterator srcDataInfoIter;

     srcDataInfoIter=srcDataInfo.find(trgSrcInfoNodeIter->first);
     psst.first=srcDataInfoIter->second;
     psst.second=trgSrcInfoNodeIter->second;
     tnode[trgSrcInfoNodeIter->first]=psst;
   }
   return true;
 }
 else
 {
   tnode.clear();
   return false;
 }
}

//-------------------------
template<class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
bool IncrCondProbTable<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::getEntriesForSource(const SRCDATA& s,
                                                                                  typename IncrCondProbTable<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::TrgTableNode& trgtn)
{
  typename TrgSrcInfo::const_iterator trgSrcInfoIter;
  typename TrgSrcInfoNode::const_iterator trgSrcNodeIter;
  bool ret=false;
  im_pair<SRC_INFO,SRCTRG_INFO> psst;
  typename SrcDataInfo::const_iterator srcDataInfoIter;

  srcDataInfoIter=srcDataInfo.find(s);

  for(trgSrcInfoIter=table.begin();trgSrcInfoIter!=table.end();++trgSrcInfoIter)
  {
    for(trgSrcNodeIter=trgSrcInfoIter->second.begin();trgSrcNodeIter!=trgSrcInfoIter->second.end();++trgSrcNodeIter)
    {
      if(s==trgSrcNodeIter->first)
      {
        ret=true;
        psst.first=srcDataInfoIter->second;
        psst.second=trgSrcNodeIter->second;
        trgtn[trgSrcInfoIter->first]=psst;
      }
    }
  }
  return ret;
}

//-------------------------
template<class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
bool IncrCondProbTable<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::getNbestForSrc(const SRCDATA& s,
                                                                             NbestTableNode<TRGDATA>& nbt)
{
  TrgTableNode tnode;
  typename TrgTableNode::const_iterator tNodeIter;
  typename SrcDataInfo::const_iterator srcDataInfoIter;
  bool ret;

  nbt.clear();
  ret=getEntriesForSource(s,tnode);
  srcDataInfoIter=srcDataInfo.find(s);

  for(tNodeIter=tnode.begin();tNodeIter!=tnode.end();++tNodeIter)
  {
    nbt.insert((float)tNodeIter->second.second.get_lc_st()-(float)srcDataInfoIter->second.get_c_s(),tNodeIter->first);
  }
  return ret;
}

//-------------------------
template<class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
bool IncrCondProbTable<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::getNbestForTrg(const TRGDATA& t,
                                                                             NbestTableNode<SRCDATA>& nbt,
                                                                             int N)
{
  SrcTableNode tnode;
  typename SrcTableNode::const_iterator tNodeIter;
  bool ret;
  
  nbt.clear();
  ret=getEntriesForTarget(t,tnode);
  for(tNodeIter=tnode.begin();tNodeIter!=tnode.end();++tNodeIter)
  {
    typename SrcDataInfo::const_iterator srcDataInfoIter;

    srcDataInfoIter=srcDataInfo.find(tNodeIter->first);
    nbt.insert((float)tNodeIter->second.second.get_c_st()-(float)srcDataInfoIter->second.get_c_s(),tNodeIter->first);
  }

      // Prune N-best options if necessary
  while(nbt.size()>(unsigned int)N && N>=0)
  {
    nbt.removeLastElement();
  }

  return ret;
}

//-------------------------
template<class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
LogCount IncrCondProbTable<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::lcSrcTrg(const SRCDATA& s,const TRGDATA& t)
{
  bool found;
  return getSrcTrgInfo(s,t,found).get_lc_st();
}

//-------------------------
template<class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
LogCount IncrCondProbTable<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::lcSrc(const SRCDATA& s)
{
  bool found;
  return getSrcInfo(s,found).get_lc_s();
}

//-------------------------
template<class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
LogCount IncrCondProbTable<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::lcTrg(const TRGDATA& t)
{
   typename TrgSrcInfo::const_iterator trgSrcInfoIter;
   typename TrgSrcInfoNode::const_iterator trgSrcNodeIter;
   LogCount lctrg=SMALL_LG_NUM;	
    
   trgSrcInfoIter=table.find(t);
   if(trgSrcInfoIter!=table.end())
   {
     for(trgSrcNodeIter=trgSrcInfoIter->second.begin();trgSrcNodeIter!=trgSrcInfoIter->second.end();++trgSrcNodeIter) 
       lctrg=MathFuncs::lns_sumlog(lctrg,trgSrcNodeIter->second.get_lc_st());
     return lctrg; 
   }
   else return SMALL_LG_NUM;
}

// const iterator functions for the trie class
//---------------
template<class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
typename IncrCondProbTable<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::const_iterator
IncrCondProbTable<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::begin(void)const
{
  typename IncrCondProbTable<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::const_iterator iter(this->table.begin());
  return iter;
}

//---------------
template<class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
typename IncrCondProbTable<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::const_iterator
IncrCondProbTable<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::end(void)const
{
  typename IncrCondProbTable<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::const_iterator iter(this->table.end());
  return iter;
}

#endif
