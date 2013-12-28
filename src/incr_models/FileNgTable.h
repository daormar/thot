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
/* Module: FileNgTable                                              */
/*                                                                  */
/* Prototype file: FileNgTable                                      */
/*                                                                  */
/* Description: Implements an n-gram table stored in files          */
/*                                                                  */
/********************************************************************/

#ifndef _FileNgTable
#define _FileNgTable

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <LM_Defs.h>
#include "im_pair.h"
#include "StlNgtComponent.h"
#include "FileHistInfo.h"
#include <stdio.h>
#include <map>
#include <myVector.h>
#include <awkInputStream.h>

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------


//--------------- FileNgTable class

class FileNgTable
{
 public:
    typedef im_pair<Count,Count> NgInfo;
    typedef std::map<WordIndex,NgInfo> WordTableNode;
  
      // Constructor
    FileNgTable(void);

        // Initialization
    bool init(const char *fileName);

        // Functions to access table
    Count getHistInfo(const Vector<WordIndex>& hist,
                      bool& found);
    Count getNgInfo(const Vector<WordIndex>& hist,
                    const WordIndex w,
                    bool &found);
    bool getEntriesForHist(const Vector<WordIndex>& hist,
                           WordTableNode& wtnode,
                           int N=-1);

        // Function aliases
    Count getSrcInfo(const Vector<WordIndex>& srcVec,
                      bool& found);
    Count getSrcTrgInfo(const Vector<WordIndex>& srcVec,
                        const WordIndex t,
                        bool &found);
    bool getEntriesForSrc(const Vector<WordIndex>& srcVec,
                          WordTableNode& wtnode,
                          int N=-1);

        // Functions to obtain idx for histories
    NgIdx obtainIdxForHistFile(const Vector<WordIndex>& hist,
                               bool& found);

        // size and clear functions
    size_t size(void);
    void clear(void);
    void clearTempVars(void);
    
        // Destructor
    virtual ~FileNgTable();
	
 protected:
    
    FILE* ngFileHandle;
    off_t numNgRecords;
      
    FILE* histsFileHandle;
    Vector<off_t> numRecordsInHistInfoFile;

        // Auxiliary functions 
    FileHistInfo getHistInfoAux(const Vector<WordIndex>& hist,
                                bool& found);
    Count getNgInfoAux1(const Vector<WordIndex>& hist,
                        WordIndex w,
                        bool &found);
    Count getNgInfoAux2(const Vector<WordIndex>& hist,
                        WordIndex t,
                        bool &found);
    bool getEntriesForHistAux1(const Vector<WordIndex>& hist,
                               WordTableNode& wtnode,
                               Vector<pair<WordIndex,off_t> >& offsetOfRecordsVec,
                               int N);
    bool getEntriesForHistAux2(const Vector<WordIndex>& hist,
                               WordTableNode& wtnode,
                               Vector<pair<WordIndex,off_t> >& offsetOfRecordsVec,
                               int N);
    size_t getNgRecordLen(void);
    size_t getHistInfoRecordLen(unsigned int histlen);
    void obtainLimitsForHistFile(unsigned int histlen,
                                 off_t& offset,
                                 off_t& left,
                                 off_t& right);
    off_t getCurrOffsetInRecordsForNg(void);
    bool jumpNgRecords(off_t numRecords);
    size_t readNgRecord(NgIdx& hngidx,
                        WordIndex& w,
                        Count& stc);
    void obtainInfoGivenHistIdx(NgIdx idx,
                                Vector<WordIndex>& hist,
                                Count& c,
                                bool& found);
    off_t obtainOffsetOfHistGivenIdx(NgIdx idx,
                                     unsigned int& len,
                                     bool& found);
    void obtainLimitsForNgFile(NgIdx nghidx,
                               off_t& offset,
                               off_t& left,
                               off_t& right);
};

#endif
