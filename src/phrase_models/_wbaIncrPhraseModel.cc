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
                                                     bool pseudoML,
                                                     int verbose/*=0*/) 
{
  clear();
  phraseExtractionTable.numSegmentationPrunings=0;
  return extendModel(aligFileName,phePars,pseudoML,verbose);
}

//-------------------------
bool _wbaIncrPhraseModel::extendModel(const char *aligFileName,
                                      PhraseExtractParameters phePars,
                                      bool pseudoML,
                                      int verbose/*=0*/)
{
  ofstream outF;
  	
  if(phePars.maxNumbOfCombsInTable<=0)
    phePars.maxNumbOfCombsInTable=DEFAULT_MAX_NUM_OF_COMBS_IN_TABLE;
  
      // Add parameters to Log File
  if(logFileOpen())
    printPars(logF,phePars,pseudoML);
 
      // Estimate the phrase model
  if(alignmentExtractor.open(aligFileName,GIZA_ALIG_FILE_FORMAT)==ERROR) 
  {
    cerr<<"Error while reading alignment file."<<endl;
    return ERROR;
  } 
  cerr<<"Estimating the phrase model from the alignment file "<<aligFileName<<endl;
  if(logFileOpen()) logF<<"Estimating the phrase model from the alignment file "<<aligFileName<<endl;
  extendModelFromAlignments(phePars,pseudoML,alignmentExtractor,verbose);
  alignmentExtractor.close();
  
  return OK;  
}

//-------------------------
void _wbaIncrPhraseModel::extendModelFromAlignments(PhraseExtractParameters phePars,
                                                    bool pseudoML,
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
   if((numSent%10)==0 && pseudoML) cerr<<"Processing sent. pair #"<<numSent<<"..."<<endl;
   t=outAlignments.get_t();
   ns=outAlignments.get_ns();	
   waMatrix=outAlignments.get_wamatrix();
   
   numReps=outAlignments.get_numReps();
   extendModelFromPairPlusAlig(phePars,pseudoML,ns,t,waMatrix,numReps,verbose);
 }
 logF<<"* #Pairs where the Segmentation Pruning was applied: "<<phraseExtractionTable.numSegmentationPrunings<<endl;
}

//-------------------------
void _wbaIncrPhraseModel::extModelFromPairAligVec(PhraseExtractParameters phePars,
                                                  bool pseudoML,
                                                  Vector<Vector<string> > sVec,
                                                  Vector<Vector<string> > tVec,
                                                  Vector<WordAligMatrix> waMatrixVec,
                                                  float numReps,
                                                  int verbose/*=0*/)
{
  if(sVec.size()==tVec.size() && sVec.size()==waMatrixVec.size())
  {
    for(unsigned int i=0;i<sVec.size();++i)
      extendModelFromPairPlusAlig(phePars,pseudoML,addNullWordToStrVec(sVec[i]),tVec[i],waMatrixVec[i],numReps,verbose);
  }
  else
  {
    cerr<<"Warning: wrong size of input vectors"<<endl;
  }
}

//-------------------------
void _wbaIncrPhraseModel::extendModelFromPairPlusAlig(PhraseExtractParameters phePars,
                                                      bool pseudoML,
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
    if(!pseudoML)
    { // RF estimation
      Vector<PhrasePair> vecPhPair;
      phraseExtractionTable.extractConsistentPhrases(phePars,ns,t,waMatrix,vecPhPair);
      storePhrasePairs(vecPhPair,numReps,verbose);
    }
    else
    { // pml estimation
#     ifdef THOT_DISABLE_ITERATORS
      double logNumSegms;
          // If iterators are disabled, a new estimation proposal
          // different to the original one is used
          //
          // This commented code uses the old estimation proposal
          // unsigned int numSegm=storePairsFromSegms(phePars,ns,t,waMatrix,numReps);
          // logNumSegm=log((double)numSegm);

          // The following code access the new estimation functionality
      Vector<PhrasePair> vecPhPair;
      logNumSegms=phraseExtractionTable.segmBasedExtraction(phePars,ns,t,waMatrix,vecPhPair,verbose);
      storePhrasePairs(vecPhPair,numReps,verbose);
      if(verbose)
      {
        cerr<<"  log(Number of segmentations): "<< logNumSegms<<endl;
      }
      if(exp(logNumSegms)==0) logF<< "  Warning: Zero segmentations for sentence pair "<<numSent<<endl;

#     else
      unsigned int numSegms=storePairsFromSegmsIter(phePars,ns,t,waMatrix,numReps,verbose);
      if(verbose)
      {
        cerr<<"  Number of segmentations: "<< numSegms<<endl;
      }
      if(numSegms==0) logF<< "  Warning: Zero segmentations for sentence pair "<<numSent<<endl;
#     endif
       
      if(phraseExtractionTable.pruneWasApplied) logF<< "  Warning: Segmentation pruning was applied for sentence pair "<< numSent<<" (t length: "<< t.size()<<", s length: "<< ns.size()-1<<", numReps: "<<numReps<<")"<<endl;
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
    phraseExtractionTable.extractConsistentPhrases(phePars,ns,t,waMatrix,vecPhPair);
  }
  else
  {
    logF<< "  Warning: Max. sentence length exceeded for sentence pair "<<numSent<<endl;
    cerr<< "  Warning: Max. sentence length exceeded for sentence pair "<<numSent<<endl;
  }  
}

//-------------------------
bool _wbaIncrPhraseModel::obtainBestAlignments(const char *bestAligFileName,
                                               const char *outputFileName,
                                               PhraseExtractParameters phePars,
                                               int verbose/*=0*/)
{
 ofstream outF;

 if(phePars.maxNumbOfCombsInTable<=0)
   phePars.maxNumbOfCombsInTable=DEFAULT_MAX_NUM_OF_COMBS_IN_TABLE;
 
 // Obtain the best alignments
 outF.open(outputFileName,ios::out);
 if(!outF)
 {
   cerr<<"Error while creating the best alignment file."<<endl;
   return ERROR;
 }
    
 cerr<<"Obtaining the best alignments for file "<<bestAligFileName<<endl;
 if(logFileOpen())logF<<"Obtaining the best alignments for file "<<bestAligFileName<<endl;
 if(alignmentExtractor.open(bestAligFileName,GIZA_ALIG_FILE_FORMAT)==ERROR) 
 {
   cerr<<"Error while opening file for obtaining the best alignments."<<endl;
   outF.close();  
   return ERROR;
 }
 else
 {
   phraseExtractionTable.numSegmentationPrunings=0;
   obtainBestAlignments(phePars,alignmentExtractor,outF,verbose);
 }
 if(verbose==1 || verbose==2)
 {
   cerr<<"Log-likelihood for file "<<bestAligFileName<<": "<<logLikelihood<<endl;
   cerr<<"Log-likelihood for file "<<bestAligFileName<<" assuming max. approx.: "<<logLikelihoodMaxApprox<<endl;      
 }
 alignmentExtractor.close();	 
 outF.close();
	 
 return OK;
}

//-------------------------
void _wbaIncrPhraseModel::obtainBestAlignments(PhraseExtractParameters phePars,
                                              AlignmentExtractor &bestAlignmentsFile,
                                              ostream &outF,
                                              int verbose/*=0*/)
{
 float numReps;
 Vector<string> ns,t;
 WordAligMatrix waMatrix,bestWaMatrix;	
 Prob bestProb;
 char cad[1024];
	
 numSent=0;	
 logLikelihood=0; // Initialize log-likelihood 
 logLikelihoodMaxApprox=0; // Initialize log-likelihood assuming maximum approximation
	
 while(bestAlignmentsFile.getNextAlignment())
 {
   ++numSent;
   t=bestAlignmentsFile.get_t();
   ns=bestAlignmentsFile.get_ns();	
   waMatrix=bestAlignmentsFile.get_wamatrix();
	
   numReps=bestAlignmentsFile.get_numReps();
     
   obtainBestAlignment(phePars,ns,t,waMatrix,bestWaMatrix,bestProb,verbose);
   sprintf(cad,"# Sentence %d , Align. probability: %g",numSent,(double)bestProb);
	 
   printAlignmentInGIZAFormat(outF,ns,t,bestWaMatrix,cad);		 
 }
	
 logF<<"* #Pairs where the Segmentation Pruning was applied: "<<phraseExtractionTable.numSegmentationPrunings<<endl;	
}
//-------------------------
unsigned int _wbaIncrPhraseModel::obtainBestAlignment(PhraseExtractParameters phePars,
                                                      const Vector<string>& ns,
                                                      const Vector<string>& t,
                                                      const WordAligMatrix& waMatrix,
                                                      WordAligMatrix &bestWaMatrix,
                                                      Prob &bestProb,
                                                      int verbose/*=0*/)
{	
 Vector<CellID> best_s;
 unsigned int numSegm;
 
 if(verbose)
   cerr<<"* Processing sent. pair "<<numSent<<" (t length: "<< t.size()<<" , s length: "<< ns.size()-1<<")"<<endl;
	 
 if(t.size()<MAX_SENTENCE_LENGTH && ns.size()-1<MAX_SENTENCE_LENGTH)
 {
#  ifdef THOT_DISABLE_ITERATORS
   numSegm=getBestAlignment(phePars,ns,t,waMatrix,best_s,verbose);
#  else
   numSegm=getBestAligUsingIter(phePars,ns,t,waMatrix,best_s,verbose);
#  endif

   if(verbose)
   {
     cerr<<"  Number of segmentations: "<< numSegm<<endl;
   }

   bestWaMatrix.init(ns.size()-1,t.size());	  
   if(numSegm==0)
   {
     logF<< "  Warning: Zero segmentations for sentence pair "<<numSent<<endl;
     cerr<< "  Warning: Zero segmentations for sentence pair "<<numSent<<endl;
     bestWaMatrix.set();	
     bestProb=0;		
   }
   else
   {
     bestWaMatrix=getWaMatrixForSegmentation(best_s);	
     bestProb=getProbForSegmentation(best_s);
     // Print best segmentation
     if(verbose==1 || verbose==2)
     {
       cerr<<"  Best segmentation:"<<endl;
       printSegmInfo(best_s,waMatrix);
       cerr<<endl;
     }
   }
   return numSegm;
 }
 else
 {
   bestWaMatrix.init(ns.size()-1,t.size());
   bestWaMatrix.set();	
   bestProb=0;
   logF<< "  Warning! max. sentence length exceeded for sentence pair "<<numSent<<endl;
   cerr<< "  Warning! max. sentence length exceeded for sentence pair "<<numSent<<endl;
   return OK;
 }
}

//-------------------------
unsigned int _wbaIncrPhraseModel::getBestAlignment(PhraseExtractParameters phePars,
                                                   const Vector<string>& ns,
                                                   const Vector<string>& t,
                                                   const WordAligMatrix& waMatrix,
                                                   Vector<CellID>& best_s,
                                                   int verbose/*=0*/)
{
 unsigned int numSegm,i,j;
 PhrasePair phPair;
 CellID cid;
 LgProb maxLogProb,segLogProb;   
 Prob sentLikelihood=0;
 Bisegm::const_iterator bsIter;

 Bisegm &segmentations=phraseExtractionTable.obtainPossibleSegmentations(phePars,ns,t,waMatrix);
   if(phraseExtractionTable.pruneWasApplied)
     logF<< "  Warning: Segmentation pruning was applied for sentence pair "<<numSent<<" (t length: "<< t.size()<<" , s length: "<< ns.size()-1<<")"<<endl;
   
 maxLogProb=-FLT_MAX;
 numSegm=0;
 for(bsIter=segmentations.begin();bsIter!=segmentations.end();++bsIter) // For each segmentation...
 {	 
  if(verbose==2) cerr<<"- Segmentation # "<<numSegm<<endl;
  segLogProb=0;
  
  for(i=0;i<bsIter->size();++i) // For each phrase pair of a given segmentation...
  {
   cid=(*bsIter)[i];
   phPair=phraseExtractionTable.getPhrasePairFromCellID(cid);
   segLogProb=segLogProb+log((double)strPt_s_(phPair.s_,phPair.t_));
   if(verbose==2) for(j=0;j<phPair.s_.size();++j) cerr<< phPair.s_[j]<<" ";	  
   if(verbose==2) cerr<<"| ";
   if(verbose==2) for(j=0;j<phPair.t_.size();++j) cerr<< phPair.t_[j]<<" ";
   if(verbose==2) cerr<<" "<<strPt_s_(phPair.s_,phPair.t_)<<endl;	    
  }
  segLogProb=(double)segLogProb+log((double)pk_tlen(phraseExtractionTable.get_tlen(),bsIter->size()));
  if(maxLogProb<segLogProb)
  {
    maxLogProb=segLogProb;
    best_s=*bsIter;
  }
  sentLikelihood=sentLikelihood+exp((double)segLogProb);
  
  if(verbose==2) 
      cerr<<"  p(k= "<<bsIter->size()<<"|J= "<<phraseExtractionTable.get_tlen()<<")= "<<pk_tlen(phraseExtractionTable.get_tlen(),bsIter->size())<<endl<<endl;					 

  if(verbose==2) cerr<<"  segProbability: "<<exp((double)segLogProb)<<endl; 
  if(verbose==2) cerr<<"  log(segProbability): "<<segLogProb<<endl<<endl;
 
  ++numSegm;
 }
 
 sentLikelihood=(double)sentLikelihood/segmentations.size();
 logLikelihood=logLikelihood+log((double)sentLikelihood); // add sent-likelihood to the log-likelihood
 logLikelihoodMaxApprox=logLikelihoodMaxApprox+maxLogProb;
 if(verbose==2) cerr<<"  sentence likelihood: "<<sentLikelihood<<endl;
 if(verbose==2) cerr<<"  sentence log-likelihood: "<<log((double)sentLikelihood)<<endl;
 if(verbose==2) cerr<<"  maxProbability: "<<exp((double)maxLogProb)<<endl;
 if(verbose==2) cerr<<endl;	

 return segmentations.size(); 
}

//-------------------------
unsigned int _wbaIncrPhraseModel::getBestAligUsingIter(PhraseExtractParameters phePars,
                                                       const Vector<string>& ns,
                                                       const Vector<string>& t,
                                                       const WordAligMatrix& waMatrix,
                                                       Vector<CellID>& best_s,
                                                       int verbose/*=0*/)
{
  unsigned int numSegm,i,j;
  PhrasePair phPair;
  CellID cid;
  LgProb maxLogProb,segLogProb;   
  Prob sentLikelihood=0;  
  PhraseExtractionTable::iterator bsIter;
  Vector<CellID> vcid;
  
  maxLogProb=-FLT_MAX;
  numSegm=0;
  bsIter=phraseExtractionTable.getSegmIter(phePars,ns,t,waMatrix);
  if(bsIter.end()) return 0;
  do
  {
    if(verbose==2) cerr<<"- Segmentation # "<<numSegm<<endl;
    segLogProb=0;
    vcid=*bsIter;
    for(i=0;i<vcid.size();++i) // For each phrase pair of a given segmentation...
    {
      cid=vcid[i];
      phPair=phraseExtractionTable.getPhrasePairFromCellID(cid);
      segLogProb=segLogProb+log((double)strPt_s_(phPair.s_,phPair.t_));
	
      if(verbose==2) for(j=0;j<phPair.s_.size();++j) cerr<< phPair.s_[j]<<" ";	  
      if(verbose==2) cerr<<"| ";
      if(verbose==2) for(j=0;j<phPair.t_.size();++j) cerr<< phPair.t_[j]<<" ";
      if(verbose==2) cerr<<" "<<strPt_s_(phPair.s_,phPair.t_)<<endl;	    
    }
    segLogProb=segLogProb+log((double)pk_tlen(phraseExtractionTable.get_tlen(),vcid.size()));
    if(maxLogProb<segLogProb)
    {
      maxLogProb=segLogProb;
      best_s=*bsIter;
    }
    sentLikelihood=sentLikelihood+exp((double)segLogProb);
    
    if(verbose==2) 
      cerr<<"  p(k= "<<vcid.size()<<"|J= "<<phraseExtractionTable.get_tlen()<<")= "<<pk_tlen(phraseExtractionTable.get_tlen(),vcid.size())<<endl<<endl;					 
    
    if(verbose==2) cerr<<"  segProbability: "<<exp((double)segLogProb)<<endl; 
    if(verbose==2) cerr<<"  log(segProbability): "<<segLogProb<<endl<<endl;
    
    ++numSegm;
  } while(++bsIter);

  sentLikelihood=(double)sentLikelihood/numSegm;
  logLikelihood=logLikelihood+log((double)sentLikelihood); // add sent-likelihood to the log-likelihood
  logLikelihoodMaxApprox=logLikelihoodMaxApprox+maxLogProb;
  if(verbose==2) cerr<<"  sentence likelihood: "<<sentLikelihood<<endl;
  if(verbose==2) cerr<<"  sentence log-likelihood: "<<log((double)sentLikelihood)<<endl;
  if(verbose==2) cerr<<"  maxProbability: "<<exp((double)maxLogProb)<<endl;
  if(verbose==2) cerr<<endl;	

  return numSegm;
}

//-------------------------
WordAligMatrix _wbaIncrPhraseModel::getWaMatrixForSegmentation(Vector<CellID>& segmentation)
{
 WordAligMatrix segWaMatrix;
 CellID cid;
 Bitset<MAX_SENTENCE_LENGTH> coverage;	
 unsigned int I,J,leftPos,rightPos,i,j,s;
	
 I=phraseExtractionTable.get_nslen()-1;
 J=phraseExtractionTable.get_tlen();	
 segWaMatrix.init(I,J);	
 for(s=0;s<segmentation.size();++s)
 {
   cid=segmentation[s];
   coverage=phraseExtractionTable.getCoverageForCellID(cid);
   leftPos=phraseExtractionTable.leftMostPosInCell(cid.x,cid.y);
   rightPos=phraseExtractionTable.rightMostPosInCell(cid.x,cid.y);
	  
   for(j=leftPos;j<=rightPos;++j)
     for(i=1;i<=I;++i)
       if(coverage.test(i)) segWaMatrix.set(i-1,j);
 }	
 
 return segWaMatrix;
}

//-------------------------
Prob _wbaIncrPhraseModel::getProbForSegmentation(Vector<CellID>& segmentation)
{
 LgProb segLgProb=0;
 PhrasePair phPair;	
 CellID cid;	
 unsigned int i;
	
 for(i=0;i<segmentation.size();++i)
 {
   cid=segmentation[i];
   phPair=phraseExtractionTable.getPhrasePairFromCellID(cid);
   segLgProb=segLgProb+log((double)strPt_s_(phPair.s_,phPair.t_));	 
 } 	
	
 return exp((double)segLgProb);
}

//-------------------------
unsigned int _wbaIncrPhraseModel::storePairsFromSegms(PhraseExtractParameters phePars,
                                                      const Vector<string> &ns,
                                                      const Vector<string> &t,
                                                      const WordAligMatrix &waMatrix,
                                                      float numReps,
                                                      int verbose/*=0*/)
{
  Bisegm& segmentations=phraseExtractionTable.obtainPossibleSegmentations(phePars,ns,t,waMatrix);
  storePhrasePairs(segmentations,waMatrix,numReps,verbose);
  return segmentations.size();
}

//-------------------------
unsigned int _wbaIncrPhraseModel::storePairsFromSegmsIter(PhraseExtractParameters phePars,
                                                          const Vector<string> &ns,
                                                          const Vector<string> &t,
                                                          const WordAligMatrix &waMatrix,
                                                          float numReps,
                                                          int verbose/*=0*/)
{
  unsigned int numSegm;
  std::map<CellID,unsigned int> cellMap;
  std::map<CellID,unsigned int>::iterator cellMapIter;
  Vector<CellID> vcid;
  CellID cid;
  PhrasePair phPair;
  
  numSegm=0;
  PhraseExtractionTable::iterator bsIter;
  bsIter=phraseExtractionTable.getSegmIter(phePars,ns,t,waMatrix);
  if(!bsIter.end())
  {
    do
    { // Iterate over the set of segmentations
      if(verbose==2) cerr<<"- Segmentation # "<<numSegm<<endl;
      vcid=(*bsIter);
      // Revise segmentation-length counts
      segLenTable.incrCountOf_tlenk(phraseExtractionTable.get_tlen(),vcid.size());
      segLenTable.incrCountOf_tlen(phraseExtractionTable.get_tlen());
      // Update counts of each cell id
      for(unsigned int i=0;i<vcid.size();++i)
      {
        cellMapIter=cellMap.find(vcid[i]);
        if(cellMapIter==cellMap.end()) cellMap[vcid[i]]=1;
        else ++(cellMapIter->second);
      }
      // Print segmentation information
      if(verbose==2) printSegmInfo(vcid,waMatrix);
      else if(verbose==VERBOSE_AACHEN) printSegmInfoAachen(vcid,waMatrix);
      
      ++numSegm;
    } while(++bsIter);


        // Store phrase pairs
    for(cellMapIter=cellMap.begin();cellMapIter!=cellMap.end();++cellMapIter)
    {
      cid=cellMapIter->first;
      phPair=phraseExtractionTable.getPhrasePairFromCellID(cid);
      strIncrCountsOfEntry(phPair.s_,
                           phPair.t_,
                           numReps*(float) phPair.weight*((float)cellMapIter->second/numSegm));
    }
  }
  
  if(verbose==2 || verbose==VERBOSE_AACHEN) cerr<<endl;
  
  return numSegm;
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
void _wbaIncrPhraseModel::storePhrasePairs(Bisegm& segmentations,
                                          const WordAligMatrix& alig,
                                          float numReps,
                                          int verbose/*=0*/)
{
 unsigned int s;
 Bisegm::const_iterator bsIter;
 std::map<CellID,unsigned int> cellMap;
 std::map<CellID,unsigned int>::iterator cellMapIter;
 Vector<CellID> vcid;
 CellID cid;
 PhrasePair phPair;
  
 s=0;
 for(bsIter=segmentations.begin();bsIter!=segmentations.end();++bsIter)
 {	 
   if(verbose==2) cerr<<"- Segmentation # "<<s;
   vcid=(*bsIter);
       // Revise segmentation-length counts
   segLenTable.incrCountOf_tlenk(phraseExtractionTable.get_tlen(),vcid.size());
   segLenTable.incrCountOf_tlen(phraseExtractionTable.get_tlen());
      // Update counts of each cell id
   for(unsigned int i=0;i<vcid.size();++i)
   {
     cellMapIter=cellMap.find(vcid[i]);
     if(cellMapIter==cellMap.end()) cellMap[vcid[i]]=1;
     else ++(cellMapIter->second);
   }
   // Print segmentation information
   if(verbose==2) printSegmInfo(vcid,alig);
   else if(verbose==VERBOSE_AACHEN) printSegmInfoAachen(vcid,alig);
     
   ++s;
 }

 // Store phrase pairs
 for(cellMapIter=cellMap.begin();cellMapIter!=cellMap.end();++cellMapIter)
 {
   cid=cellMapIter->first;
   phPair=phraseExtractionTable.getPhrasePairFromCellID(cid);
   strIncrCountsOfEntry(phPair.s_,
                        phPair.t_,
                        numReps*(float) phPair.weight*((float)cellMapIter->second/segmentations.size()));
 }
 if(verbose==2 || verbose==VERBOSE_AACHEN) cerr<<endl;		 
}

//-------------------------
void _wbaIncrPhraseModel::storePhrasePairs(const Vector<CellID> vcid,
                                           const WordAligMatrix& alig,
                                           float numReps,
                                           unsigned int numSegm,
                                           int verbose/*=0*/)
{
  PhrasePair phPair;

  // Print segmentation information
  if(verbose==2) printSegmInfo(vcid,alig);
  else if(verbose==VERBOSE_AACHEN) printSegmInfoAachen(vcid,alig);

  segLenTable.incrCountOf_tlenk(phraseExtractionTable.get_tlen(),vcid.size());
  segLenTable.incrCountOf_tlen(phraseExtractionTable.get_tlen());

  strIncrCountsOfEntry(phPair.s_,
                       phPair.t_,
                       numReps*(float) phPair.weight/(float)numSegm);	  
}

//-------------------------
void _wbaIncrPhraseModel::printSegmInfo(const Vector<CellID> vcid,
                                       const WordAligMatrix& alig)
{
  unsigned int i,j;
  PhrasePair phPair;
  CellID cid;

  cerr<<"  (Best segm. length: "<<vcid.size()<<")"<<endl;
  for(i=0;i<vcid.size();++i)
  {
    cid=vcid[i];
    phPair=phraseExtractionTable.getPhrasePairFromCellID(cid);
    for(j=0;j<phPair.s_.size();++j) cerr<< phPair.s_[j]<<" ";
    cerr<<"| ";	 
    for(j=0;j<phPair.t_.size();++j) cerr<< phPair.t_[j]<<" ";	   
    cerr<<endl;	  
  }
}

//-------------------------
void _wbaIncrPhraseModel::printSegmInfoAachen(const Vector<CellID> vcid,
                                              const WordAligMatrix& alig)
{
  unsigned int i;
  PhrasePair phPair;
  CellID cid;

  cerr<<"SENT: "<< numSent<<endl;
  printSentAligInAachenFormat(alig);
  cerr<<endl;
    
  for(i=0;i<vcid.size();++i)
  {
    cid=vcid[i];
    phPair=phraseExtractionTable.getPhrasePairFromCellID(cid);
   
    printPhraseAligInAachenFormat(phraseExtractionTable.leftMostPosInCell(cid.x,cid.y),
                                  phraseExtractionTable.rightMostPosInCell(cid.x,cid.y),
                                  phraseExtractionTable.get_nslen(),
                                  phraseExtractionTable.getCoverageForCellID(cid)); 	   
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
                                       bool pseudoML)
{
 outS<<"* Monotone mode: "<<phePars.monotone<<endl;
 outS<<"* Estimation mode: ";
 if(pseudoML) outS<<"pseudoML"<<endl;
 else outS<<"RF"<<endl;
 outS<<"* max. target phrase length: "<< phePars.maxTrgPhraseLength<<endl;
 outS<<"* Constrain source phrase length: "<< phePars.constraintSrcLen<<endl;
 outS<<"* Count spurious words: " <<phePars.countSpurious <<endl;
 if(pseudoML)
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
