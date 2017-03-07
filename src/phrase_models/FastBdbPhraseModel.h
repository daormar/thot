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
/* Module: FastBdbPhraseModel                                       */
/*                                                                  */
/* Prototype file: FastBdbPhraseModel                               */
/*                                                                  */
/* Description: Defines the FastBdbPhraseModel base class.          */
/*              FastBdbPhraseModel is derived from the abstract     */
/*              class BasePhraseModel and implements a phrase model */
/*              stored and accessed using Berkeley databases.       */
/*                                                                  */
/********************************************************************/

#ifndef _FastBdbPhraseModel_h
#define _FastBdbPhraseModel_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "FastBdbPhraseTable.h"
#include "BaseCountPhraseModel.h"
#include "NbestTransTable.h"
#include "SingleWordVocab.h"
#include "printAligFuncs.h"
#include "SegLenTable.h"
#include "SrcSegmLenTable.h"
#include "TrgCutsTable.h"
#include "TrgSegmLenTable.h"
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


//--------------- typedefs -------------------------------------------


//--------------- Classes --------------------------------------------


//--------------- FastBdbPhraseModel class

class FastBdbPhraseModel: public BaseCountPhraseModel
{
 public:

    typedef BaseCountPhraseModel::SrcTableNode SrcTableNode;
    typedef BaseCountPhraseModel::TrgTableNode TrgTableNode;

        // Constructor
    FastBdbPhraseModel(void);

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
    bool load_seglentable(const char *segmLengthTableFileName);
        // Load a table with segmentation length information

        // Printing functions
    bool print(const char* prefix);
        // Prints the whole model
    
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

        // destructor
    ~FastBdbPhraseModel();
	
 protected:
    
        // Data Members

    std::string prefixOfModelFiles;

    SingleWordVocab singleWordVocab;

    FastBdbPhraseTable fastBdbPhraseTable;
	
    SegLenTable segLenTable;

    SrcSegmLenTable srcSegmLenTable;

    TrgCutsTable trgCutsTable;
    
    TrgSegmLenTable trgSegmLenTable;
};

#endif
