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

#include "PhraseExtractionTable.h"
#include "WordAligMatrix.h"
#include THOT_PPINFO_H // Define PpInfo type. It is set in
                       // configure by checking PPINFO_H variable
                       // (default value: PpInfo.h)
#include "BaseSwAligModel.h"
#include "PhrasePair.h"
#include "PhraseDefs.h"
#include <stdio.h>
#include "myVector.h"
#include <string>

namespace PhraseExtractUtils
{
  int extractPhrPairsFromCorpusFiles(std::string srcCorpusFileName,
                                     std::string trgCorpusFileName,
                                     BaseSwAligModel<PpInfo>* swAligModelPtr,
                                     BaseSwAligModel<PpInfo>* invSwAligModelPtr,
                                     Vector<Vector<PhrasePair> >& invPhrPairs,
                                     int verbose=0);
  int extractConsistentPhrasePairs(const Vector<std::string>& srcSentStrVec,
                                   const Vector<std::string>& refSentStrVec,
                                   BaseSwAligModel<PpInfo>* swAligModelPtr,
                                   BaseSwAligModel<PpInfo>* invSwAligModelPtr,
                                   Vector<PhrasePair>& vecInvPhPair,
                                   bool verbose=0);
  void extractPhrasesFromPairPlusAlig(PhraseExtractParameters phePars,
                                      Vector<string> ns,
                                      Vector<string> t,
                                      WordAligMatrix waMatrix,
                                      Vector<PhrasePair>& vecPhPair,
                                      int verbose=0);
}

#endif
