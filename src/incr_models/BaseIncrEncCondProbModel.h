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
/* Module: BaseIncrEncCondProbModel                                 */
/*                                                                  */
/* Prototype file: BaseIncrEncCondProbModel                         */
/*                                                                  */
/* Description: Abstract class to manage incremental encoded        */
/*              conditional probability models Pr(t|s).             */
/*                                                                  */
/********************************************************************/

#ifndef _BaseIncrEncCondProbModel
#define _BaseIncrEncCondProbModel

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseIncrCondProbModel.h"

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------

//--------------- BaseIncrEncCondProbModel class

#ifdef INCRMODELS_HAVE_BASECONDPROBTABLE_H
#include <BaseEncCondProbModel.h>
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
class BaseIncrEncCondProbModel: public BaseIncrCondProbModel<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>,public BaseEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,im_pair<SRC_INFO,SRCTRG_INFO> >
#else
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
class BaseIncrEncCondProbModel: public BaseIncrCondProbModel<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>
#endif
{
 public:
  typedef typename BaseIncrCondProbModel<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::SrcTableNode SrcTableNode;
  typedef typename BaseIncrCondProbModel<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::TrgTableNode TrgTableNode;

      // Basic functions
  virtual void addTableEntryHigh(const HSRCDATA& hs,
                                 const HTRGDATA& ht,
                                 im_pair<SRC_INFO,SRCTRG_INFO> inf)=0;
      // High level version of the addTableEntry function
  virtual void addHSrcInfo(const HSRCDATA& hs,SRC_INFO s_inf)=0;
  virtual void addHSrcHTrgInfo(const HSRCDATA& hs,
                               const HTRGDATA& ht,
                               SRCTRG_INFO st_inf)=0;
  virtual void incrCountsOfEntryHigh(const HSRCDATA& hs,
                                     const HTRGDATA& ht,
                                     Count c)=0;
      // High level version of the incrCountOfEntry function
  void incrCountsOfEntryHighLog(const HSRCDATA& hs,
                                const HTRGDATA& ht,
                                LogCount lc);
      // High level version of the incrCountOfEntryLog function

  virtual im_pair<SRC_INFO,SRCTRG_INFO> infHTrgHSrc(const HSRCDATA& hs,
                                                    const HTRGDATA& ht,
                                                    bool& found)=0;
  virtual SRC_INFO getHSrcInfo(const HSRCDATA& hs,bool& found)=0;
  virtual SRCTRG_INFO getHSrcHTrgInfo(const HSRCDATA& hs,
                                      const HTRGDATA& ht,
                                      bool& found)=0;
  virtual Prob pHTrgGivenHSrc(const HSRCDATA& hs,const HTRGDATA& ht)=0;
      // High level version of the pTrgGivenSrc function
  virtual LgProb logpHTrgGivenHSrc(const HSRCDATA& hs,const HTRGDATA& ht)=0;
      // High level version of the logpTrgGivenSrc function
  virtual Prob pHSrcGivenHTrg(const HSRCDATA& s,const HTRGDATA& t)=0;
      // High level version of the pSrcGivenTrg function
  virtual LgProb logpHSrcGivenHTrg(const HSRCDATA& s,const HTRGDATA& t)=0;
      // High level version of the logpSrcGivenTrg function  
  virtual bool getEntriesForHTarget(const HTRGDATA& ht,SrcTableNode& tnode)=0;
      // High level version of the getEntriesForTarget function
  virtual bool getEntriesForHSource(const HSRCDATA& ht,TrgTableNode& tnode)=0;
      // High level version of the getEntriesForSource function
  virtual bool getNbestForHSrc(const HSRCDATA& hs,
                               NbestTableNode<TRGDATA>& nbt)=0;
      // High level version of the getNbestForSrc function
  virtual bool getNbestForHTrg(const HTRGDATA& ht,
                               NbestTableNode<SRCDATA>& nbt,int N=-1)=0;
      // High level version of the getNbestForTrg function

      // Count-related functions
  Count cHSrcHTrg(const HSRCDATA& hs,const HTRGDATA& ht);
  Count cHSrc(const HSRCDATA& hs);
  Count cHTrg(const HTRGDATA& ht);

  virtual LogCount lcHSrcHTrg(const HSRCDATA& hs,const HTRGDATA& ht)=0;
  virtual LogCount lcHSrc(const HSRCDATA& hs)=0;
  virtual LogCount lcHTrg(const HTRGDATA& ht)=0;

      // Encoding-related functions
  virtual bool HighSrc_to_Src(const HSRCDATA& hs,SRCDATA& s)const=0;
      // Given a HSRCDATA object "hs" obtains its corresponding
      // encoded value in "s". Returns true if the encoding
      // was successful ("hs" exists in the vocabulary).
  virtual bool HighTrg_to_Trg(const HTRGDATA& ht,TRGDATA& t)const=0;
      // The same for HTRGDATA objects

  virtual bool Src_to_HighSrc(const SRCDATA& s,HSRCDATA& hs)const=0;
      // Performs the inverse process (s -> hs)
  virtual bool Trg_to_HighTrg(const TRGDATA& t,HTRGDATA& ht)const=0;
      // The same for TRGDATA objects (t -> ht)

  virtual SRCDATA addHSrcCode(const HSRCDATA &hs)=0;
       // sets the codification for hs (hs->s)
  virtual TRGDATA addHTrgCode(const HTRGDATA &ht)=0;
       // sets the codifcation for ht (ht->t)
  virtual unsigned int sizeSrcEnc(void)=0;
  virtual unsigned int sizeTrgEnc(void)=0;
  virtual bool loadEncodingInfo(const char *prefixFileName)=0;
      // Load encoding information given a prefix file name
  virtual bool printEncodingInfo(const char *prefixFileName)=0;
      // Prints encoding information
  virtual void clearEncodingInfo(void)=0;
      // Clears encoding information
      
      // Destructor
  ~BaseIncrEncCondProbModel();
   
 protected:

};

//--------------- Template function definitions

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
BaseIncrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::~BaseIncrEncCondProbModel()
{
  
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
void BaseIncrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::incrCountsOfEntryHighLog(const HSRCDATA& hs,
                                                                                                                const HTRGDATA& ht,
                                                                                                                LogCount lc)
{
  incrCountsOfEntryHigh(hs,ht,exp((float)lc));
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
Count BaseIncrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::cHSrcHTrg(const HSRCDATA& hs,
                                                                                                  const HTRGDATA& ht)
{
  return exp((float)lcHSrcHTrg(hs,ht));
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
Count BaseIncrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::cHSrc(const HSRCDATA& hs)
{
  return exp((float)lcHSrc(hs));
}

//---------------
template<class HSRCDATA,class HTRGDATA,class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
Count BaseIncrEncCondProbModel<HSRCDATA,HTRGDATA,SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::cHTrg(const HTRGDATA& ht)
{
  return exp((float)lcHTrg(ht));
}

#endif
