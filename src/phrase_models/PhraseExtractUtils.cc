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
 

#include "PhraseExtractUtils.h"

namespace PhraseExtractUtils
{
  //---------------
  int extractPhrPairsFromCorpusFiles(BaseSwAligModel<PpInfo>* swAligModelPtr,
                                     BaseSwAligModel<PpInfo>* invSwAligModelPtr,
                                     std::string srcCorpusFileName,
                                     std::string trgCorpusFileName,
                                     std::vector<std::vector<PhrasePair> >& phrPairs,
                                     int verbose/*=0*/)
  {
        // NOTE: this function requires the ability to extract new translation
        // options. This can be achieved using the well-known phrase-extract
        // algorithm.
  
    AwkInputStream srcStream;
    AwkInputStream trgStream;

        // Open files
    if(srcStream.open(srcCorpusFileName.c_str())==THOT_ERROR)
    {
      std::cerr<<"Unable to open file with source development sentences."<<std::endl;
      return THOT_ERROR;
    }  
    if(trgStream.open(trgCorpusFileName.c_str())==THOT_ERROR)
    {
      std::cerr<<"Unable to open file with target development sentences."<<std::endl;
      return THOT_ERROR;
    }  

        // Iterate over all sentences
    phrPairs.clear();
    while(srcStream.getln())
    {
      if(!trgStream.getln())
      {
        std::cerr<<"Unexpected end of file with target development sentences."<<std::endl;
        return THOT_ERROR;      
      }

          // Obtain sentence pair
      std::vector<std::string> srcSentStrVec;
      std::vector<std::string> refSentStrVec;
      Count c;

          // Extract source sentence
      for(unsigned int i=1;i<=srcStream.NF;++i)
        srcSentStrVec.push_back(srcStream.dollar(i));

          // Extract target sentence
      for(unsigned int i=1;i<=trgStream.NF;++i)
        refSentStrVec.push_back(trgStream.dollar(i));

          // Extract consistent phrase pairs
      std::vector<PhrasePair> vecPhrPair;
      int ret=extractConsistentPhrasePairs(swAligModelPtr,invSwAligModelPtr,srcSentStrVec,refSentStrVec,vecPhrPair,verbose);
      if(ret==THOT_ERROR)
        return THOT_ERROR;
      
          // Add vector of phrase pairs
      phrPairs.push_back(vecPhrPair);
    }
    
        // Close files
    srcStream.close();
    trgStream.close();

    return THOT_OK;
  }
  
  //---------------------------------
  int extractConsistentPhrasePairs(BaseSwAligModel<PpInfo>* swAligModelPtr,
                                   BaseSwAligModel<PpInfo>* invSwAligModelPtr,
                                   const std::vector<std::string>& srcSentStrVec,
                                   const std::vector<std::string>& trgSentStrVec,
                                   std::vector<PhrasePair>& vecPhrPair,
                                   bool verbose/*=0*/)
  {
        // Generate alignments
    WordAligMatrix waMatrix;
    WordAligMatrix invWaMatrix;
  
    swAligModelPtr->obtainBestAlignmentVecStr(srcSentStrVec,trgSentStrVec,waMatrix);
    invSwAligModelPtr->obtainBestAlignmentVecStr(trgSentStrVec,srcSentStrVec,invWaMatrix);
  
        // Operate alignments
    invWaMatrix.transpose();
    waMatrix.symmetr1(invWaMatrix);

        // Extract consistent pairs
    std::vector<std::string> nsrcSentStrVec=swAligModelPtr->addNullWordToStrVec(srcSentStrVec);
    PhraseExtractParameters phePars;
    extractPhrasesFromPairPlusAlig(phePars,
                                   nsrcSentStrVec,
                                   trgSentStrVec,
                                   waMatrix,
                                   vecPhrPair,
                                   verbose);
    return THOT_OK;
  }

  //---------------
  void extractPhrasesFromPairPlusAlig(PhraseExtractParameters phePars,
                                      std::vector<std::string> ns,
                                      std::vector<std::string> t,
                                      WordAligMatrix waMatrix,
                                      std::vector<PhrasePair>& vecPhrPair,
                                      int /*verbose=0*/)
  {
    if(t.size()<MAX_SENTENCE_LENGTH && ns.size()-1<MAX_SENTENCE_LENGTH)
    {
          // Obtain vector of unfiltered phrase pairs
      PhraseExtractionTable phraseExtract;
      phraseExtract.extractConsistentPhrases(phePars,ns,t,waMatrix,vecPhrPair);
    }
    else
    {
      std::cerr<< "Warning: Max. sentence length exceeded for sentence pair"<<std::endl;
    }
  }

  //---------------
  void extractPhrasesFromPairPlusAligBrf(PhraseExtractParameters phePars,
                                         std::vector<std::string> ns,
                                         std::vector<std::string> t,
                                         WordAligMatrix waMatrix,
                                         std::vector<PhrasePair>& vecPhrPair,
                                         int /*verbose=0*/)
  {
    if(t.size()<MAX_SENTENCE_LENGTH && ns.size()-1<MAX_SENTENCE_LENGTH)
    {
          // Obtain vector of unfiltered phrase pairs
      PhraseExtractionTable phraseExtract;
      phraseExtract.segmBasedExtraction(phePars,ns,t,waMatrix,vecPhrPair);
    }
    else
    {
      std::cerr<< "Warning: Max. sentence length exceeded for sentence pair"<<std::endl;
    }
  }

  //---------------
  void filterPhrasePairs(const std::vector<PhrasePair>& vecUnfiltPhrPair,
                         std::vector<PhrasePair>& vecPhrPair)
  {
    CategPhrasePairFilter phrasePairFilter;
    vecPhrPair.clear();
    for(unsigned int i=0;i<vecUnfiltPhrPair.size();++i)
    {
      if(phrasePairFilter.phrasePairIsOk(vecUnfiltPhrPair[i].s_,vecUnfiltPhrPair[i].t_))
        vecPhrPair.push_back(vecUnfiltPhrPair[i]);
    }
  }
  
}
