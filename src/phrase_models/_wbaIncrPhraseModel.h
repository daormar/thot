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
/* Module: _wbaIncrPhraseModel                                      */
/*                                                                  */
/* Prototype file: _wbaIncrPhraseModel.h                            */
/*                                                                  */
/* Description: Defines the _wbaIncrPhraseModel class.              */
/*              _wbaIncrPhraseModel is a predecessor class for      */
/*              derivating new phrase model classes which use       */
/*              word-based alignments (as those obtained with the   */
/*              GIZA++ tool).                                       */
/*                                                                  */
/********************************************************************/

#ifndef __wbaIncrPhraseModel_h
#define __wbaIncrPhraseModel_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "CategPhrasePairFilter.h"
#include "_incrPhraseModel.h"
#ifdef USE_OCH_PHRASE_EXTRACT
#include "PhraseExtractor.h"
#else
#include "PhraseExtractionTable.h"
#endif

//--------------- Constants ------------------------------------------

	 
//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------


//--------------- _wbaIncrPhraseModel class

class _wbaIncrPhraseModel: public _incrPhraseModel
{
 public:

    typedef _incrPhraseModel::SrcTableNode SrcTableNode;
    typedef _incrPhraseModel::TrgTableNode TrgTableNode;
  	
    _wbaIncrPhraseModel(void):_incrPhraseModel()
      {
        numSent=0;
      }
    
	bool generateWbaIncrPhraseModel(const char *aligFileName,
                                    PhraseExtractParameters phePars,
                                    bool pseudoML,
                                    int verbose=0); 
        // Generates a WBA Phrase Model from the provided ????.A3.final
        // Giza-style file. The data structures of the class are
        // cleared at the beginning of the process.
            
    bool extendModel(const char *aligFileName,
                     PhraseExtractParameters phePars,
                     bool pseudoML,
                     int verbose=0); 
        // Extends a WBA Phrase Model from the word alignments provided
        // ????.A3.final Giza-style file.
    void extendModelFromAlignments(PhraseExtractParameters phePars,
                                   bool pseudoML,
                                   AlignmentExtractor &outAlignments,
                                   int verbose=0);
        // Extends the model giving a previously initialized
        // AlignmentExtractor object.
    virtual void extModelFromPairAligVec(PhraseExtractParameters phePars,
                                         bool pseudoML,
                                         std::vector<std::vector<string> > sVec,
                                         std::vector<std::vector<string> > tVec,
                                         std::vector<WordAligMatrix> waMatrixVec,
                                         float numReps,
                                         int verbose=0);
        // Extends the model given a vector of sentence pairs and their
        // corresponding alignment matrix.
    virtual void extendModelFromPairPlusAlig(PhraseExtractParameters phePars,
                                             bool pseudoML,
                                             std::vector<string> ns,
                                             std::vector<string> t,
                                             WordAligMatrix waMatrix,
                                             float numReps,
                                             int verbose=0);
        // Extends the model given a sentence pair and its corresponding
        // alignment matrix.
    void extractPhrasesFromPairPlusAlig(PhraseExtractParameters phePars,
                                        std::vector<string> ns,
                                        std::vector<string> t,
                                        WordAligMatrix waMatrix,
                                        std::vector<PhrasePair>& vecPhPair,
                                        int /*verbose=0*/);
        // Extracts the set of consistent phrases given a sentence pair
        // and its corresponding alignment matrix.
    
	void clear(void);

        // Utilities
    std::vector<std::string> addNullWordToStrVec(const std::vector<std::string>& vw);

        // Destructor
	~_wbaIncrPhraseModel();
	
 protected:

#ifdef USE_OCH_PHRASE_EXTRACT
	PhraseExtractor phraseExtract;
#else
	PhraseExtractionTable phraseExtract;
#endif
    
    LgProb logLikelihood;
    LgProb logLikelihoodMaxApprox;
	unsigned int numSent;
    CategPhrasePairFilter phrasePairFilter;
      
	bool existRowOfNulls(unsigned int j1,
                         unsigned int j2,
                         std::vector<unsigned int> &alig);
	void storePhrasePairs(const std::vector<PhrasePair>& vecPhPair,
                          float numReps,
                          int verbose=0);
	Bitset<MAX_SENTENCE_LENGTH> zeroFertBitset(std::vector<unsigned int> &alig);
    std::ostream& printPars(std::ostream &outS,
                       PhraseExtractParameters phePars,
                       bool pseudoML);

};

#endif
