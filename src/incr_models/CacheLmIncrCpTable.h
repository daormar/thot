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
/* Module: CacheLmIncrCpTable                                       */
/*                                                                  */
/* Prototype file: CacheLmIncrCpTable                               */
/*                                                                  */
/* Description: Class to manage n-gram language models probability  */
/*              tables using a cache architecture. The              */
/*              probabilities are of the form p(w|Vector<w>) (Note: */
/*              w is an object of the WordIndex data type).         */
/*                                                                  */
/********************************************************************/

#ifndef _CacheLmIncrCpTable
#define _CacheLmIncrCpTable

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "LM_Defs.h"
#include "StlNgtComponent.h"
#include "FileNgTable.h"
#include "BaseIncrCondProbTable.h"
#include <set>

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------

//--------------- CacheLmIncrCpTable class

class CacheLmIncrCpTable: public BaseIncrCondProbTable<Vector<WordIndex>,WordIndex,Count,Count>
{
 public:

  typedef BaseIncrCondProbTable<Vector<WordIndex>,WordIndex,Count,Count>::SrcTableNode SrcTableNode;
  typedef BaseIncrCondProbTable<Vector<WordIndex>,WordIndex,Count,Count>::TrgTableNode TrgTableNode;

      // Initialization
  bool init(const char *fileName);
  void init_alpha100(void);

      // Basic functions
  void addTableEntry(const Vector<WordIndex>& s,
                     const WordIndex& t,
                     im_pair<Count,Count> inf);
  void addSrcInfo(const Vector<WordIndex>& s,Count s_inf);
  void addSrcTrgInfo(const Vector<WordIndex>& s,
                     const WordIndex& t,
                     Count st_inf);
  void incrCountsOfEntryLog(const Vector<WordIndex>& s,
                            const WordIndex& t,
                            LogCount lc);
  im_pair<Count,Count> infSrcTrg(const Vector<WordIndex>& s,
                                 const WordIndex& t,
                                 bool& found);
  Count getSrcInfo(const Vector<WordIndex>& s,bool& found);
  Count getSrcTrgInfo(const Vector<WordIndex>& s,
                      const WordIndex& t,
                      bool& found);
  Prob pTrgGivenSrc(const Vector<WordIndex>& s,const WordIndex& t);
  LgProb logpTrgGivenSrc(const Vector<WordIndex>& s,const WordIndex& t);
  Prob pSrcGivenTrg(const Vector<WordIndex>& s,const WordIndex& t);
  LgProb logpSrcGivenTrg(const Vector<WordIndex>& s,const WordIndex& t);
  bool getEntriesForSource(const Vector<WordIndex>& s,TrgTableNode& trgtn);
  bool getEntriesForTarget(const WordIndex& t,SrcTableNode& tnode);
  bool getNbestForSrc(const Vector<WordIndex>& s,
                      NbestTableNode<WordIndex>& nbt);
  bool getNbestForTrg(const WordIndex& t,
                      NbestTableNode<Vector<WordIndex> >& nbt,
                      int N=-1);

      // Count-related functions
  Count cSrcTrg(const Vector<WordIndex>& s,const WordIndex& t);
  Count cSrc(const Vector<WordIndex>& s);
  Count cTrg(const WordIndex& t);

  LogCount lcSrcTrg(const Vector<WordIndex>& s,const WordIndex& t);
  LogCount lcSrc(const Vector<WordIndex>& s);
  LogCount lcTrg(const WordIndex& t);

      // size and clear functions
  size_t size(void);
  void clear(void);
  void clearTempVars(void);

      // destructor
  ~CacheLmIncrCpTable(){}
  
      // const_iterator
  class const_iterator;
  friend class const_iterator;
  class const_iterator
  {
   protected:
    const CacheLmIncrCpTable* clictPtr;
      
   public:
    const_iterator(void){}
    /* const_iterator(CachePhrTable::const_iterator _cachePtIter); */
    bool operator++(void); //prefix
    bool operator++(int);  //postfix
    int operator==(const const_iterator& right);
    int operator!=(const const_iterator& right);
    /* const CachePhrTable::const_iterator& operator->(void)const; */
    ~const_iterator(){}
  };
      // const_iterator-related functions
  const_iterator begin(void)const;
  const_iterator end(void)const;

 protected:

  double alpha;
  StlNgtComponent firstLevelTable;
  StlNgtComponent secondLevelTable;
  FileNgTable fileNgTable;
  std::set<Vector<WordIndex> > nodesPresentInFirstLevel;

      // Auxiliary functions
  bool nodeForSrcIsPresentInFirstLevelTable(const Vector<WordIndex>& srcVec);
      // Returns true if the n-gram history srcVec in first level table has
      // no succesive words or has not but is marked as present by means of
      // the function markSrcAsPresentInFirstLevelTable
  void markSrcAsPresentInFirstLevelTable(const Vector<WordIndex>& srcVec);
      // Marks the n-gram history srcVec in first level table as present
  void preprocSrcVector(Vector<WordIndex>& s);
};

//---------------

#endif
