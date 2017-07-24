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
/* Definitions file: _wbaIncrPhraseModel.cc                         */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "_wbaIncrPhraseModel.h"


//--------------- Function definitions

//-------------------------
bool _wbaIncrPhraseModel::generateWbaIncrPhraseModel(const char *aligFileName,
                                                     PhraseExtractParameters phePars,
                                                     bool BRF,
                                                     int verbose/*=0*/) 
{
  clear();
  return extendModel(aligFileName,phePars,BRF,verbose);
}

//-------------------------
bool _wbaIncrPhraseModel::extendModel(const char *aligFileName,
                                      PhraseExtractParameters phePars,
                                      bool BRF,
                                      int verbose/*=0*/)
{
  ofstream outF;
  	  
      // Add parameters to Log File
  if(logFileOpen())
    printPars(logF,phePars,BRF);
 
      // Estimate the phrase model
  if(alignmentExtractor.open(aligFileName,GIZA_ALIG_FILE_FORMAT)==THOT_ERROR) 
  {
    cerr<<"Error while reading alignment file."<<endl;
    return THOT_ERROR;
  } 
  if(logFileOpen()) logF<<"Estimating the phrase model from the alignment file "<<aligFileName<<endl;
  extendModelFromAlignments(phePars,BRF,alignmentExtractor,verbose);
  alignmentExtractor.close();
  
  return THOT_OK;  
}

//-------------------------
void _wbaIncrPhraseModel::extendModelFromAlignments(PhraseExtractParameters phePars,
                                                    bool BRF,
                                                    AlignmentExtractor &outAlignments,
                                                    int verbose/*=0*/)
{
 Vector<string> ns,t;
 WordAligMatrix waMatrix;	
 float numReps;
    
 numSent=0;	

 while(outAlignments.getNextAlignment())
 {
   ++numSent;
   if((numSent%10)==0 && BRF) cerr<<"Processing sent. pair #"<<numSent<<"..."<<endl;
   t=outAlignments.get_t();
   ns=outAlignments.get_ns();	
   waMatrix=outAlignments.get_wamatrix();
   
   numReps=outAlignments.get_numReps();
   extendModelFromPairPlusAlig(phePars,BRF,ns,t,waMatrix,numReps,verbose);
 }
}

//-------------------------
void _wbaIncrPhraseModel::extModelFromPairAligVec(PhraseExtractParameters phePars,
                                                  bool BRF,
                                                  Vector<Vector<string> > sVec,
                                                  Vector<Vector<string> > tVec,
                                                  Vector<WordAligMatrix> waMatrixVec,
                                                  float numReps,
                                                  int verbose/*=0*/)
{
  if(sVec.size()==tVec.size() && sVec.size()==waMatrixVec.size())
  {
    for(unsigned int i=0;i<sVec.size();++i)
      extendModelFromPairPlusAlig(phePars,BRF,addNullWordToStrVec(sVec[i]),tVec[i],waMatrixVec[i],numReps,verbose);
  }
  else
  {
    cerr<<"Warning: wrong size of input vectors"<<endl;
  }
}

//-------------------------
void _wbaIncrPhraseModel::extendModelFromPairPlusAlig(PhraseExtractParameters phePars,
                                                      bool BRF,
                                                      Vector<string> ns,
                                                      Vector<string> t,
                                                      WordAligMatrix waMatrix,
                                                      float numReps,
                                                      int verbose/*=0*/)
{  
  if(t.size()<MAX_SENTENCE_LENGTH && ns.size()-1<MAX_SENTENCE_LENGTH)
  {         
    if(verbose)
    {
      cerr<<"* Processing sent. pair "<<numSent<<" (t length: "<< t.size()<<" , s length: "<< ns.size()-1<<" , numReps: "<<numReps<<")";
      cerr<<endl;
    }
    if(!BRF)
    {
          // RF estimation
      Vector<PhrasePair> vecPhPair;
      phraseExtract.extractConsistentPhrases(phePars,ns,t,waMatrix,vecPhPair);

          // Filter phrase pairs
      Vector<PhrasePair> vecFiltPhPair;
      for(unsigned int i=0;i<vecPhPair.size();++i)
      {
        if(phrasePairFilter.phrasePairIsOk(vecPhPair[i].s_,vecPhPair[i].t_))
          vecFiltPhPair.push_back(vecPhPair[i]);
      }

      storePhrasePairs(vecFiltPhPair,numReps,verbose);
    }
    else
    { // brf estimation
      double logNumSegms;
          // If iterators are disabled, a new estimation proposal
          // different to the original one is used
          //
          // This commented code uses the old estimation proposal
          // unsigned int numSegm=storePairsFromSegms(phePars,ns,t,waMatrix,numReps);
          // logNumSegm=log((double)numSegm);

          // The following code access the new estimation functionality
      Vector<PhrasePair> vecPhPair;
      logNumSegms=phraseExtract.segmBasedExtraction(phePars,ns,t,waMatrix,vecPhPair,verbose);
      storePhrasePairs(vecPhPair,numReps,verbose);
      if(verbose)
      {
        cerr<<"  log(Number of segmentations): "<< logNumSegms<<endl;
      }
      if(exp(logNumSegms)==0) logF<< "  Warning: Zero segmentations for sentence pair "<<numSent<<endl;
    }
  }
  else
  {
    logF<< "  Warning: Max. sentence length exceeded for sentence pair "<<numSent<<endl;
    cerr<< "  Warning: Max. sentence length exceeded for sentence pair "<<numSent<<endl;
  }
}

//-------------------------
void _wbaIncrPhraseModel::extractPhrasesFromPairPlusAlig(PhraseExtractParameters phePars,
                                                         Vector<string> ns,
                                                         Vector<string> t,
                                                         WordAligMatrix waMatrix,
                                                         Vector<PhrasePair>& vecPhPair,
                                                         int /*verbose=0*/)
{
  if(t.size()<MAX_SENTENCE_LENGTH && ns.size()-1<MAX_SENTENCE_LENGTH)
  {         
    phraseExtract.extractConsistentPhrases(phePars,ns,t,waMatrix,vecPhPair);
  }
  else
  {
    logF<< "  Warning: Max. sentence length exceeded for sentence pair "<<numSent<<endl;
    cerr<< "  Warning: Max. sentence length exceeded for sentence pair "<<numSent<<endl;
  }  
}

//-------------------------
void _wbaIncrPhraseModel::storePhrasePairs(const Vector<PhrasePair>& vecPhPair,
                                           float numReps,
                                           int verbose/*=0*/)
{
 Vector<string> t_,s_;

 for(unsigned int x=0;x<vecPhPair.size();++x)
 {
  t_=vecPhPair[x].t_;
  s_=vecPhPair[x].s_; 
  if(verbose==2) cerr<<"- ";	   
  if(verbose==2) for(unsigned int i=0;i<s_.size();++i) cerr<<s_[i]<<" ";
  if(verbose==2) cerr<<"| ";
  if(verbose==2) for(unsigned int i=0;i<t_.size();++i) cerr<<t_[i]<<" ";
  if(verbose==2 && s_.size()>0) cerr<<endl; 	   
  
  strIncrCountsOfEntry(s_,t_,numReps*vecPhPair[x].weight);
 }
}

//-------------------------
bool _wbaIncrPhraseModel::existRowOfNulls(unsigned int j1,
                                         unsigned int j2,
                                         Vector<unsigned int> &alig)
{
 unsigned int j;
	
 if(j1>=j2) return false;	
 for(j=j1;j<j2;++j) if(alig[j]!=0) return false;
 return true;	 
}

//-------------------------
ostream& _wbaIncrPhraseModel::printPars(ostream &outS,
                                        PhraseExtractParameters phePars,
                                        bool BRF)
{
 outS<<"* Monotone mode: "<<phePars.monotone<<endl;
 outS<<"* Estimation mode: ";
 if(BRF) outS<<"BRF"<<endl;
 else outS<<"RF"<<endl;
 outS<<"* max. target phrase length: "<< phePars.maxTrgPhraseLength<<endl;
 outS<<"* Constrain source phrase length: "<< phePars.constraintSrcLen<<endl;
 outS<<"* Count spurious words: " <<phePars.countSpurious <<endl;
 if(BRF)
	 outS<<"* max. number of combinations in table: "<< phePars.maxNumbOfCombsInTable<<endl;
 
 return outS;
}

//-------------------------

void _wbaIncrPhraseModel::clear(void)
{
  _incrPhraseModel::clear();
  numSent=0;
}

//---------------------------------
Vector<std::string>
_wbaIncrPhraseModel::addNullWordToStrVec(const Vector<std::string>& vw)
{
  Vector<std::string> result;

  result.push_back(NULL_WORD_STR);
  for(unsigned int i=0;i<vw.size();++i)
    result.push_back(vw[i]);

  return result;
}

//-------------------------
_wbaIncrPhraseModel::~_wbaIncrPhraseModel()
{
}

//-------------------------
