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

#include "_incrPhraseModel.h"
#include "PhraseExtractionTable.h"

using namespace std;

//--------------- Constants ------------------------------------------

#define VERBOSE_AACHEN  -1
	 
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
                                         Vector<Vector<string> > sVec,
                                         Vector<Vector<string> > tVec,
                                         Vector<WordAligMatrix> waMatrixVec,
                                         float numReps,
                                         int verbose=0);
        // Extends the model given a vector of sentence pairs and their
        // corresponding alignment matrix.
    virtual void extendModelFromPairPlusAlig(PhraseExtractParameters phePars,
                                             bool pseudoML,
                                             Vector<string> ns,
                                             Vector<string> t,
                                             WordAligMatrix waMatrix,
                                             float numReps,
                                             int verbose=0);
        // Extends the model given a sentence pair and its corresponding
        // alignment matrix.
    void extractPhrasesFromPairPlusAlig(PhraseExtractParameters phePars,
                                        Vector<string> ns,
                                        Vector<string> t,
                                        WordAligMatrix waMatrix,
                                        Vector<PhrasePair>& vecPhPair,
                                        int /*verbose=0*/);
        // Extracts the set of consistent phrases given a sentence pair
        // and its corresponding alignment matrix.
    
	bool obtainBestAlignments(const char *bestAligFileName,
                              const char *outFileName,
                              PhraseExtractParameters phePars,
                              int verbose=0);
        // Once generated or loaded a phrase model, obtains the best
        // alignments for the sentence pairs given in bestAligFileName.
    unsigned int obtainBestAlignment(PhraseExtractParameters phePars,
                                     const Vector<string>& ns,
                                     const Vector<string>& t,
                                     const WordAligMatrix& waMatrix,
                                     WordAligMatrix &bestWaMatrix,
                                     Prob &bestProb,
                                     int verbose=0);
        // Once generated or loaded a phrase model, and given the
        // sentences s and t and their word alignment, obtains the best
        // phrase alignment and stores it in the bestWaMatrix
        // variable. The probability associated to this is also stored
        // in the bestProb variable. The function returns the total
        // number of alignments checked.
	void clear(void);

        // Utilities
    Vector<std::string> addNullWordToStrVec(const Vector<std::string>& vw);

        // Destructor
	~_wbaIncrPhraseModel();
	
 protected:

	PhraseExtractionTable phraseExtractionTable;	

    LgProb logLikelihood;
    LgProb logLikelihoodMaxApprox;
	unsigned int numSent;

	bool existRowOfNulls(unsigned int j1,
                         unsigned int j2,
                         Vector<unsigned int> &alig);
    unsigned int storePairsFromSegms(PhraseExtractParameters phePars,
                                     const Vector<string> &ns,
                                     const Vector<string> &t,
                                     const WordAligMatrix &waMatrix,
                                     float numReps,
                                     int verbose=0);
    unsigned int storePairsFromSegmsIter(PhraseExtractParameters phePars,
                                         const Vector<string> &ns,
                                         const Vector<string> &t,
                                         const WordAligMatrix &waMatrix,
                                         float numReps,
                                         int verbose=0);
	void storePhrasePairs(const Vector<PhrasePair>& vecPhPair,
                          float numReps,
                          int verbose=0);
	void storePhrasePairs(Bisegm& segmentations,
                          const WordAligMatrix& alig,
                          float numReps,
                          int verbose=0);
    void storePhrasePairs(const Vector<CellID> vcid,
                          const WordAligMatrix& alig,
                          float numReps,
                          unsigned int numSegm,
                          int verbose=0);
    void printSegmInfo(const Vector<CellID> vcid,
                       const WordAligMatrix& alig);
    void printSegmInfoAachen(const Vector<CellID> vcid,
                             const WordAligMatrix& alig);
    void obtainBestAlignments(PhraseExtractParameters phePars,
                              AlignmentExtractor &bestAlignmentsFile,
                              ostream& outF,
                              int verbose=0);
	unsigned int getBestAlignment(PhraseExtractParameters phePars,
                                  const Vector<string>& ns,
                                  const Vector<string>& t,
                                  const WordAligMatrix& waMatrix,
                                  Vector<CellID>& best_s,
                                  int verbose=0);
    unsigned int getBestAligUsingIter(PhraseExtractParameters phePars,
                                      const Vector<string>& ns,
                                      const Vector<string>& t,
                                      const WordAligMatrix& waMatrix,
                                      Vector<CellID>& best_s,
                                      int verbose=0);
	WordAligMatrix getWaMatrixForSegmentation(Vector<CellID>& segmentation);
    Prob getProbForSegmentation(Vector<CellID>& segmentation);	
	Bitset<MAX_SENTENCE_LENGTH> zeroFertBitset(Vector<unsigned int> &alig);
    ostream& printPars(ostream &outS,
                       PhraseExtractParameters phePars,
                       bool pseudoML);

};

#endif
