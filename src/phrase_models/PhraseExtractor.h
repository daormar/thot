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
/* Module: PhraseExtractor                                          */
/*                                                                  */
/* Prototype file: PhraseExtractor                                  */
/*                                                                  */
/* Description: Defines the PhraseExtractor class for               */
/*              extracting all consistent phrases from valid        */
/*              segmentations given a phrase pair and its word      */
/*              alignment matrix.                                   */
/*                                                                  */
/********************************************************************/

#ifndef _PhraseExtractor
#define _PhraseExtractor

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

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
#include "myVector.h"

using namespace std;

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------


//--------------- PhraseExtractor class

class PhraseExtractor
{
 public:
	 	
	PhraseExtractor(void);
        
	void extractConsistentPhrases(PhraseExtractParameters phePars,
                                  const Vector<string> &ns,
                                  const Vector<string> &t,
                                  const WordAligMatrix &alig,
                                  Vector<PhrasePair>& outvph);
    double segmBasedExtraction(PhraseExtractParameters phePars,
                               const Vector<string> &ns,
                               const Vector<string> &t,
                               const WordAligMatrix &alig,
                               Vector<PhrasePair>& outvph,
                               int verbose/*=0*/);
	void clear(void);
	~PhraseExtractor();

 private:
        
    void extractConsistentPhrasesOch(PhraseExtractParameters phePars,
                                     const Vector<string> &ns,
                                     const Vector<string> &t,
                                     const WordAligMatrix &alig,
                                     Vector<PhrasePair>& outvph,
                                     BpSet& bpSet);
    double gen01RandNum(void);
    double srfPhraseExtract(PositionIndex nslen,
                            PositionIndex tlen,
                            const BpSet& bpSet,
                            BpSet& C);
    double srfPhraseExtractRec(PositionIndex nslen,
                               PositionIndex tlen,
                               const BpSet& bpSet,
                               const Bitset<MAX_SENTENCE_LENGTH>& SP,
                               const Bitset<MAX_SENTENCE_LENGTH>& TP,
                               BpSet& C);
    double srfPhraseExtractDp(PositionIndex nslen,
                              PositionIndex tlen,
                              const BpSet& bpSet,
                              BpSet& C,
                              int verbose=false);
    double approxSrfPhraseExtract(PositionIndex nslen,
                                  PositionIndex tlen,
                                  const BpSet& bpSet,
                                  BpSet& C,
                                  int verbose=false);
    void srfPhrExtrEstBisegLenRand(const BpSet& bpSet,
                                   SrfNodeInfoMap& sniMap);
    void srfPhrExtrEstBisegLenRandRec(const BpSet& bpSet,
                                      const Bitset<MAX_SENTENCE_LENGTH>& SP,
                                      const Bitset<MAX_SENTENCE_LENGTH>& TP,
                                      const SrfNodeKey& snk,
                                      SrfNodeInfoMap& sniMap);
    void fillSrfNodeInfoMap(PositionIndex nslen,
                            PositionIndex tlen,
                            const BpSet& bpSet,
                            SrfNodeInfoMap& sniMap,
                            bool calcCSet=true);
    void fillSrfNodeInfoMapRec(PositionIndex nslen,
                               PositionIndex tlen,
                               const BpSet& bpSet,
                               const Bitset<MAX_SENTENCE_LENGTH>& SP,
                               const Bitset<MAX_SENTENCE_LENGTH>& TP,
                               SrfNodeInfoMap& sniMap,
                               bool calcCSet=true);
    double bisegmRandWalk(PositionIndex nslen,
                          PositionIndex tlen,
                          const BpSet& bpSet,
                          const SrfNodeInfoMap& sniMap,
                          BpSet& C);
    bool bisegmRandWalkRec(PositionIndex nslen,
                           PositionIndex tlen,
                           const BpSet& bpSet,
                           const Bitset<MAX_SENTENCE_LENGTH>& SP,
                           const Bitset<MAX_SENTENCE_LENGTH>& TP,
                           const SrfBisegm& sb,
                           const SrfNodeInfoMap& sniMap,
                           SrfBisegm& result);
    void obtainPhrPairVecFromBpSet(const Vector<string> &ns,
                                   const Vector<string> &t,
                                   const BpSet& bpSet,
                                   Vector<PhrasePair>& outvph,
                                   double logNumSegms=0);
};

#endif
