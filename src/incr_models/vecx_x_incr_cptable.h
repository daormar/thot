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
/* Module: vecx_x_incr_cptable                                      */
/*                                                                  */
/* Prototype file: vecx_x_incr_cptable                              */
/*                                                                  */
/* Description: Class to manage incremental conditional probability */
/*              tables of the form p(x|Vector<x>) (Note: x is a     */
/*              data type)                                          */
/*                                                                  */
/********************************************************************/

#ifndef _vecx_x_incr_cptable
#define _vecx_x_incr_cptable

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseIncrCondProbTable.h"
#include "TrieVecs.h"

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------

//--------------- vecx_x_incr_cptable class

template<class X,class SRC_INFO,class SRCTRG_INFO>
class vecx_x_incr_cptable: public BaseIncrCondProbTable<Vector<X>,X,SRC_INFO,SRCTRG_INFO>
{
 public:

  typedef typename BaseIncrCondProbTable<Vector<X>,X,SRC_INFO,SRCTRG_INFO>::SrcTableNode SrcTableNode;
  typedef typename BaseIncrCondProbTable<Vector<X>,X,SRC_INFO,SRCTRG_INFO>::TrgTableNode TrgTableNode;
  typedef TrieVecs<X,SRCTRG_INFO> SrcTrgInfo;
  typedef TrieVecs<X,SRC_INFO> SrcInfo;

      // Basic functions
  void addTableEntry(const Vector<X>& s,
                     const X& t,
                     im_pair<SRC_INFO,SRCTRG_INFO> inf);
  void addSrcInfo(const Vector<X>& s,SRC_INFO s_inf);
  void addSrcTrgInfo(const Vector<X>& s,const X& t,SRCTRG_INFO st_inf);
  void incrCountsOfEntryLog(const Vector<X>& s,
                            const X& t,
                            LogCount lc);
  im_pair<SRC_INFO,SRCTRG_INFO> infSrcTrg(const Vector<X>& s,
                                          const X& t,
                                          bool& found);
  SRC_INFO getSrcInfo(const Vector<X>& s,bool& found);
  SRCTRG_INFO getSrcTrgInfo(const Vector<X>& s,const X& t,bool& found);
  Prob pTrgGivenSrc(const Vector<X>& s,const X& t);
  LgProb logpTrgGivenSrc(const Vector<X>& s,const X& t);
  Prob pSrcGivenTrg(const Vector<X>& s,const X& t);
  LgProb logpSrcGivenTrg(const Vector<X>& s,const X& t);
  bool getEntriesForSource(const Vector<X>& s,TrgTableNode& trgtn);
  bool getEntriesForTarget(const X& t,SrcTableNode& tnode);
  bool getNbestForSrc(const Vector<X>& s,NbestTableNode<X>& nbt);
  bool getNbestForTrg(const X& t,NbestTableNode<Vector<X> >& nbt,int N=-1);

      // Count-related functions
  Count cSrcTrg(const Vector<X>& s,const X& t);
  Count cSrc(const Vector<X>& s);
  Count cTrg(const X& t);
  LogCount lcSrcTrg(const Vector<X>& s,const X& t);
  LogCount lcSrc(const Vector<X>& s);
  LogCount lcTrg(const X& t);

      // size, clear functions
  size_t size(void);
  void clear(void);

      // load function
  bool load(const char *fileName);

      // destructor
  ~vecx_x_incr_cptable(){}
  
      // const_iterator
  class const_iterator;
  friend class const_iterator;
  class const_iterator
  {
   protected:
    typename SrcTrgInfo::const_iterator srcTrgInfoIter;
      
   public:
    const_iterator(void){}
    const_iterator(typename SrcTrgInfo::const_iterator _srcTrgInfoIter);
    bool operator++(void); //prefix
    bool operator++(int);  //postfix
    int operator==(const const_iterator& right);
    int operator!=(const const_iterator& right);
    const typename SrcTrgInfo::const_iterator& operator->(void)const;
    pair<Vector<X>,SRCTRG_INFO> operator*(void)const;
    ~const_iterator(){}
  };
      // const_iterator-related functions
  const_iterator begin(void)const;
  const_iterator end(void)const;
  
 protected:
  SrcTrgInfo srcTrgInfo;
  SrcInfo srcInfo;
  SRC_INFO srcInfoNull;
};

//--------------- Template function definitions

//-------------------------
template<class X,class SRC_INFO,class SRCTRG_INFO>
size_t vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::size(void)
{
  return srcTrgInfo.size();
}
//-------------------------
template<class X,class SRC_INFO,class SRCTRG_INFO>
void vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::clear(void)
{
  srcTrgInfo.clear();
  srcInfo.clear();
  SRC_INFO srcInfoNullAux;
  srcInfoNull=srcInfoNullAux;
}
//-------------------------
template<class X,class SRC_INFO,class SRCTRG_INFO>
bool vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::load(const char *fileName)
{
  // Not implemented
}
//-------------------------
template<class X,class SRC_INFO,class SRCTRG_INFO>
void vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::addTableEntry(const Vector<X>& s,
                                                                const X& t,
                                                                im_pair<SRC_INFO,SRCTRG_INFO> inf)
{
  addSrcInfo(s,inf.first);
  addSrcTrgInfo(s,t,inf.second);
}

//-------------------------
template<class X,class SRC_INFO,class SRCTRG_INFO>
void vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::addSrcInfo(const Vector<X>& s,
                                                             SRC_INFO s_inf)
{
  if(s.size()!=0)
  {
    srcInfo.insert(s,s_inf);
  }
  else
  {
    srcInfoNull=s_inf;
  }
}

//-------------------------
template<class X,class SRC_INFO,class SRCTRG_INFO>
void vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::addSrcTrgInfo(const Vector<X>& s,
                                                                const X& t,
                                                                SRCTRG_INFO st_inf)
{
  Vector<X> vecx;
  unsigned int i;

  
  for(i=0;i<s.size();++i)
  {
    vecx.push_back(s[i]);
  }

  vecx.push_back(t);

  srcTrgInfo.insert(vecx,st_inf);
}

//-------------------------
template<class X,class SRC_INFO,class SRCTRG_INFO>
void vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::incrCountsOfEntryLog(const Vector<X>& s,
                                                                       const X& t,
                                                                       LogCount lc)
{
  SRCTRG_INFO* stiPtr;
  SRC_INFO* siPtr;
  Vector<X> vecx;
    
  for(unsigned int i=0;i<s.size();++i)
  {
    vecx.push_back(s[i]);
  }

  vecx.push_back(t);

  stiPtr=srcTrgInfo.find(vecx);
  if(stiPtr==NULL)
  {
    SRCTRG_INFO sti;

    sti.incr_logcount((float)lc);
    srcTrgInfo.insert(vecx,sti);
  }
  else
  {
    stiPtr->incr_logcount((float)lc);
  }

  if(s.size()!=0)
  {
    siPtr=srcInfo.find(s);
    if(siPtr==NULL)
    {
      SRC_INFO si;
      
      si.incr_logcount((float)lc);
      srcInfo.insert(s,si);
    }
    else
    {
      siPtr->incr_logcount((float)lc);
    }
  }
  else
  {
    srcInfoNull.incr_logcount((float)lc);
  }
}

//-------------------------
template<class X,class SRC_INFO,class SRCTRG_INFO>
im_pair<SRC_INFO,SRCTRG_INFO>
vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::infSrcTrg(const Vector<X>& s,
                                                       const X& t,
                                                       bool& found)
{
  SRCTRG_INFO* stiPtr;
  im_pair<SRC_INFO,SRCTRG_INFO> psst;
  Vector<X> vecx;
  unsigned int i;
  
  for(i=0;i<s.size();++i)
  {
    vecx.push_back(s[i]);
  }

  vecx.push_back(t);

  if(s.size()==0)
  {
    psst.first=srcInfoNull;
  }
  else
  {
    SRC_INFO* srcInfoPtr=srcInfo.find(s);
    if(srcInfoPtr!=NULL) psst.first=*srcInfoPtr;
    else psst.first=0;
  }

  stiPtr=srcTrgInfo.find(vecx);
  if(stiPtr==NULL)
  {
    found=false;
  }
  else
  {
    psst.second=*stiPtr;
    found=true;
  }
  return psst;
}

//-------------------------
template<class X,class SRC_INFO,class SRCTRG_INFO>
SRC_INFO vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::getSrcInfo(const Vector<X>& s,
                                                                 bool& found)
{
  if(s.size()!=0)
  {
    SRC_INFO* sinfoPtr;
  
    sinfoPtr=srcInfo.find(s);
    if(sinfoPtr==NULL)
    {
      SRC_INFO sinfo;
      
      found=false;
      return sinfo;
    }
    else
    {
      found=true;
      return *sinfoPtr;
    }
  }
  else
  {
    found=true;
    return srcInfoNull;
  }
}
//-------------------------
template<class X,class SRC_INFO,class SRCTRG_INFO>
SRCTRG_INFO vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::getSrcTrgInfo(const Vector<X>& s,
                                                                       const X& t,
                                                                       bool& found)
{
  SRCTRG_INFO* stiPtr;
  Vector<X> vecx;
  unsigned int i;
  
  for(i=0;i<s.size();++i)
  {
    vecx.push_back(s[i]);
  }

  vecx.push_back(t);

  stiPtr=srcTrgInfo.find(vecx);
  if(stiPtr==NULL)
  {
    SRCTRG_INFO sti;

    found=false;
    return sti;
  }
  else
  {
    found=true;
    return *stiPtr;
  }
}

//-------------------------
template<class X,class SRC_INFO,class SRCTRG_INFO>
Prob vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::pTrgGivenSrc(const Vector<X>& s,
                                                               const X& t)
{
  im_pair<SRC_INFO,SRCTRG_INFO> psst;
  bool found;
  
  psst=infSrcTrg(s,t,found);
  if(!found)
  {
    return 0;
  }
  else
  {
    if((float)psst.first==0) return 0;
    else
    {
      return (float)psst.second.get_c_st()/(float)psst.first.get_c_s();
    }
  }
}

//-------------------------
template<class X,class SRC_INFO,class SRCTRG_INFO>
LgProb vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::logpTrgGivenSrc(const Vector<X>& s,
                                                                    const X& t)
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
template<class X,class SRC_INFO,class SRCTRG_INFO>
Prob vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::pSrcGivenTrg(const Vector<X>& s,const X& t)
{
  return logpSrcGivenTrg(s,t).get_p();    
}

//-------------------------
template<class X,class SRC_INFO,class SRCTRG_INFO>
LgProb vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::logpSrcGivenTrg(const Vector<X>& s,
                                                                    const X& t)
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
template<class X,class SRC_INFO,class SRCTRG_INFO>
bool vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::getEntriesForSource(const Vector<X>& s,
                                                                      TrgTableNode& trgtn)
{
  typename SrcTrgInfo::const_iterator titer;
  unsigned int i;
  pair<X,im_pair<SRC_INFO,SRCTRG_INFO> > pdp;
  Vector<X> vecx;
  SRC_INFO* siPtr;

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
template<class X,class SRC_INFO,class SRCTRG_INFO>
bool
vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::getEntriesForTarget(const X& t,typename vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::SrcTableNode& tnode)
{
  typename SrcTrgInfo::const_iterator titer;
  unsigned int i;
  pair<Vector<X>,im_pair<SRC_INFO,SRCTRG_INFO> > pdp;

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
template<class X,class SRC_INFO,class SRCTRG_INFO>
bool vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::getNbestForSrc(const Vector<X>& s,
                                                                 NbestTableNode<X>& nbt)
{
  TrgTableNode tnode;
  typename TrgTableNode::iterator tNodeIter;
  SRC_INFO* siPtr;
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
template<class X,class SRC_INFO,class SRCTRG_INFO>
bool vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::getNbestForTrg(const X& t,
                                                                 NbestTableNode<Vector<X> >& nbt,
                                                                 int N)
{
  SrcTableNode tnode;
  typename SrcTableNode::iterator tNodeIter;
  bool ret;
  
  nbt.clear();
  ret=getEntriesForTarget(t,tnode);
  for(tNodeIter=tnode.begin();tNodeIter!=tnode.end();++tNodeIter)
  {
    SRC_INFO* siPtr;

    siPtr=srcInfo.find(tNodeIter->first);
    nbt.insert((float)tNodeIter->second.second.get_lc_st()-(float)siPtr->get_lc_s(),tNodeIter->first);
  }

  if(N>=0)
    while(nbt.size()>(unsigned int) N) nbt.removeLastElement();
  
  return ret;
}

//-------------------------
template<class X,class SRC_INFO,class SRCTRG_INFO>
Count vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::cSrcTrg(const Vector<X>& s,
                                                           const X& t)
{
  SRCTRG_INFO* stiPtr;
  Vector<X> vecx;
  unsigned int i;
  
  for(i=0;i<s.size();++i)
  {
    vecx.push_back(s[i]);
  }
  vecx.push_back(t);

  stiPtr=srcTrgInfo.find(vecx);

  if(stiPtr==NULL)
  {
    return 0;
  }
  else
  {
    return stiPtr->get_c_st();
  }
}

//-------------------------
template<class X,class SRC_INFO,class SRCTRG_INFO>
Count vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::cSrc(const Vector<X>& s)
{
  SRC_INFO* siPtr;

  if(s.size()==0)
  {
    return srcInfoNull.get_c_s();
  }
  else
  {
    siPtr=srcInfo.find(s);
    if(siPtr==NULL)
    {
      return 0;
    }
    else return siPtr->get_c_s();
  }
}

//-------------------------
template<class X,class SRC_INFO,class SRCTRG_INFO>
Count vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::cTrg(const X& t)
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
template<class X,class SRC_INFO,class SRCTRG_INFO>
LogCount vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::lcSrcTrg(const Vector<X>& s,
                                                               const X& t)
{
  SRCTRG_INFO* stiPtr;
  Vector<X> vecx;
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
template<class X,class SRC_INFO,class SRCTRG_INFO>
LogCount vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::lcSrc(const Vector<X>& s)
{
  SRC_INFO* siPtr;

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
template<class X,class SRC_INFO,class SRCTRG_INFO>
LogCount vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::lcTrg(const X& t)
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
}

// nested-const_iterator member functions
//---------------
template<class X,class SRC_INFO,class SRCTRG_INFO>
vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::const_iterator::const_iterator(typename SrcTrgInfo::const_iterator _srcTrgInfoIter)
{
  srcTrgInfoIter=_srcTrgInfoIter;
}
//---------------
template<class X,class SRC_INFO,class SRCTRG_INFO>
bool vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::const_iterator::operator++(void) //prefix
{
  typename SrcTrgInfo::const_iterator srcTrgInfoIterAux;

  srcTrgInfoIterAux=srcTrgInfoIter;
  ++srcTrgInfoIter;
  return srcTrgInfoIterAux!=srcTrgInfoIter;
}
//---------------
template<class X,class SRC_INFO,class SRCTRG_INFO>
bool vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::const_iterator::operator++(int)  //postfix
{
  return operator++();
}
//---------------
template<class X,class SRC_INFO,class SRCTRG_INFO>
int vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::const_iterator::operator==(const const_iterator& right)
{
  return this->srcTrgInfoIter==right.srcTrgInfoIter;
}
//---------------
template<class X,class SRC_INFO,class SRCTRG_INFO>
int vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::const_iterator::operator!=(const const_iterator& right)
{
  return !(*this==right);
}
//---------------
template<class X,class SRC_INFO,class SRCTRG_INFO>
const typename vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::SrcTrgInfo::const_iterator&
vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::const_iterator::operator->(void)const
{
  return srcTrgInfoIter;
}
//---------------
template<class X,class SRC_INFO,class SRCTRG_INFO>
pair<Vector<X>,SRCTRG_INFO > vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::const_iterator::operator*(void)const
{
  return *srcTrgInfoIter;
}

// const iterator functions
//---------------
template<class X,class SRC_INFO,class SRCTRG_INFO>
typename vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::const_iterator
vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::begin(void)const
{
  typename vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::const_iterator iter(this->srcTrgInfo.begin());

  return iter;
}

//---------------
template<class X,class SRC_INFO,class SRCTRG_INFO>
typename vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::const_iterator
vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::end(void)const
{
  typename vecx_x_incr_cptable<X,SRC_INFO,SRCTRG_INFO>::const_iterator iter(this->srcTrgInfo.end());

  return iter;
}

#endif
