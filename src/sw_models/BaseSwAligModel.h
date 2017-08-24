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
/* Module: BaseSwAligModel                                          */
/*                                                                  */
/* Prototype file: BaseSwAligModel.h                                */
/*                                                                  */
/* Description: Defines the BaseSwAligModel class. BaseSwAligModel  */
/*              is a base class for derivating single-word          */
/*              statistical alignment models.                       */
/*                                                                  */
/********************************************************************/

#ifndef _BaseSwAligModel_h
#define _BaseSwAligModel_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string>
#include "awkInputStream.h"
#include "SwDefs.h"
#include <ErrorDefs.h>
#include <StrProcUtils.h>
#include <WordAligMatrix.h>

using namespace std;

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- Function declarations ------------------------------


//--------------- Classes --------------------------------------------


//--------------- BaseSwAligModel class

template<class PPINFO>
class BaseSwAligModel
{
 public:

    typedef PPINFO PpInfo;

    // Declarations related to dynamic class loading
    typedef BaseSwAligModel* create_t(std::string);
    typedef std::string type_id_t(void);

    // Constructor
	BaseSwAligModel(void);

    // Functions to read and add sentence pairs
	virtual bool readSentencePairs(const char *srcFileName,
                                   const char *trgFileName,
                                   const char *sentCountsFile,
                                   pair<unsigned int,unsigned int>& sentRange)=0;
    virtual void addSentPair(Vector<std::string> srcSentStr,
                             Vector<std::string> trgSentStr,
                             Count c,
                             pair<unsigned int,unsigned int>& sentRange)=0;
    virtual unsigned int numSentPairs(void)=0;
        // NOTE: the whole valid range in a given moment is
        // [ 0 , numSentPairs() )
    virtual int nthSentPair(unsigned int n,
                            Vector<std::string>& srcSentStr,
                            Vector<std::string>& trgSentStr,
                            Count& c)=0;

    // Functions to print sentence pairs
    virtual bool printSentPairs(const char *srcSentFile,
                                const char *trgSentFile,
                                const char *sentCountsFile)=0;

    // Functions to train model
    virtual void trainSentPairRange(pair<unsigned int,unsigned int> sentPairRange,
                                    int verbosity=0);
        // train model for range [uint,uint]
    virtual void trainAllSents(int verbosity=0);
    virtual pair<double,double> loglikelihoodForPairRange(pair<unsigned int,unsigned int> sentPairRange,
                                                          int verbosity=0);
        // Returns log-likelihood. The first double contains the
        // loglikelihood for all sentences, and the second one, the same
        // loglikelihood normalized by the number of sentences
    virtual pair<double,double> loglikelihoodForAllSents(int verbosity=0);
        // Returns log-likelihood. The first double contains the
        // loglikelihood for all sentences, and the second one, the same
        // loglikelihood normalized by the number of sentences
    virtual void clearInfoAboutSentRange(void);
        // clear info about the whole sentence range without clearing
        // information about current model parameters

        // Sentence length model functions
    virtual Prob sentLenProb(unsigned int slen,
                             unsigned int tlen)=0;
        // returns p(tlen|slen)
    virtual LgProb sentLenLgProb(unsigned int slen,
                                 unsigned int tlen)=0;

    // Scoring functions for a given alignment
    LgProb calcLgProbForAligChar(const char *sSent,
                                 const char *tSent,
                                 WordAligMatrix aligMatrix,
                                 int verbose=0);
    LgProb calcLgProbForAligVecStr(const Vector<std::string>& sSent,
                                   const Vector<std::string>& tSent,
                                   WordAligMatrix aligMatrix,
                                   int verbose=0);
    virtual LgProb calcLgProbForAlig(const Vector<WordIndex>& sSent,
                                     const Vector<WordIndex>& tSent,
                                     WordAligMatrix aligMatrix,
                                     int verbose=0)=0;

    // Scoring functions without giving an alignment
    LgProb calcLgProbChar(const char *sSent,
                          const char *tSent,
                          int verbose=0);
    LgProb calcLgProbVecStr(const Vector<std::string>& sSent,
                            const Vector<std::string>& tSent,
                            int verbose=0);
    virtual LgProb calcLgProb(const Vector<WordIndex>& sSent,
                              const Vector<WordIndex>& tSent,
                              int verbose=0)=0;
    virtual LgProb calcLgProbPhr(const Vector<WordIndex>& sPhr,
                                 const Vector<WordIndex>& tPhr,
                                 int verbose=0);
        // Scoring function for phrase pairs

    // Partial scoring functions
    virtual void initPpInfo(unsigned int slen,
                            const Vector<WordIndex>& tSent,
                            PpInfo& ppInfo);
    virtual void partialProbWithoutLen(unsigned int srcPartialLen,
                                       unsigned int slen,
                                       const Vector<WordIndex>& s_,
                                       const Vector<WordIndex>& tSent,
                                       PpInfo& ppInfo);
        // Calculate partial probability
        // srcPartialLen-> partial length of the source phrase
        // slen-> final length of the source sentence
        // s_-> sequence of words added to the partial hypothesis
        // tSent-> target sentence
        // ppInfo-> PpInfo object containing info about new partial probability
    virtual LgProb lpFromPpInfo(const PpInfo& ppInfo);
    virtual void addHeurForNotAddedWords(int numSrcWordsToBeAdded,
                                         const Vector<WordIndex>& tSent,
                                         PpInfo& ppInfo);
    virtual void sustHeurForNotAddedWords(int numSrcWordsToBeAdded,
                                          const Vector<WordIndex>& tSent,
                                          PpInfo& ppInfo);

    // Best-alignment functions
    bool obtainBestAlignments(const char *sourceTestFileName,
                              const char *targetTestFilename,
                              const char *outFileName);
        // Obtains the best alignments for the sentence pairs given in
        // the files 'sourceTestFileName' and 'targetTestFilename'. The
        // results are stored in the file 'outFileName'
    LgProb obtainBestAlignmentChar(const char *sourceSentence,
                                   const char *targetSentence,
                                   WordAligMatrix& bestWaMatrix);	
        // Obtains the best alignment for the given sentence pair
    LgProb obtainBestAlignmentVecStr(Vector<std::string> srcSentenceVector,
                                     Vector<std::string> trgSentenceVector,
                                     WordAligMatrix& bestWaMatrix);	
        // Obtains the best alignment for the given sentence pair (input
        // parameters are now string vectors)
    virtual LgProb obtainBestAlignment(Vector<WordIndex> srcSentIndexVector,
                                       Vector<WordIndex> trgSentIndexVector,
                                       WordAligMatrix& bestWaMatrix)=0;	
        // Obtains the best alignment for the given sentence pair
        // (input parameters are now index vectors) depending on the
        // value of the modelNumber data member.
    
    ostream& printAligInGizaFormat(const char *sourceSentence,
                                   const char *targetSentence,
                                   Prob p,
                                   Vector<PositionIndex> alig,
                                   ostream &outS);
        // Prints the given alignment to 'outS' stream in GIZA format

    // load() function
    virtual bool load(const char* prefFileName)=0;
    
    // print() function
    virtual bool print(const char* prefFileName)=0;

    // Functions for loading vocabularies
	virtual bool loadGIZASrcVocab(const char *srcInputVocabFileName)=0;
        // Reads source vocabulary from a file in GIZA format
    virtual bool loadGIZATrgVocab(const char *trgInputVocabFileName)=0;
        // Reads target vocabulary from a file in GIZA format
    
    // Functions for printing vocabularies
	virtual bool printGIZASrcVocab(const char *srcOutputVocabFileName)=0;
        // Reads source vocabulary from a file in GIZA format
    virtual bool printGIZATrgVocab(const char *trgOutputVocabFileName)=0;
        // Reads target vocabulary from a file in GIZA format
    
    // Source and target vocabulary functions    
	virtual size_t getSrcVocabSize(void)const=0;
        // Returns the source vocabulary size
	virtual WordIndex stringToSrcWordIndex(std::string s)const=0;
	virtual std::string wordIndexToSrcString(WordIndex w)const=0;
	virtual bool existSrcSymbol(std::string s)const=0;
	virtual Vector<WordIndex> strVectorToSrcIndexVector(Vector<std::string> s)=0;
	virtual WordIndex addSrcSymbol(std::string s)=0;
	
	virtual size_t getTrgVocabSize(void)const=0;
        // Returns the target vocabulary size
	virtual WordIndex stringToTrgWordIndex(std::string t)const=0;
	virtual std::string wordIndexToTrgString(WordIndex w)const=0;
	virtual bool existTrgSymbol(std::string t)const=0;
	virtual Vector<WordIndex> strVectorToTrgIndexVector(Vector<std::string> t)=0;
	virtual WordIndex addTrgSymbol(std::string t)=0;

    // clear() function
    virtual void clear(void)=0;

    // clearTempVars() function
    virtual void clearTempVars(void){};

    // Utilities
    Vector<WordIndex> addNullWordToWidxVec(const Vector<WordIndex>& vw);
    Vector<std::string> addNullWordToStrVec(const Vector<std::string>& vw);
    
    // Destructor
	virtual ~BaseSwAligModel();
};

//--------------- BaseSwAligModel class method definitions

//-------------------------
template<class PPINFO>
BaseSwAligModel<PPINFO>::BaseSwAligModel(void)
{
}

//-------------------------
template<class PPINFO>
void BaseSwAligModel<PPINFO>::trainSentPairRange(pair<unsigned int,unsigned int> /*sentPairRange*/,
                                                 int /*verbosity*/)
{
  cerr<<"Warning: training for sentence pair range not implemented!"<<endl;
}

//-------------------------
template<class PPINFO>
void BaseSwAligModel<PPINFO>::trainAllSents(int /*verbosity*/)
{
  cerr<<"Warning: training for all sentence pairs not implemented!"<<endl;
}

//-------------------------
template<class PPINFO>
void BaseSwAligModel<PPINFO>::clearInfoAboutSentRange(void)
{
  cerr<<"Warning: clearInfoAboutSentRange() functionality not implemented!"<<endl;
}

//-------------------------
template<class PPINFO>
pair<double,double> BaseSwAligModel<PPINFO>::loglikelihoodForPairRange(pair<unsigned int,unsigned int> /*sentPairRange*/,
                                                                       int /*verbosity*//*=0*/)
{
  cerr<<"Warning: loglikelihoodForAllSents() functionality not implemented!"<<endl;
  return make_pair(0.0,0.0);  
}

//-------------------------
template<class PPINFO>
pair<double,double> BaseSwAligModel<PPINFO>::loglikelihoodForAllSents(int verbosity/*=0*/)
{
  pair<unsigned int,unsigned int> sentPairRange=make_pair(0,numSentPairs()-1);
  return loglikelihoodForPairRange(sentPairRange,verbosity);
}

//-------------------------
template<class PPINFO>
LgProb BaseSwAligModel<PPINFO>::calcLgProbForAligChar(const char *sSent,
                                                      const char *tSent,
                                                      WordAligMatrix aligMatrix,
                                                      int verbose)
{
 Vector<std::string> sSentVec,tSentVec;

 sSentVec=StrProcUtils::charItemsToVector(sSent);
 tSentVec=StrProcUtils::charItemsToVector(tSent);   
 return calcLgProbForAligVecStr(sSentVec,tSentVec,aligMatrix,verbose);
}

//-------------------------
template<class PPINFO>
LgProb BaseSwAligModel<PPINFO>::calcLgProbForAligVecStr(const Vector<std::string>& sSent,
                                                        const Vector<std::string>& tSent,
                                                        WordAligMatrix aligMatrix,
                                                        int verbose)
{
 Vector<WordIndex> sIndexVector,tIndexVector;
    
 sIndexVector=strVectorToSrcIndexVector(sSent);
 tIndexVector=strVectorToTrgIndexVector(tSent);  
 return calcLgProbForAlig(sIndexVector,tIndexVector,aligMatrix,verbose);
}

//-------------------------
template<class PPINFO>
LgProb BaseSwAligModel<PPINFO>::calcLgProbChar(const char *sSent,
                                               const char *tSent,
                                               int verbose)
{
 Vector<std::string> sSentVec,tSentVec;

 sSentVec=StrProcUtils::charItemsToVector(sSent);
 tSentVec=StrProcUtils::charItemsToVector(tSent);   
 return calcLgProbVecStr(sSentVec,tSentVec,verbose);
}

//-------------------------
template<class PPINFO>
LgProb BaseSwAligModel<PPINFO>::calcLgProbVecStr(const Vector<std::string>& sSent,
                                                 const Vector<std::string>& tSent,
                                                 int verbose)
{
 Vector<WordIndex> sIndexVector,tIndexVector;
 Vector<PositionIndex> aligIndexVec;
    
 sIndexVector=strVectorToSrcIndexVector(sSent);
 tIndexVector=strVectorToTrgIndexVector(tSent);
  
 return calcLgProb(sIndexVector,tIndexVector,verbose);   
}

//-------------------------
template<class PPINFO>
LgProb BaseSwAligModel<PPINFO>::calcLgProbPhr(const Vector<WordIndex>& sPhr,
                                              const Vector<WordIndex>& tPhr,
                                              int verbose/*=0*/)
{
  return calcLgProb(sPhr,tPhr,verbose);
}

//-------------------------
template<class PPINFO>
void BaseSwAligModel<PPINFO>::initPpInfo(unsigned int /*slen*/,
                                         const Vector<WordIndex>& /*tSent*/,
                                         PpInfo& /*ppInfo*/)
{
  cerr<<"Warning! partial probability initialization not implemented"<<endl;  
}

//-------------------------
template<class PPINFO>
void BaseSwAligModel<PPINFO>::partialProbWithoutLen(unsigned int /*srcPartialLen*/,
                                                    unsigned int /*slen*/,
                                                    const Vector<WordIndex>& /*s_*/,
                                                    const Vector<WordIndex>& /*tSent*/,
                                                    PpInfo& /*ppInfo*/)
{
  cerr<<"Warning! partial probability assignment not implemented"<<endl;
}

//-------------------------
template<class PPINFO>
LgProb BaseSwAligModel<PPINFO>::lpFromPpInfo(const PpInfo& /*ppInfo*/)
{
  cerr<<"Warning! lpFromPpInfo() function not implemented"<<endl;
  return 0;
}

//-------------------------
template<class PPINFO>
void BaseSwAligModel<PPINFO>::addHeurForNotAddedWords(int /*numSrcWordsToBeAdded*/,
                                                      const Vector<WordIndex>& /*tSent*/,
                                                      PpInfo& /*ppInfo*/)
{
  cerr<<"Warning! lpFromPpInfo() function not implemented"<<endl;
}

//-------------------------
template<class PPINFO>
void BaseSwAligModel<PPINFO>::sustHeurForNotAddedWords(int /*numSrcWordsToBeAdded*/,
                                                       const Vector<WordIndex>& /*tSent*/,
                                                       PpInfo& /*ppInfo*/)
{
  cerr<<"Warning! lpFromPpInfo() function not implemented"<<endl;
}

//-------------------------
template<class PPINFO>
bool BaseSwAligModel<PPINFO>::obtainBestAlignments(const char *sourceTestFileName,
                                                   const char *targetTestFilename,
                                                   const char *outFileName)
{
 awkInputStream srcTest,trgTest;
 Vector<PositionIndex> bestAlig;
 LgProb bestLgProb;
 ofstream outF;

 outF.open(outFileName,ios::out);
 if(!outF)
 {
   cerr<<"Error while opening output file."<<endl;
   return 1;
 }

 if(srcTest.open(sourceTestFileName)==THOT_ERROR)
 {
   cerr<<"Error in source test file, file "<<sourceTestFileName<<" does not exist.\n";
   return THOT_ERROR;
 }
 if(trgTest.open(targetTestFilename)==THOT_ERROR)
 {
   cerr<<"Error in target test file, file "<<targetTestFilename<<" does not exist.\n";
   return THOT_ERROR;
 }  
 while(srcTest.getln())
 {
   if(trgTest.getln())
   {
     if(srcTest.NF>0 && trgTest.NF>0)
     {
       bestLgProb=obtainBestAlignmentChar((char*)srcTest.dollar(0).c_str(),(char*)trgTest.dollar(0).c_str(),bestAlig);
       outF<<"# Sentence pair "<<srcTest.FNR<<" ";
       printAligInGizaFormat((char*)srcTest.dollar(0).c_str(),(char*)trgTest.dollar(0).c_str(),bestLgProb.get_p(),bestAlig,outF);
     }
   }
   else
   {
     cerr<<"Error: Source and target test files have not the same size."<<endl;
   }
 }
 outF.close();

 return THOT_OK;
}
//-------------------------
template<class PPINFO>
LgProb BaseSwAligModel<PPINFO>::obtainBestAlignmentChar(const char *sourceSentence,
                                                        const char *targetSentence,
                                                        WordAligMatrix& bestWaMatrix)
{
 Vector<std::string> targetVector,sourceVector;
 LgProb lp;

     // Convert sourceSentence into a vector of strings
 sourceVector=StrProcUtils::charItemsToVector(sourceSentence);
 
     // Convert targetSentence into a vector of strings
 targetVector=StrProcUtils::charItemsToVector(targetSentence);
 lp=obtainBestAlignmentVecStr(sourceVector,targetVector,bestWaMatrix);
 
 return lp;
}
//-------------------------
template<class PPINFO>
LgProb BaseSwAligModel<PPINFO>::obtainBestAlignmentVecStr(Vector<std::string> srcSentenceVector,
                                                          Vector<std::string> trgSentenceVector,
                                                          WordAligMatrix& bestWaMatrix)
{
 LgProb lp;
 Vector<WordIndex> srcSentIndexVector,trgSentIndexVector;

 srcSentIndexVector=strVectorToSrcIndexVector(srcSentenceVector);
 trgSentIndexVector=strVectorToTrgIndexVector(trgSentenceVector);
 lp=obtainBestAlignment(srcSentIndexVector,trgSentIndexVector,bestWaMatrix);

 return lp;
}
//-------------------------
template<class PPINFO>
ostream& BaseSwAligModel<PPINFO>::printAligInGizaFormat(const char *sourceSentence,
                                                        const char *targetSentence,
                                                        Prob p,
                                                        Vector<PositionIndex> alig,
                                                        ostream &outS)
{
 Vector<std::string> targetVector,sourceVector;
 unsigned int i,j;

 outS<<"alignment score : "<<p<<endl;
 outS<<targetSentence<<endl;
 sourceVector=StrProcUtils::charItemsToVector(sourceSentence);
 targetVector=StrProcUtils::charItemsToVector(targetSentence);

 outS<<"NULL ({ ";
 for(j=0;j<alig.size();++j)
   if(alig[j]==0) outS<<j+1<<" ";
 outS<<"}) ";
 for(i=0;i<sourceVector.size();++i)
 {
   outS<<sourceVector[i]<< " ({ ";
   for(j=0;j<alig.size();++j)
     if(alig[j]==i+1) outS<<j+1<<" ";
   outS<<"}) ";
 }
 outS<<endl;
 return outS;
}

//---------------------------------
template<class PPINFO>
Vector<WordIndex> BaseSwAligModel<PPINFO>::addNullWordToWidxVec(const Vector<WordIndex>& vw)
{
  Vector<WordIndex> result;

  result.push_back(NULL_WORD);
  for(unsigned int i=0;i<vw.size();++i)
    result.push_back(vw[i]);

  return result;
}

//---------------------------------
template<class PPINFO>
Vector<std::string> BaseSwAligModel<PPINFO>::addNullWordToStrVec(const Vector<std::string>& vw)
{
  Vector<std::string> result;

  result.push_back(NULL_WORD_STR);
  for(unsigned int i=0;i<vw.size();++i)
    result.push_back(vw[i]);

  return result;
}

//-------------------------
template<class PPINFO>
BaseSwAligModel<PPINFO>::~BaseSwAligModel()
{
}

//-------------------------

#endif
