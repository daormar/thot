/*
incr_models package for statistical machine translation
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
/* Module: _incrInterpEncCondProbModel                              */
/*                                                                  */
/* Prototype file: _incrInterpEncCondProbModel                      */
/*                                                                  */
/* Description: Abstract class to manage interpolated encoded       */
/*              conditional probability models Pr(t|s).             */
/*                                                                  */
/********************************************************************/

#ifndef __incrInterpEncCondProbModel
#define __incrInterpEncCondProbModel

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <incr_models_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseIncrEncCondProbModel.h"
#include "IncrEncoder.h"

//--------------- Constants ------------------------------------------

#define INVALID_MODEL_INDEX -1

//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------

//--------------- _incrInterpEncCondProbModel class

template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
class _incrInterpEncCondProbModel: public BaseIncrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>
{
 public:

  typedef typename BaseIncrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::SrcTableNode SrcTableNode;
  typedef typename BaseIncrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::TrgTableNode TrgTableNode;

      //Constructor
  _incrInterpEncCondProbModel();

        // Functions to access each model
  void resetModelIndex(void); // makes modelIndex to point to the first
                              // model of tablePtrVec
  
  int incrModelIndex(void);   // makes modelIndex to point to the next
                              // model of tablePtrVec. Returns -1 if
                              // modelIndex cannot be increased,
                              // otherwise returns the value of
                              // modelIndex

  int getModelIndex(void);    // returns the value of modelIndex
    

      // Basic functions
  void addTableEntry(const SRCDATA& s,
                     const TRGDATA& t,
                     im_pair<SRC_INFO,SRCTRG_INFO> inf);
      // Adds an entry to the probability model pointed by modelIndex
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
  im_pair<SRC_INFO,SRCTRG_INFO> infSrcTrg(const SRCDATA& s,
                                          const TRGDATA& t,
                                          bool& found);
      // Returns the info associated to s and t contained in the model
      // pointed by modelIndex
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

          // Count-related functions  
  LogCount lcSrcTrg(const SRCDATA& s,const TRGDATA& t);
  LogCount lcSrc(const SRCDATA& s);
  LogCount lcTrg(const TRGDATA& t);
  LogCount lcHSrcHTrg(const HSRCDATA& hs,const HTRGDATA& ht);
  LogCount lcHSrc(const HSRCDATA& hs);
  LogCount lcHTrg(const HTRGDATA& ht);

        // Encoding-related functions
  SRCDATA addHSrcCode(const HSRCDATA &hs);
       // sets the codification for hs (hs->s)
  TRGDATA addHTrgCode(const HTRGDATA &ht);
       // sets the codifcation for ht (ht->t)
  unsigned int sizeSrcEnc(void);
  unsigned int sizeTrgEnc(void);
  bool loadEncodingInfo(const char *prefixFileName);
      // Loads encoding information given a prefix file name
  bool printEncodingInfo(const char *prefixFileName);
      // Prints encoding information
  void clearEncodingInfo(void);
      // Clears encoding information

      // specific functions for interpolated ngram language models
  void setWeights(const Vector<float>& _weights);

      // size, clear functions
  size_t size(void);
  void clear(void);

      // Release the pointer to the table
  void release(void);

      // Destructor
  ~_incrInterpEncCondProbModel();
   
 protected:

      // typedefs
  typedef std::map<SRCDATA,SRCDATA> GlobalToLocalSrcDataMap;
  typedef std::map<TRGDATA,TRGDATA> GlobalToLocalTrgDataMap;
  
      // data members
  Vector<float> weights;
  Vector<float> normWeights;
  Vector<BaseIncrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>*> modelPtrVec;
  Vector<GlobalToLocalSrcDataMap> gtlSrcMapVec;
  Vector<GlobalToLocalTrgDataMap> gtlTrgMapVec;
  BaseIncrEncoder<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA>* encPtr;
  int modelIndex;
      // modelIndex determines to which model are applied the functions
      // addTableEntry, infSrcTrg and getEntriesForTarget

      // Auxiliary encoding functions
  SRCDATA mapGlobalToLocalSrcData(unsigned int index,const SRCDATA& global_s);
  TRGDATA mapGlobalToLocalTrgData(unsigned int index,const TRGDATA& global_t);
  
      // specific functions for interpolated ngram language models
  Vector<float> obtainNormWeights(const Vector<float>& unnormWeights);
};

//--------------- Template function definitions

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
_incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::_incrInterpEncCondProbModel()
{
  modelIndex=INVALID_MODEL_INDEX;
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
void _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::resetModelIndex(void)
{
  if(modelPtrVec.size()>0) modelIndex=0;
  else modelIndex=INVALID_MODEL_INDEX;
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
int _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::incrModelIndex(void)
{
  ++modelIndex;
  if(modelIndex>=modelPtrVec.size()) return INVALID_MODEL_INDEX;
  else return modelIndex;
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
void _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::addTableEntry(const SRCDATA& s,
                                                                                                        const TRGDATA& t,
                                                                                                        im_pair<SRC_INFO,SRCTRG_INFO> inf)
{
  this->modelPtrVec[modelIndex]->addTableEntry(mapGlobalToLocalSrcData(modelIndex,s),
                                               mapGlobalToLocalTrgData(modelIndex,t),
                                               inf);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
void
_incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::addTableEntryHigh(const HSRCDATA& hs,
                                                                                                       const HTRGDATA& ht,
                                                                                                       im_pair<SRC_INFO,SRCTRG_INFO> inf)
{
  this->modelPtrVec[modelIndex]->addTableEntryHigh(hs,ht,inf);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
void _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::addSrcInfo(const SRCDATA& s,
                                                                                                     SRC_INFO s_inf)
{
  return this->modelPtrVec[modelIndex]->addSrcInfo(mapGlobalToLocalSrcData(modelIndex,s),
                                                   s_inf);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
void _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::addHSrcInfo(const HSRCDATA& hs,
                                                                                                      SRC_INFO s_inf)
{
  this->modelPtrVec[modelIndex]->addHSrcInfo(hs,s_inf);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
void _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::addSrcTrgInfo(const SRCDATA& s,
                                                                                                        const TRGDATA& t,
                                                                                                        SRCTRG_INFO st_inf)
{
  return this->modelPtrVec[modelIndex]->addSrcTrgInfo(mapGlobalToLocalSrcData(modelIndex,s),
                                                      mapGlobalToLocalTrgData(modelIndex,t),
                                                      st_inf);  
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
void _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::addHSrcHTrgInfo(const HSRCDATA& hs,
                                                                                                          const HTRGDATA& ht,
                                                                                                          SRCTRG_INFO st_inf)
{
  return this->modelPtrVec[modelIndex]->addHSrcHTrgInfo(hs,ht,st_inf);  
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
void _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::incrCountsOfEntry(const SRCDATA& s,
                                                                                                            const TRGDATA& t,
                                                                                                            Count c)
{
  this->modelPtrVec[modelIndex]->incrCountsOfEntry(mapGlobalToLocalSrcData(modelIndex,s),
                                                   mapGlobalToLocalTrgData(modelIndex,t),
                                                   c);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
void _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::incrCountsOfEntryHigh(const HSRCDATA& hs,
                                                                                                                const HTRGDATA& ht,
                                                                                                                Count c)
{
  this->modelPtrVec[modelIndex]->incrCountsOfEntryHigh(hs,ht,c);
}


//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
im_pair<SRC_INFO,SRCTRG_INFO>
_incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::infSrcTrg(const SRCDATA& s,
                                                                                               const TRGDATA& t,
                                                                                               bool& found)
{
  return modelPtrVec[modelIndex]->infSrcTrg(mapGlobalToLocalSrcData(modelIndex,s),
                                            mapGlobalToLocalTrgData(modelIndex,t),
                                            found);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
im_pair<SRC_INFO,SRCTRG_INFO>
_incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::infHTrgHSrc(const HSRCDATA& hs,
                                                                                                 const HTRGDATA& ht,
                                                                                                 bool& found)
{
  SRCDATA s;
  TRGDATA t;

  if(!HighSrc_to_Src(hs,s))
  {
    im_pair<SRC_INFO,SRCTRG_INFO> sti;
    
    found=false;
    return sti;
  }
  if(!HighTrg_to_Trg(ht,t))
  {
    im_pair<SRC_INFO,SRCTRG_INFO> sti;

    found=false;
    return sti;
  }

  return modelPtrVec[modelIndex]->infSrcTrg(mapGlobalToLocalSrcData(modelIndex,s),
                                            mapGlobalToLocalTrgData(modelIndex,t),
                                            found);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
SRC_INFO _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::getSrcInfo(const SRCDATA& s,
                                                                                                         bool& found)
{
  return this->modelPtrVec[modelIndex]->getSrcInfo(mapGlobalToLocalSrcData(modelIndex,s),
                                                   found);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
SRC_INFO _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::getHSrcInfo(const HSRCDATA& hs,
                                                                                                          bool& found)
{
  SRCDATA s;

  if(!HighSrc_to_Src(hs,s))
  {
    SRC_INFO si;
    
    found=false;
    return si;
  }

  return this->modelPtrVec[modelIndex]->getSrcInfo(mapGlobalToLocalSrcData(modelIndex,s),
                                                   found);
}
//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
SRCTRG_INFO _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::getSrcTrgInfo(const SRCDATA& s,
                                                                                                               const TRGDATA& t,
                                                                                                               bool& found)
{
  return this->modelPtrVec[modelIndex]->getSrcTrgInfo(s,t,found);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
SRCTRG_INFO _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::getHSrcHTrgInfo(const HSRCDATA& hs,
                                                                                                                 const HTRGDATA& ht,
                                                                                                                 bool& found)
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

  return this->modelPtrVec[modelIndex]->getSrcTrgInfo(mapGlobalToLocalSrcData(modelIndex,s),
                                                      mapGlobalToLocalTrgData(modelIndex,t),
                                                      found);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
Prob _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::pTrgGivenSrc(const SRCDATA& s,
                                                                                                       const TRGDATA& t)
{
  Prob p=0;
    
  for(unsigned int i=0;i<modelPtrVec.size();++i)
  {
    p+=(Prob)normWeights[i]*((Prob)modelPtrVec[i]->pTrgGivenSrc(mapGlobalToLocalSrcData(modelIndex,s),
                                                                mapGlobalToLocalTrgData(modelIndex,t)));
  }
  return p;
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
Prob _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::pHTrgGivenHSrc(const HSRCDATA& hs,
                                                                                                         const HTRGDATA& ht)
{
  SRCDATA s;
  TRGDATA t;

  if(!HighSrc_to_Src(hs,s)) return 0;
  if(!HighTrg_to_Trg(ht,t)) return 0;

  return this->pTrgGivenSrc(s,t);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
LgProb _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::logpTrgGivenSrc(const SRCDATA& s,
                                                                                                            const TRGDATA& t)
{
  return log((double)pTrgGivenSrc(s,t));
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
LgProb _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::logpHTrgGivenHSrc(const HSRCDATA& hs,
                                                                                                              const HTRGDATA& ht)
{
  return log((double)pHTrgGivenHSrc(hs,ht)); 
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
Prob _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::pSrcGivenTrg(const SRCDATA& s,
                                                                                                       const TRGDATA& t)
{
  return this->modelPtrVec[modelIndex]->pSrcGivenTrg(mapGlobalToLocalSrcData(modelIndex,s),
                                                     mapGlobalToLocalTrgData(modelIndex,t));
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
Prob _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::pHSrcGivenHTrg(const HSRCDATA& hs,
                                                                                                         const HTRGDATA& ht)
{
  SRCDATA s;
  TRGDATA t;

  if(!HighSrc_to_Src(hs,s)) return 0;
  if(!HighTrg_to_Trg(ht,t)) return 0;

  return this->pSrcGivenTrg(s,t);  
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
LgProb _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::logpSrcGivenTrg(const SRCDATA& s,
                                                                                                            const TRGDATA& t)
{
  return log((double)pSrcGivenTrg(s,t));
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
LgProb _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::logpHSrcGivenHTrg(const HSRCDATA& hs,
                                                                                                              const HTRGDATA& ht)
{
  return log((double)pHSrcGivenHTrg(hs,ht)); 
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
bool
_incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::getEntriesForTarget(const TRGDATA& t,typename _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::SrcTableNode& tnode)
{
      // Obtain node for current model
  typename _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::SrcTableNode tnode_aux;
  bool ret=this->modelPtrVec[modelIndex]->getEntriesForTarget(mapGlobalToLocalTrgData(modelIndex,t),
                                                              tnode_aux);
      // Adapt list to global vocabulary
  tnode.clear();
  typename _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::SrcTableNode::iterator iter;
  for(iter=tnode_aux.begin();iter!=tnode_aux.end();++iter)
  {
    HSRCDATA hs;
    SRCDATA global_s;
    this->modelPtrVec[modelIndex]->Src_to_HighSrc(iter->first,hs);
    this->HighSrc_to_Src(hs,global_s);
    tnode[global_s]=iter->second;
  }
  return ret;
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
bool
_incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::getEntriesForHTarget(const HTRGDATA& ht,typename _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::SrcTableNode& tnode)
{
  TRGDATA t;

  tnode.clear();
  
  if(!HighTrg_to_Trg(ht,t)) return false;
  else return this->getEntriesForTarget(t,tnode);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
bool _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::getNbestForTrg(const TRGDATA& t,
                                                                                                         NbestTableNode<SRCDATA>& nbt,
                                                                                                         int N)
{
        // Obtain nbest list for current model
  NbestTableNode<SRCDATA> nbt_aux;
  bool ret=this->modelPtrVec[modelIndex]->getNbestForTrg(mapGlobalToLocalTrgData(modelIndex,t),
                                                         nbt_aux);
      // Adapt list to global vocabulary
  nbt.clear();
  typename NbestTableNode<SRCDATA>::iterator iter;
  for(iter=nbt_aux.begin();iter!=nbt_aux.end();++iter)
  {
    HSRCDATA hs;
    SRCDATA global_s;
    this->modelPtrVec[modelIndex]->Src_to_HighSrc(iter->second,hs);
    this->HighSrc_to_Src(hs,global_s);
    nbt.insert(iter->first,global_s);
    if(nbt.size()>(unsigned int) N) nbt.removeLastElement();
  }
  return ret;
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
bool _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::getNbestForHTrg(const HTRGDATA& ht,
                                                                                                          NbestTableNode<SRCDATA>& nbt,
                                                                                                          int N)
{
  TRGDATA t;
  
  if(!HighTrg_to_Trg(ht,t)) return false;
  else return this->getNbestForTrg(t,nbt,N);  
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
bool
_incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::getEntriesForSource(const SRCDATA& s,typename _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::TrgTableNode& tnode)
{
      // Obtain node for current model
  typename _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::TrgTableNode tnode_aux;
  bool ret=this->modelPtrVec[modelIndex]->getEntriesForSource(mapGlobalToLocalSrcData(modelIndex,s),
                                                              tnode_aux);
      // Adapt list to global vocabulary
  tnode.clear();
  typename _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::TrgTableNode::iterator iter;
  for(iter=tnode_aux.begin();iter!=tnode_aux.end();++iter)
  {
    HTRGDATA ht;
    TRGDATA global_t;
    this->modelPtrVec[modelIndex]->Trg_to_HighTrg(iter->first,ht);
    this->HighTrg_to_Trg(ht,global_t);
    tnode[global_t]=iter->second;
  }
  return ret;
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
bool
_incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::getEntriesForHSource(const HSRCDATA& hs,typename _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::TrgTableNode& tnode)
{
  SRCDATA s;

  tnode.clear();
  
  if(!HighSrc_to_Src(hs,s)) return false;
  else return this->getEntriesForSource(s,tnode);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
bool _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::getNbestForSrc(const SRCDATA& s,
                                                                                                         NbestTableNode<TRGDATA>& nbt)
{
      // Obtain nbest list for current model
  NbestTableNode<TRGDATA> nbt_aux;
  bool ret=this->modelPtrVec[modelIndex]->getNbestForSrc(mapGlobalToLocalSrcData(modelIndex,s),
                                                         nbt_aux);
      // Adapt list to global vocabulary
  nbt.clear();
  typename NbestTableNode<TRGDATA>::iterator iter;
  for(iter=nbt_aux.begin();iter!=nbt_aux.end();++iter)
  {
    HTRGDATA ht;
    TRGDATA global_t;
    this->modelPtrVec[modelIndex]->Trg_to_HighTrg(iter->second,ht);
    this->HighTrg_to_Trg(ht,global_t);
    nbt.insert(iter->first,global_t);
  }
  return ret;
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
bool _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::getNbestForHSrc(const HSRCDATA& hs,
                                                                                                          NbestTableNode<TRGDATA>& nbt)
{
  SRCDATA s;
  
  if(!HighSrc_to_Src(hs,s)) return false;
  else return this->getNbestForSrc(s,nbt);  
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
LogCount _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::lcSrcTrg(const SRCDATA& s,
                                                                                                       const TRGDATA& t)
{
  return this->modelPtrVec[modelIndex]->lcSrcTrg(mapGlobalToLocalSrcData(modelIndex,s),
                                                 mapGlobalToLocalTrgData(modelIndex,t));    
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
LogCount _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::lcSrc(const SRCDATA& s)
{
  return this->modelPtrVec[modelIndex]->lcSrc(mapGlobalToLocalSrcData(modelIndex,s));  
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
LogCount _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::lcTrg(const TRGDATA& t)
{
  return this->modelPtrVec[modelIndex]->lcTrg(mapGlobalToLocalTrgData(modelIndex,t));
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
LogCount _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::lcHSrcHTrg(const HSRCDATA& hs,
                                                                                                         const HTRGDATA& ht)
{
  SRCDATA s;
  TRGDATA t;

  if(!HighSrc_to_Src(hs,s)) return SMALL_LG_NUM;
  if(!HighTrg_to_Trg(ht,t)) return SMALL_LG_NUM;

  return this->lcSrcTrg(s,t);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
LogCount _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::lcHSrc(const HSRCDATA& hs)
{
  SRCDATA s;

  if(!HighSrc_to_Src(hs,s)) return SMALL_LG_NUM;
  return this->lcSrc(s);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
LogCount _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::lcHTrg(const HTRGDATA& ht)
{
  TRGDATA t;

  if(!HighTrg_to_Trg(ht,t)) return SMALL_LG_NUM;
  return this->lcTrg(t);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
bool _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::HighSrc_to_Src(const HSRCDATA& hs,
                                                                                                         SRCDATA& s)const
{
  // Search for code in encPtr
  bool found=encPtr->HighSrc_to_Src(hs,s);
  if(found)
  {
        // Code found
    return true;
  }
  else
  {
        // Code not found
        // Iterate over the different models
    for(unsigned int i=0;i<modelPtrVec.size();++i)
    {
      found=this->modelPtrVec[i]->HighSrc_to_Src(hs,s);
      if(found)
      {
            // Code found, add code to encoder
        s=encPtr->genHSrcCode(hs);
        encPtr->addHSrcCode(hs,s);
        return true;
      }
    }
        // Code not present in the different models
    return false;
  }
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
bool _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::HighTrg_to_Trg(const HTRGDATA& ht,
                                                                                                         TRGDATA& t)const
{
      // Search for code in encPtr
  bool found=encPtr->HighTrg_to_Trg(ht,t);
  if(found)
  {
        // Code found
    return true;
  }
  else
  {
        // Code not found
        // Iterate over the different models
    for(unsigned int i=0;i<modelPtrVec.size();++i)
    {
      found=this->modelPtrVec[i]->HighTrg_to_Trg(ht,t);
      if(found)
      {
            // Code found, add code to encoder
        t=encPtr->genHTrgCode(ht);
        encPtr->addHTrgCode(ht,t);
        return true;
      }
    }
        // Code not present in the different models
    return false;
  }
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
bool _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::Src_to_HighSrc(const SRCDATA& s,
                                                                                                         HSRCDATA& hs)const
{
  return encPtr->Src_to_HighSrc(s,hs);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
bool _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::Trg_to_HighTrg(const TRGDATA& t,
                                                                                                         HTRGDATA& ht)const
{
  return encPtr->Trg_to_HighTrg(t,ht);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
SRCDATA _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::addHSrcCode(const HSRCDATA &hs)
{
  SRCDATA s=encPtr->genHSrcCode(hs);
  encPtr->addHSrcCode(hs,s);
  return s;
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
TRGDATA  _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::addHTrgCode(const HTRGDATA &ht)
{
  TRGDATA t=encPtr->genHTrgCode(ht);
  encPtr->addHTrgCode(ht,t);
  return t;
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
unsigned int _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::sizeSrcEnc(void)
{
  return encPtr->sizeSrc();
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
unsigned int _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::sizeTrgEnc(void)
{
  return encPtr->sizeTrg();
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
bool _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::loadEncodingInfo(const char *prefixFileName)
{
  return encPtr->load(prefixFileName);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
bool _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::printEncodingInfo(const char *prefixFileName)
{
  return encPtr->print(prefixFileName);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
void _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::clearEncodingInfo(void)
{
  encPtr->clear();
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
SRCDATA _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::mapGlobalToLocalSrcData(unsigned int idx,const SRCDATA& global_s)
{
  typename GlobalToLocalSrcDataMap::iterator iter=gtlSrcMapVec[idx].find(global_s);
  if(iter!=gtlSrcMapVec[idx].end())
  {
    return iter->second;
  }
  else
  {
    HSRCDATA hs;
    SRCDATA s;
    bool found=Src_to_HighSrc(global_s,hs);
    found=this->modelPtrVec[idx]->HighSrc_to_Src(hs,s);
    if(found)
    {
      gtlSrcMapVec[idx][global_s]=s;
    }
    return s;
  }
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
TRGDATA _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::mapGlobalToLocalTrgData(unsigned int idx,const TRGDATA& global_t)
{
  typename GlobalToLocalTrgDataMap::iterator iter=gtlTrgMapVec[idx].find(global_t);
  if(iter!=gtlTrgMapVec[idx].end())
  {
    return iter->second;
  }
  else
  {
    HTRGDATA ht;
    TRGDATA t;
    bool found=Trg_to_HighTrg(global_t,ht);
    found=this->modelPtrVec[idx]->HighTrg_to_Trg(ht,t);
    if(found)
    {
      gtlTrgMapVec[idx][global_t]=t;
    }
    return t;
  }
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
void _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::setWeights(const Vector<float>& _weights)
{
  weights=_weights;
  normWeights=obtainNormWeights(weights);
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
Vector<float>
_incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::obtainNormWeights(const Vector<float>& unnormWeights)
{
      // Obtain sum
  float sum=0;
  for(unsigned int i=0;i<unnormWeights.size();++i)
    sum+=unnormWeights[i];

      // Create norm. weights vector
  Vector<float> result;
  for(unsigned int i=0;i<unnormWeights.size();++i)
  {
    result.push_back(unnormWeights[i]/sum);
  }
      // Return result
  return result;
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
size_t _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::size(void)
{
  size_t s=0;
  
  for(unsigned int i=0;i<modelPtrVec.size();++i)
  {
    s+=modelPtrVec[i]->size();
  }
  return s;
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
void _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::clear(void)
{
  for(unsigned int i=0;i<modelPtrVec.size();++i)
  {
    modelPtrVec[i]->clear();
  }
  weights.clear();
  normWeights.clear();
  gtlSrcMapVec.clear();
  gtlTrgMapVec.clear();
  modelIndex=INVALID_MODEL_INDEX;
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
void _incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::release(void)
{
  for(unsigned int i=0;i<modelPtrVec.size();++i)
  {
    delete modelPtrVec[i];
  }
  modelPtrVec.clear();
  weights.clear();
  normWeights.clear();
  gtlSrcMapVec.clear();
  gtlTrgMapVec.clear();
  modelIndex=INVALID_MODEL_INDEX;
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
_incrInterpEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::~_incrInterpEncCondProbModel()
{
  release();
}

#endif
