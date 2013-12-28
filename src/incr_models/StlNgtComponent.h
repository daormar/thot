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
/* Module: StlNgtComponent                                          */
/*                                                                  */
/* Prototype file: StlNgtComponent.h                                */
/*                                                                  */
/* Description: Defines the StlNgtComponent class. This class       */
/*              is used as a component when implementing the        */
/*              StlPhraseTable class.                               */
/*                                                                  */
/********************************************************************/

#ifndef _StlNgtComponent_h
#define _StlNgtComponent_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <LM_Defs.h>
#include "im_pair.h"
#include <myVector.h>
#include <map>
#include <utility>

using namespace std;

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------


//--------------- StlNgtComponent class

class StlNgtComponent
{
  public:
    typedef std::map<Vector<WordIndex>,std::pair<NgIdx,Count> > HistInfo;
    typedef Vector<HistInfo::iterator> HistNgIdxInfo;
    typedef std::map<WordIndex,Count> NgcNode;
    typedef Vector<NgcNode> NgCounts;

        // Constructor
    StlNgtComponent(void);

        // Basic functions
    NgIdx updateHistInfo(const Vector<WordIndex>& hist,
                         Count histCount);
    NgIdx updateHistInfoGivenIdx(const Vector<WordIndex>& hist,
                                 NgIdx hngidx,
                                 Count histCount,
                                 bool& idxAlreadyExists);
    std::pair<NgIdx,Count> getHistInfo(const Vector<WordIndex>& hist,
                                       bool& found);
    Count getSrcInfo(const Vector<WordIndex>& srcVec,
                     bool& found);
    void getHistGivenNgIdx(NgIdx hngidx,
                           Vector<WordIndex>& hist,
                           Count& c,
                           bool& found);
    std::pair<Count,Count> getNgCounts(const Vector<WordIndex>& hist,
                                       WordIndex w,
                                       bool& found);
    Count getSrcTrgInfo(const Vector<WordIndex>& srcVec,
                        WordIndex t,
                        bool& found);
    NgIdx updateNgCounts(const Vector<WordIndex>& hist,
                         WordIndex w,
                         Count histCount,
                         Count srcTrgCount);
    void addTableEntry(const Vector<WordIndex>& s,
                       const WordIndex& t,
                       im_pair<Count,Count> inf);
    NgIdx increaseNgCount(const Vector<WordIndex>& hist,
                          WordIndex w,
                          Count c);
    void updateJointCount(NgIdx histNgIdx,
                          WordIndex w,
                          Count jcount);
    Count getNgJointCount(NgIdx histNgIdx,
                          WordIndex w,
                          NgcNode::iterator& nnIter,
                          bool& found);

        // Additional Functions
    bool getWordsForHist(const Vector<WordIndex>& hist,
                         NgcNode& nnode);
    bool getEntriesForSource(const Vector<WordIndex>& srcVec,
                             std::map<WordIndex,im_pair<Count,Count> >& trgtn);
    bool nodeForHistHasAtLeastOneWord(const Vector<WordIndex>& hist);
        // Returns true if t has one translation or more

        // size() function
    size_t size(void);

        // clear() function
    void clear(void);

          // const_iterator
    class const_iterator;
    friend class const_iterator;
    class const_iterator
      {
        protected:
           const StlNgtComponent* sptcPtr;
           /* PhraseDict::const_iterator pdIter; */
           
        public:
           const_iterator(void){sptcPtr=NULL;}
           /* const_iterator(const StlNgtComponent* _sptcPtr, */
           /*                PhraseDict::const_iterator iter):sptcPtr(_sptcPtr),pdIter(iter) */
           /*   { */
           /*   } */
           bool operator++(void); //prefix
           bool operator++(int);  //postfix
           int operator==(const const_iterator& right); 
           int operator!=(const const_iterator& right); 
           /* const PhraseDict::const_iterator& operator->(void)const; */
      };

        // const_iterator related functions
    const_iterator begin(void)const; 
    const_iterator end(void)const; 

  private:
        // Data members
    HistInfo histInfo;
    HistNgIdxInfo histNgIdxInfo;
    NgCounts ngCounts;
};

#endif
