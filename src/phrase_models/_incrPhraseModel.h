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
/* Module: _incrPhraseModel                                         */
/*                                                                  */
/* Prototype file: _incrPhraseModel                                 */
/*                                                                  */
/* Description: Defines the _incrPhraseModel base class.            */
/*              _incrPhraseModel is derived from the abstract       */
/*              class BasePhraseModel.                              */
/*                                                                  */
/********************************************************************/

#ifndef __incrPhraseModel_h
#define __incrPhraseModel_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseIncrPhraseModel.h"
#include "PhraseTable.h"
#include "PhraseTableLog.h"
#include "NbestTransTable.h"
#include "WordAligMatrix.h"
#include "SingleWordVocab.h"
#include "printAligFuncs.h"
#include "SegLenTable.h"
#include "SrcSegmLenTable.h"
#include "TrgCutsTable.h"
#include "TrgSegmLenTable.h"
#include "AlignmentExtractor.h"
#include "Bitset.h"
#include "awkInputStream.h"
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>

using namespace std;

//--------------- Constants ------------------------------------------

#define THOT_COUNT_OUTPUT       2 

//--------------- typedefs -------------------------------------------

typedef NbestTransTable<Vector<WordIndex>,PhraseTransTableNodeData> PhraseNbestTransTable;

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
    void strAddTableEntry(const Vector<string>& s,
                          const Vector<string>& t,
                          PhrasePairInfo inf);
    void addTableEntry(const Vector<WordIndex>& s,
                       const Vector<WordIndex>& t,
                       PhrasePairInfo inf);
	void strIncrCountsOfEntry(const Vector<string>& s,
                              const Vector<string>& t,
                              Count count=1);
    void incrCountsOfEntry(const Vector<WordIndex>& s,
                           const Vector<WordIndex>& t,
                           Count count=1);

        // Counts-related functions
    Count cSrcTrg(const Vector<WordIndex>& s,
                  const Vector<WordIndex>& t);
    Count cSrc(const Vector<WordIndex>& s);
    Count cTrg(const Vector<WordIndex>& t);

    Count cHSrcHTrg(const Vector<std::string>& hs,
                    const Vector<std::string>& ht);
    Count cHSrc(const Vector<std::string>& hs);
    Count cHTrg(const Vector<std::string>& ht);

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

    PhrasePairInfo infSrcTrg(const Vector<WordIndex>& s,
                             const Vector<WordIndex>& t,
                             bool& found);

	LgProb logpt_s_(const Vector<WordIndex>& s,
                    const Vector<WordIndex>& t);
	
	LgProb logps_t_(const Vector<WordIndex>& s,
                    const Vector<WordIndex>& t);


        // Functions to obtain translations for source or target phrases
    bool getTransFor_s_(const Vector<WordIndex>& s,
                        TrgTableNode& trgtn);
    bool getTransFor_t_(const Vector<WordIndex>& t,
                        SrcTableNode& srctn);
	bool getNbestTransFor_s_(const Vector<WordIndex>& s,
                             NbestTableNode<PhraseTransTableNodeData>& nbt);
	bool getNbestTransFor_t_(const Vector<WordIndex>& t,
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
    
        // Functions to print the translation table in different
        // formats
    virtual bool printTTable(const char *outputFileName);
	bool printSegmLengthTable(const char *outputFileName);

        // Source vocabulary functions
	size_t getSrcVocabSize(void)const;
        // Returns the source vocabulary size
    WordIndex stringToSrcWordIndex(string s)const;
    string wordIndexToSrcString(WordIndex w)const;
    bool existSrcSymbol(string s)const;
    Vector<WordIndex> strVectorToSrcIndexVector(const Vector<string>& s,
                                                Count numTimes=1);
        //converts a string vector into a source word index Vector, this
        //function automatically handles the source vocabulary,
        //increasing and modifying it if necessary
    Vector<string> srcIndexVectorToStrVector(const Vector<WordIndex>& s);
        //Inverse operation
    WordIndex addSrcSymbol(string s,Count numTimes=1);
    bool loadSrcVocab(const char *srcInputVocabFileName);
        // loads source vocabulary, returns non-zero if error
    bool printSrcVocab(const char *outputFileName);

        // Target vocabulary functions
    size_t getTrgVocabSize(void)const;
        // Returns the target vocabulary size
    WordIndex stringToTrgWordIndex(string t)const;
    string wordIndexToTrgString(WordIndex w)const;
    bool existTrgSymbol(string t)const;
    Vector<WordIndex> strVectorToTrgIndexVector(const Vector<string>& t,
                                                Count numTimes=1);
        //converts a string vector into a target word index Vector, this
        //function automatically handles the target vocabulary,
        //increasing and modifying it if necessary
    Vector<string> trgIndexVectorToStrVector(const Vector<WordIndex>& t);
        //Inverse operation
    WordIndex addTrgSymbol(string t,Count numTimes=1);
    bool loadTrgVocab(const char *trgInputVocabFileName);
        // loads target vocabulary, returns non-zero if error
    bool printTrgVocab(const char *outputFileName);
	
	Vector<string> stringToStringVector(string s);
	Vector<string> extractCharItemsToVector(char *ch)const;
        // Extracts the words in the string 'ch' with the form "w1
        // ... wn" to a string Vector

        // size and clear functions
    size_t size(void);
    void clear(void);
    void clearTempVars(void);

        // Log functions
    bool createLogFile(char *_logFileName);
    bool addToLogFile(char *s);	
    bool logFileOpen(void);
    void closeLogFile(void);

        // destructor
    ~_incrPhraseModel();
	
 protected:
    
        // Data Members
    AlignmentExtractor alignmentExtractor;

    SingleWordVocab singleWordVocab;
    
#   ifdef THOT_HAVE_BASEICONDPROBTABLE_H
    BaseICondProbTable<Vector<WordIndex>,Vector<WordIndex>,PhrasePairInfo>* basePhraseTablePtr;
#   else
    BasePhraseTable* basePhraseTablePtr;
#   endif
	
    SegLenTable segLenTable;

    SrcSegmLenTable srcSegmLenTable;

    TrgCutsTable trgCutsTable;
    
    TrgSegmLenTable trgSegmLenTable;
    
	string logFileName;
    ofstream logF;

# ifdef _GLIBCXX_USE_LFS
        // Functions to print models if C++ "LARGE FILE SYSTEM (LFS)" IS
        // ENABLED
    void printTTable(ostream &outS);
# endif
        // Functions to print models using standard C library
    void printTTable(FILE* file);

    void printNbestTransTableNode(NbestTableNode<PhraseTransTableNodeData> tTableNode,
                                  ostream &outS);
    void printSegmLengthTable(ostream &outS);								

        // Functions to load ttable
    virtual bool loadPlainTextTTable(const char *phraseTTableFileName);
        // Reads a plain text phrase model file, returns non-zero if
        // error
};

#endif
