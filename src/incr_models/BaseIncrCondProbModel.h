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
/* Module: BaseIncrCondProbModel                                    */
/*                                                                  */
/* Prototype file: BaseIncrCondProbModel                            */
/*                                                                  */
/* Description: Abstract base class to manage incremental           */
/*              conditional probability models p(t|s).              */
/*                                                                  */
/********************************************************************/

#ifndef _BaseIncrCondProbModel
#define _BaseIncrCondProbModel

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <ErrorDefs.h>
#include <StatModelDefs.h>
#include "im_pair.h"
#include <map>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <NbestTableNode.h>

using namespace std;

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------

//--------------- BaseIncrCondProbModel class

#ifdef INCRMODELS_HAVE_BASECONDPROBTABLE_H
#include <BaseCondProbModel.h>
template<class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
class BaseIncrCondProbModel: public BaseCondProbModel<SRCDATA,TRGDATA,im_pair<SRC_INFO,SRCTRG_INFO> >
#else
template<class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
class BaseIncrCondProbModel
#endif
{
 public:
   typedef typename std::map<SRCDATA,im_pair<SRC_INFO,SRCTRG_INFO> > SrcTableNode;
   typedef typename std::map<TRGDATA,im_pair<SRC_INFO,SRCTRG_INFO> > TrgTableNode;
   
   struct value_type
   {
     SRCDATA s;
     TRGDATA t;
     SRCTRG_INFO inf_srctrg;
     SRC_INFO inf_src;
   };
   
       // Basic functions
   virtual void addTableEntry(const SRCDATA& s,
                              const TRGDATA& t,
                              im_pair<SRC_INFO,SRCTRG_INFO> inf)=0;
       // Adds an entry to the probability table
   void addTableEntry(const value_type& vt);
   virtual void addSrcInfo(const SRCDATA& s,SRC_INFO s_inf)=0;
   virtual void addSrcTrgInfo(const SRCDATA& s,
                              const TRGDATA& t,
                              SRCTRG_INFO st_inf)=0;
   
       // Increase the counts of a given phrase pair
   virtual void incrCountsOfEntry(const SRCDATA& s,
                                  const TRGDATA& t,
                                  Count c)=0;
   virtual void incrCountsOfEntryLog(const SRCDATA& s,
                                     const TRGDATA& t,
                                     LogCount lc);
   
       // Get information about source and target
   virtual im_pair<SRC_INFO,SRCTRG_INFO> infSrcTrg(const SRCDATA& s,
                                                   const TRGDATA& t,
                                                   bool& found)=0;
       // Returns information related to a given s and t.  Note:
       // SRCTRG_INFO must have a member function called get_c_st() that
       // returns the Count of s and t, and SRC_INFO must have a
       // function called get_c_s() that returns the Count of s
   virtual SRC_INFO getSrcInfo(const SRCDATA& s,bool& found)=0;
   virtual SRCTRG_INFO getSrcTrgInfo(const SRCDATA& s,
                                     const TRGDATA& t,
                                     bool& found)=0;
   virtual Prob pTrgGivenSrc(const SRCDATA& s,const TRGDATA& t)=0;
   virtual LgProb logpTrgGivenSrc(const SRCDATA& s,const TRGDATA& t)=0;
   virtual Prob pSrcGivenTrg(const SRCDATA& s,const TRGDATA& t)=0;
   virtual LgProb logpSrcGivenTrg(const SRCDATA& s,const TRGDATA& t)=0;
   virtual bool getEntriesForTarget(const TRGDATA& t,SrcTableNode& srctn)=0;
       // Returns the entries associated to a given TRGDATA t
   virtual bool getEntriesForSource(const SRCDATA& s,TrgTableNode& trgtn)=0;
       // Returns the entries associated to a given SRCDATA s
   virtual bool getNbestForSrc(const SRCDATA& s,
                               NbestTableNode<TRGDATA>& nbt)=0;
   virtual bool getNbestForTrg(const TRGDATA& t,
                               NbestTableNode<SRCDATA>& nbt,int N=-1)=0;

       // Count-related functions
   virtual Count cSrcTrg(const SRCDATA& s,const TRGDATA& t);
   virtual Count cSrc(const SRCDATA& s);
   virtual Count cTrg(const TRGDATA& t);

   virtual LogCount lcSrcTrg(const SRCDATA& s,const TRGDATA& t)=0;
   virtual LogCount lcSrc(const SRCDATA& s)=0;
   virtual LogCount lcTrg(const TRGDATA& t)=0;

       // size, clear functions
   virtual size_t size(void)=0;
   virtual void clear(void)=0;

       // Functions to load and print the model
   virtual bool load(const char *fileName)=0;
   virtual bool print(const char *fileName)=0;

       // destructor
   virtual ~BaseIncrCondProbModel(){};
   
 protected:
  
};

//--------------- Template function definitions

//---------------
template<class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
void BaseIncrCondProbModel<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::addTableEntry(const value_type& vt)
{
  this->addTableEntry(vt.s,vt.t,vt.inf);
}

//---------------
template<class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
void BaseIncrCondProbModel<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::incrCountsOfEntryLog(const SRCDATA& s,
                                                                                       const TRGDATA& t,
                                                                                       LogCount lc)
{
  incrCountsOfEntry(s,t,exp((float)lc));
}

//---------------
template<class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
Count BaseIncrCondProbModel<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::cSrcTrg(const SRCDATA& s,const TRGDATA& t)
{
  return exp((float)lcSrcTrg(s,t));
}

//---------------
template<class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
Count BaseIncrCondProbModel<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::cSrc(const SRCDATA& s)
{
  return exp((float)lcSrc(s));  
}

//---------------
template<class SRCDATA,class TRGDATA,class SRC_INFO,class SRCTRG_INFO>
Count BaseIncrCondProbModel<SRCDATA,TRGDATA,SRC_INFO,SRCTRG_INFO>::cTrg(const TRGDATA& t)
{
  return exp((float)lcTrg(t));
}

#endif
