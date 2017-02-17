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
/* Module: _incrMuxPhraseModel                                      */
/*                                                                  */
/* Prototype file: _incrMuxPhraseModel                              */
/*                                                                  */
/* Description: Defines the _incrMuxPhraseModel base class.         */
/*              _incrMuxPhraseModel is derived from the             */
/*              abstract class BasePhraseModel.                     */
/*                                                                  */
/********************************************************************/

#ifndef __incrMuxPhraseModel_h
#define __incrMuxPhraseModel_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseIncrPhraseModel.h"
#include "BasePhraseModel.h"
#include "SingleWordVocab.h"
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>

using namespace std;

//--------------- Constants ------------------------------------------

#define INVALID_PMODEL_INDEX    -1

//--------------- typedefs -------------------------------------------


//--------------- Classes --------------------------------------------


//--------------- _incrMuxPhraseModel class

class _incrMuxPhraseModel: public BaseIncrPhraseModel
{
 public:

    typedef BaseIncrPhraseModel::SrcTableNode SrcTableNode;
    typedef BaseIncrPhraseModel::TrgTableNode TrgTableNode;

        // Constructor
    _incrMuxPhraseModel(void);

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
	
        // size function
    size_t size(void);

        // Get model information
    int getNumModels(void);
    virtual std::string getModelPrefixFileName(int idx)=0;
    virtual std::string getModelStatus(int idx)=0;
    
        // Model index related functions
    int getModelIndex(void);
    void setModelIndex(int idx);
    void resetModelIndex(void);
    
        // destructor
    ~_incrMuxPhraseModel();
	
 protected:

        // typedefs
    typedef std::map<WordIndex,WordIndex> GlobalToLocalDataMap;

        // Data members
    Vector<BasePhraseModel*> modelPtrVec;
    Vector<GlobalToLocalDataMap> srcGtlDataMapVec;
    Vector<GlobalToLocalDataMap> trgGtlDataMapVec;
    SingleWordVocab* swVocPtr;
    int modelIndex;
        // modelIndex determines to which model are applied certain
        // functions

        // Auxiliary functions to handle vocabularies
    bool srcGlobalStrVecToWidxVec(const Vector<std::string>& rq,
                                  Vector<WordIndex>& vu)const;
    bool srcGlobalStringToWordIndex(const std::string& str,
                                    WordIndex& w)const;
    bool trgGlobalStrVecToWidxVec(const Vector<std::string>& rq,
                                  Vector<WordIndex>& vu)const;
    bool trgGlobalStringToWordIndex(const std::string& str,
                                    WordIndex& w)const;

        // Auxiliary encoding functions
    bool existSrcGlobalString(const std::string& str)const;
    Vector<WordIndex> srcMapGlobalToLocalWidxVec(unsigned int index,
                                                 const Vector<WordIndex>& widxVec);
    WordIndex srcMapGlobalToLocalWidx(unsigned int index,
                                      const WordIndex& widx);
    bool existTrgGlobalString(const std::string& str)const;
    Vector<WordIndex> trgMapGlobalToLocalWidxVec(unsigned int index,
                                                 const Vector<WordIndex>& widxVec);
    WordIndex trgMapGlobalToLocalWidx(unsigned int index,
                                      const WordIndex& widx);
  
        // Auxiliary functions to handle strings
    Vector<string> stringToStringVector(string s);
	Vector<string> extractCharItemsToVector(char *ch)const;
        // Extracts the words in the string 'ch' with the form "w1
        // ... wn" to a string Vector
};

#endif
