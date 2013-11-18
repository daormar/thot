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
/* Module: _swAligModel                                             */
/*                                                                  */
/* Prototype file: _swAligModel.h                                   */
/*                                                                  */
/* Description: Defines the _swAligModel class. _swAligModel is a   */
/*              predecessor class for derivating single-word        */
/*              statistical alignment models.                       */
/*                                                                  */
/********************************************************************/

#ifndef __swAligModel_h
#define __swAligModel_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <set>
#include "BaseSwAligModel.h"
#include "SingleWordVocab.h"
#include "SentenceHandler.h"
#include "LightSentenceHandler.h"
#include "SwModelsShTypes.h"

using namespace std;

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- Function declarations ------------------------------


//--------------- Classes --------------------------------------------


//--------------- _swAligModel class

template<class PPINFO>
class _swAligModel: public BaseSwAligModel<PPINFO>
{
 public:

    typedef typename BaseSwAligModel<PPINFO>::PpInfo PpInfo;
  
    // Constructor
	_swAligModel(void);

    // Functions to read and add sentence pairs
	bool readSentencePairs(const char *srcFileName,
                           const char *trgFileName,
                           const char *sentCountsFile,
                           pair<unsigned int,unsigned int>& sentRange);
    void addSentPair(Vector<std::string> srcSentStr,
                     Vector<std::string> trgSentStr,
                     Count c,
                     pair<unsigned int,unsigned int>& sentRange);
    unsigned int numSentPairs(void);
        // NOTE: the whole valid range in a given moment is
        // [ 0 , numSentPairs() )
    int nthSentPair(unsigned int n,
                    Vector<std::string>& srcSentStr,
                    Vector<std::string>& trgSentStr,
                    Count& c);

    // Functions to print sentence pairs
    bool printSentPairs(const char *srcSentFile,
                        const char *trgSentFile,
                        const char *sentCountsFile);

    // Functions for loading vocabularies
	bool loadGIZASrcVocab(const char *srcInputVocabFileName);
        // Reads source vocabulary from a file in GIZA format
    bool loadGIZATrgVocab(const char *trgInputVocabFileName);
        // Reads target vocabulary from a file in GIZA format
    
    // Functions for printing vocabularies
	bool printGIZASrcVocab(const char *srcOutputVocabFileName);
        // Reads source vocabulary from a file in GIZA format
    bool printGIZATrgVocab(const char *trgOutputVocabFileName);
        // Reads target vocabulary from a file in GIZA format
    
    // Source and target vocabulary functions    
	size_t getSrcVocabSize(void)const; // Returns the source vocabulary size
	WordIndex stringToSrcWordIndex(std::string s)const;
	std::string wordIndexToSrcString(WordIndex w)const;
	bool existSrcSymbol(std::string s)const;
	Vector<WordIndex> strVectorToSrcIndexVector(Vector<std::string> s);
	WordIndex addSrcSymbol(std::string s,Count numTimes=1);
	
	size_t getTrgVocabSize(void)const; // Returns the target vocabulary size
	WordIndex stringToTrgWordIndex(std::string t)const;
	std::string wordIndexToTrgString(WordIndex w)const;
	bool existTrgSymbol(std::string t)const;
	Vector<WordIndex> strVectorToTrgIndexVector(Vector<std::string> t);
	WordIndex addTrgSymbol(std::string t,Count numTimes=1);

    // clear() function
    void clear(void);
    
    // Destructor
	virtual ~_swAligModel();
	
 protected:
	
	SingleWordVocab swVocab;

    CURR_SH_TYPE sentenceHandler;
};

//--------------- _swAligModel class method definitions

//-------------------------
template<class PPINFO>
_swAligModel<PPINFO>::_swAligModel(void)
{
}

//-------------------------
template<class PPINFO>
bool _swAligModel<PPINFO>::readSentencePairs(const char *srcFileName,
                                             const char *trgFileName,
                                             const char *sentCountsFile,
                                             pair<unsigned int,unsigned int>& sentRange)
{
  return sentenceHandler.readSentencePairs(srcFileName,trgFileName,sentCountsFile,sentRange);
}

//-------------------------
template<class PPINFO>
void _swAligModel<PPINFO>::addSentPair(Vector<std::string> srcSentStr,
                                       Vector<std::string> trgSentStr,
                                       Count c,
                                       pair<unsigned int,unsigned int>& sentRange)
{
  sentenceHandler.addSentPair(srcSentStr,trgSentStr,c,sentRange);
}

//-------------------------
template<class PPINFO>
unsigned int _swAligModel<PPINFO>::numSentPairs(void)
{
  return sentenceHandler.numSentPairs();
}

//-------------------------
template<class PPINFO>
int _swAligModel<PPINFO>::nthSentPair(unsigned int n,
                                      Vector<std::string>& srcSentStr,
                                      Vector<std::string>& trgSentStr,
                                      Count& c)
{
  return sentenceHandler.nthSentPair(n,srcSentStr,trgSentStr,c);
}

//-------------------------
template<class PPINFO>
bool _swAligModel<PPINFO>::loadGIZASrcVocab(const char *srcInputVocabFileName)
{
 return swVocab.loadGIZASrcVocab(srcInputVocabFileName);
}

//-------------------------
template<class PPINFO>
bool _swAligModel<PPINFO>::loadGIZATrgVocab(const char *trgInputVocabFileName)
{
 return swVocab.loadGIZATrgVocab(trgInputVocabFileName);
}

//-------------------------
template<class PPINFO>
bool _swAligModel<PPINFO>::printGIZASrcVocab(const char *srcOutputVocabFileName)
{
  return swVocab.printSrcVocab(srcOutputVocabFileName);
}

//-------------------------
template<class PPINFO>
bool _swAligModel<PPINFO>::printGIZATrgVocab(const char *trgOutputVocabFileName)
{
  return swVocab.printTrgVocab(trgOutputVocabFileName);
}

//-------------------------
template<class PPINFO>
bool _swAligModel<PPINFO>::printSentPairs(const char *srcSentFile,
                                          const char *trgSentFile,
                                          const char *sentCountsFile)
{
  return sentenceHandler.printSentPairs(srcSentFile,trgSentFile,sentCountsFile);
}
//-------------------------
template<class PPINFO>
size_t _swAligModel<PPINFO>::getSrcVocabSize(void)const
{
 return swVocab.getSrcVocabSize();	
}

//-------------------------
template<class PPINFO>
WordIndex _swAligModel<PPINFO>::stringToSrcWordIndex(std::string s)const
{
 return swVocab.stringToSrcWordIndex(s);
}

//-------------------------
template<class PPINFO>
std::string _swAligModel<PPINFO>::wordIndexToSrcString(WordIndex w)const
{
 return swVocab.wordIndexToSrcString(w);
}

//-------------------------
template<class PPINFO>
bool _swAligModel<PPINFO>::existSrcSymbol(std::string s)const
{
 return swVocab.existSrcSymbol(s);
}

//-------------------------
template<class PPINFO>
Vector<WordIndex> _swAligModel<PPINFO>::strVectorToSrcIndexVector(Vector<std::string> s)
{
 return swVocab.strVectorToSrcIndexVector(s,0);
}

//-------------------------
template<class PPINFO>
WordIndex _swAligModel<PPINFO>::addSrcSymbol(std::string s,
                                             Count numTimes)
{
 return swVocab.addSrcSymbol(s,numTimes);
}

//-------------------------
template<class PPINFO>
size_t _swAligModel<PPINFO>::getTrgVocabSize(void)const
{
 return swVocab.getTrgVocabSize();
}

//-------------------------
template<class PPINFO>
WordIndex _swAligModel<PPINFO>::stringToTrgWordIndex(std::string t)const
{
 return swVocab.stringToTrgWordIndex(t);
}

//-------------------------
template<class PPINFO>
std::string _swAligModel<PPINFO>::wordIndexToTrgString(WordIndex w)const
{
 return swVocab.wordIndexToTrgString(w);
}

//-------------------------
template<class PPINFO>
bool _swAligModel<PPINFO>::existTrgSymbol(std::string t)const
{
 return swVocab.existTrgSymbol(t);
}

//-------------------------
template<class PPINFO>
Vector<WordIndex> _swAligModel<PPINFO>::strVectorToTrgIndexVector(Vector<std::string> t)
{
 return swVocab.strVectorToTrgIndexVector(t,0);	
}

//-------------------------
template<class PPINFO>
WordIndex _swAligModel<PPINFO>::addTrgSymbol(std::string t,
                                             Count numTimes)
{
 return swVocab.addTrgSymbol(t,numTimes);
}

//-------------------------
template<class PPINFO>
void _swAligModel<PPINFO>::clear(void)
{
 swVocab.clear();
 sentenceHandler.clear();
}

//-------------------------
template<class PPINFO>
_swAligModel<PPINFO>::~_swAligModel()
{
}

//-------------------------

#endif
