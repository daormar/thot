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
/* Module: PhrLocalSwLiTm                                           */
/*                                                                  */
/* Definitions file: PhrLocalSwLiTm.cc                              */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "PhrLocalSwLiTm.h"
#include <_incrSwAligModel.h>

//--------------- PhrLocalSwLiTm class functions
//

//---------------------------------------
PhrLocalSwLiTm::PhrLocalSwLiTm(void):_phrSwTransModel<PhrLocalSwLiTmHypRec<HypEqClassF> >()
{
      // Initialize stepNum data member
  stepNum=0;
}

//---------------------------------------
BaseSmtModel<PhrLocalSwLiTmHypRec<HypEqClassF> >* PhrLocalSwLiTm::clone(void)
{
  return new PhrLocalSwLiTm(*this);
}

//---------------------------------
bool PhrLocalSwLiTm::loadAligModel(const char* prefixFileName)
{
  bool ret=_phrSwTransModel<PhrLocalSwLiTmHypRec<HypEqClassF> >::loadAligModel(prefixFileName);
  if(ret==ERROR) return ERROR;

      // Load lambda file
  std::string lambdaFile=prefixFileName;
  lambdaFile=lambdaFile+".lambda";
  ret=load_lambdas(lambdaFile.c_str());
  if(ret==ERROR) return ERROR;
    
  return OK;
}

//---------------------------------
bool PhrLocalSwLiTm::printAligModel(std::string printPrefix)
{
  bool ret=_phrSwTransModel<PhrLocalSwLiTmHypRec<HypEqClassF> >::printAligModel(printPrefix);
  if(ret==ERROR) return ERROR;

      // Print lambda file
  std::string lambdaFile=printPrefix;
  lambdaFile=lambdaFile+".lambda";
  ret=print_lambdas(lambdaFile.c_str());
  if(ret==ERROR) return ERROR;
  
  return OK;
}

//---------------------------------
void PhrLocalSwLiTm::clear(void)
{
  _phrSwTransModel<PhrLocalSwLiTmHypRec<HypEqClassF> >::clear();
  vecVecInvPhPair.clear();
  vecSrcSent.clear();
  vecTrgSent.clear();
  stepNum=0;
}

//---------------------------------
int PhrLocalSwLiTm::updateLinInterpWeights(std::string srcDevCorpusFileName,
                                           std::string trgDevCorpusFileName,
                                           int verbose/*=0*/)
{
      // Initialize downhill simplex input parameters
  Vector<double> initial_weights;
  initial_weights.push_back(swModelInfoPtr->lambda_swm);
  initial_weights.push_back(swModelInfoPtr->lambda_invswm);
  int ndim=initial_weights.size();
  double* start=(double*) malloc(ndim*sizeof(double));
  int nfunk;
  double* x=(double*) malloc(ndim*sizeof(double));
  double y;

      // Create temporary file
  FILE* tmp_file=tmpfile();
  
  if(tmp_file==0)
  {
    cerr<<"Error updating linear interpolation weights of the phrase model, tmp file could not be created"<<endl;
    return ERROR;
  }

      // Extract phrase pairs from development corpus
  Vector<Vector<PhrasePair> > invPhrPairs;
  int ret=extractPhrPairsFromDevCorpus(srcDevCorpusFileName,trgDevCorpusFileName,invPhrPairs,verbose);
  if(ret!=OK)
    return ERROR;
  
      // Execute downhill simplex algorithm
  bool end=false;
  while(!end)
  {
        // Set initial weights (each call to step_by_step_simplex starts
        // from the initial weights)
    for(unsigned int i=0;i<initial_weights.size();++i)
      start[i]=initial_weights[i];
    
        // Execute step by step simplex
    double curr_dhs_ftol;
    ret=step_by_step_simplex(start,ndim,PHRSWLITM_DHS_FTOL,PHRSWLITM_DHS_SCALE_PAR,NULL,tmp_file,&nfunk,&y,x,&curr_dhs_ftol,false);

    switch(ret)
    {
      case OK: end=true;
        break;
      case DSO_NMAX_ERROR: cerr<<"Error updating linear interpolation weights of the phrase model, maximum number of iterations exceeded"<<endl;
        end=true;
        break;
      case DSO_EVAL_FUNC: // A new function evaluation is requested by downhill simplex
        double perp;
        int retEval=new_dhs_eval(invPhrPairs,tmp_file,x,perp);
        if(retEval==ERROR)
        {
          end=true;
          break;
        }
            // Print verbose information
        if(verbose>=1)
        {
          cerr<<"niter= "<<nfunk<<" ; current ftol= "<<curr_dhs_ftol<<" (FTOL="<<PHRSWLITM_DHS_FTOL<<") ; ";
          cerr<<"weights= "<<swModelInfoPtr->lambda_swm<<" "<<swModelInfoPtr->lambda_invswm;
          cerr<<" ; perp= "<<perp<<endl; 
        }
        break;
    }
  }
  
      // Set new weights if updating was successful
  if(ret==OK)
  {
    swModelInfoPtr->lambda_swm=start[0];
    swModelInfoPtr->lambda_invswm=start[1];
  }
  else
  {
    swModelInfoPtr->lambda_swm=initial_weights[0];
    swModelInfoPtr->lambda_invswm=initial_weights[1];
  }
  
      // Clear variables
  free(start);
  free(x);
  fclose(tmp_file);

  if(ret!=OK)
    return ERROR;
  else
    return OK; 
}

//---------------
int PhrLocalSwLiTm::extractConsistentPhrasePairs(const Vector<std::string>& srcSentStrVec,
                                                 const Vector<std::string>& refSentStrVec,
                                                 Vector<PhrasePair>& vecInvPhPair,
                                                 bool verbose/*=0*/)
{
      // Generate alignments
  WordAligMatrix waMatrix;
  WordAligMatrix invWaMatrix;
  
  swModelInfoPtr->swAligModelPtr->obtainBestAlignmentVecStr(srcSentStrVec,refSentStrVec,waMatrix);
  swModelInfoPtr->invSwAligModelPtr->obtainBestAlignmentVecStr(refSentStrVec,srcSentStrVec,invWaMatrix);
  
      // Operate alignments
  Vector<std::string> nsrcSentStrVec=swModelInfoPtr->swAligModelPtr->addNullWordToStrVec(srcSentStrVec);
  Vector<std::string> nrefSentStrVec=swModelInfoPtr->swAligModelPtr->addNullWordToStrVec(refSentStrVec);  

  waMatrix.transpose();

      // Execute symmetrization
  invWaMatrix.symmetr1(waMatrix);

      // Extract consistent pairs
  _wbaIncrPhraseModel* wbaIncrPhraseModelPtr=dynamic_cast<_wbaIncrPhraseModel* >(phrModelInfoPtr->invPbModelPtr);
  if(wbaIncrPhraseModelPtr)
  {
    PhraseExtractParameters phePars;
    wbaIncrPhraseModelPtr->extractPhrasesFromPairPlusAlig(phePars,
                                                          nrefSentStrVec,
                                                          srcSentStrVec,
                                                          invWaMatrix,
                                                          vecInvPhPair,
                                                          verbose);
    return OK;
  }
  else
  {
        // If the model is not a subclass of _wbaIncrPhraseModel,
        // extract phrases using an instance of WbaIncrPhraseModel
    PhraseExtractParameters phePars;
    WbaIncrPhraseModel wbaIncrPhraseModel;
    wbaIncrPhraseModel.extractPhrasesFromPairPlusAlig(phePars,
                                                      nrefSentStrVec,
                                                      srcSentStrVec,
                                                      invWaMatrix,
                                                      vecInvPhPair,
                                                      verbose);
    return OK;
  }
}

//---------------
int PhrLocalSwLiTm::extractPhrPairsFromDevCorpus(std::string srcDevCorpusFileName,
                                                 std::string trgDevCorpusFileName,
                                                 Vector<Vector<PhrasePair> >& invPhrPairs,
                                                 int verbose/*=0*/)
{
// NOTE: this function requires the ability to extract new translation
// options. This can be achieved using the well-known phrase-extract
// algorithm.
  
  awkInputStream srcDevStream;
  awkInputStream trgDevStream;

      // Open files
  if(srcDevStream.open(srcDevCorpusFileName.c_str())==ERROR)
  {
    cerr<<"Unable to open file with source development sentences."<<endl;
    return ERROR;
  }  
  if(trgDevStream.open(trgDevCorpusFileName.c_str())==ERROR)
  {
    cerr<<"Unable to open file with target development sentences."<<endl;
    return ERROR;
  }  

      // Iterate over all sentences
  invPhrPairs.clear();
  while(srcDevStream.getln())
  {
    if(!trgDevStream.getln())
    {
      cerr<<"Unexpected end of file with target development sentences."<<endl;
      return ERROR;      
    }

        // Obtain sentence pair
    Vector<std::string> srcSentStrVec;
    Vector<std::string> refSentStrVec;
    Count c;

        // Extract source sentence
    for(unsigned int i=1;i<=srcDevStream.NF;++i)
      srcSentStrVec.push_back(srcDevStream.dollar(i));

        // Extract target sentence
    for(unsigned int i=1;i<=trgDevStream.NF;++i)
      refSentStrVec.push_back(trgDevStream.dollar(i));

        // Extract consistent phrase pairs
    Vector<PhrasePair> vecInvPhPair;
    int ret=extractConsistentPhrasePairs(srcSentStrVec,refSentStrVec,vecInvPhPair,verbose);
    if(ret==ERROR)
      return ERROR;
      
        // Add vector of phrase pairs
    invPhrPairs.push_back(vecInvPhPair);
  }
    
      // Close files
  srcDevStream.close();
  trgDevStream.close();
    
  return OK;
}

//---------------
double PhrLocalSwLiTm::phraseModelPerplexity(const Vector<Vector<PhrasePair> >& invPhrPairs,
                                             int /*verbose=0*/)
{
      // Iterate over all sentences
  double loglikelihood=0;
  unsigned int numPhrPairs=0;
  
      // Obtain perplexity contribution for consistent phrase pairs
  for(unsigned int i=0;i<invPhrPairs.size();++i)
  {
    // cerr<<endl;
    for(unsigned int j=0;j<invPhrPairs[i].size();++j)
    {
      Vector<WordIndex> srcPhrasePair=strVectorToSrcIndexVector(invPhrPairs[i][j].t_);
      Vector<WordIndex> trgPhrasePair=strVectorToTrgIndexVector(invPhrPairs[i][j].s_);
      loglikelihood+=(double)smoothedPhrScore_s_t_(srcPhrasePair,trgPhrasePair);
      loglikelihood+=(double)smoothedPhrScore_t_s_(srcPhrasePair,trgPhrasePair);

      // for(unsigned int k=0;k<invPhrPairs[i][j].s_.size();++k)
      //   cerr<<invPhrPairs[i][j].s_[k]<<" ";
      // cerr<<"|||";
      // for(unsigned int k=0;k<invPhrPairs[i][j].t_.size();++k)
      //   cerr<<" "<<invPhrPairs[i][j].t_[k];
      // cerr<<" ||| "<<(double)smoothedPhrScore_s_t_(srcPhrasePair,trgPhrasePair)<<" "<<(double)smoothedPhrScore_t_s_(srcPhrasePair,trgPhrasePair)<<endl;
    }
        // Update number of phrase pairs
    numPhrPairs+=invPhrPairs[i].size();
  }

      // Return perplexity
  return -1*(loglikelihood/(double)numPhrPairs);
}


//---------------
int PhrLocalSwLiTm::new_dhs_eval(const Vector<Vector<PhrasePair> >& invPhrPairs,
                                 FILE* tmp_file,
                                 double* x,
                                 double& obj_func)
{
  LgProb totalLogProb;
  bool weightsArePositive=true;
  bool weightsAreBelowOne=true;
  
      // Fix weights to be evaluated
  swModelInfoPtr->lambda_swm=x[0];
  swModelInfoPtr->lambda_invswm=x[1];
  for(unsigned int i=0;i<2;++i)
  {
    if(x[i]<0) weightsArePositive=false;
    if(x[i]>=1) weightsAreBelowOne=false;
  }
  
  if(weightsArePositive && weightsAreBelowOne)
  {
        // Obtain perplexity
    obj_func=phraseModelPerplexity(invPhrPairs,obj_func);
  }
  else
  {
    obj_func=DBL_MAX;
  }
  
      // Print result to tmp file
  fprintf(tmp_file,"%g\n",obj_func);
  fflush(tmp_file);
      // step_by_step_simplex needs that the file position
      // indicator is set at the start of the stream
  rewind(tmp_file);

  return OK;
}

//---------------------------------
PhrLocalSwLiTm::Hypothesis PhrLocalSwLiTm::nullHypothesis(void)
{
  Hypothesis hyp;
  Hypothesis::DataType dataType;
  Hypothesis::ScoreInfo scoreInfo;

      // Init scoreInfo
  scoreInfo.score=0;

      // Init language model state
  langModelInfoPtr->lModelPtr->getStateForBeginOfSentence(scoreInfo.lmHist);

      // Initial word penalty lgprob
  scoreInfo.score+=sumWordPenaltyScore(0);

      // Add sentence length model contribution
  Hypothesis hypAux;
  hypAux.setData(nullHypothesisHypData());
  scoreInfo.score+=sentLenScoreForPartialHyp(hypAux.getKey(),0);

#ifdef THOT_DEBUG
  HypDebugData hdData;

  hdData.opCode="init";
  hdData.parameters.clear();
  hdData.partialContribs.insert(hdData.partialContribs.begin(),getNumWeights(),0);
  hdData.partialContribs[WPEN]=sumWordPenaltyScore(0);
  hdData.partialContribs[SWLENLI]=sentLenScoreForPartialHyp(hypAux.getKey(),0);
  hdData.accum=scoreInfo.score;
  hyp.hDebug.push_back(hdData);
#endif

      // Set ScoreInfo
  hyp.setScoreInfo(scoreInfo);

      // Set DataType
  dataType=nullHypothesisHypData();
  hyp.setData(dataType);

  return hyp; 
}

//---------------------------------
PhrLocalSwLiTm::HypDataType
PhrLocalSwLiTm::nullHypothesisHypData(void)
{
  HypDataType dataType;

  dataType.ntarget.clear();  
  dataType.ntarget.push_back(NULL_WORD);
  dataType.sourceSegmentation.clear();  
  dataType.targetSegmentCuts.clear();

  return dataType;
}

//---------------------------------
bool PhrLocalSwLiTm::obtainPredecessorHypData(HypDataType& hypd)
{
  HypDataType predData;

  predData=hypd;
      // verify if hyp has a predecessor
  if(predData.ntarget.size()<=1) return false;
  else
  {
    unsigned int i;
    unsigned int cuts;

    if(predData.targetSegmentCuts.size()==0)
    {
      cerr<<"Warning: hypothesis data corrupted"<<endl;
      return false;
    }

        // get previous ntarget
    cuts=predData.targetSegmentCuts.size();
    if(cuts==1)
    {
      i=predData.targetSegmentCuts[0];
    }
    else
    {
      i=predData.targetSegmentCuts[cuts-1]-predData.targetSegmentCuts[cuts-2];
    }
    while(i>0)
    {
      predData.ntarget.pop_back();
      --i;
    }
        // get previous sourceSegmentation
    predData.sourceSegmentation.pop_back();
        // get previous targetSegmentCuts
    predData.targetSegmentCuts.pop_back();
        // set data
    hypd=predData;
    
    return true;
  }
}

//---------------------------------
bool PhrLocalSwLiTm::isCompleteHypData(const HypDataType& hypd)const
{
  if(numberOfUncoveredSrcWordsHypData(hypd)==0) return true;
  else return false;
}

//---------------------------------
void PhrLocalSwLiTm::setWeights(Vector<float> wVec)
{
  if(wVec.size()>WPEN) langModelInfoPtr->langModelPars.wpScaleFactor=smoothLlWeight(wVec[WPEN]);
  if(wVec.size()>LMODEL) langModelInfoPtr->langModelPars.lmScaleFactor=smoothLlWeight(wVec[LMODEL]);
  if(wVec.size()>TSEGMLEN) phrModelInfoPtr->phraseModelPars.trgSegmLenWeight=smoothLlWeight(wVec[TSEGMLEN]);
  if(wVec.size()>SJUMP) phrModelInfoPtr->phraseModelPars.srcJumpWeight=smoothLlWeight(wVec[SJUMP]);
  if(wVec.size()>SSEGMLEN) phrModelInfoPtr->phraseModelPars.srcSegmLenWeight=smoothLlWeight(wVec[SSEGMLEN]);
  if(wVec.size()>PTS) phrModelInfoPtr->phraseModelPars.ptsWeight=smoothLlWeight(wVec[PTS]);
  if(wVec.size()>PST) phrModelInfoPtr->phraseModelPars.pstWeight=smoothLlWeight(wVec[PST]);
  if(wVec.size()>SWLENLI) swModelInfoPtr->invSwModelPars.lenWeight=smoothLlWeight(wVec[SWLENLI]);
}

//---------------------------------
void PhrLocalSwLiTm::getWeights(Vector<pair<std::string,float> >& compWeights)
{
  compWeights.clear();
  
  pair<std::string,float> compWeight;

  compWeight.first="wpw";
  compWeight.second=langModelInfoPtr->langModelPars.wpScaleFactor;
  compWeights.push_back(compWeight);

  compWeight.first="lmw";
  compWeight.second=langModelInfoPtr->langModelPars.lmScaleFactor;
  compWeights.push_back(compWeight);

  compWeight.first="tseglenw";
  compWeight.second=phrModelInfoPtr->phraseModelPars.trgSegmLenWeight;
  compWeights.push_back(compWeight);

  compWeight.first="sjumpw";
  compWeight.second=phrModelInfoPtr->phraseModelPars.srcJumpWeight;
  compWeights.push_back(compWeight);

  compWeight.first="sseglenw";
  compWeight.second=phrModelInfoPtr->phraseModelPars.srcSegmLenWeight;
  compWeights.push_back(compWeight);

  compWeight.first="ptsw";
  compWeight.second=phrModelInfoPtr->phraseModelPars.ptsWeight;
  compWeights.push_back(compWeight);

  compWeight.first="pstw";
  compWeight.second=phrModelInfoPtr->phraseModelPars.pstWeight;
  compWeights.push_back(compWeight);

  compWeight.first="swlenliw";
  compWeight.second=swModelInfoPtr->invSwModelPars.lenWeight;
  compWeights.push_back(compWeight);
}

//---------------------------------
void PhrLocalSwLiTm::printWeights(ostream &outS)
{
  outS<<"wpw: "<<langModelInfoPtr->langModelPars.wpScaleFactor<<" , ";
  outS<<"lmw: "<<langModelInfoPtr->langModelPars.lmScaleFactor<<" , ";
  outS<<"tseglenw: "<<phrModelInfoPtr->phraseModelPars.trgSegmLenWeight<<" , ";
  outS<<"sjumpw: "<<phrModelInfoPtr->phraseModelPars.srcJumpWeight <<" , ";
  outS<<"sseglenw: "<<phrModelInfoPtr->phraseModelPars.srcSegmLenWeight<<" , ";
  outS<<"ptsw: "<<phrModelInfoPtr->phraseModelPars.ptsWeight <<" , ";
  outS<<"pstw: "<<phrModelInfoPtr->phraseModelPars.pstWeight <<" , ";
  outS<<"swlenliw: "<<swModelInfoPtr->invSwModelPars.lenWeight;
}

//---------------------------------
unsigned int PhrLocalSwLiTm::getNumWeights(void)
{
  return 8;
}


//---------------------------------
void PhrLocalSwLiTm::setOnlineTrainingPars(OnlineTrainingPars _onlineTrainingPars,
                                           int verbose)
{
      // Invoke base class function
  _phrSwTransModel<PhrLocalSwLiTmHypRec<HypEqClassF> >::setOnlineTrainingPars(_onlineTrainingPars,verbose);
    
      // Set R parameter for the direct and the inverse single word models
  _incrSwAligModel<PpInfo>* _incrSwAligModelPtr=
    dynamic_cast<_incrSwAligModel<PpInfo>*>(swModelInfoPtr->swAligModelPtr);

  _incrSwAligModel<PpInfo>* _incrInvSwAligModelPtr=
    dynamic_cast<_incrSwAligModel<PpInfo>*>(swModelInfoPtr->invSwAligModelPtr);

  if(_incrSwAligModelPtr && _incrInvSwAligModelPtr)
  {
    _incrSwAligModelPtr->set_expval_maxnsize(onlineTrainingPars.R_par);
    _incrInvSwAligModelPtr->set_expval_maxnsize(onlineTrainingPars.R_par);
  }
}

//---------------------------------
int PhrLocalSwLiTm::onlineTrainFeatsSentPair(const char *srcSent,
                                             const char *refSent,
                                             const char *sysSent,
                                             int verbose)
{
      // Check if input sentences are empty
  if(strlen(srcSent)==0 || strlen(refSent)==0)
  {
    cerr<<"Error: cannot process empty input sentences"<<endl;
    return ERROR;
  }

      // Train pair according to chosen algorithm
  switch(onlineTrainingPars.onlineLearningAlgorithm)
  {
    case BASIC_INCR_TRAINING:
      return incrTrainFeatsSentPair(srcSent,refSent,verbose);
      break;
    case MINIBATCH_TRAINING:
      return minibatchTrainFeatsSentPair(srcSent,refSent,sysSent,verbose);
      break;
    case BATCH_RETRAINING:
      return batchRetrainFeatsSentPair(srcSent,refSent,verbose);
      break;
    default:
      cerr<<"Warning: requested online learning algoritm with id="<<onlineTrainingPars.onlineLearningAlgorithm<<" is not implemented."<<endl;
      return ERROR;
      break;
  }
}

//---------------------------------
int PhrLocalSwLiTm::incrTrainFeatsSentPair(const char *srcSent,
                                           const char *refSent,
                                           int verbose/*=0*/)
{
  int ret;
  Vector<std::string> srcSentStrVec=StrProcUtils::charItemsToVector(srcSent);
  Vector<std::string> refSentStrVec=StrProcUtils::charItemsToVector(refSent);
  pair<unsigned int,unsigned int> sentRange;

      // Train language model
  if(verbose) cerr<<"Training language model..."<<endl;
  ret=langModelInfoPtr->lModelPtr->trainSentence(refSentStrVec,onlineTrainingPars.learnStepSize,0,verbose);
  if(ret==ERROR) return ERROR;

      // Revise vocabularies of the alignment models
  updateAligModelsSrcVoc(srcSentStrVec);
  updateAligModelsTrgVoc(refSentStrVec);

      // Add sentence pair to the single word models
  swModelInfoPtr->swAligModelPtr->addSentPair(srcSentStrVec,refSentStrVec,onlineTrainingPars.learnStepSize,sentRange);
  swModelInfoPtr->invSwAligModelPtr->addSentPair(refSentStrVec,srcSentStrVec,onlineTrainingPars.learnStepSize,sentRange);

      // Iterate over E_par interlaced samples
  unsigned int curr_sample=sentRange.second;
  unsigned int oldest_sample=curr_sample-onlineTrainingPars.R_par;
  for(unsigned int i=1;i<=onlineTrainingPars.E_par;++i)
  {
    int n=oldest_sample+(i*(onlineTrainingPars.R_par/onlineTrainingPars.E_par));
    if(n>=0)
    {
      if(verbose)
        cerr<<"Alig. model training iteration over sample "<<n<<" ..."<<endl;

          // Train sw model
      if(verbose) cerr<<"Training single-word model..."<<endl;
      swModelInfoPtr->swAligModelPtr->trainSentPairRange(make_pair(n,n),verbose);

          // Train inverse sw model
      if(verbose) cerr<<"Training inverse single-word model..."<<endl;
      swModelInfoPtr->invSwAligModelPtr->trainSentPairRange(make_pair(n,n),verbose);

          // Add new translation options
      if(verbose) cerr<<"Adding new translation options..."<<endl;
      ret=addNewTransOpts(n,verbose);
    }
  }

      // Discard unnecessary phrase-based model sufficient statistics
  int last_n=curr_sample-((onlineTrainingPars.E_par-1)*(onlineTrainingPars.R_par/onlineTrainingPars.E_par));
  if(last_n>=0)
  {
    int mapped_last_n=map_n_am_suff_stats(last_n);
    int idx_to_discard=mapped_last_n;
    if(idx_to_discard>0 && vecVecInvPhPair.size()>(unsigned int)idx_to_discard)
      vecVecInvPhPair[idx_to_discard].clear();
  }

  return ret;
}

//---------------------------------
int PhrLocalSwLiTm::minibatchTrainFeatsSentPair(const char *srcSent,
                                                const char *refSent,
                                                const char *sysSent,
                                                int verbose/*=0*/)
{
  Vector<std::string> srcSentStrVec=StrProcUtils::charItemsToVector(srcSent);
  Vector<std::string> trgSentStrVec=StrProcUtils::charItemsToVector(refSent);
  Vector<std::string> sysSentStrVec=StrProcUtils::charItemsToVector(sysSent);

      // Store source and target sentences
  vecSrcSent.push_back(srcSentStrVec);
  vecTrgSent.push_back(trgSentStrVec);
  vecSysSent.push_back(sysSentStrVec);
  
      // Check if a mini-batch has to be processed
      // (onlineTrainingPars.learnStepSize determines the size of the
      // mini-batch)
  unsigned int minibatchSize=(unsigned int)onlineTrainingPars.learnStepSize;
  if(!vecSrcSent.empty() &&
     (vecSrcSent.size()%minibatchSize)==0)
  {    
    Vector<WordAligMatrix> invWaMatrixVec;
    pair<unsigned int,unsigned int> sentRange;
    float learningRate=calculateNewLearningRate(verbose);
       
    for(unsigned int n=0;n<vecSrcSent.size();++n)
    {
          // Revise vocabularies of the alignment models
      updateAligModelsSrcVoc(vecSrcSent[n]);
      updateAligModelsTrgVoc(vecTrgSent[n]);

          // Add sentence pair to the single word models
      swModelInfoPtr->swAligModelPtr->addSentPair(vecSrcSent[n],vecTrgSent[n],1,sentRange);
      swModelInfoPtr->invSwAligModelPtr->addSentPair(vecTrgSent[n],vecSrcSent[n],1,sentRange);
    }

        // Initialize minibatchSentRange variable
    pair<unsigned int,unsigned int> minibatchSentRange;
    minibatchSentRange.first=sentRange.second-minibatchSize+1;
    minibatchSentRange.second=sentRange.second;

    if(verbose)
      cerr<<"Processing mini-batch of size "<<minibatchSize<<" , "<<minibatchSentRange.first<<" - "<<minibatchSentRange.second<<endl;

        // Set learning rate for sw model if possible
    BaseStepwiseAligModel* bswamPtr=dynamic_cast<BaseStepwiseAligModel*>(swModelInfoPtr->swAligModelPtr);
    if(bswamPtr) bswamPtr->set_nu_val(learningRate);

        // Train sw model
    if(verbose) cerr<<"Training single-word model..."<<endl;
    for(unsigned int i=0;i<onlineTrainingPars.emIters;++i)
    {
      swModelInfoPtr->swAligModelPtr->trainSentPairRange(minibatchSentRange,verbose);
    }

        // Set learning rate for inverse sw model if possible
    BaseStepwiseAligModel* ibswamPtr=dynamic_cast<BaseStepwiseAligModel*>(swModelInfoPtr->invSwAligModelPtr);
    if(ibswamPtr) ibswamPtr->set_nu_val(learningRate);

        // Train inverse sw model
    if(verbose) cerr<<"Training inverse single-word model..."<<endl;
    for(unsigned int i=0;i<onlineTrainingPars.emIters;++i)
    {
      swModelInfoPtr->invSwAligModelPtr->trainSentPairRange(minibatchSentRange,verbose);
    }

        // Generate word alignments
    if(verbose) cerr<<"Generating word alignments..."<<endl;
    for(unsigned int n=0;n<vecSrcSent.size();++n)
    {
          // Generate alignments
      WordAligMatrix waMatrix;
      WordAligMatrix invWaMatrix;
  
      swModelInfoPtr->swAligModelPtr->obtainBestAlignmentVecStr(vecSrcSent[n],vecTrgSent[n],waMatrix);
      swModelInfoPtr->invSwAligModelPtr->obtainBestAlignmentVecStr(vecTrgSent[n],vecSrcSent[n],invWaMatrix);
  
          // Operate alignments
      Vector<std::string> nrefSentStrVec=swModelInfoPtr->swAligModelPtr->addNullWordToStrVec(vecTrgSent[n]);  

      waMatrix.transpose();

          // Execute symmetrization
      invWaMatrix.symmetr1(waMatrix);
      if(verbose)
      {
        printAlignmentInGIZAFormat(cerr,nrefSentStrVec,vecSrcSent[n],invWaMatrix,"Operated word alignment for phrase model training:");
      }

          // Store word alignment matrix
      invWaMatrixVec.push_back(invWaMatrix);
    }

        // Train phrase-based model
    _wbaIncrPhraseModel* wbaIncrPhraseModelPtr=dynamic_cast<_wbaIncrPhraseModel* >(phrModelInfoPtr->invPbModelPtr);
    if(wbaIncrPhraseModelPtr)
    {
      if(verbose) cerr<<"Training phrase-based model..."<<endl;
      PhraseExtractParameters phePars;
      wbaIncrPhraseModelPtr->extModelFromPairAligVec(phePars,false,vecTrgSent,vecSrcSent,invWaMatrixVec,(Count)learningRate,verbose);
    }
    
        // Train language model
    if(verbose) cerr<<"Training language model..."<<endl;    
    langModelInfoPtr->lModelPtr->trainSentenceVec(vecTrgSent,(Count)learningRate,(Count)0,verbose);

        // Clear vectors with source and target sentences
    vecSrcSent.clear();
    vecTrgSent.clear();
    vecSysSent.clear();

        // Increase stepNum
    ++stepNum;
  }
  
  return OK;
}

//---------------------------------
int PhrLocalSwLiTm::batchRetrainFeatsSentPair(const char *srcSent,
                                              const char *refSent,
                                              int verbose/*=0*/)
{
  Vector<std::string> srcSentStrVec=StrProcUtils::charItemsToVector(srcSent);
  Vector<std::string> trgSentStrVec=StrProcUtils::charItemsToVector(refSent);

      // Store source and target sentences
  vecSrcSent.push_back(srcSentStrVec);
  vecTrgSent.push_back(trgSentStrVec);
  
      // Check if a batch has to be processed
      // (onlineTrainingPars.learnStepSize determines the number of samples
      // that are to be seen before retraining)
  unsigned int batchSize=(unsigned int)onlineTrainingPars.learnStepSize;
  if(!vecSrcSent.empty() &&
     (vecSrcSent.size()%batchSize)==0)
  {    
    Vector<WordAligMatrix> invWaMatrixVec;
    pair<unsigned int,unsigned int> sentRange;
    float learningRate=1;
      
        // Batch learning is being performed, clear models
    if(verbose) cerr<<"Clearing previous model..."<<endl;
    swModelInfoPtr->swAligModelPtr->clear();
    swModelInfoPtr->invSwAligModelPtr->clear();
    phrModelInfoPtr->invPbModelPtr->clear();
    langModelInfoPtr->lModelPtr->clear();

    for(unsigned int n=0;n<vecSrcSent.size();++n)
    {
          // Revise vocabularies of the alignment models
      updateAligModelsSrcVoc(vecSrcSent[n]);
      updateAligModelsTrgVoc(vecTrgSent[n]);

          // Add sentence pair to the single word models
      swModelInfoPtr->swAligModelPtr->addSentPair(vecSrcSent[n],vecTrgSent[n],1,sentRange);
      swModelInfoPtr->invSwAligModelPtr->addSentPair(vecTrgSent[n],vecSrcSent[n],1,sentRange);
    }

        // Initialize batchSentRange variable
    pair<unsigned int,unsigned int> batchSentRange;
    batchSentRange.first=0;
    batchSentRange.second=sentRange.second;

    if(verbose)
      cerr<<"Processing batch of size "<<batchSentRange.second-batchSentRange.first+1<<" , "<<batchSentRange.first<<" - "<<batchSentRange.second<<endl;

        // Set learning rate for sw model if possible
    BaseStepwiseAligModel* bswamPtr=dynamic_cast<BaseStepwiseAligModel*>(swModelInfoPtr->swAligModelPtr);
    if(bswamPtr) bswamPtr->set_nu_val(learningRate);

        // Train sw model
    if(verbose) cerr<<"Training single-word model..."<<endl;
    for(unsigned int i=0;i<onlineTrainingPars.emIters;++i)
    {
          // Execute batch training
      _incrSwAligModel<PpInfo>* iswamPtr=
        dynamic_cast<_incrSwAligModel<PpInfo>*>(swModelInfoPtr->swAligModelPtr);

      if(iswamPtr) iswamPtr->efficientBatchTrainingForRange(batchSentRange,verbose);
    }

        // Set learning rate for inverse sw model if possible
    BaseStepwiseAligModel* ibswamPtr=dynamic_cast<BaseStepwiseAligModel*>(swModelInfoPtr->invSwAligModelPtr);
    if(ibswamPtr) ibswamPtr->set_nu_val(learningRate);

        // Train inverse sw model
    if(verbose) cerr<<"Training inverse single-word model..."<<endl;
    for(unsigned int i=0;i<onlineTrainingPars.emIters;++i)
    {
          // Execute batch training
      _incrSwAligModel<PpInfo>* iswamPtr=dynamic_cast<_incrSwAligModel<PpInfo>*>(swModelInfoPtr->invSwAligModelPtr);
      if(iswamPtr) iswamPtr->efficientBatchTrainingForRange(batchSentRange,verbose);
    }

        // Generate word alignments
    if(verbose) cerr<<"Generating word alignments..."<<endl;
    for(unsigned int n=0;n<vecSrcSent.size();++n)
    {
          // Generate alignments
      WordAligMatrix waMatrix;
      WordAligMatrix invWaMatrix;
  
      swModelInfoPtr->swAligModelPtr->obtainBestAlignmentVecStr(vecSrcSent[n],vecTrgSent[n],waMatrix);
      swModelInfoPtr->invSwAligModelPtr->obtainBestAlignmentVecStr(vecTrgSent[n],vecSrcSent[n],invWaMatrix);
  
          // Operate alignments
      Vector<std::string> nrefSentStrVec=swModelInfoPtr->swAligModelPtr->addNullWordToStrVec(vecTrgSent[n]);  

      waMatrix.transpose();

          // Execute symmetrization
      invWaMatrix.symmetr1(waMatrix);
      if(verbose)
      {
        printAlignmentInGIZAFormat(cerr,nrefSentStrVec,vecSrcSent[n],invWaMatrix,"Operated word alignment for phrase model training:");
      }

          // Store word alignment matrix
      invWaMatrixVec.push_back(invWaMatrix);
    }

        // Train phrase-based model
    _wbaIncrPhraseModel* wbaIncrPhraseModelPtr=dynamic_cast<_wbaIncrPhraseModel* >(phrModelInfoPtr->invPbModelPtr);
    if(wbaIncrPhraseModelPtr)
    {
      if(verbose) cerr<<"Training phrase-based model..."<<endl;
      PhraseExtractParameters phePars;
      wbaIncrPhraseModelPtr->extModelFromPairAligVec(phePars,false,vecTrgSent,vecSrcSent,invWaMatrixVec,(Count)learningRate,verbose);
    }
        // Train language model
    if(verbose) cerr<<"Training language model..."<<endl;    
    langModelInfoPtr->lModelPtr->trainSentenceVec(vecTrgSent,(Count)learningRate,(Count)0,verbose);
  }
  
  return OK;
}

//---------------------------------
float PhrLocalSwLiTm::calculateNewLearningRate(int verbose/*=0*/)

{
  if(verbose) cerr<<"Calculating new learning rate..."<<endl;
                
  float lr;
  
  switch(onlineTrainingPars.learningRatePolicy)
  {
    float alpha;
    float par1;
    float par2;
    case FIXED_LEARNING_RATE_POL:
      if(verbose) cerr<<"Using fixed learning rate."<<endl;
      lr=PHRSWLITM_DEFAULT_LR;
      break;
    case LIANG_LEARNING_RATE_POL:
      if(verbose) cerr<<"Using Liang learning rate."<<endl;
      alpha=PHRSWLITM_DEFAULT_LR_ALPHA_PAR;
      lr=1.0/(float)pow((float)stepNum+2,(float)alpha);
      break;
    case OWN_LEARNING_RATE_POL:
      if(verbose) cerr<<"Using own learning rate."<<endl;
      par1=PHRSWLITM_DEFAULT_LR_PAR1;
      par2=PHRSWLITM_DEFAULT_LR_PAR2;
      lr=par1/(1.0+((float)stepNum/par2));
      break;
    case WER_BASED_LEARNING_RATE_POL:
      if(verbose) cerr<<"Using WER-based learning rate."<<endl;
      lr=werBasedLearningRate(verbose);
      break;
    default:
      lr=PHRSWLITM_DEFAULT_LR;
      break;
  }

  if(verbose)
    cerr<<"New learning rate: "<<lr<<endl;

  if(lr>=1) cerr<<"WARNING: learning rate greater or equal than 1.0!"<<endl;
  
  return lr;
}

//---------------------------------
float PhrLocalSwLiTm::werBasedLearningRate(int verbose/*=0*/)
{
  EditDistForVec<std::string> edDistVecStr;
  unsigned int hCount,iCount,sCount,dCount;
  unsigned int totalOps=0;
  unsigned int totalTrgWords=0;
  float wer;
  float lr;
   
      // Set error model
  edDistVecStr.setErrorModel(0,1,1,1);

  for(unsigned int n=0;n<vecTrgSent.size();++n)
  {
    double dist=edDistVecStr.calculateEditDistOps(vecTrgSent[n],vecSysSent[n],hCount,iCount,sCount,dCount,0);
    unsigned int ops=(unsigned int) dist;
    unsigned int trgWords=vecTrgSent[n].size();
    totalOps+=ops;
    totalTrgWords+=trgWords;
    if(verbose)
    {
      cerr<<"Sentence pair "<<n;
      cerr<<" ; PARTIAL WER= "<<(float)ops/trgWords<<" ( "<<ops<<" , "<<trgWords<<" )";
      cerr<<" ; ACUM WER= "<<(float)totalOps/totalTrgWords<<" ( "<<totalOps<<" , "<<totalTrgWords<<" )"<<endl;
    }
  }

      // Obtain WER for block of sentences
  if(totalTrgWords>0) wer=(float)totalOps/totalTrgWords;
  else wer=0;

      // Obtain learning rate
  lr=wer-PHRSWLITM_LR_RESID_WER;
  if(lr>0.999) lr=0.999;
  if(lr<0.001) lr=0.001;
   
  if(verbose)
    cerr<<"WER of block: "<<wer<<endl;
   
  return lr;
}

//---------------------------------
unsigned int PhrLocalSwLiTm::map_n_am_suff_stats(unsigned int n)
{
  return n;
}

//---------------------------------
int PhrLocalSwLiTm::addNewTransOpts(unsigned int n,
                                    int verbose/*=0*/)
{
// NOTE: a complete training step requires the addition of new
// translation options. This can be achieved using the well-known
// phrase-extract algorithm. The required functionality is only
// implemented at this moment by the pb models deriving from the
// _wbaIncrPhraseModel class

  _wbaIncrPhraseModel* wbaIncrPhraseModelPtr=dynamic_cast<_wbaIncrPhraseModel* >(phrModelInfoPtr->invPbModelPtr);
  if(wbaIncrPhraseModelPtr)
  {
        // Obtain sentence pair
    Vector<std::string> srcSentStrVec;
    Vector<std::string> refSentStrVec;
    Count c;
    swModelInfoPtr->swAligModelPtr->nthSentPair(n,srcSentStrVec,refSentStrVec,c);

        // Extract consistent phrase pairs
    Vector<PhrasePair> vecInvPhPair;
    extractConsistentPhrasePairs(srcSentStrVec,refSentStrVec,vecInvPhPair,verbose);

        // Obtain mapped_n
    unsigned int mapped_n=map_n_am_suff_stats(n);
  
        // Grow vecVecInvPhPair if necessary
    Vector<PhrasePair> vpp;
    while(vecVecInvPhPair.size()<=mapped_n) vecVecInvPhPair.push_back(vpp);
    
        // Subtract current phrase model sufficient statistics
    for(unsigned int i=0;i<vecVecInvPhPair[mapped_n].size();++i)
    {
      wbaIncrPhraseModelPtr->strIncrCountsOfEntry(vecVecInvPhPair[mapped_n][i].s_,
                                                  vecVecInvPhPair[mapped_n][i].t_,
                                                  -1);
    }

        // Add new phrase model current sufficient statistics
    if(verbose) cerr<<"List of extracted consistent phrase pairs:"<<endl;
    for(unsigned int i=0;i<vecInvPhPair.size();++i)
    {
      wbaIncrPhraseModelPtr->strIncrCountsOfEntry(vecInvPhPair[i].s_,
                                                  vecInvPhPair[i].t_,
                                                  1);
      if(verbose)
      {
        for(unsigned int j=0;j<vecInvPhPair[i].s_.size();++j) cerr<<vecInvPhPair[i].s_[j]<<" ";
        cerr<<"|||";
        for(unsigned int j=0;j<vecInvPhPair[i].t_.size();++j) cerr<<" "<<vecInvPhPair[i].t_[j];
        cerr<<endl;
      }
    }
  
        // Store new phrase model current sufficient statistics
    vecVecInvPhPair[mapped_n]=vecInvPhPair;

    return OK;
  }
  else
  {
    cerr<<"Warning: addition of new translation options not supported in this configuration!"<<endl;
    return ERROR;
  }
}

//---------------------------------
bool PhrLocalSwLiTm::load_lambdas(const char* lambdaFileName)
{
  awkInputStream awk;
  
  if(awk.open(lambdaFileName)==ERROR)
  {
    cerr<<"Error in file containing the lambda value, file "<<lambdaFileName<<" does not exist. Current values-> lambda_swm="<<swModelInfoPtr->lambda_swm<<" , lambda_invswm"<<swModelInfoPtr->lambda_invswm<<endl;
    return OK;
  }
  else
  {
    if(awk.getln())
    {
      if(awk.NF==1)
      {
        swModelInfoPtr->lambda_swm=atof(awk.dollar(1).c_str());
        swModelInfoPtr->lambda_invswm=atof(awk.dollar(1).c_str());
        cerr<<"Read lambda value from file: "<<lambdaFileName<<" (lambda_swm="<<swModelInfoPtr->lambda_swm<<", lambda_invswm="<<swModelInfoPtr->lambda_invswm<<")"<<endl;
        return OK;
      }
      else
      {
        if(awk.NF==2)
        {
          swModelInfoPtr->lambda_swm=atof(awk.dollar(1).c_str());
          swModelInfoPtr->lambda_invswm=atof(awk.dollar(2).c_str());
          cerr<<"Read lambda value from file: "<<lambdaFileName<<" (lambda_swm="<<swModelInfoPtr->lambda_swm<<", lambda_invswm="<<swModelInfoPtr->lambda_invswm<<")"<<endl;
          return OK;
        }
        else
        {
          cerr<<"Anomalous file with lambda values."<<endl;
          return ERROR;
        }
      }
    }
    else
    {
      cerr<<"Anomalous file with lambda values."<<endl;
      return ERROR;
    }
  }  
  return OK;
}

//---------------------------------
bool PhrLocalSwLiTm::print_lambdas(const char* lambdaFileName)
{
  ofstream outF;

  outF.open(lambdaFileName,ios::out);
  if(!outF)
  {
    cerr<<"Error while printing file with lambda values."<<endl;
    return ERROR;
  }
  else
  {
    print_lambdas(outF);
    outF.close();	
    return OK;
  }   
}

//-------------------------
ostream& PhrLocalSwLiTm::print_lambdas(ostream &outS)
{
  outS<<swModelInfoPtr->lambda_swm<<" "<<swModelInfoPtr->lambda_invswm<<endl;
  return outS;
}

//---------------------------------
unsigned int
PhrLocalSwLiTm::numberOfUncoveredSrcWordsHypData(const HypDataType& hypd)const
{
  unsigned int k,n;

  n=0;
  for(k=0;k<hypd.sourceSegmentation.size();k++)
	n+=hypd.sourceSegmentation[k].second-hypd.sourceSegmentation[k].first+1; 

  return (pbtmInputVars.srcSentVec.size()-n);  
}

//---------------------------------
Score PhrLocalSwLiTm::incrScore(const Hypothesis& pred_hyp,
                                const HypDataType& new_hypd,
                                Hypothesis& new_hyp,
                                Vector<Score>& scoreComponents)
{
  HypScoreInfo hypScoreInfo=pred_hyp.getScoreInfo();
  HypDataType pred_hypd=pred_hyp.getData();
  unsigned int trglen=pred_hypd.ntarget.size()-1;
  Bitset<MAX_SENTENCE_LENGTH_ALLOWED> hypKey=pred_hyp.getKey();
    
      // Init scoreComponents
  scoreComponents.clear();
  for(unsigned int i=0;i<getNumWeights();++i) scoreComponents.push_back(0);

#ifdef THOT_DEBUG
  new_hyp.hDebug=pred_hyp.hDebug;
  Vector<Score> prev_scoreComponents=scoreComponents;
#endif
  
  for(unsigned int i=pred_hypd.sourceSegmentation.size();i<new_hypd.sourceSegmentation.size();++i)
  {
        // Source segment is not present in the previous data
    unsigned int srcLeft=new_hypd.sourceSegmentation[i].first;
    unsigned int srcRight=new_hypd.sourceSegmentation[i].second;
    unsigned int trgLeft;
    unsigned int trgRight;
    Vector<WordIndex> trgphrase;
    Vector<WordIndex> s_;
      
    trgRight=new_hypd.targetSegmentCuts[i];
    if(i==0) trgLeft=1;
    else trgLeft=new_hypd.targetSegmentCuts[i-1]+1;
    for(unsigned int k=trgLeft;k<=trgRight;++k)
    {
      trgphrase.push_back(new_hypd.ntarget[k]);
    }
        // Calculate new sum word penalty score
    scoreComponents[WPEN]-=sumWordPenaltyScore(trglen);
    scoreComponents[WPEN]+=sumWordPenaltyScore(trglen+trgphrase.size());

        // Obtain language model score
    scoreComponents[LMODEL]+=getNgramScoreGivenState(trgphrase,hypScoreInfo.lmHist);

        // target segment length score
    scoreComponents[TSEGMLEN]+=this->trgSegmLenScore(trglen+trgphrase.size(),trglen,0);

        // phrase alignment score      
    int lastSrcPosStart=srcLeft;
    int prevSrcPosEnd;
    if(i>0) prevSrcPosEnd=new_hypd.sourceSegmentation[i-1].second;
    else prevSrcPosEnd=0;
    scoreComponents[SJUMP]+=this->srcJumpScore(abs(lastSrcPosStart-(prevSrcPosEnd+1))); 

        // source segment length score
    scoreComponents[SSEGMLEN]+=srcSegmLenScore(i,new_hypd.sourceSegmentation,this->pbtmInputVars.srcSentVec.size(),trgphrase.size());

        // Obtain translation score
    for(unsigned int k=srcLeft;k<=srcRight;++k)
    {
      s_.push_back(pbtmInputVars.nsrcSentIdVec[k]);
    }
        // p(s_|t_) smoothed phrase score
    scoreComponents[PST]+=smoothedPhrScore_s_t_(s_,trgphrase);

        // p(t_|s_) smoothed phrase score
    scoreComponents[PTS]+=smoothedPhrScore_t_s_(s_,trgphrase);

        // Calculate sentence length model contribution
    scoreComponents[SWLENLI]-=sentLenScoreForPartialHyp(hypKey,trglen);
    for(unsigned int j=srcLeft;j<=srcRight;++j)
      hypKey.set(j);
    scoreComponents[SWLENLI]+=sentLenScoreForPartialHyp(hypKey,trglen+trgphrase.size());

        // Increase trglen
    trglen+=trgphrase.size();

#ifdef THOT_DEBUG
    HypDebugData hdData;

    hdData.opCode="extend";
    hdData.parameters.push_back(srcLeft);
    hdData.parameters.push_back(srcRight);
    for(unsigned int k=0;k<trgphrase.size();++k)
    {
      hdData.parameters.push_back(trgphrase[k]);
    }
    hdData.accum=0;
    for(unsigned int k=0;k<scoreComponents.size();++k)
    {
      hdData.partialContribs.push_back(scoreComponents[k]-prev_scoreComponents[k]);
      hdData.accum+=scoreComponents[k]-prev_scoreComponents[k];
    }
    new_hyp.hDebug.push_back(hdData);
    prev_scoreComponents=scoreComponents;
#endif
  }
  if(numberOfUncoveredSrcWordsHypData(new_hypd)==0 &&
     numberOfUncoveredSrcWordsHypData(pred_hypd)!=0)
  {
        // Calculate word penalty score
    scoreComponents[WPEN]-=sumWordPenaltyScore(trglen);
    scoreComponents[WPEN]+=wordPenaltyScore(trglen);

        // End of sentence score
    scoreComponents[LMODEL]+=getScoreEndGivenState(hypScoreInfo.lmHist);

        // Calculate sentence length score
    scoreComponents[SWLENLI]-=sentLenScoreForPartialHyp(hypKey,trglen);
    scoreComponents[SWLENLI]+=sentLenScore(pbtmInputVars.srcSentVec.size(),trglen);

#ifdef THOT_DEBUG
    HypDebugData hdData;
      
    hdData.opCode="close";
    hdData.accum=0;
    for(unsigned int k=0;k<scoreComponents.size();++k)
    {
      hdData.partialContribs.push_back(scoreComponents[k]-prev_scoreComponents[k]);
      hdData.accum+=scoreComponents[k]-prev_scoreComponents[k];
    }
    new_hyp.hDebug.push_back(hdData);
#endif
  }

      // Accumulate the score stored in scoreComponents
  for(unsigned int i=0;i<scoreComponents.size();++i)
    hypScoreInfo.score +=scoreComponents[i];

  new_hyp.setScoreInfo(hypScoreInfo);
  new_hyp.setData(new_hypd);
  
  return hypScoreInfo.score;
}

//---------------------------------------
Score PhrLocalSwLiTm::smoothedPhrScore_s_t_(const Vector<WordIndex>& s_,
                                            const Vector<WordIndex>& t_)
{
  if(phrModelInfoPtr->phraseModelPars.pstWeight!=0)
  {
    if(swModelInfoPtr->lambda_invswm==1.0)
    {
      return phrModelInfoPtr->phraseModelPars.pstWeight*(double)phrModelInfoPtr->invPbModelPtr->logpt_s_(t_,s_);
    }
    else
    {
      float sum1=log(swModelInfoPtr->lambda_invswm)+(float)phrModelInfoPtr->invPbModelPtr->logpt_s_(t_,s_);
// #if THOT_PBM_TYPE == ML_PBM
          // Avoid those cases in which the phrase model
          // smoothing probability is greater than the one given by the
          // lexical distribution
      if(sum1<=log(PHRASE_PROB_SMOOTH))
        sum1=PHRSWLITM_LGPROB_SMOOTH;
// #endif
      float sum2=log(1.0-swModelInfoPtr->lambda_invswm)+(float)invSwLgProb(s_,t_);
      float interp=MathFuncs::lns_sumlog(sum1,sum2);
      // cerr<<" ( "<<phrModelInfoPtr->invPbModelPtr->logpt_s_(t_,s_)<<" "<<invSwLgProb(s_,t_)<<" "<<interp<<" ) ";
      return phrModelInfoPtr->phraseModelPars.pstWeight*(double)interp;
    }
  }
  else return 0;
}

//---------------------------------------
Score PhrLocalSwLiTm::smoothedPhrScore_t_s_(const Vector<WordIndex>& s_,
                                            const Vector<WordIndex>& t_)
{
  if(phrModelInfoPtr->phraseModelPars.ptsWeight!=0)
  {
    if(swModelInfoPtr->lambda_swm==1.0)
    {
      return phrModelInfoPtr->phraseModelPars.ptsWeight*(double)phrModelInfoPtr->invPbModelPtr->logps_t_(t_,s_);
    }
    else
    {
      float sum1=log(swModelInfoPtr->lambda_swm)+(float)phrModelInfoPtr->invPbModelPtr->logps_t_(t_,s_);
// #if THOT_PBM_TYPE == ML_PBM
          // Avoid those cases in which the phrase model
          // smoothing probability is greater than the one given by the
          // lexical distribution
      if(sum1<=log(PHRASE_PROB_SMOOTH))
        sum1=PHRSWLITM_LGPROB_SMOOTH;
// #endif
      float sum2=log(1.0-swModelInfoPtr->lambda_swm)+(float)swLgProb(s_,t_);
      float interp=MathFuncs::lns_sumlog(sum1,sum2);
      // cerr<<" ( "<<phrModelInfoPtr->invPbModelPtr->logps_t_(t_,s_)<<" "<<swLgProb(s_,t_)<<" "<<interp<<" ) ";

      return phrModelInfoPtr->phraseModelPars.ptsWeight*(double)interp;
    }
  }
  else return 0;
}

//---------------------------------------
Score PhrLocalSwLiTm::nbestTransScore(const Vector<WordIndex>& s_,
                                      const Vector<WordIndex>& t_)
{
  Score score=0;

      // word penalty contribution
  score+=wordPenaltyScore(t_.size());

      // Language model contribution
  score+=nbestLmScoringFunc(t_);

      // Phrase model contribution
  score+=smoothedPhrScore_t_s_(s_,t_);
  score+=smoothedPhrScore_s_t_(s_,t_);

  return score;
}

//---------------------------------------
Score PhrLocalSwLiTm::nbestTransScoreLast(const Vector<WordIndex>& s_,
                                          const Vector<WordIndex>& t_)
{
  return nbestTransScore(s_,t_);
}

//---------------------------------
void PhrLocalSwLiTm::extendHypDataIdx(PositionIndex srcLeft,
                                      PositionIndex srcRight,
                                      const Vector<WordIndex>& trgPhraseIdx,
                                      HypDataType& hypd)
{
  pair<PositionIndex,PositionIndex> sourceSegm;
  
        // Add trgPhraseIdx to the target vector
  for(unsigned int i=0;i<trgPhraseIdx.size();++i)
  {
    hypd.ntarget.push_back(trgPhraseIdx[i]);
  }
  
      // Add source segment and target cut
  sourceSegm.first=srcLeft;
  sourceSegm.second=srcRight;
  hypd.sourceSegmentation.push_back(sourceSegm);
  
  hypd.targetSegmentCuts.push_back(hypd.ntarget.size()-1);
}

//---------------------------------
PositionIndex PhrLocalSwLiTm::getLastSrcPosCoveredHypData(const HypDataType& hypd)
{
  SourceSegmentation sourceSegmentation;

  sourceSegmentation=hypd.sourceSegmentation;
  if(sourceSegmentation.size()>0)
    return sourceSegmentation.back().second;
  else return 0;
}

//---------------------------------
bool PhrLocalSwLiTm::hypDataTransIsPrefixOfTargetRef(const HypDataType& hypd,
                                                     bool& equal)const
{
  PositionIndex ntrgSize,nrefSentSize;
  
  ntrgSize=hypd.ntarget.size();
  nrefSentSize=pbtmInputVars.nrefSentIdVec.size();	
	
  if(ntrgSize>nrefSentSize) return false;
  for(PositionIndex i=1;i<ntrgSize;++i)
  {
    if(pbtmInputVars.nrefSentIdVec[i]!=hypd.ntarget[i]) return false;
  }
  if(ntrgSize==nrefSentSize) equal=true;
  else equal=false;

  return true;
}

//---------------------------------
PhrLocalSwLiTm::~PhrLocalSwLiTm()
{
  
}
