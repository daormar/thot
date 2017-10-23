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
/* Module: LevelDbPhraseModel                                       */
/*                                                                  */
/* Prototype file: LevelDbPhraseModel                               */
/*                                                                  */
/* Description: Defines the LevelDbPhraseModel base class.          */
/*              LevelDbPhraseModel is derived from the abstract     */
/*              class BasePhraseModel and implements a phrase       */
/*              model stored and accessed using Berkeley databases. */
/*                                                                  */
/********************************************************************/

#ifndef _LevelDbPhraseModel_h
#define _LevelDbPhraseModel_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "LevelDbPhraseTable.h"
#include "BaseIncrPhraseModel.h"
#include "NbestTransTable.h"
#include "SingleWordVocab.h"
#include "printAligFuncs.h"
#include "SegLenTable.h"
#include "SrcSegmLenTable.h"
#include "TrgCutsTable.h"
#include "TrgSegmLenTable.h"
#include "ModelDescriptorUtils.h"
#include "awkInputStream.h"
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- Classes --------------------------------------------


//--------------- LevelDbPhraseModel class

class LevelDbPhraseModel: public BaseIncrPhraseModel
{
 public:

    typedef BaseCountPhraseModel::SrcTableNode SrcTableNode;
    typedef BaseCountPhraseModel::TrgTableNode TrgTableNode;

        // Constructor
    LevelDbPhraseModel(void);

        // Functions to extend or modify the model
    void strAddTableEntry(const std::vector<string>& s,
                          const std::vector<string>& t,
                          PhrasePairInfo inf);
    void addTableEntry(const std::vector<WordIndex>& s,
                       const std::vector<WordIndex>& t,
                       PhrasePairInfo inf);
	void strIncrCountsOfEntry(const std::vector<string>& s,
                              const std::vector<string>& t,
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
    std::vector<WordIndex> strVectorToSrcIndexVector(const std::vector<string>& s);
        //converts a string vector into a source word index vector, this
        //function automatically handles the source vocabulary,
        //increasing and modifying it if necessary
    std::vector<string> srcIndexVectorToStrVector(const std::vector<WordIndex>& s);
        //Inverse operation
    WordIndex addSrcSymbol(string s);
    bool loadSrcVocab(const char *srcInputVocabFileName);
        // loads source vocabulary, returns non-zero if error
    bool printSrcVocab(const char *outputFileName);

        // Target vocabulary functions
    size_t getTrgVocabSize(void)const;
        // Returns the target vocabulary size
    WordIndex stringToTrgWordIndex(string t)const;
    string wordIndexToTrgString(WordIndex w)const;
    bool existTrgSymbol(string t)const;
    std::vector<WordIndex> strVectorToTrgIndexVector(const std::vector<string>& t);
        //converts a string vector into a target word index vector, this
        //function automatically handles the target vocabulary,
        //increasing and modifying it if necessary
    std::vector<string> trgIndexVectorToStrVector(const std::vector<WordIndex>& t);
        //Inverse operation
    WordIndex addTrgSymbol(string t);
    bool loadTrgVocab(const char *trgInputVocabFileName);
        // loads target vocabulary, returns non-zero if error
    bool printTrgVocab(const char *outputFileName);
	
        // size and clear functions
    size_t size(void);
    void clear(void);

        // destructor
    ~LevelDbPhraseModel();
	
 protected:
    
        // Data Members

    std::string prefixOfModelFiles;

    SingleWordVocab singleWordVocab;

    LevelDbPhraseTable levelDbPhraseTable;
	
    SegLenTable segLenTable;

    SrcSegmLenTable srcSegmLenTable;

    TrgCutsTable trgCutsTable;

    TrgSegmLenTable trgSegmLenTable;

        // Auxiliary functions
    bool load_given_prefix(const char *prefix);
};

#endif
