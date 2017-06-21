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
                                     Vector<Vector<PhrasePair> >& invPhrPairs,
                                     int verbose/*=0*/)
  {
        // NOTE: this function requires the ability to extract new translation
        // options. This can be achieved using the well-known phrase-extract
        // algorithm.
  
    awkInputStream srcStream;
    awkInputStream trgStream;

        // Open files
    if(srcStream.open(srcCorpusFileName.c_str())==ERROR)
    {
      cerr<<"Unable to open file with source development sentences."<<endl;
      return ERROR;
    }  
    if(trgStream.open(trgCorpusFileName.c_str())==ERROR)
    {
      cerr<<"Unable to open file with target development sentences."<<endl;
      return ERROR;
    }  

        // Iterate over all sentences
    invPhrPairs.clear();
    while(srcStream.getln())
    {
      if(!trgStream.getln())
      {
        cerr<<"Unexpected end of file with target development sentences."<<endl;
        return ERROR;      
      }

          // Obtain sentence pair
      Vector<std::string> srcSentStrVec;
      Vector<std::string> refSentStrVec;
      Count c;

          // Extract source sentence
      for(unsigned int i=1;i<=srcStream.NF;++i)
        srcSentStrVec.push_back(srcStream.dollar(i));

          // Extract target sentence
      for(unsigned int i=1;i<=trgStream.NF;++i)
        refSentStrVec.push_back(trgStream.dollar(i));

          // Extract consistent phrase pairs
      Vector<PhrasePair> vecInvPhPair;
      int ret=extractConsistentPhrasePairs(swAligModelPtr,invSwAligModelPtr,srcSentStrVec,refSentStrVec,vecInvPhPair,verbose);
      if(ret==ERROR)
        return ERROR;
      
          // Add vector of phrase pairs
      invPhrPairs.push_back(vecInvPhPair);
    }
    
        // Close files
    srcStream.close();
    trgStream.close();

    return OK;
  }
  
  //---------------------------------
  int extractConsistentPhrasePairs(BaseSwAligModel<PpInfo>* swAligModelPtr,
                                   BaseSwAligModel<PpInfo>* invSwAligModelPtr,
                                   const Vector<std::string>& srcSentStrVec,
                                   const Vector<std::string>& refSentStrVec,
                                   Vector<PhrasePair>& vecInvPhPair,
                                   bool verbose/*=0*/)
  {
        // Generate alignments
    WordAligMatrix waMatrix;
    WordAligMatrix invWaMatrix;
  
    swAligModelPtr->obtainBestAlignmentVecStr(srcSentStrVec,refSentStrVec,waMatrix);
    invSwAligModelPtr->obtainBestAlignmentVecStr(refSentStrVec,srcSentStrVec,invWaMatrix);
  
        // Operate alignments
    Vector<std::string> nsrcSentStrVec=swAligModelPtr->addNullWordToStrVec(srcSentStrVec);
    Vector<std::string> nrefSentStrVec=swAligModelPtr->addNullWordToStrVec(refSentStrVec);  

    waMatrix.transpose();

        // Execute symmetrization
    invWaMatrix.symmetr1(waMatrix);

        // Extract consistent pairs
    PhraseExtractParameters phePars;
    extractPhrasesFromPairPlusAlig(phePars,
                                   nrefSentStrVec,
                                   srcSentStrVec,
                                   invWaMatrix,
                                   vecInvPhPair,
                                   verbose);
    return OK;
  }

  //---------------
  void extractPhrasesFromPairPlusAlig(PhraseExtractParameters phePars,
                                      Vector<string> ns,
                                      Vector<string> t,
                                      WordAligMatrix waMatrix,
                                      Vector<PhrasePair>& vecPhPair,
                                      int /*verbose=0*/)
  {
    if(t.size()<MAX_SENTENCE_LENGTH && ns.size()-1<MAX_SENTENCE_LENGTH)
    {
      PhraseExtractionTable phraseExtract;
      phraseExtract.extractConsistentPhrases(phePars,ns,t,waMatrix,vecPhPair);
    }
    else
    {
      cerr<< "Warning: Max. sentence length exceeded for sentence pair"<<endl;
    }
  }
  
}
