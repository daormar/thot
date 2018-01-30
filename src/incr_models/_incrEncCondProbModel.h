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

/**
 * @file _incrEncCondProbModel
 * 
 * @brief Abstract class to manage incremental encoded conditional
 * probability models Pr(t|s).
 */

#ifndef __incrEncCondProbModel
#define __incrEncCondProbModel

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "IncrCondProbTable.h"
#include "BaseIncrEncCondProbModel.h"
#include <IncrEncoder.h>

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------

//--------------- _incrEncCondProbModel class

template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
class _incrEncCondProbModel: public BaseIncrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>
{
 public:

  typedef typename BaseIncrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::SrcTableNode SrcTableNode;
  typedef typename BaseIncrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::TrgTableNode TrgTableNode;

      //Constructor
  _incrEncCondProbModel();

      // Basic functions
  void addTableEntry(const SRCDATA& s,
                     const TRGDATA& t,
                     im_pair<SRC_INFO,SRCTRG_INFO> inf);
      // Adds an entry to the probability table
  void addTableEntryHigh(const HSRCDATA& hs,
                         const HTRGDATA& ht,
                         im_pair<SRC_INFO,SRCTRG_INFO> inf);
      // High level version of the addTableEntry function
  void addSrcInfo(const SRCDATA& s,SRC_INFO s_inf);
  void addHSrcInfo(const HSRCDATA& hs,SRC_INFO s_inf);
  void addSrcTrgInfo(const SRCDATA& s,const TRGDATA& t,SRCTRG_INFO st_inf);
  void addHSrcHTrgInfo(const HSRCDATA& hs,
                       const HTRGDATA& ht,
                       SRCTRG_INFO st_inf);
  void incrCountsOfEntry(const SRCDATA& s,const TRGDATA& t,Count c);
       // Increase the counts of a given phrase pair
  void incrCountsOfEntryHigh(const HSRCDATA& hs,const HTRGDATA& ht,Count c);
      // High level version of the incrCountOfEntryHigh function
  im_pair<SRC_INFO,SRCTRG_INFO> infSrcTrg(const SRCDATA& s,
                                          const TRGDATA& t,
                                          bool& found);
  im_pair<SRC_INFO,SRCTRG_INFO> infHTrgHSrc(const HSRCDATA& hs,
                                            const HTRGDATA& ht,
                                            bool& found);
  SRC_INFO getSrcInfo(const SRCDATA& s,bool& found);
  SRC_INFO getHSrcInfo(const HSRCDATA& hs,bool& found);
  SRCTRG_INFO getSrcTrgInfo(const SRCDATA& s,const TRGDATA& t,bool& found);
  SRCTRG_INFO getHSrcHTrgInfo(const HSRCDATA& hs,
                              const HTRGDATA& ht,
                              bool& found);
  Prob pTrgGivenSrc(const SRCDATA& s,const TRGDATA& t);
  Prob pHTrgGivenHSrc(const HSRCDATA& hs,const HTRGDATA& ht);
      // High level version of the pTrgGivenSrc function
  LgProb logpTrgGivenSrc(const SRCDATA& s,const TRGDATA& t);
  LgProb logpHTrgGivenHSrc(const HSRCDATA& hs,const HTRGDATA& ht);
      // High level version of the logpTrgGivenSrc function
  Prob pSrcGivenTrg(const SRCDATA& s,const TRGDATA& t);
  Prob pHSrcGivenHTrg(const HSRCDATA& hs,const HTRGDATA& ht);
      // High level version of the pSrcGivenTrg function
  LgProb logpSrcGivenTrg(const SRCDATA& s,const TRGDATA& t);
  LgProb logpHSrcGivenHTrg(const HSRCDATA& hs,const HTRGDATA& ht);
      // High level version of the logpSrcGivenTrg function  
  bool getEntriesForTarget(const TRGDATA& t,SrcTableNode& tnode);
      // Stores in tnode the entries associated to a given TRGDATA t
  bool getEntriesForHTarget(const HTRGDATA& ht,SrcTableNode& tnode);
      // High level version of the getEntriesForTarget function
  bool getNbestForTrg(const TRGDATA& t,NbestTableNode<SRCDATA>& nbt,int N=-1);
      // Returns a n-best list for a given target data t
  bool getNbestForHTrg(const HTRGDATA& ht,
                       NbestTableNode<SRCDATA>& nbt,
                       int N=-1);
      // High level version of the getNbestForTrg function
  bool getEntriesForSource(const SRCDATA& ht,TrgTableNode& tnode);
      // High level version of the getEntriesForSource function
  bool getEntriesForHSource(const HSRCDATA& ht,TrgTableNode& tnode);
      // High level version of the getEntriesForSource function
  bool getNbestForSrc(const SRCDATA& s,NbestTableNode<TRGDATA>& nbt);
      // Returns a n-best list for a given source data s
  bool getNbestForHSrc(const HSRCDATA& hs,NbestTableNode<TRGDATA>& nbt);
      // High level version of the getNbestForSrc function

        // Count-related functions
  LogCount lcSrcTrg(const SRCDATA& s,const TRGDATA& t);
  LogCount lcSrc(const SRCDATA& s);
  LogCount lcTrg(const TRGDATA& t);
  LogCount lcHSrcHTrg(const HSRCDATA& hs,const HTRGDATA& ht);
  LogCount lcHSrc(const HSRCDATA& hs);
  LogCount lcHTrg(const HTRGDATA& ht);

      // Encoding-related functions
  bool HighSrc_to_Src(const HSRCDATA& hs,SRCDATA& s)const;
      // Given a HSRCDATA object "hs" obtains its corresponding encoded
      // value in "s". Returns true if the encoding was successful ("hs"
      // exists in the vocabulary).
  bool HighTrg_to_Trg(const HTRGDATA& ht,TRGDATA& t)const;
      // The same for HTRGDATA objects

  bool Src_to_HighSrc(const SRCDATA& s,HSRCDATA& hs)const;
      // Performs the inverse process (s -> hs)
  bool Trg_to_HighTrg(const TRGDATA& t,HTRGDATA& ht)const;
      // The same for TRGDATA objects (t -> ht)

  SRCDATA addHSrcCode(const HSRCDATA &hs);
       // sets the codification for hs (hs->s)
  TRGDATA addHTrgCode(const HTRGDATA &ht);
       // sets the codification for ht (ht->t)
  unsigned int sizeSrcEnc(void);
  unsigned int sizeTrgEnc(void);
  bool loadEncodingInfo(const char *prefixFileName);
      // Loads encoding information given a prefix file name
  bool printEncodingInfo(const char *prefixFileName);
      // Prints encoding information
  void clearEncodingInfo(void);
      // Clears encoding information

  void setEncodingPtr(BaseIncrEncoder<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA>* _encPtr);

      // size, clear functions
  size_t size(void);
  void clear(void);

      // Release the pointer to the table
  void release(void);

      // Destructor
  ~_incrEncCondProbModel();
   
 protected:

      // data members
  BaseIncrCondProbTable<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>* tablePtr;
  std::string modelFileName;

  BaseIncrEncoder<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA>* encPtr;
  bool encoderOwnedByTheObject;
};

//--------------- Template function definitions

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
_incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::_incrEncCondProbModel()
{
  this->tablePtr=NULL;
  this->encPtr=NULL;
  encoderOwnedByTheObject=true;
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
void _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::addTableEntry(const SRCDATA& s,const TRGDATA& t,im_pair<SRC_INFO,SRCTRG_INFO> inf)
{
  this->tablePtr->addTableEntry(s,t,inf);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
void _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::addTableEntryHigh(const HSRCDATA& hs,const HTRGDATA& ht,im_pair<SRC_INFO,SRCTRG_INFO> inf)
{
  SRCDATA s;
  TRGDATA t;
 
  if(!HighSrc_to_Src(hs,s))
  {
    s=this->encPtr->genHSrcCode(hs);
    this->encPtr->addHSrcCode(hs,s);
  }
  if(!HighTrg_to_Trg(ht,t))
  {
    t=this->encPtr->genHTrgCode(ht);
    this->encPtr->addHTrgCode(ht,t);
  }
  this->addTableEntry(s,t,inf);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
void _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::addSrcInfo(const SRCDATA& s,SRC_INFO s_inf)
{
  return tablePtr->addSrcInfo(s,s_inf);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
void _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::addHSrcInfo(const HSRCDATA& hs,SRC_INFO s_inf)
{
  SRCDATA s;
 
  if(!HighSrc_to_Src(hs,s))
  {
    s=this->encPtr->genHSrcCode(hs);
    this->encPtr->addHSrcCode(hs,s);
  }
  this->addSrcInfo(s,s_inf);  
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
void _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::addSrcTrgInfo(const SRCDATA& s,const TRGDATA& t,SRCTRG_INFO st_inf)
{
  return tablePtr->addSrcTrgInfo(s,t,st_inf);  
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
void _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::addHSrcHTrgInfo(const HSRCDATA& hs,const HTRGDATA& ht,SRCTRG_INFO st_inf)
{
  SRCDATA s;
  TRGDATA t;
 
  if(!HighSrc_to_Src(hs,s))
  {
    s=this->encPtr->genHSrcCode(hs);
    this->encPtr->addHSrcCode(hs,s);
  }
  if(!HighTrg_to_Trg(ht,t))
  {
    t=this->encPtr->genHTrgCode(ht);
    this->encPtr->addHTrgCode(ht,t);
  }
  this->addSrcTrgInfo(s,t,st_inf);  
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
void _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::incrCountsOfEntry(const SRCDATA& s,const TRGDATA& t,Count c)
{
  tablePtr->incrCountsOfEntry(s,t,c);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
void _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::incrCountsOfEntryHigh(const HSRCDATA& hs,const HTRGDATA& ht,Count c)
{
  SRCDATA s;
  TRGDATA t;
 
  if(!HighSrc_to_Src(hs,s))
  {
    s=this->encPtr->genHSrcCode(hs);
    this->encPtr->addHSrcCode(hs,s);
  }
  if(!HighTrg_to_Trg(ht,t))
  {
    t=this->encPtr->genHTrgCode(ht);
    this->encPtr->addHTrgCode(ht,t);
  }

  this->incrCountsOfEntry(s,t,c);  
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
im_pair<SRC_INFO,SRCTRG_INFO> _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::infSrcTrg(const SRCDATA& s,const TRGDATA& t,bool& found)
{
  return tablePtr->infSrcTrg(s,t,found);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
im_pair<SRC_INFO,SRCTRG_INFO> _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::infHTrgHSrc(const HSRCDATA& hs,const HTRGDATA& ht,bool& found)
{
  SRCDATA s;
  TRGDATA t;

  if(!HighSrc_to_Src(hs,s))
  {
    im_pair<SRC_INFO,SRCTRG_INFO> pssti;
    
    found=false;
    return pssti;
  }
  if(!HighTrg_to_Trg(ht,t))
  {   
    im_pair<SRC_INFO,SRCTRG_INFO> pssti;

    found=false;
    return pssti;
  }

  return tablePtr->infSrcTrg(s,t,found);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
SRC_INFO _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::getSrcInfo(const SRCDATA& s,bool& found)
{
  return tablePtr->getSrcInfo(s,found);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
SRC_INFO _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::getHSrcInfo(const HSRCDATA& hs,bool& found)
{
  SRCDATA s;

  if(!HighSrc_to_Src(hs,s))
  {
    SRC_INFO si;
    
    found=false;
    return si;
  }

  return tablePtr->getSrcInfo(s,found);
}
//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
SRCTRG_INFO _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::getSrcTrgInfo(const SRCDATA& s,const TRGDATA& t,bool& found)
{
  return tablePtr->getSrcTrgInfo(s,t,found);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
SRCTRG_INFO _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::getHSrcHTrgInfo(const HSRCDATA& hs,const HTRGDATA& ht,bool& found)
{
  SRCDATA s;
  TRGDATA t;

  if(!HighSrc_to_Src(hs,s))
  {
    SRCTRG_INFO sti;
    
    found=false;
    return sti;
  }
  if(!HighTrg_to_Trg(ht,t))
  {   
    SRCTRG_INFO sti;

    found=false;
    return sti;
  }

  return tablePtr->getSrcTrgInfo(s,t,found);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
Prob _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::pTrgGivenSrc(const SRCDATA& s,const TRGDATA& t)
{
  return this->tablePtr->pTrgGivenSrc(s,t);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
Prob _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::pHTrgGivenHSrc(const HSRCDATA& hs,const HTRGDATA& ht)
{
  SRCDATA s;
  TRGDATA t;

  if(!HighSrc_to_Src(hs,s)) return 0;
  if(!HighTrg_to_Trg(ht,t)) return 0;

  return this->pTrgGivenSrc(s,t);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
LgProb _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::logpTrgGivenSrc(const SRCDATA& s,const TRGDATA& t)
{
  return log((double)pTrgGivenSrc(s,t));
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
LgProb _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::logpHTrgGivenHSrc(const HSRCDATA& hs,const HTRGDATA& ht)
{
  return log((double)pHTrgGivenHSrc(hs,ht)); 
}
//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
Prob _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::pSrcGivenTrg(const SRCDATA& s,const TRGDATA& t)
{
  return this->tablePtr->pSrcGivenTrg(s,t);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
Prob _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::pHSrcGivenHTrg(const HSRCDATA& hs,const HTRGDATA& ht)
{
  SRCDATA s;
  TRGDATA t;

  if(!HighSrc_to_Src(hs,s)) return 0;
  if(!HighTrg_to_Trg(ht,t)) return 0;

  return this->pSrcGivenTrg(s,t);  
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
LgProb _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::logpSrcGivenTrg(const SRCDATA& s,const TRGDATA& t)
{
  return log((double)pSrcGivenTrg(s,t));
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
LgProb _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::logpHSrcGivenHTrg(const HSRCDATA& hs,const HTRGDATA& ht)
{
  return log((double)pHSrcGivenHTrg(hs,ht)); 
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
bool
_incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::getEntriesForTarget(const TRGDATA& t,typename _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::SrcTableNode& tnode)
{
  return this->tablePtr->getEntriesForTarget(t,tnode);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
bool
_incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::getEntriesForHTarget(const HTRGDATA& ht,typename _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::SrcTableNode& tnode)
{
  TRGDATA t;

  tnode.clear();
  
  if(!HighTrg_to_Trg(ht,t)) return false;
  else return this->getEntriesForTarget(t,tnode);
}
//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
bool _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::getNbestForTrg(const TRGDATA& t,NbestTableNode<SRCDATA>& nbt,int N)
{
  return this->tablePtr->getNbestForTrg(t,nbt,N);  
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
bool _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::getNbestForHTrg(const HTRGDATA& ht,NbestTableNode<SRCDATA>& nbt,int N)
{
  TRGDATA t;
  
  if(!HighTrg_to_Trg(ht,t)) return false;
  else return this->getNbestForTrg(t,nbt,N);  
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
bool
_incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::getEntriesForSource(const SRCDATA& s,typename _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::TrgTableNode& tnode)
{
  return this->tablePtr->getEntriesForSource(s,tnode);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
bool
_incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::getEntriesForHSource(const HSRCDATA& hs,typename _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::TrgTableNode& tnode)
{
  SRCDATA s;

  tnode.clear();
  
  if(!HighSrc_to_Src(hs,s)) return false;
  else return this->getEntriesForSource(s,tnode);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
bool _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::getNbestForSrc(const SRCDATA& s,NbestTableNode<TRGDATA>& nbt)
{
  return this->tablePtr->getNbestForSrc(s,nbt);  
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
bool _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::getNbestForHSrc(const HSRCDATA& hs,NbestTableNode<TRGDATA>& nbt)
{
  SRCDATA s;
  
  if(!HighSrc_to_Src(hs,s)) return false;
  else return this->getNbestForSrc(s,nbt);  
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
LogCount _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::lcSrcTrg(const SRCDATA& s,const TRGDATA& t)
{
  return this->tablePtr->lcSrcTrg(s,t);    
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
LogCount _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::lcSrc(const SRCDATA& s)
{
  return this->tablePtr->lcSrc(s);  
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
LogCount _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::lcTrg(const TRGDATA& t)
{
  return this->tablePtr->lcTrg(t);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
LogCount _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::lcHSrcHTrg(const HSRCDATA& hs,const HTRGDATA& ht)
{
  SRCDATA s;
  TRGDATA t;

  if(!HighSrc_to_Src(hs,s)) return SMALL_LG_NUM;
  if(!HighTrg_to_Trg(ht,t)) return SMALL_LG_NUM;

  return this->lcSrcTrg(s,t);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
LogCount _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::lcHSrc(const HSRCDATA& hs)
{
  SRCDATA s;

  if(!HighSrc_to_Src(hs,s)) return SMALL_LG_NUM;
  return this->lcSrc(s);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
LogCount _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::lcHTrg(const HTRGDATA& ht)
{
  TRGDATA t;

  if(!HighTrg_to_Trg(ht,t)) return SMALL_LG_NUM;
  return this->lcTrg(t);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
bool _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::HighSrc_to_Src(const HSRCDATA& hs,SRCDATA& s)const
{
  return this->encPtr->HighSrc_to_Src(hs,s);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
bool _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::HighTrg_to_Trg(const HTRGDATA& ht,TRGDATA& t)const
{
  return this->encPtr->HighTrg_to_Trg(ht,t);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
bool _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::Src_to_HighSrc(const SRCDATA& s,HSRCDATA& hs)const
{
  return this->encPtr->Src_to_HighSrc(s,hs);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
bool _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::Trg_to_HighTrg(const TRGDATA& t,HTRGDATA& ht)const
{
  return this->encPtr->Trg_to_HighTrg(t,ht);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
SRCDATA _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::addHSrcCode(const HSRCDATA &hs)
{
  SRCDATA s;

  if(!HighSrc_to_Src(hs,s))
  {
    s=this->encPtr->genHSrcCode(hs);
    this->encPtr->addHSrcCode(hs,s);
  }
  return s;
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
TRGDATA  _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::addHTrgCode(const HTRGDATA &ht)
{
  TRGDATA t;

  if(!HighTrg_to_Trg(ht,t))
  {
    t=this->encPtr->genHTrgCode(ht);
    this->encPtr->addHTrgCode(ht,t);
  }
  return t;
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
unsigned int _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::sizeSrcEnc(void)
{
  return this->encPtr->sizeSrc();
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
unsigned int _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::sizeTrgEnc(void)
{
  return this->encPtr->sizeTrg();
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
bool _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::loadEncodingInfo(const char *prefixFileName)
{
  bool ret=this->encPtr->load(prefixFileName);
  if(ret==THOT_ERROR) return THOT_ERROR;
    
  return THOT_OK;
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
bool _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::printEncodingInfo(const char *prefixFileName)
{
  bool ret=this->encPtr->print(prefixFileName);
  if(ret==THOT_ERROR) return THOT_ERROR;
  
  return THOT_OK;
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
void _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::clearEncodingInfo(void)
{
  this->encPtr->clear();
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
void _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::setEncodingPtr(BaseIncrEncoder<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA>* _encPtr)
{
      // Delete previously assigned pointer if required
  if(this->encPtr!=NULL && encoderOwnedByTheObject)
    delete encPtr;

      // Set new pointer
  encPtr=_encPtr;

      // The pointer is no longer owned by the class
  encoderOwnedByTheObject=false;
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
size_t _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::size(void)
{
  return tablePtr->size();
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
void _incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::clear(void)
{
  tablePtr->clear();
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
_incrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::~_incrEncCondProbModel()
{
  if(this->tablePtr!=NULL) delete tablePtr;
  if(this->encPtr!=NULL && encoderOwnedByTheObject)
    delete encPtr;
}

#endif
