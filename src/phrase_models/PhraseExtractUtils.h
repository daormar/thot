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
 * @file PhraseExtractUtils.h
 * @brief Defines string processing utilities
 */

#ifndef _PhraseExtractUtils_h
#define _PhraseExtractUtils_h

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "CategPhrasePairFilter.h"
#include "PhraseExtractionTable.h"
#include "WordAligMatrix.h"
#include THOT_PPINFO_H // Define PpInfo type. It is set in
                       // configure by checking PPINFO_H variable
                       // (default value: PpInfo.h)
#include "BaseSwAligModel.h"
#include "BaseIncrPhraseModel.h"
#include "PhrasePair.h"
#include "PhraseDefs.h"
#include <stdio.h>
#include <string>
#include <vector>

namespace PhraseExtractUtils
{
  int extractPhrPairsFromCorpusFiles(BaseSwAligModel<PpInfo>* swAligModelPtr,
                                     BaseSwAligModel<PpInfo>* invSwAligModelPtr,
                                     std::string srcCorpusFileName,
                                     std::string trgCorpusFileName,
                                     std::vector<std::vector<PhrasePair> >& phrPairs,
                                     int verbose=0);
  int extractConsistentPhrasePairs(BaseSwAligModel<PpInfo>* swAligModelPtr,
                                   BaseSwAligModel<PpInfo>* invSwAligModelPtr,
                                   const std::vector<std::string>& srcSentStrVec,
                                   const std::vector<std::string>& refSentStrVec,
                                   std::vector<PhrasePair>& vecPhrPair,
                                   bool verbose=0);
  void extractPhrasesFromPairPlusAlig(PhraseExtractParameters phePars,
                                      std::vector<string> ns,
                                      std::vector<string> t,
                                      WordAligMatrix waMatrix,
                                      std::vector<PhrasePair>& vecPhrPair,
                                      int verbose=0);
  void extractPhrasesFromPairPlusAligBrf(PhraseExtractParameters phePars,
                                         std::vector<string> ns,
                                         std::vector<string> t,
                                         WordAligMatrix waMatrix,
                                         std::vector<PhrasePair>& vecPhrPair,
                                         int verbose=0);
  void filterPhrasePairs(const std::vector<PhrasePair>& vecUnfiltPhrPair,
                         std::vector<PhrasePair>& vecPhrPair);
}

#endif
