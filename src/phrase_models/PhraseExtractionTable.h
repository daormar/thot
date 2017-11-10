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
/* Module: PhraseExtractionTable                                    */
/*                                                                  */
/* Prototype file: PhraseExtractionTable                            */
/*                                                                  */
/* Description: Defines the PhraseExtractionTable class for         */
/*              extracting all consistent phrases from valid        */
/*              segmentations given a phrase pair and its word      */
/*              alignment matrix.                                   */
/*                                                                  */
/********************************************************************/

#ifndef _PhraseExtractionTable
#define _PhraseExtractionTable

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "PhraseExtractionCell.h"
#include "CellID.h"
#include "WordAligMatrix.h"
#include "PhraseDefs.h"
#include "PhraseExtractParameters.h"
#include "PhrasePair.h"
#include "BpSet.h"
#include "SrfNodeInfoMap.h"
#include "SrfBisegm.h"
#include "Bitset.h"
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------


//--------------- PhraseExtractionTable class

class PhraseExtractionTable
{
 public:
	 	
	PhraseExtractionTable(void);        
	void extractConsistentPhrases(PhraseExtractParameters phePars,
                                  const std::vector<std::string> &_ns,
                                  const std::vector<std::string> &_t,
                                  const WordAligMatrix &_alig,
                                  std::vector<PhrasePair>& outvph);
    double segmBasedExtraction(PhraseExtractParameters phePars,
                               const std::vector<std::string> &_ns,
                               const std::vector<std::string> &_t,
                               const WordAligMatrix &_alig,
                               std::vector<PhrasePair>& outvph,
                               int verbose=0);
	void clear(void);
	~PhraseExtractionTable();

 private:

    std::vector<std::vector<PhraseExtractionCell> > pecMatrix;
    
    std::vector<std::string> ns;
	std::vector<std::string> t;
	WordAligMatrix alig;
	unsigned int nslen;
	unsigned int tlen;
    Bitset<MAX_SENTENCE_LENGTH> zFertBitset;
    Bitset<MAX_SENTENCE_LENGTH> spurBitset;
    
	int maxTrgPhraseLength;
    int maxSrcPhraseLength;
    bool countSpurious;
	bool monotone;

    void extractConsistentPhrasesOld(PhraseExtractParameters phePars,
                                     const std::vector<std::string> &_ns,
                                     const std::vector<std::string> &_t,
                                     const WordAligMatrix &_alig,
                                     std::vector<PhrasePair>& outvph);
    void extractConsistentPhrasesOch(PhraseExtractParameters phePars,
                                     const std::vector<std::string> &_ns,
                                     const std::vector<std::string> &_t,
                                     const WordAligMatrix &_alig,
                                     std::vector<PhrasePair>& outvph);
    double gen01RandNum(void);
    void obtainConsistentPhrases(void);
    void obtainBpSet(BpSet& bpSet);
    double srfPhraseExtract(const BpSet& bpSet,
                            BpSet& C);
    double srfPhraseExtractRec(const BpSet& bpSet,
                               const Bitset<MAX_SENTENCE_LENGTH>& SP,
                               const Bitset<MAX_SENTENCE_LENGTH>& TP,
                               BpSet& C);
    double srfPhraseExtractDp(const BpSet& bpSet,
                              BpSet& C,
                              int verbose=false);
    double approxSrfPhraseExtract(const BpSet& bpSet,
                                  BpSet& C,
                                  int verbose=false);
    void srfPhrExtrEstBisegLenRand(const BpSet& bpSet,
                                   SrfNodeInfoMap& sniMap);
    void srfPhrExtrEstBisegLenRandRec(const BpSet& bpSet,
                                      const Bitset<MAX_SENTENCE_LENGTH>& SP,
                                      const Bitset<MAX_SENTENCE_LENGTH>& TP,
                                      const SrfNodeKey& snk,
                                      SrfNodeInfoMap& sniMap);
    void fillSrfNodeInfoMap(const BpSet& bpSet,
                            SrfNodeInfoMap& sniMap,
                            bool calcCSet=true);
    void fillSrfNodeInfoMapRec(const BpSet& bpSet,
                               const Bitset<MAX_SENTENCE_LENGTH>& SP,
                               const Bitset<MAX_SENTENCE_LENGTH>& TP,
                               SrfNodeInfoMap& sniMap,
                               bool calcCSet=true);
    double bisegmRandWalk(const BpSet& bpSet,
                          const SrfNodeInfoMap& sniMap,
                          BpSet& C);
    bool bisegmRandWalkRec(const BpSet& bpSet,
                           const Bitset<MAX_SENTENCE_LENGTH>& SP,
                           const Bitset<MAX_SENTENCE_LENGTH>& TP,
                           const SrfBisegm& sb,
                           const SrfNodeInfoMap& sniMap,
                           SrfBisegm& result);
    void obtainPhrPairVecFromBpSet(const BpSet& bpSet,
                                   std::vector<PhrasePair>& outvph,
                                   double logNumSegms=0);
    void createVectorWithConsPhrases(std::vector<PhrasePair>& consistentPhrases);
	void getSegmentationsForEachCell(void);
	void getSegmentationsForEachCellFast(void);
	bool validCoverageForCell(Bitset<MAX_SENTENCE_LENGTH>& c,
	                          unsigned int x,
                              unsigned int y);
    bool validSegmentationForCell(const std::vector<CellID> &cidVec,
                                  Bitset<MAX_SENTENCE_LENGTH>& zFertBits,
								  unsigned int x,
                                  unsigned int y,
                                  unsigned int first=0);
    Bitset<MAX_SENTENCE_LENGTH> zeroFertBitset(WordAligMatrix &waMatrix);
	Bitset<MAX_SENTENCE_LENGTH> spuriousWordsBitset(WordAligMatrix &waMatrix);
	bool existCellAlig(const std::vector<CellAlignment> &cellAligs,
                       CellAlignment calig);
	unsigned int leftMostPosInCell(unsigned int x,
                                   unsigned int y);
	unsigned int rightMostPosInCell(unsigned int x,
                                    unsigned int y);
	bool sourcePosInCell(unsigned int j,
                         unsigned int x,
                         unsigned int y);
    unsigned int trgPhraseLengthInCell(unsigned int x,
                                       unsigned int y);
	unsigned int
      trgPhraseLengthInCellNonSpurious(unsigned int x,
                                       unsigned int y,
                                       Bitset<MAX_SENTENCE_LENGTH>& spurBits);
};

#endif
