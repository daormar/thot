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
/* Module: FilePt                                                   */
/*                                                                  */
/* Prototype file: FilePt                                           */
/*                                                                  */
/* Description: Implements a phrase table stored in files           */
/*                                                                  */
/********************************************************************/

#ifndef _FilePt
#define _FilePt

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "PhraseTransTableNodeData.h"
#include "NbestTableNode.h"
#include "PhrasePairInfo.h"
#include "PhraseDefs.h"
#include "MathDefs.h"
#include <stdio.h>
#include <map>
#include <myVector.h>
#include <awkInputStream.h>

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------


//--------------- FilePt class

class FilePt
{
 public:
    typedef std::map<Vector<WordIndex>,PhrasePairInfo> SrcTableNode;
    typedef std::map<Vector<WordIndex>,PhrasePairInfo> TrgTableNode;
  
        // Constructor
    FilePt(void);

        // Initialization
    bool init(const char *fileName);

        // Functions to access table
    Count getSrcInfo(const Vector<WordIndex>& s,
                     bool& found);
    Count getTrgInfo(const Vector<WordIndex>& t,
                     bool& found);
    Count getSrcTrgInfo(const Vector<WordIndex>& s,
                        const Vector<WordIndex>& t,
                        bool &found);
    bool getEntriesForTarget(const Vector<WordIndex>& t,
                             SrcTableNode& srctn,
                             int N=-1);

        // Functions to obtain log-probabilities
    Prob pTrgGivenSrc(const Vector<WordIndex>& s,
                      const Vector<WordIndex>& t);
    LgProb logpTrgGivenSrc(const Vector<WordIndex>& s,
                           const Vector<WordIndex>& t);
    Prob pSrcGivenTrg(const Vector<WordIndex>& s,
                      const Vector<WordIndex>& t);
    LgProb logpSrcGivenTrg(const Vector<WordIndex>& s,
                           const Vector<WordIndex>& t);

        // Additional functions
    bool getNbestForTrg(const Vector<WordIndex>& t,
                        NbestTableNode<PhraseTransTableNodeData>& nbt,
                        int N);

        // size and clear functions
    size_t size(void);
    void clear(void);
    void clearTempVars(void);
    
        // Destructor
    virtual ~FilePt();
	
 protected:
    
    FILE* srcTransForTrgFileHandle;
    off_t numDictRecords;
      
    FILE* srcPhrasesFileHandle;
    Vector<off_t> numRecordsInSrcInfoFile;

    FILE* trgPhrasesFileHandle;
    Vector<off_t> numRecordsInTrgInfoFile;

        // Auxiliary functions
    PhrIndex obtainIdxForSrcPhraseFile(const Vector<WordIndex>& s,
                                       bool& found);
    PhrIndex obtainIdxForTrgPhraseFile(const Vector<WordIndex>& t,
                                       bool& found);
    bool loadSrcCountsFile(std::string fileName);
    bool loadTrgCountsFile(std::string fileName);
    bool processDictFile(std::string fileName);
    pair<Count,PhrIndex> getSrcInfoAux(const Vector<WordIndex>& s,
                                       bool& found);
    Count getSrcTrgInfoAux1(const Vector<WordIndex>& s,
                            const Vector<WordIndex>& t,
                            bool &found);
    Count getSrcTrgInfoAux2(const Vector<WordIndex>& s,
                            const Vector<WordIndex>& t,
                            bool &found);
    bool obtainInfoForTrgPhraseFile(const Vector<WordIndex>& t,
                                    PhrIndex& phrIdx,
                                    Count& count,
                                    long long& dictStartRecord);
    bool getEntriesForTargetAux(const Vector<WordIndex>& t,
                                SrcTableNode& srctn,
                                Vector<pair<Vector<WordIndex>,off_t> >& offsetOfRecordsVec,
                                int N=-1);
    void rewindUntilFirstOcurrenceOfTrgPhrase(PhrIndex tidx);
    size_t getDictRecordLen(void);
    size_t getSrcInfoRecordLen(unsigned int srcphrlen);
    size_t getTrgInfoRecordLen(unsigned int trgphrlen);
    void obtainLimitsForSrcPhrasesFile(unsigned int srcphrlen,
                                       off_t& offset,
                                       off_t& left,
                                       off_t& right);
    off_t getCurrOffsetInRecordsForDict(void);
    bool jumpDictRecords(off_t numRecords);
    size_t readDictRecord(PhrIndex& spidx,
                          PhrIndex& tpidx,
                          Count& stc);
    void obtainInfoGivenScrPhrIdx(PhrIndex idx,
                                  Vector<WordIndex>& s,
                                  Count& c,
                                  bool& found);
    off_t obtainOffsetOfSrcPhraseGivenIdx(PhrIndex idx,
                                          unsigned int& len,
                                          bool& found);
    bool searchTargetPhraseInDict(PhrIndex tidx);
    void obtainLimitsForTrgPhrasesFile(unsigned int trgphrlen,
                                       off_t& offset,
                                       off_t& left,
                                       off_t& right);
    void addTransOptsFromCurrentFilePos(PhrIndex tidx,
                                        Count trgCount,
                                        FilePt::SrcTableNode& srctn,
                                        Vector<pair<Vector<WordIndex>,off_t> >& offsetOfRecordsVec);
    void obtainLimitsForDictFile(off_t& offset,
                                 off_t& left,
                                 off_t& right);
};

#endif
