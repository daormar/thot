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
 * @file _incrPhraseModel.h
 * 
 * @brief Defines the _incrPhraseModel base class. _incrPhraseModel is
 * derived from the abstract class BasePhraseModel.
 */

#ifndef __incrPhraseModel_h
#define __incrPhraseModel_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseIncrPhraseModel.h"
#include "NbestTransTable.h"
#include "WordAligMatrix.h"
#include "SingleWordVocab.h"
#include "printAligFuncs.h"
#include "SegLenTable.h"
#include "SrcSegmLenTable.h"
#include "TrgCutsTable.h"
#include "TrgSegmLenTable.h"
#include "AlignmentExtractor.h"
#include "ModelDescriptorUtils.h"
#include "Bitset.h"
#include "AwkInputStream.h"
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>

//--------------- Constants ------------------------------------------

#define THOT_COUNT_OUTPUT       2 

//--------------- typedefs -------------------------------------------

typedef NbestTransTable<std::vector<WordIndex>,PhraseTransTableNodeData> PhraseNbestTransTable;

//--------------- Classes --------------------------------------------


//--------------- _incrPhraseModel class

class _incrPhraseModel: public BaseIncrPhraseModel
{
 public:

    typedef BaseIncrPhraseModel::SrcTableNode SrcTableNode;
    typedef BaseIncrPhraseModel::TrgTableNode TrgTableNode;

        // Constructor
    _incrPhraseModel(void);

        // Functions to extend or modify the model
    void strAddTableEntry(const std::vector<std::string>& s,
                          const std::vector<std::string>& t,
                          PhrasePairInfo inf);
    void addTableEntry(const std::vector<WordIndex>& s,
                       const std::vector<WordIndex>& t,
                       PhrasePairInfo inf);
	void strIncrCountsOfEntry(const std::vector<std::string>& s,
                              const std::vector<std::string>& t,
                              Count count=1);
    void incrCountsOfEntry(const std::vector<WordIndex>& s,
                           const std::vector<WordIndex>& t,
                           Count count=1);

        // Counts-related functions
    Count cSrcTrg(const std::vector<WordIndex>& s,
                  const std::vector<WordIndex>& t);
    Count cSrc(const std::vector<WordIndex>& s);
    Count cTrg(const std::vector<WordIndex>& t);

    Count cHSrcHTrg(const std::vector<std::string>& hs,
                    const std::vector<std::string>& ht);
    Count cHSrc(const std::vector<std::string>& hs);
    Count cHTrg(const std::vector<std::string>& ht);

        // Functions to access model probabilities

    Prob pk_tlen(unsigned int tlen,unsigned int k);
        // Returns p(k|J), k-> segmentation length, tlen-> length of the
        // target sentence

    LgProb srcSegmLenLgProb(unsigned int x_k,
                            unsigned int x_km1,
                            unsigned int srcLen);
        // obtains the log-probability for the length of a source
        // segment log(p(x_k|x_{k-1},srcLen))

    LgProb trgCutsLgProb(int offset);
        // Returns phrase alignment log-probability given the offset
        // between the last target phrase and the new one
        // log(p(y_k|y_{k-1}))

    LgProb trgSegmLenLgProb(unsigned int k,
                            const SentSegmentation& trgSegm,
                            unsigned int trgLen,
                            unsigned int lastSrcSegmLen);
        // obtains the log-probability for the length of a target
        // segment log(p(z_k|y_k,x_k-x_{k-1},trgLen))

    PhrasePairInfo infSrcTrg(const std::vector<WordIndex>& s,
                             const std::vector<WordIndex>& t,
                             bool& found);

	LgProb logpt_s_(const std::vector<WordIndex>& s,
                    const std::vector<WordIndex>& t);
	
	LgProb logps_t_(const std::vector<WordIndex>& s,
                    const std::vector<WordIndex>& t);


        // Functions to obtain translations for source or target phrases
    bool getTransFor_s_(const std::vector<WordIndex>& s,
                        TrgTableNode& trgtn);
    bool getTransFor_t_(const std::vector<WordIndex>& t,
                        SrcTableNode& srctn);
	bool getNbestTransFor_s_(const std::vector<WordIndex>& s,
                             NbestTableNode<PhraseTransTableNodeData>& nbt);
	bool getNbestTransFor_t_(const std::vector<WordIndex>& t,
                             NbestTableNode<PhraseTransTableNodeData>& nbt,
                             int N=-1);
    
        // Loading functions
    bool load(const char *prefix);
    bool load_given_prefix(const char *prefix);
    virtual bool load_ttable(const char *phraseTTableFileName);
        // Reads a (plain text or binarized) translation table, returns
        // non-zero if error
    bool load_seglentable(const char *segmLengthTableFileName);
        // Load a table with segmentation length information

        // Printing functions
    bool print(const char* prefix);
        // Prints the whole model
    
        // Functions to print the model tables
    virtual bool printTTable(const char *outputFileName);
	bool printSegmLengthTable(const char *outputFileName);

        // Source vocabulary functions
	size_t getSrcVocabSize(void)const;
        // Returns the source vocabulary size
    WordIndex stringToSrcWordIndex(std::string s)const;
    std::string wordIndexToSrcString(WordIndex w)const;
    bool existSrcSymbol(std::string s)const;
    std::vector<WordIndex> strVectorToSrcIndexVector(const std::vector<std::string>& s);
        //converts a string vector into a source word index std::vector, this
        //function automatically handles the source vocabulary,
        //increasing and modifying it if necessary
    std::vector<std::string> srcIndexVectorToStrVector(const std::vector<WordIndex>& s);
        //Inverse operation
    WordIndex addSrcSymbol(std::string s);
    bool loadSrcVocab(const char *srcInputVocabFileName);
        // loads source vocabulary, returns non-zero if error
    bool printSrcVocab(const char *outputFileName);

        // Target vocabulary functions
    size_t getTrgVocabSize(void)const;
        // Returns the target vocabulary size
    WordIndex stringToTrgWordIndex(std::string t)const;
    std::string wordIndexToTrgString(WordIndex w)const;
    bool existTrgSymbol(std::string t)const;
    std::vector<WordIndex> strVectorToTrgIndexVector(const std::vector<std::string>& t);
        //converts a string vector into a target word index std::vector, this
        //function automatically handles the target vocabulary,
        //increasing and modifying it if necessary
    std::vector<std::string> trgIndexVectorToStrVector(const std::vector<WordIndex>& t);
        //Inverse operation
    WordIndex addTrgSymbol(std::string t);
    bool loadTrgVocab(const char *trgInputVocabFileName);
        // loads target vocabulary, returns non-zero if error
    bool printTrgVocab(const char *outputFileName);
	
	std::vector<std::string> stringToStringVector(std::string s);
	std::vector<std::string> extractCharItemsToVector(char *ch)const;
        // Extracts the words in the string 'ch' with the form "w1
        // ... wn" to a string std::vector

        // size and clear functions
    size_t size(void);
    void clear(void);
    void clearTempVars(void);

        // destructor
    ~_incrPhraseModel();
	
 protected:
    
        // Data Members
    AlignmentExtractor alignmentExtractor;

    SingleWordVocab singleWordVocab;
    
#   ifdef THOT_HAVE_BASEICONDPROBTABLE_H
    BaseICondProbTable<std::vector<WordIndex>,std::vector<WordIndex>,PhrasePairInfo>* basePhraseTablePtr;
#   else
    BasePhraseTable* basePhraseTablePtr;
#   endif
	
    SegLenTable segLenTable;

    SrcSegmLenTable srcSegmLenTable;

    TrgCutsTable trgCutsTable;
    
    TrgSegmLenTable trgSegmLenTable;
    
        // Functions to print models using standard C library
    virtual void printTTable(FILE* file)=0;

    void printNbestTransTableNode(NbestTableNode<PhraseTransTableNodeData> tTableNode,
                                  std::ostream &outS);
    void printSegmLengthTable(std::ostream &outS);								

        // Functions to load ttable
    virtual bool loadPlainTextTTable(const char *phraseTTableFileName);
        // Reads a plain text phrase model file, returns non-zero if
        // error
};

#endif
