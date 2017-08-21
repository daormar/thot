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
/* Module: IncrIbm1AligModel                                        */
/*                                                                  */
/* Definitions file: IncrIbm1AligModel.cc                           */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "IncrIbm1AligModel.h"

//--------------- Classes --------------------------------------------


//--------------- IncrIbm1AligModel class function definitions

//-------------------------
IncrIbm1AligModel::IncrIbm1AligModel()
{
      // Link pointers with sentence length model
  sentLengthModel.linkVocabPtr(&swVocab);
  sentLengthModel.linkSentPairInfo(&sentenceHandler);
}

//-------------------------   
void IncrIbm1AligModel::set_expval_maxnsize(unsigned int _anji_maxnsize)
{
  anji.set_maxnsize(_anji_maxnsize);
}

//-------------------------   
unsigned int IncrIbm1AligModel::numSentPairs(void)
{
  return sentenceHandler.numSentPairs();
}

//-------------------------
void IncrIbm1AligModel::trainSentPairRange(pair<unsigned int,unsigned int> sentPairRange,
                                           int verbosity)
{
      // Train sentence length model
  sentLengthModel.trainSentPairRange(sentPairRange,verbosity);
      
      // EM algorithm
  calcNewLocalSuffStats(sentPairRange,verbosity);
  updatePars();
}

//-------------------------   
void IncrIbm1AligModel::trainAllSents(int verbosity)
{
  trainSentPairRange(make_pair(0,numSentPairs()-1),verbosity);
}

//-------------------------
void IncrIbm1AligModel::efficientBatchTrainingForRange(pair<unsigned int,unsigned int> sentPairRange,
                                                       int verbosity/*=0*/)
{
      // Set maximum size of the matrices of expected values to zero
  set_expval_maxnsize(0);

      // Train sentence length model
  sentLengthModel.trainSentPairRange(sentPairRange,verbosity);

      // EM algorithm
  calcNewLocalSuffStats(sentPairRange,verbosity);
  incrLexTable.clear();
  updatePars();
}

//-------------------------
pair<double,double> IncrIbm1AligModel::loglikelihoodForPairRange(pair<unsigned int,unsigned int> sentPairRange,
                                                                 int verbosity/*=0*/)
{
  double loglikelihood=0;
  unsigned int numSents=0;
  
  for(unsigned int n=sentPairRange.first;n<=sentPairRange.second;++n)
  {
    if(verbosity) cerr<<"* Calculating log-likelihood for sentence "<<n<<endl;
        // Add log-likelihood
    Vector<WordIndex> nthSrcSent=getSrcSent(n);
    Vector<WordIndex> nthTrgSent=getTrgSent(n);
    if(sentenceLengthIsOk(nthSrcSent) && sentenceLengthIsOk(nthTrgSent))
    {
      loglikelihood+=(double)calcLgProb(nthSrcSent,nthTrgSent,verbosity);
      ++numSents;
    }
  }
  return make_pair(loglikelihood,loglikelihood/(double) numSents);
}

//-------------------------
void IncrIbm1AligModel::clearInfoAboutSentRange(void)
{
      // Clear info about sentence range
  sentenceHandler.clear();
  anji.clear();
  anji_aux.clear();
}

//-------------------------
Vector<WordIndex> IncrIbm1AligModel::getSrcSent(unsigned int n)
{
  Vector<std::string> srcsStr;
  Vector<WordIndex> result;

  sentenceHandler.getSrcSent(n,srcsStr);
  for(unsigned int i=0;i<srcsStr.size();++i)
  {
    WordIndex widx=stringToSrcWordIndex(srcsStr[i]);
    if(widx==UNK_WORD)
      widx=addSrcSymbol(srcsStr[i]);
    result.push_back(widx);
  }
  return result;
}

//-------------------------
Vector<WordIndex> IncrIbm1AligModel::extendWithNullWord(const Vector<WordIndex>& srcWordIndexVec)
{
  return addNullWordToWidxVec(srcWordIndexVec);
}

//-------------------------
Vector<WordIndex> IncrIbm1AligModel::getTrgSent(unsigned int n)
{
  Vector<std::string> trgsStr;
  Vector<WordIndex> trgs;

  sentenceHandler.getTrgSent(n,trgsStr);
  for(unsigned int i=0;i<trgsStr.size();++i)
  {
    WordIndex widx=stringToTrgWordIndex(trgsStr[i]);
    if(widx==UNK_WORD)
      widx=addTrgSymbol(trgsStr[i]);
    trgs.push_back(widx);
  }
  return trgs;
}

//-------------------------
bool IncrIbm1AligModel::sentenceLengthIsOk(const Vector<WordIndex> sentence)
{
  if(sentence.empty() || sentence.size()>IBM_SWM_MAX_SENT_LENGTH)
    return false;
  else
    return true;
}

//-------------------------   
void IncrIbm1AligModel::calcNewLocalSuffStats(pair<unsigned int,unsigned int> sentPairRange,
                                              int verbosity)
{
      // Iterate over the training samples
  for(unsigned int n=sentPairRange.first;n<=sentPairRange.second;++n)
  {
        // Calculate sufficient statistics

        // Init vars for n'th sample
    Vector<WordIndex> srcSent=getSrcSent(n);
    Vector<WordIndex> nsrcSent=extendWithNullWord(srcSent);
    Vector<WordIndex> trgSent=getTrgSent(n);

    Count weight;
    sentenceHandler.getCount(n,weight);

        // Process sentence pair only if both sentences are not empty
    if(sentenceLengthIsOk(srcSent) && sentenceLengthIsOk(trgSent))
    {
          // Calculate sufficient statistics for anji values
      calc_anji(n,nsrcSent,trgSent,weight);
    }
    else
    {
      if(verbosity)
      {
        cerr<<"Warning, training pair "<<n+1<<" discarded due to sentence length (slen: "<<srcSent.size()<<" , tlen: "<<trgSent.size()<<")"<<endl;
      }
    }
  }
}

//-------------------------   
void IncrIbm1AligModel::calc_anji(unsigned int n,
                                  const Vector<WordIndex>& nsrcSent,
                                  const Vector<WordIndex>& trgSent,
                                  const Count& weight)
{
      // Initialize anji and anji_aux
  unsigned int mapped_n;
  anji.init_nth_entry(n,nsrcSent.size(),trgSent.size(),mapped_n);
    
  unsigned int n_aux=1;
  unsigned int mapped_n_aux;
  anji_aux.init_nth_entry(n_aux,nsrcSent.size(),trgSent.size(),mapped_n_aux);

      // Calculate new estimation of anji
  for(unsigned int j=1;j<=trgSent.size();++j)
  {
        // Obtain sum_anji_num_forall_s
    double sum_anji_num_forall_s=0;
    Vector<double> numVec;
    for(unsigned int i=0;i<nsrcSent.size();++i)
    {
          // Smooth numerator
      double d=calc_anji_num(nsrcSent,trgSent,i,j);
      if(d<SMOOTHING_ANJI_NUM) d=SMOOTHING_ANJI_NUM;
          // Add contribution to sum
      sum_anji_num_forall_s+=d;
          // Store num in numVec
      numVec.push_back(d);
    }
        // Set value of anji_aux
    for(unsigned int i=0;i<nsrcSent.size();++i)
    {
      anji_aux.set_fast(mapped_n_aux,j,i,numVec[i]/sum_anji_num_forall_s);
    }
  }

      // Gather sufficient statistics
  if(anji_aux.n_size()!=0)
  {
    for(unsigned int j=1;j<=trgSent.size();++j)
    {
      for(unsigned int i=0;i<nsrcSent.size();++i)
      {
            // Fill variables for n_aux,j,i
        fillEmAuxVars(mapped_n,mapped_n_aux,i,j,nsrcSent,trgSent,weight);

            // Update anji
        anji.set_fast(mapped_n,j,i,anji_aux.get_invp(n_aux,j,i));
      }
    }
        // clear anji_aux data structure
    anji_aux.clear();
  }
}

//-------------------------   
double IncrIbm1AligModel::calc_anji_num(const Vector<WordIndex>& nsrcSent,
                                        const Vector<WordIndex>& trgSent,
                                        unsigned int i,
                                        unsigned int j)
{
  bool found;
  WordIndex s=nsrcSent[i];
  WordIndex t=trgSent[j-1];
  
  incrLexTable.getLexNumer(s,t,found);
  if(found)
  {
        // s,t has previously been seen
    return unsmoothed_pts(s,t);    
  }
  else
  {
        // s,t has never been seen
    return ARBITRARY_PTS;
  }
}

//-------------------------   
void IncrIbm1AligModel::fillEmAuxVars(unsigned int mapped_n,
                                      unsigned int mapped_n_aux,
                                      PositionIndex i,
                                      PositionIndex j,
                                      const Vector<WordIndex>& nsrcSent,
                                      const Vector<WordIndex>& trgSent,
                                      const Count& weight)
{
      // Init vars
  float weighted_curr_anji=0;
  float curr_anji=anji.get_fast(mapped_n,j,i);
  if(curr_anji!=INVALID_ANJI_VAL)
  {
    weighted_curr_anji=(float)weight*curr_anji;
    if(weighted_curr_anji<SMOOTHING_WEIGHTED_ANJI)
      weighted_curr_anji=SMOOTHING_WEIGHTED_ANJI;
  }

  float weighted_new_anji=(float)weight*anji_aux.get_invp_fast(mapped_n_aux,j,i);
  if(weighted_new_anji!=0 && weighted_new_anji<SMOOTHING_WEIGHTED_ANJI)
    weighted_new_anji=SMOOTHING_WEIGHTED_ANJI;

  WordIndex s=nsrcSent[i];
  WordIndex t=trgSent[j-1];
  
      // Obtain logarithms
  float weighted_curr_lanji;
  if(weighted_curr_anji==0)
    weighted_curr_lanji=SMALL_LG_NUM;
  else
    weighted_curr_lanji=log(weighted_curr_anji);
  
  float weighted_new_lanji=log(weighted_new_anji);

      // Store contributions
  while(lexAuxVar.size()<=s)
  {
    LexAuxVarElem lexAuxVarElem;
    lexAuxVar.push_back(lexAuxVarElem);
  }
  
  LexAuxVarElem::iterator lexAuxVarElemIter=lexAuxVar[s].find(t);
  if(lexAuxVarElemIter!=lexAuxVar[s].end())
  {
    if(weighted_curr_lanji!=SMALL_LG_NUM)
      lexAuxVarElemIter->second.first=MathFuncs::lns_sumlog_float(lexAuxVarElemIter->second.first,weighted_curr_lanji);
    lexAuxVarElemIter->second.second=MathFuncs::lns_sumlog_float(lexAuxVarElemIter->second.second,weighted_new_lanji);
  }
  else
  {
    lexAuxVar[s][t]=make_pair(weighted_curr_lanji,weighted_new_lanji);
  }
}

//-------------------------   
void IncrIbm1AligModel::updatePars(void)
{
      // Update parameters
  for(unsigned int i=0;i<lexAuxVar.size();++i)
  {
    for(LexAuxVarElem::iterator lexAuxVarElemIter=lexAuxVar[i].begin();lexAuxVarElemIter!=lexAuxVar[i].end();++lexAuxVarElemIter)
    {
      WordIndex s=i;//lexAuxVarElemIter->first.first;
      WordIndex t=lexAuxVarElemIter->first;
      float log_suff_stat_curr=lexAuxVarElemIter->second.first;
      float log_suff_stat_new=lexAuxVarElemIter->second.second;

          // Update parameters only if current and new sufficient statistics
          // are different
      if(log_suff_stat_curr!=log_suff_stat_new)
      {
            // Obtain lexNumer for s,t
        bool found;
        float numer=incrLexTable.getLexNumer(s,t,found);
        if(!found) numer=SMALL_LG_NUM;
      
            // Obtain lexDenom for s,t
        float denom=incrLexTable.getLexDenom(s,found);
        if(!found) denom=SMALL_LG_NUM;
      
            // Obtain new sufficient statistics
        float new_numer=obtainLogNewSuffStat(numer,log_suff_stat_curr,log_suff_stat_new);
        float new_denom=denom;
        if(numer!=SMALL_LG_NUM)
          new_denom=MathFuncs::lns_sublog_float(denom,numer);
        new_denom=MathFuncs::lns_sumlog_float(new_denom,new_numer);
        
            // Set lexical numerator and denominator
        incrLexTable.setLexNumDen(s,t,new_numer,new_denom);   
      }
    }
  }
      // Clear auxiliary variables
  lexAuxVar.clear();
}

//-------------------------
float IncrIbm1AligModel::obtainLogNewSuffStat(float lcurrSuffStat,
                                              float lLocalSuffStatCurr,
                                              float lLocalSuffStatNew)
{
  float lresult=MathFuncs::lns_sublog_float(lcurrSuffStat,lLocalSuffStatCurr);
  lresult=MathFuncs::lns_sumlog_float(lresult,lLocalSuffStatNew);
  return lresult;
}

//-------------------------   
Prob IncrIbm1AligModel::pts(WordIndex s,
                            WordIndex t)
{
  return unsmoothed_pts(s,t);
}

//-------------------------   
double IncrIbm1AligModel::unsmoothed_pts(WordIndex s,
                                         WordIndex t)
{
  return exp(unsmoothed_logpts(s,t));  
}

//-------------------------   
LgProb IncrIbm1AligModel::logpts(WordIndex s,WordIndex t)
{
  return unsmoothed_logpts(s,t);
}

//-------------------------   
double IncrIbm1AligModel::unsmoothed_logpts(WordIndex s,
                                            WordIndex t)
{
  bool found;
  double numer;

  numer=incrLexTable.getLexNumer(s,t,found);
  if(found)
  {
        // lexNumer for pair s,t exists
    double denom;
    
    denom=incrLexTable.getLexDenom(s,found);
    if(!found) return SMALL_LG_NUM;
    else
    {
      return numer-denom;
    }
  }
  else
  {
        // lexNumer for pair s,t does not exist
    return SMALL_LG_NUM;
  }
}

//-------------------------
Prob IncrIbm1AligModel::aProbIbm1(PositionIndex slen,
                                  PositionIndex tlen)
{
  return (double) exp((double)logaProbIbm1(slen,tlen));
}

//-------------------------
LgProb IncrIbm1AligModel::logaProbIbm1(PositionIndex slen,
                                       PositionIndex tlen)
{
  LgProb aligLgProb=0;
  
  for(unsigned int j=0;j<tlen;++j)
  {
    aligLgProb=(double)aligLgProb-(double)log((double)slen+1);
  }
  return aligLgProb;  
}

//-------------------------
Prob IncrIbm1AligModel::sentLenProb(unsigned int slen,
                                    unsigned int tlen)
{
  return sentLengthModel.sentLenProb(slen,tlen);
}

//-------------------------
LgProb IncrIbm1AligModel::sentLenLgProb(unsigned int slen,
                                        unsigned int tlen)
{
  return sentLengthModel.sentLenLgProb(slen,tlen);
}

//-------------------------
LgProb IncrIbm1AligModel::lexM1LpForBestAlig(Vector<WordIndex> nSrcSentIndexVector,
                                             Vector<WordIndex> trgSentIndexVector,
                                             Vector<PositionIndex>& bestAlig)
{
 LgProb aligLgProb;
 LgProb lp;
 LgProb max_lp;
 unsigned int best_i=0;

 bestAlig.clear();
 aligLgProb=0;
 for(unsigned int j=0;j<trgSentIndexVector.size();++j)
 {
   max_lp=-FLT_MAX;
   for(unsigned int i=0;i<nSrcSentIndexVector.size();++i)
   {
     lp=log((double)pts(nSrcSentIndexVector[i],trgSentIndexVector[j]));
     if(max_lp<=lp)
     {
       max_lp=lp; best_i=i;
     }
   }
   aligLgProb=aligLgProb+max_lp;
   bestAlig.push_back(best_i);
 }

 return aligLgProb;
}

//-------------------------
bool IncrIbm1AligModel::getEntriesForTarget(WordIndex t,
                                            SrcTableNode& srctn)
{
  std::set<WordIndex> transSet;
  bool ret=incrLexTable.getTransForTarget(t,transSet);
  if(ret==false) return false;
  
  srctn.clear();
  std::set<WordIndex>::const_iterator setIter;
  for(setIter=transSet.begin();setIter!=transSet.end();++setIter)
  {
    WordIndex s=*setIter;
    srctn[s]=pts(s,t);
  }
  return true;
}

//-------------------------
LgProb IncrIbm1AligModel::obtainBestAlignment(Vector<WordIndex> srcSentIndexVector,
                                              Vector<WordIndex> trgSentIndexVector,
                                              WordAligMatrix& bestWaMatrix)
{
  if(sentenceLengthIsOk(srcSentIndexVector) && sentenceLengthIsOk(trgSentIndexVector))
  {
    Vector<PositionIndex> bestAlig;
    LgProb lgProb=logaProbIbm1(srcSentIndexVector.size(),
                               trgSentIndexVector.size());
    lgProb+=sentLenLgProb(srcSentIndexVector.size(),
                          trgSentIndexVector.size());
    lgProb+=lexM1LpForBestAlig(addNullWordToWidxVec(srcSentIndexVector),
                               trgSentIndexVector,
                               bestAlig);

    bestWaMatrix.init(srcSentIndexVector.size(),trgSentIndexVector.size());
    bestWaMatrix.putAligVec(bestAlig);

    return lgProb;
  }
  else
  {
    bestWaMatrix.init(srcSentIndexVector.size(),trgSentIndexVector.size());    
    return SMALL_LG_NUM;
  }
}

//-------------------------
LgProb IncrIbm1AligModel::calcLgProbForAlig(const Vector<WordIndex>& sSent,
                                            const Vector<WordIndex>& tSent,
                                            WordAligMatrix aligMatrix,
                                            int verbose)
{
  unsigned int i;

  Vector<PositionIndex> alig;
  aligMatrix.getAligVec(alig);

  if(verbose)
  {
    for(i=0;i<sSent.size();++i) cerr<<sSent[i]<<" ";
    cerr<<"\n";
    for(i=0;i<tSent.size();++i) cerr<<tSent[i]<<" ";
    cerr<<"\n";   
    for(i=0;i<alig.size();++i) cerr<<alig[i]<<" ";
    cerr<<"\n";
  }
  if(tSent.size()!=alig.size())
  {
    cerr<<"Error: the sentence t and the alignment vector have not the same size."<<endl;
    return THOT_ERROR;
  }     
  else
  {
    return incrIBM1LgProb(addNullWordToWidxVec(sSent),tSent,alig,verbose);
  }
}

//-------------------------
LgProb IncrIbm1AligModel::incrIBM1LgProb(Vector<WordIndex> nsSent,
                                         Vector<WordIndex> tSent,
                                         Vector<PositionIndex> alig,
                                         int verbose)
{
  Prob p;
  LgProb lgProb;    
  unsigned int j;
  if(verbose) cerr<<"Obtaining IBM Model 1 logprob...\n"; 
  
  lgProb=logaProbIbm1(nsSent.size()-1,tSent.size()); 
  if(verbose)
    cerr<<"- aligLgProb(tlen="<<tSent.size() <<" | slen="<<nsSent.size()-1 <<")= "<<logaProbIbm1(nsSent.size()-1,tSent.size())<<endl;
  
  lgProb+=sentLenLgProb(nsSent.size()-1,tSent.size());
  if(verbose)
    cerr<<"- lenLgProb(tlen="<<tSent.size() <<" | slen="<<nsSent.size()-1 <<")= "<<sentLenLgProb(nsSent.size()-1,tSent.size())<<endl;
  
  for(j=0;j<alig.size();++j)
  { 
    p=pts(nsSent[alig[j]],tSent[j]);
    if(verbose)
      cerr<<"t("<<tSent[j] <<"|"<<nsSent[alig[j]] <<")= "<<p<<" ; logp="<<(double)log((double)p)<<endl;   
    lgProb=lgProb+(double)log((double)p); 
  }

  return lgProb;
}

//-------------------------
LgProb IncrIbm1AligModel::calcLgProb(const Vector<WordIndex>& sSent,
                                     const Vector<WordIndex>& tSent,
                                     int verbose)
{
  if(sentenceLengthIsOk(sSent) && sentenceLengthIsOk(tSent))
  {
    return calcSumIBM1LgProb(addNullWordToWidxVec(sSent),tSent,verbose);
  }
  else
  {
    return SMALL_LG_NUM;
  }
}

//-------------------------
LgProb IncrIbm1AligModel::calcSumIBM1LgProb(const char *sSent,
                                            const char *tSent,
                                            int verbose)
{
 Vector<std::string> nsSentVec,tSentVec;

 nsSentVec=StrProcUtils::charItemsToVector(sSent);
 nsSentVec=addNullWordToStrVec(nsSentVec);
 tSentVec=StrProcUtils::charItemsToVector(tSent);   
 return calcSumIBM1LgProb(nsSentVec,tSentVec,verbose);
}

//-------------------------
LgProb IncrIbm1AligModel::calcSumIBM1LgProb(Vector<std::string> nsSent,
                                            Vector<std::string> tSent,
                                            int verbose)
{
 Vector<WordIndex> neIndexVector,fIndexVector;
    
 neIndexVector=strVectorToSrcIndexVector(nsSent);
 fIndexVector=strVectorToTrgIndexVector(tSent);
  
 return calcSumIBM1LgProb(neIndexVector,fIndexVector,verbose);
}

//-------------------------
LgProb IncrIbm1AligModel::calcSumIBM1LgProb(Vector<WordIndex> nsSent,
                                            Vector<WordIndex> tSent,
                                            int verbose)
{
 Prob sump;
 LgProb lexContrib;
 LgProb lgProb;    
 unsigned int i,j;
 
 if(verbose) cerr<<"Obtaining Sum IBM Model 1 logprob...\n"; 
     
 lgProb=logaProbIbm1(nsSent.size()-1,tSent.size());
 
 if(verbose) cerr<<"- aligLgProb(tlen="<<tSent.size() <<" | slen="<<nsSent.size()-1<<")= "<<logaProbIbm1(nsSent.size()-1,tSent.size())<<endl;

 lgProb+=sentLenLgProb(nsSent.size()-1,tSent.size());
 if(verbose) cerr<<"- lenLgProb(tlen="<<tSent.size() <<" | slen="<<nsSent.size()-1 <<")= "<<sentLenLgProb(nsSent.size()-1,tSent.size())<<endl;

 lexContrib=0;
 for(j=0;j<tSent.size();++j)
 {
   sump=0;
   for(i=0;i<nsSent.size();++i)
   {
     sump+=pts(nsSent[i],tSent[j]);
     if(verbose==2)
       cerr<<"t( " <<tSent[j] <<" | " <<nsSent[i]<<" )= "<<pts(nsSent[i],tSent[j]) <<endl;
   }
   lexContrib+=(double)log((double)sump);  
   if(verbose) cerr<<"- sumt(j="<<j<<")= "<<sump<<endl;
   if(verbose==2) cerr<<endl;
 }

 if(verbose) cerr<<"- Lexical model contribution= "<<lexContrib<<endl;
 lgProb+=lexContrib;
 
 return lgProb;
}

//-------------------------
void IncrIbm1AligModel::initPpInfo(unsigned int slen,
                                   const Vector<WordIndex>& tSent,
                                   PpInfo& ppInfo)
{
      // Make room in ppInfo
  ppInfo.clear();
  for(unsigned int j=0;j<tSent.size();++j)
  {
    ppInfo.push_back(0);
  }
      // Add NULL word
  for(unsigned int j=0;j<tSent.size();++j)
  {
    ppInfo[j]+=pts(NULL_WORD,tSent[j])*((double)1.0/(slen+1));
  }
}

//-------------------------
void IncrIbm1AligModel::partialProbWithoutLen(unsigned int /*srcPartialLen*/,
                                              unsigned int slen,
                                              const Vector<WordIndex>& s_,
                                              const Vector<WordIndex>& tSent,
                                              PpInfo& ppInfo)
{
  for(unsigned int i=0;i<s_.size();++i)
  {
    for(unsigned int j=0;j<tSent.size();++j)
    {
      ppInfo[j]+=pts(s_[i],tSent[j])*((double)1.0/(slen+1));
    }
  }
}

//-------------------------
LgProb IncrIbm1AligModel::lpFromPpInfo(const PpInfo& ppInfo)
{
  LgProb lgProb=0;

  for(unsigned int j=0;j<ppInfo.size();++j)
  {
    lgProb+=log((double)ppInfo[j]);
  }
  return lgProb;
}

//-------------------------
void IncrIbm1AligModel::addHeurForNotAddedWords(int numSrcWordsToBeAdded,
                                                const Vector<WordIndex>& tSent,
                                                PpInfo& ppInfo)
{
  for(unsigned int j=0;j<tSent.size();++j)
  {
    ppInfo[j]+=numSrcWordsToBeAdded*exp((double)lgProbOfBestTransForTrgWord(tSent[j]));
  }
}

//-------------------------
void IncrIbm1AligModel::sustHeurForNotAddedWords(int numSrcWordsToBeAdded,
                                                 const Vector<WordIndex>& tSent,
                                                 PpInfo& ppInfo)
{
  for(unsigned int j=0;j<tSent.size();++j)
  {
    ppInfo[j]-=numSrcWordsToBeAdded*exp((double)lgProbOfBestTransForTrgWord(tSent[j]));
  }
}

//---------------------------------
LgProb IncrIbm1AligModel::lgProbOfBestTransForTrgWord(WordIndex t)
{
  BestLgProbForTrgWord::iterator tnIter;

  tnIter=bestLgProbForTrgWord.find(make_pair(0,t));
  if(tnIter==bestLgProbForTrgWord.end())
  {
    IncrIbm1AligModel::SrcTableNode tNode;    
    bool b=getEntriesForTarget(t,tNode);
    if(b)
    {
      IncrIbm1AligModel::SrcTableNode::const_iterator ctnIter;
      Prob bestProb=0;
      for(ctnIter=tNode.begin();ctnIter!=tNode.end();++ctnIter)
      {
        if(bestProb<ctnIter->second)
        {
          bestProb=ctnIter->second;
        }
      }
      bestLgProbForTrgWord[make_pair(0,t)]=log((double)bestProb);
      return log((double)bestProb);
    }
    else
    {
      bestLgProbForTrgWord[make_pair(0,t)]=-FLT_MAX;
      return -FLT_MAX;
    }
  }
  else return tnIter->second;
}

//-------------------------
bool IncrIbm1AligModel::load(const char* prefFileName)
{
  if(prefFileName[0]!=0)
  {
    bool retVal;

    cerr<<"Loading incremental IBM 1 Model data..."<<endl;

        // Load vocabularies if they exist
    std::string srcVocFileName=prefFileName;
    srcVocFileName=srcVocFileName+".svcb";
    loadGIZASrcVocab(srcVocFileName.c_str());

    std::string trgVocFileName=prefFileName;
    trgVocFileName=trgVocFileName+".tvcb";    
    loadGIZATrgVocab(trgVocFileName.c_str());

        // Load files with source and target sentences
        // Warning: this must be made before reading file with anji
        // values
    std::string srcsFile=prefFileName;
    srcsFile=srcsFile+".src";
    std::string trgsFile=prefFileName;
    trgsFile=trgsFile+".trg";
    std::string srctrgcFile=prefFileName;
    srctrgcFile=srctrgcFile+".srctrgc";
    pair<unsigned int,unsigned int> pui;
    retVal=readSentencePairs(srcsFile.c_str(),trgsFile.c_str(),srctrgcFile.c_str(),pui);
    if(retVal==THOT_ERROR) return THOT_ERROR;

        // Load file with anji values
    retVal=anji.load(prefFileName);
    if(retVal==THOT_ERROR) return THOT_ERROR;

        // Load file with lexical nd values
    std::string lexNumDenFile=prefFileName;
    lexNumDenFile=lexNumDenFile+".ibm_lexnd";
    retVal=incrLexTable.load(lexNumDenFile.c_str());
    if(retVal==THOT_ERROR) return THOT_ERROR;

        // Load average sentence lengths
    std::string slmodelFile=prefFileName;
    slmodelFile=slmodelFile+".slmodel";
    retVal=sentLengthModel.load(slmodelFile.c_str());
    if(retVal==THOT_ERROR) return THOT_ERROR;

    return THOT_OK;
  }
  else return THOT_ERROR;
}
   
//-------------------------
bool IncrIbm1AligModel::print(const char* prefFileName)
{
  bool retVal;

      // Print vocabularies 
  std::string srcVocFileName=prefFileName;
  srcVocFileName=srcVocFileName+".svcb";
  retVal=printGIZASrcVocab(srcVocFileName.c_str());
  if(retVal==THOT_ERROR) return THOT_ERROR;

  std::string trgVocFileName=prefFileName;
  trgVocFileName=trgVocFileName+".tvcb";    
  retVal=printGIZATrgVocab(trgVocFileName.c_str());
  if(retVal==THOT_ERROR) return THOT_ERROR;

      // Print files with source and target sentences
  std::string srcsFile=prefFileName;
  srcsFile=srcsFile+".src";
  std::string trgsFile=prefFileName;
  trgsFile=trgsFile+".trg";
  std::string srctrgcFile=prefFileName;
  srctrgcFile=srctrgcFile+".srctrgc";
  retVal=printSentPairs(srcsFile.c_str(),trgsFile.c_str(),srctrgcFile.c_str());
  if(retVal==THOT_ERROR) return THOT_ERROR;

      // Print file anji values
  retVal=anji.print(prefFileName);
  if(retVal==THOT_ERROR) return THOT_ERROR;

      // Print file with lexical nd values
  std::string lexNumDenFile=prefFileName;
  lexNumDenFile=lexNumDenFile+".ibm_lexnd";
  retVal=incrLexTable.print(lexNumDenFile.c_str());
  if(retVal==THOT_ERROR) return THOT_ERROR;

      // Print file with sentence length model
  std::string slmodelFile=prefFileName;
  slmodelFile=slmodelFile+".slmodel";
  retVal=sentLengthModel.print(slmodelFile.c_str());
  if(retVal==THOT_ERROR) return THOT_ERROR;

  return THOT_OK;
}

//-------------------------
void IncrIbm1AligModel::clear(void)
{
  _swAligModel<Vector<Prob> >::clear();
  anji.clear();
  anji_aux.clear();
  incrLexTable.clear();
  sentLengthModel.clear();
}

//-------------------------
void IncrIbm1AligModel::clearTempVars(void)
{
  bestLgProbForTrgWord.clear();  
}

//-------------------------
IncrIbm1AligModel::~IncrIbm1AligModel(void)
{
  
}
