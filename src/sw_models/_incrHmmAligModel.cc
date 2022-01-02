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

/**
 * @file _incrHmmAligModel.cc
 * 
 * @brief Definitions file for _incrHmmAligModel.h
 */

//--------------- Include files --------------------------------------

#include "_incrHmmAligModel.h"

//--------------- _incrHmmAligModel class function definitions

//-------------------------
_incrHmmAligModel::_incrHmmAligModel()
{
      // Link pointers with sentence length model
  sentLengthModel.linkVocabPtr(&swVocab);
  sentLengthModel.linkSentPairInfo(&sentenceHandler);

      // Set default value for aligSmoothInterpFactor
  aligSmoothInterpFactor=DEFAULT_ALIG_SMOOTH_INTERP_FACTOR;

      // Set default value for lexSmoothInterpFactor
  lexSmoothInterpFactor=DEFAULT_LEX_SMOOTH_INTERP_FACTOR;
}

//-------------------------
void _incrHmmAligModel::set_expval_maxnsize(unsigned int _expval_maxnsize)
{
  lanji.set_maxnsize(_expval_maxnsize);
  lanjm1ip_anji.set_maxnsize(_expval_maxnsize);
}

//-------------------------
unsigned int _incrHmmAligModel::numSentPairs(void)
{
  return sentenceHandler.numSentPairs();
}

//-------------------------
void _incrHmmAligModel::trainSentPairRange(std::pair<unsigned int,unsigned int> sentPairRange,
                                           int verbosity)
{
      // Train sentence length model
  sentLengthModel.trainSentPairRange(sentPairRange,verbosity);

      // EM algorithm
#ifdef THOT_ENABLE_VITERBI_TRAINING
  calcNewLocalSuffStatsVit(sentPairRange,verbosity);
#else
  calcNewLocalSuffStats(sentPairRange,verbosity);
#endif
  updateParsLex();
  updateParsAlig();
}

//-------------------------
void _incrHmmAligModel::trainAllSents(int verbosity)
{
  trainSentPairRange(std::make_pair(0,numSentPairs()-1),verbosity);
}

//-------------------------
void _incrHmmAligModel::efficientBatchTrainingForRange(std::pair<unsigned int,unsigned int> sentPairRange,
                                                       int verbosity/*=0*/)
{
      // Set maximum size of the matrices of expected values to zero
  set_expval_maxnsize(0);

      // Train sentence length model
  sentLengthModel.trainSentPairRange(sentPairRange,verbosity);

      // EM algorithm
#ifdef THOT_ENABLE_VITERBI_TRAINING
  calcNewLocalSuffStatsVit(sentPairRange,verbosity);
#else
  calcNewLocalSuffStats(sentPairRange,verbosity);
#endif
  incrLexTable->clear();
  incrHmmAligTable.clear();
  updateParsLex();
  updateParsAlig();
}

//-------------------------
std::pair<double,double> _incrHmmAligModel::loglikelihoodForPairRange(std::pair<unsigned int,unsigned int> sentPairRange,
                                                                 int verbosity/*=0*/)
{
  double loglikelihood=0;
  unsigned int numSents=0;

  for(unsigned int n=sentPairRange.first;n<=sentPairRange.second;++n)
  {
    if(verbosity) std::cerr<<"* Calculating log-likelihood for sentence "<<n<<std::endl;
        // Add log-likelihood
    std::vector<WordIndex> nthSrcSent=getSrcSent(n);
    std::vector<WordIndex> nthTrgSent=getTrgSent(n);
    if(sentenceLengthIsOk(nthSrcSent) && sentenceLengthIsOk(nthTrgSent))
    {
      loglikelihood+=(double)calcLgProb(nthSrcSent,nthTrgSent,verbosity);
      ++numSents;
    }
  }
  return std::make_pair(loglikelihood,loglikelihood/(double) numSents);
}

//-------------------------
std::pair<double,double> _incrHmmAligModel::vitLoglikelihoodForPairRange(std::pair<unsigned int,unsigned int> sentPairRange,
                                                                    int verbosity/*=0*/)
{
  double vitLoglikelihood=0;
  unsigned int numSents=0;

  for(unsigned int n=sentPairRange.first;n<=sentPairRange.second;++n)
  {
    if(verbosity) std::cerr<<"* Calculating log-likelihood for sentence "<<n<<std::endl;
        // Add Viterbi log-likelihood
    std::vector<WordIndex> nthSrcSent=getSrcSent(n);
    std::vector<WordIndex> nthTrgSent=getTrgSent(n);
    if(sentenceLengthIsOk(nthSrcSent) && sentenceLengthIsOk(nthTrgSent))
    {
      WordAligMatrix bestWaMatrix;
      vitLoglikelihood+=(double)obtainBestAlignment(nthSrcSent,nthTrgSent,bestWaMatrix);
      ++numSents;
    }
  }
  return std::make_pair(vitLoglikelihood,vitLoglikelihood/(double) numSents);
}

//-------------------------
void _incrHmmAligModel::clearInfoAboutSentRange(void)
{
      // Clear info about sentence range
  sentenceHandler.clear();
  lanji.clear();
  lanji_aux.clear();
  lanjm1ip_anji.clear();
  lanjm1ip_anji_aux.clear();
}

//-------------------------
void _incrHmmAligModel::setLexSmIntFactor(double _lexSmoothInterpFactor)
{
  lexSmoothInterpFactor=_lexSmoothInterpFactor;
  std::cerr<<"Lexical smoothing interpolation factor has been set to "<<lexSmoothInterpFactor<<std::endl;
}

//-------------------------
Prob _incrHmmAligModel::pts(WordIndex s,
                            WordIndex t)
{
  return exp((double)logpts(s,t));
}

//-------------------------
double _incrHmmAligModel::unsmoothed_logpts(WordIndex s,
                                            WordIndex t)
{
  bool found;
  double numer;

  numer=incrLexTable->getLexNumer(s,t,found);
  if(found)
  {
        // lexNumer for pair s,t exists
    double denom;

    denom=incrLexTable->getLexDenom(s,found);
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
LgProb _incrHmmAligModel::logpts(WordIndex s,
                                 WordIndex t)
{
  LgProb lexLgProb=(LgProb)log(1.0-lexSmoothInterpFactor)+unsmoothed_logpts(s,t);
  LgProb smoothLgProb=log(lexSmoothInterpFactor)+log(1.0/(double)(getTrgVocabSize()));
  return MathFuncs::lns_sumlog(lexLgProb,smoothLgProb);
}

//-------------------------
void _incrHmmAligModel::setAlSmIntFactor(double _aligSmoothInterpFactor)
{
  aligSmoothInterpFactor=_aligSmoothInterpFactor;
  std::cerr<<"Alignment smoothing interpolation factor has been set to "<<aligSmoothInterpFactor<<std::endl;
}

//-------------------------
Prob _incrHmmAligModel::aProb(PositionIndex prev_i,
                              PositionIndex slen,
                              PositionIndex i)
{
  return exp((double)logaProb(prev_i,slen,i));
}

//-------------------------
double _incrHmmAligModel::unsmoothed_logaProb(PositionIndex prev_i,
                                              PositionIndex slen,
                                              PositionIndex i)
{
  HmmAligInfo hmmAligInfo;
  getHmmAligInfo(prev_i,slen,i,hmmAligInfo);
  if(!hmmAligInfo.validAlig)
  {
    return SMALL_LG_NUM;
  }
  else
  {
    bool found;
    double numer;
    aSourceHmm asHmm;
    asHmm.prev_i=hmmAligInfo.modified_ip;
    asHmm.slen=slen;

    if(hmmAligInfo.nullAlig)
    {
      nullAligSpecialPar(prev_i,slen,asHmm,i);
    }

    numer=incrHmmAligTable.getAligNumer(asHmm,i,found);
    if(found)
    {
          // aligNumer for pair asHmm,i exists
      double denom;
      denom=incrHmmAligTable.getAligDenom(asHmm,found);
      if(!found) return SMALL_LG_NUM;
      else
      {
        return numer-denom;
      }
    }
    else
    {
          // aligNumer for pair asHmm,i does not exist
      return SMALL_LG_NUM;
    }
  }
}

//-------------------------
double _incrHmmAligModel::cached_logaProb(PositionIndex prev_i,
                                          PositionIndex slen,
                                          PositionIndex i,
                                          const std::vector<WordIndex>& /*nsrcSent*/,
                                          const std::vector<WordIndex>& /*trgSent*/)
{
  double d=cachedAligLogProbs.get(prev_i,slen,i);
  if(d<CACHED_HMM_ALIG_LGPROB_VIT_INVALID_VAL)
  {
    return d;
  }
  else
  {
    double d=(double)logaProb(prev_i,slen,i);
    cachedAligLogProbs.set(prev_i,slen,i,d);
    return d;
  }
}

//-------------------------
void _incrHmmAligModel::nullAligSpecialPar(unsigned int ip,
                                           unsigned int slen,
                                           aSourceHmm& asHmm,
                                           unsigned int& i)
{
  asHmm.slen=slen;
  if(ip==0)
  {
    asHmm.prev_i=0;
    i=slen+1;
  }
  else
  {
    if(ip>slen)
      asHmm.prev_i=ip-slen;
    else
      asHmm.prev_i=ip;

    i=asHmm.prev_i+slen;
  }
}

//-------------------------
LgProb _incrHmmAligModel::logaProb(PositionIndex prev_i,
                                   PositionIndex slen,
                                   PositionIndex i)
{
  LgProb lp=unsmoothed_logaProb(prev_i,slen,i);
  if(isValidAlig(prev_i,slen,i))
  {
    LgProb aligLgProb=(LgProb)log(1.0-aligSmoothInterpFactor)+lp;
    LgProb smoothLgProb;
    if(prev_i==0)
    {
      smoothLgProb=log(aligSmoothInterpFactor)+log(1.0/(double)(2*slen));
    }
    else
    {
      smoothLgProb=log(aligSmoothInterpFactor)+log(1.0/(double)(slen+1));
    }
    return MathFuncs::lns_sumlog(aligLgProb,smoothLgProb);
  }
  else return lp;
}

//-------------------------
std::vector<WordIndex> _incrHmmAligModel::getSrcSent(unsigned int n)
{
  std::vector<std::string> srcsStr;
  std::vector<WordIndex> result;

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
std::vector<WordIndex> _incrHmmAligModel::extendWithNullWord(const std::vector<WordIndex>& srcWordIndexVec)
{
      // Initialize result using srcWordIndexVec
  std::vector<WordIndex> result=srcWordIndexVec;

      // Add NULL words
  WordIndex nullWidx=stringToSrcWordIndex(NULL_WORD_STR);
  for(unsigned int i=0;i<srcWordIndexVec.size();++i)
    result.push_back(nullWidx);

  return result;
}

//-------------------------
std::vector<WordIndex> _incrHmmAligModel::extendWithNullWordAlig(const std::vector<WordIndex>& srcWordIndexVec)
{
  return extendWithNullWord(srcWordIndexVec);
}

//-------------------------
PositionIndex _incrHmmAligModel::getSrcLen(const std::vector<WordIndex>& nsrcWordIndexVec)
{
  unsigned int result=0;
  WordIndex nullWidx=stringToSrcWordIndex(NULL_WORD_STR);
  for(unsigned int i=0;i<nsrcWordIndexVec.size();++i)
  {
    if(nsrcWordIndexVec[i]!=nullWidx)
      ++result;
  }
  return result;
}

//-------------------------
std::vector<WordIndex> _incrHmmAligModel::getTrgSent(unsigned int n)
{
  std::vector<std::string> trgsStr;
  std::vector<WordIndex> trgs;

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
bool _incrHmmAligModel::sentenceLengthIsOk(const std::vector<WordIndex> sentence)
{
  if(sentence.empty() || sentence.size()>HMM_SWM_MAX_SENT_LENGTH)
    return false;
  else
    return true;
}

//-------------------------
bool _incrHmmAligModel::loadLexSmIntFactor(const char* lexSmIntFactorFile)
{
  std::cerr<<"Loading file with lexical smoothing interpolation factor from "<<lexSmIntFactorFile<<std::endl;

  AwkInputStream awk;

  if(awk.open(lexSmIntFactorFile)==THOT_ERROR)
  {
    std::cerr<<"Error in file with lexical smoothing interpolation factor, file "<<lexSmIntFactorFile<<" does not exist. Assuming default value."<<"\n";
    setLexSmIntFactor(DEFAULT_LEX_SMOOTH_INTERP_FACTOR);
    return THOT_OK;
  }
  else
  {
    if(awk.getln())
    {
      if(awk.NF==1)
      {
        setLexSmIntFactor((Prob)atof(awk.dollar(1).c_str()));
        return THOT_OK;
      }
      else
      {
        std::cerr<<"Error: anomalous .lsifactor file, "<<lexSmIntFactorFile<<std::endl;
        return THOT_ERROR;
      }
    }
    else
    {
      std::cerr<<"Error: anomalous .lsifactor file, "<<lexSmIntFactorFile<<std::endl;
      return THOT_ERROR;
    }
  }
}

//-------------------------
bool _incrHmmAligModel::printLexSmIntFactor(const char* lexSmIntFactorFile)
{
  std::ofstream outF;
  outF.open(lexSmIntFactorFile,std::ios::out);
  if(!outF)
  {
    std::cerr<<"Error while printing file with lexical smoothing interpolation factor."<<std::endl;
    return THOT_ERROR;
  }
  else
  {
    outF<<lexSmoothInterpFactor<<std::endl;
    return THOT_OK;
  }
}

//-------------------------
bool _incrHmmAligModel::loadAlSmIntFactor(const char* alSmIntFactorFile)
{
  std::cerr<<"Loading file with alignment smoothing interpolation factor from "<<alSmIntFactorFile<<std::endl;

  AwkInputStream awk;

  if(awk.open(alSmIntFactorFile)==THOT_ERROR)
  {
    std::cerr<<"Error in file with alignment smoothing interpolation factor, file "<<alSmIntFactorFile<<" does not exist. Assuming default value."<<"\n";
    setAlSmIntFactor(DEFAULT_ALIG_SMOOTH_INTERP_FACTOR);
    return THOT_OK;
  }
  else
  {
    if(awk.getln())
    {
      if(awk.NF==1)
      {
        setAlSmIntFactor((Prob)atof(awk.dollar(1).c_str()));
        return THOT_OK;
      }
      else
      {
        std::cerr<<"Error: anomalous .asifactor file, "<<alSmIntFactorFile<<std::endl;
        return THOT_ERROR;
      }
    }
    else
    {
      std::cerr<<"Error: anomalous .asifactor file, "<<alSmIntFactorFile<<std::endl;
      return THOT_ERROR;
    }
  }
}

//-------------------------
bool _incrHmmAligModel::printAlSmIntFactor(const char* alSmIntFactorFile)
{
  std::ofstream outF;
  outF.open(alSmIntFactorFile,std::ios::out);
  if(!outF)
  {
    std::cerr<<"Error while printing file with alignment smoothing interpolation factor."<<std::endl;
    return THOT_ERROR;
  }
  else
  {
    outF<<aligSmoothInterpFactor<<std::endl;
    return THOT_OK;
  }
}

//-------------------------
void _incrHmmAligModel::initCachedLexicalLps(const std::vector<WordIndex>& nSrcSentIndexVector,
                                             const std::vector<WordIndex>& trgSentIndexVector,
                                             std::vector<std::vector<double> >& cachedLps)
{
      // Create data structure to cache lexical log-probs
  std::vector<double> dVec;
  dVec.insert(dVec.begin(),trgSentIndexVector.size()+1,SMALL_LG_NUM);
  cachedLps.clear();
  cachedLps.insert(cachedLps.begin(),nSrcSentIndexVector.size()+1,dVec);

      // Cache lexical log-probs
  for(PositionIndex j=1;j<=trgSentIndexVector.size();++j)
  {
    for(PositionIndex i=1;i<=nSrcSentIndexVector.size();++i)
    {
      cachedLps[i][j]=logpts(nSrcSentIndexVector[i-1],trgSentIndexVector[j-1]);
    }
  }
}

//-------------------------
void _incrHmmAligModel::calcNewLocalSuffStats(std::pair<unsigned int,unsigned int> sentPairRange,
                                              int verbosity)
{
      // Iterate over the training samples
  for(unsigned int n=sentPairRange.first;n<=sentPairRange.second;++n)
  {
        // Init vars for n'th sample
    std::vector<WordIndex> srcSent=getSrcSent(n);
    std::vector<WordIndex> nsrcSent=extendWithNullWord(srcSent);
    std::vector<WordIndex> trgSent=getTrgSent(n);

        // Do not process sentence pair if sentences are empty or exceed the maximum length
    if(sentenceLengthIsOk(srcSent) && sentenceLengthIsOk(trgSent))
    {
      Count weight;
      sentenceHandler.getCount(n,weight);

          // Initialize data structure to cache lexical log-probs
      initCachedLexicalLps(nsrcSent,trgSent,cachedLexLogProbs);

          // Make room for data structure to cache alignment log-probs
      cachedAligLogProbs.makeRoomGivenNSrcSentLen(nsrcSent.size());

          // Calculate alpha and beta matrices
      calcAlphaMatrix(n,nsrcSent,trgSent);
      calcBetaMatrix(n,nsrcSent,trgSent);

          // Calculate sufficient statistics for anji values
      calc_lanji(n,nsrcSent,trgSent,weight);

          // Calculate sufficient statistics for anjm1ip_anji values
      calc_lanjm1ip_anji(n,extendWithNullWordAlig(srcSent),trgSent,weight);

          // Clear cached alpha and beta values
      alphaMatrix.clear();
      betaMatrix.clear();

          // Clear cached lexical log prob
      cachedLexLogProbs.clear();
    }
    else
    {
      if(verbosity)
      {
        std::cerr<<"Warning, training pair "<<n+1<<" discarded due to sentence length (slen: "<<srcSent.size()<<" , tlen: "<<trgSent.size()<<")"<<std::endl;
      }
    }
  }
      // Clear cached alignment log probs
  cachedAligLogProbs.clear();
}

//-------------------------
void _incrHmmAligModel::calcNewLocalSuffStatsVit(std::pair<unsigned int,unsigned int> sentPairRange,
                                                 int verbosity)
{
      // Define variable to cache alignment log probs
  CachedHmmAligLgProb cached_logap;

      // Iterate over the training samples
  for(unsigned int n=sentPairRange.first;n<=sentPairRange.second;++n)
  {
        // Init vars for n'th sample
    std::vector<WordIndex> srcSent=getSrcSent(n);
    std::vector<WordIndex> nsrcSent=extendWithNullWord(srcSent);
    std::vector<WordIndex> trgSent=getTrgSent(n);

        // Do not process sentence pair if sentences are empty or exceed the maximum length
    if(sentenceLengthIsOk(srcSent) && sentenceLengthIsOk(trgSent))
    {
      Count weight;
      sentenceHandler.getCount(n,weight);

          // Execute Viterbi algorithm
      std::vector<std::vector<double> > vitMatrix;
      std::vector<std::vector<PositionIndex> > predMatrix;
      viterbiAlgorithmCached(nsrcSent,trgSent,cached_logap,vitMatrix,predMatrix);

          // Obtain Viterbi alignment
      std::vector<PositionIndex> bestAlig;
      bestAligGivenVitMatricesRaw(vitMatrix,predMatrix,bestAlig);

          // Calculate sufficient statistics for anji values
      calc_lanji_vit(n,nsrcSent,trgSent,bestAlig,weight);

          // Calculate sufficient statistics for anjm1ip_anji values
      calc_lanjm1ip_anji_vit(n,extendWithNullWordAlig(srcSent),trgSent,bestAlig,weight);
    }
    else
    {
      if(verbosity)
      {
        std::cerr<<"Warning, training pair "<<n+1<<" discarded due to sentence length (slen: "<<srcSent.size()<<" , tlen: "<<trgSent.size()<<")"<<std::endl;
      }
    }
  }
}

//-------------------------
void _incrHmmAligModel::calcAlphaMatrix(unsigned int /*n*/,
                                        const std::vector<WordIndex>& nsrcSent,
                                        const std::vector<WordIndex>& trgSent)
{
      // Obtain slen
  PositionIndex slen=getSrcLen(nsrcSent);

      // Initialize alphaMatrix
  alphaMatrix.clear();
  std::vector<double> dVec;
  dVec.insert(dVec.begin(),trgSent.size()+1,0.0);
  alphaMatrix.insert(alphaMatrix.begin(),nsrcSent.size()+1,dVec);

      // Fill matrix
  for(PositionIndex j=1;j<=trgSent.size();++j)
  {
    for(PositionIndex i=1;i<=nsrcSent.size();++i)
    {
      if(j==1)
      {
        alphaMatrix[i][j]=cached_logaProb(0,slen,i,nsrcSent,trgSent)+
          cachedLexLogProbs[i][j];
      }
      else
      {
        for(PositionIndex i_tilde=1;i_tilde<=nsrcSent.size();++i_tilde)
        {
          double lp=alphaMatrix[i_tilde][j-1]+
            cached_logaProb(i_tilde,slen,i,nsrcSent,trgSent)+
            cachedLexLogProbs[i][j];
          if(i_tilde==1)
            alphaMatrix[i][j]=lp;
          else
            alphaMatrix[i][j]=MathFuncs::lns_sumlog(lp,alphaMatrix[i][j]);
        }
      }
    }
  }
}

//-------------------------
void _incrHmmAligModel::calcBetaMatrix(unsigned int /*n*/,
                                       const std::vector<WordIndex>& nsrcSent,
                                       const std::vector<WordIndex>& trgSent)
{
      // Obtain slen
  PositionIndex slen=getSrcLen(nsrcSent);

      // Initialize betaMatrix
  betaMatrix.clear();
  std::vector<double> dVec;
  dVec.insert(dVec.begin(),trgSent.size()+1,0.0);
  betaMatrix.insert(betaMatrix.begin(),nsrcSent.size()+1,dVec);

      // Fill matrix
  for(PositionIndex j=trgSent.size();j>=1;--j)
  {
    for(PositionIndex i=1;i<=nsrcSent.size();++i)
    {
      if(j==trgSent.size())
      {
        betaMatrix[i][j]=log(1.0);
      }
      else
      {
        for(PositionIndex i_tilde=1;i_tilde<=nsrcSent.size();++i_tilde)
        {
          double lp=betaMatrix[i_tilde][j+1]+
            cached_logaProb(i,slen,i_tilde,nsrcSent,trgSent)+
            cachedLexLogProbs[i_tilde][j+1];
          if(i_tilde==1)
            betaMatrix[i][j]=lp;
          else
            betaMatrix[i][j]=MathFuncs::lns_sumlog(lp,betaMatrix[i][j]);
        }
      }
    }
  }
}

//-------------------------
void _incrHmmAligModel::calc_lanji(unsigned int n,
                                   const std::vector<WordIndex>& nsrcSent,
                                   const std::vector<WordIndex>& trgSent,
                                   const Count& weight)
{
  PositionIndex slen=getSrcLen(nsrcSent);

        // Initialize data structures
  unsigned int mapped_n;
  lanji.init_nth_entry(n,nsrcSent.size(),trgSent.size(),mapped_n);

  unsigned int n_aux=1;
  unsigned int mapped_n_aux;
  lanji_aux.init_nth_entry(n_aux,nsrcSent.size(),trgSent.size(),mapped_n_aux);

  std::vector<double> numVec(nsrcSent.size()+1,0);

      // Calculate new estimation of lanji
  for(unsigned int j=1;j<=trgSent.size();++j)
  {
        // Obtain sum_lanji_num_forall_s
    double sum_lanji_num_forall_s=INVALID_ANJI_VAL;
    for(unsigned int i=1;i<=nsrcSent.size();++i)
    {
          // Obtain numerator
      double d=calc_lanji_num(slen,i,j,nsrcSent,trgSent);

          // Add contribution to sum
      if(sum_lanji_num_forall_s==INVALID_ANJI_VAL)
        sum_lanji_num_forall_s=d;
      else
        sum_lanji_num_forall_s=MathFuncs::lns_sumlog(sum_lanji_num_forall_s,d);
          // Store num in numVec
      numVec[i]=d;
    }
        // Set value of lanji_aux
    for(unsigned int i=1;i<=nsrcSent.size();++i)
    {
          // Obtain expected value
      double lanji_val=numVec[i]-sum_lanji_num_forall_s;
          // Smooth expected value
      if(lanji_val>EXP_VAL_LOG_MAX) lanji_val=EXP_VAL_LOG_MAX;
      if(lanji_val<EXP_VAL_LOG_MIN) lanji_val=EXP_VAL_LOG_MIN;
          // Store expected value
      lanji_aux.set_fast(mapped_n_aux,j,i,lanji_val);
    }
  }
      // Gather lexical sufficient statistics
  gatherLexSuffStats(mapped_n,mapped_n_aux,nsrcSent,trgSent,weight);

      // clear lanji_aux data structure
  lanji_aux.clear();
}

//-------------------------
void _incrHmmAligModel::calc_lanji_vit(unsigned int n,
                                       const std::vector<WordIndex>& nsrcSent,
                                       const std::vector<WordIndex>& trgSent,
                                       const std::vector<PositionIndex>& bestAlig,
                                       const Count& weight)
{
        // Initialize data structures
  unsigned int mapped_n;
  lanji.init_nth_entry(n,nsrcSent.size(),trgSent.size(),mapped_n);

  unsigned int n_aux=1;
  unsigned int mapped_n_aux;
  lanji_aux.init_nth_entry(n_aux,nsrcSent.size(),trgSent.size(),mapped_n_aux);

      // Calculate new estimation of lanji
  for(unsigned int j=1;j<=trgSent.size();++j)
  {
        // Set value of lanji_aux
    for(unsigned int i=1;i<=nsrcSent.size();++i)
    {
      if(bestAlig[j-1]==i)
      {
            // Obtain expected value
        double lanji_val=0;
            // Store expected value
        lanji_aux.set_fast(mapped_n_aux,j,i,lanji_val);
      }
    }
  }

      // Gather lexical sufficient statistics
  gatherLexSuffStats(mapped_n,mapped_n_aux,nsrcSent,trgSent,weight);

      // clear lanji_aux data structure
  lanji_aux.clear();
}

//-------------------------
void _incrHmmAligModel::gatherLexSuffStats(unsigned int mapped_n,
                                           unsigned int mapped_n_aux,
                                           const std::vector<WordIndex>& nsrcSent,
                                           const std::vector<WordIndex>& trgSent,
                                           const Count& weight)
{
      // Gather lexical sufficient statistics
  for(unsigned int j=1;j<=trgSent.size();++j)
  {
    for(unsigned int i=1;i<=nsrcSent.size();++i)
    {
          // Reestimate lexical parameters
      fillEmAuxVarsLex(mapped_n,mapped_n_aux,i,j,nsrcSent,trgSent,weight);

          // Update lanji
      lanji.set_fast(mapped_n,j,i,lanji_aux.get_invlogp(mapped_n_aux,j,i));
    }
  }
}

//-------------------------
void _incrHmmAligModel::fillEmAuxVarsLex(unsigned int mapped_n,
                                         unsigned int mapped_n_aux,
                                         PositionIndex i,
                                         PositionIndex j,
                                         const std::vector<WordIndex>& nsrcSent,
                                         const std::vector<WordIndex>& trgSent,
                                         const Count& weight)
{
      // Init vars
  float curr_lanji=lanji.get_fast(mapped_n,j,i);
  float weighted_curr_lanji=SMALL_LG_NUM;
  if(curr_lanji!=INVALID_ANJI_VAL)
  {
    weighted_curr_lanji=(float)log((float)weight)+curr_lanji;
    if(weighted_curr_lanji<SMALL_LG_NUM)
      weighted_curr_lanji=SMALL_LG_NUM;
  }

  float weighted_new_lanji=(float)log((float)weight)+lanji_aux.get_invlogp_fast(mapped_n_aux,j,i);
  if(weighted_new_lanji<SMALL_LG_NUM)
    weighted_new_lanji=SMALL_LG_NUM;

  WordIndex s=nsrcSent[i-1];
  WordIndex t=trgSent[j-1];

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
    lexAuxVar[s][t]=std::make_pair(weighted_curr_lanji,weighted_new_lanji);
  }
}

//-------------------------
void _incrHmmAligModel::calc_lanjm1ip_anji(unsigned int n,
                                           const std::vector<WordIndex>& nsrcSent,
                                           const std::vector<WordIndex>& trgSent,
                                           const Count& weight)
{
  PositionIndex slen=getSrcLen(nsrcSent);

      // Initialize data structures
  unsigned int mapped_n;
  lanjm1ip_anji.init_nth_entry(n,nsrcSent.size(),trgSent.size(),mapped_n);

  unsigned int n_aux=1;
  unsigned int mapped_n_aux;
  lanjm1ip_anji_aux.init_nth_entry(n_aux,nsrcSent.size(),trgSent.size(),mapped_n_aux);

  std::vector<double> numVec(nsrcSent.size()+1,0);
  std::vector<std::vector<double> > numVecVec(nsrcSent.size()+1,numVec);

      // Calculate new estimation of lanjm1ip_anji
  for(unsigned int j=1;j<=trgSent.size();++j)
  {
        // Obtain sum_lanjm1ip_anji_num_forall_i_ip
    double sum_lanjm1ip_anji_num_forall_i_ip=INVALID_ANJM1IP_ANJI_VAL;

    for(unsigned int i=1;i<=nsrcSent.size();++i)
    {
      numVecVec[i][0]=0;
      if(j==1)
      {
            // Obtain numerator

            // Obtain information about alignment
        bool nullAlig=isNullAlig(0,slen,i);
        double d;
        if(nullAlig)
        {
          if(isFirstNullAligPar(0,slen,i))
            d=calc_lanjm1ip_anji_num_je1(slen,i,nsrcSent,trgSent);
          else d=numVecVec[slen+1][0];
        }
        else d=calc_lanjm1ip_anji_num_je1(slen,i,nsrcSent,trgSent);
            // Add contribution to sum
        if(sum_lanjm1ip_anji_num_forall_i_ip==INVALID_ANJM1IP_ANJI_VAL)
          sum_lanjm1ip_anji_num_forall_i_ip=d;
        else
          sum_lanjm1ip_anji_num_forall_i_ip=MathFuncs::lns_sumlog(sum_lanjm1ip_anji_num_forall_i_ip,d);
            // Store num in numVec
        numVecVec[i][0]=d;
      }
      else
      {
        for(unsigned int ip=1;ip<=nsrcSent.size();++ip)
        {
              // Obtain numerator

              // Obtain information about alignment
          double d;
          bool validAlig=isValidAlig(ip,slen,i);
          if(!validAlig)
          {
            d=SMALL_LG_NUM;
          }
          else
          {
            d=calc_lanjm1ip_anji_num_jg1(ip,slen,i,j,nsrcSent,trgSent);
          }
              // Add contribution to sum
          if(sum_lanjm1ip_anji_num_forall_i_ip==INVALID_ANJM1IP_ANJI_VAL)
            sum_lanjm1ip_anji_num_forall_i_ip=d;
          else
            sum_lanjm1ip_anji_num_forall_i_ip=MathFuncs::lns_sumlog(sum_lanjm1ip_anji_num_forall_i_ip,d);
              // Store num in numVec
          numVecVec[i][ip]=d;
        }
      }
    }
        // Set value of lanjm1ip_anji_aux
    for(unsigned int i=1;i<=nsrcSent.size();++i)
    {
      double lanjm1ip_anji_val;
      if(j==1)
      {
            // Obtain expected value
        lanjm1ip_anji_val=numVecVec[i][0]-sum_lanjm1ip_anji_num_forall_i_ip;
            // Smooth expected value
        if(lanjm1ip_anji_val>EXP_VAL_LOG_MAX) lanjm1ip_anji_val=EXP_VAL_LOG_MAX;
        if(lanjm1ip_anji_val<EXP_VAL_LOG_MIN) lanjm1ip_anji_val=EXP_VAL_LOG_MIN;
            // Store expected value
        lanjm1ip_anji_aux.set_fast(mapped_n_aux,j,i,0,lanjm1ip_anji_val);
      }
      else
      {
        for(unsigned int ip=1;ip<=nsrcSent.size();++ip)
        {
              // Obtain information about alignment
          bool validAlig=isValidAlig(ip,slen,i);
          if(validAlig)
          {
                // Obtain expected value
            lanjm1ip_anji_val=numVecVec[i][ip]-sum_lanjm1ip_anji_num_forall_i_ip;
                // Smooth expected value
            if(lanjm1ip_anji_val>EXP_VAL_LOG_MAX) lanjm1ip_anji_val=EXP_VAL_LOG_MAX;
            if(lanjm1ip_anji_val<EXP_VAL_LOG_MIN) lanjm1ip_anji_val=EXP_VAL_LOG_MIN;
            lanjm1ip_anji_aux.set_fast(mapped_n_aux,j,i,ip,lanjm1ip_anji_val);
          }
        }
      }
    }
  }
      // Gather alignment sufficient statistics
  gatherAligSuffStats(mapped_n,mapped_n_aux,nsrcSent,trgSent,weight);

      // clear lanjm1ip_anji_aux data structure
  lanjm1ip_anji_aux.clear();
}

//-------------------------
void _incrHmmAligModel::calc_lanjm1ip_anji_vit(unsigned int n,
                                               const std::vector<WordIndex>& nsrcSent,
                                               const std::vector<WordIndex>& trgSent,
                                               const std::vector<PositionIndex>& bestAlig,
                                               const Count& weight)
{
  PositionIndex slen=getSrcLen(nsrcSent);

      // Initialize data structures
  unsigned int mapped_n;
  lanjm1ip_anji.init_nth_entry(n,nsrcSent.size(),trgSent.size(),mapped_n);

  unsigned int n_aux=1;
  unsigned int mapped_n_aux;
  lanjm1ip_anji_aux.init_nth_entry(n_aux,nsrcSent.size(),trgSent.size(),mapped_n_aux);

      // Calculate new estimation of lanjm1ip_anji
  for(unsigned int j=1;j<=trgSent.size();++j)
  {
    for(unsigned int i=1;i<=nsrcSent.size();++i)
    {
      if(j==1)
      {
        if(bestAlig[0]==i)
        {
          double lanjm1ip_anji_val=0;
              // Store expected value
          lanjm1ip_anji_aux.set_fast(mapped_n_aux,j,i,0,lanjm1ip_anji_val);
        }
      }
      else
      {
        for(unsigned int ip=1;ip<=nsrcSent.size();++ip)
        {
          PositionIndex aligModifiedIp=getModifiedIp(bestAlig[j-2],slen,i);

          if(bestAlig[j-1]==i && aligModifiedIp==ip)
          {
            double lanjm1ip_anji_val=0;
                // Store expected value
            lanjm1ip_anji_aux.set_fast(mapped_n_aux,j,i,ip,lanjm1ip_anji_val);
          }
        }
      }
    }
  }

      // Gather alignment sufficient statistics
  gatherAligSuffStats(mapped_n,mapped_n_aux,nsrcSent,trgSent,weight);

      // clear lanjm1ip_anji_aux data structure
  lanjm1ip_anji_aux.clear();
}

//-------------------------
void _incrHmmAligModel::gatherAligSuffStats(unsigned int mapped_n,
                                            unsigned int mapped_n_aux,
                                            const std::vector<WordIndex>& nsrcSent,
                                            const std::vector<WordIndex>& trgSent,
                                            const Count& weight)
{
  PositionIndex slen=getSrcLen(nsrcSent);

      // Maximize alignment parameters
  for(unsigned int j=1;j<=trgSent.size();++j)
  {
    for(unsigned int i=1;i<=nsrcSent.size();++i)
    {
      if(j==1)
      {
            // Reestimate alignment parameters
        fillEmAuxVarsAlig(mapped_n,mapped_n_aux,slen,0,i,j,weight);

            // Update lanjm1ip_anji
        lanjm1ip_anji.set_fast(mapped_n,j,i,0,lanjm1ip_anji_aux.get_invlogp_fast(mapped_n_aux,j,i,0));
      }
      else
      {
        for(unsigned int ip=1;ip<=nsrcSent.size();++ip)
        {
              // Obtain information about alignment
          bool validAlig=isValidAlig(ip,slen,i);
          if(validAlig)
          {
                // Reestimate alignment parameters
            fillEmAuxVarsAlig(mapped_n,mapped_n_aux,slen,ip,i,j,weight);
                // Update lanjm1ip_anji
            lanjm1ip_anji.set_fast(mapped_n,j,i,ip,lanjm1ip_anji_aux.get_invlogp_fast(mapped_n_aux,j,i,ip));
          }
        }
      }
    }
  }
}

//-------------------------
void _incrHmmAligModel::fillEmAuxVarsAlig(unsigned int mapped_n,
                                          unsigned int mapped_n_aux,
                                          PositionIndex slen,
                                          PositionIndex ip,
                                          PositionIndex i,
                                          PositionIndex j,
                                          const Count& weight)
{
      // Init vars
  float curr_lanjm1ip_anji=lanjm1ip_anji.get_fast(mapped_n,j,i,ip);
  float weighted_curr_lanjm1ip_anji=SMALL_LG_NUM;
  if(curr_lanjm1ip_anji!=INVALID_ANJM1IP_ANJI_VAL)
  {
    weighted_curr_lanjm1ip_anji=(float)log((float)weight)+curr_lanjm1ip_anji;
    if(weighted_curr_lanjm1ip_anji<SMALL_LG_NUM)
      weighted_curr_lanjm1ip_anji=SMALL_LG_NUM;
  }

  float weighted_new_lanjm1ip_anji=(float)log((float)weight)+lanjm1ip_anji_aux.get_invlogp_fast(mapped_n_aux,j,i,ip);
  if(weighted_new_lanjm1ip_anji<SMALL_LG_NUM)
    weighted_new_lanjm1ip_anji=SMALL_LG_NUM;

      // Init aSourceHmm data structure
  aSourceHmm asHmm;
  asHmm.prev_i=ip;
  asHmm.slen=slen;

      // Gather local suff. statistics
  AligAuxVar::iterator aligAuxVarIter=aligAuxVar.find(std::make_pair(asHmm,i));
  if(aligAuxVarIter!=aligAuxVar.end())
  {
    if(weighted_curr_lanjm1ip_anji!=SMALL_LG_NUM)
      aligAuxVarIter->second.first=MathFuncs::lns_sumlog_float(aligAuxVarIter->second.first,weighted_curr_lanjm1ip_anji);
   aligAuxVarIter->second.second=MathFuncs::lns_sumlog_float(aligAuxVarIter->second.second,weighted_new_lanjm1ip_anji);
  }
  else
  {
    aligAuxVar[std::make_pair(asHmm,i)]=std::make_pair(weighted_curr_lanjm1ip_anji,weighted_new_lanjm1ip_anji);
  }
}

//-------------------------
bool _incrHmmAligModel::isFirstNullAligPar(PositionIndex ip,
                                           unsigned int slen,
                                           PositionIndex i)
{
  if(ip==0)
  {
    if(i==slen+1)
      return true;
    else return false;
  }
  else
  {
    if(i>slen && i-slen==ip)
      return true;
    else return false;
  }
}

//-------------------------
double _incrHmmAligModel::calc_lanji_num(PositionIndex slen,
                                         PositionIndex i,
                                         PositionIndex j,
                                         const std::vector<WordIndex>& nsrcSent,
                                         const std::vector<WordIndex>& trgSent)
{
  double result=log_alpha(slen,i,j,nsrcSent,trgSent)+log_beta(slen,i,j,nsrcSent,trgSent);
  if(result<SMALL_LG_NUM) result=SMALL_LG_NUM;
  return result;
}

//-------------------------
double _incrHmmAligModel::calc_lanjm1ip_anji_num_je1(PositionIndex slen,
                                                     PositionIndex i,
                                                     const std::vector<WordIndex>& nsrcSent,
                                                     const std::vector<WordIndex>& trgSent)
{
  double result=cached_logaProb(0,slen,i,nsrcSent,trgSent)+
    cachedLexLogProbs[i][1]+
    log_beta(slen,i,1,nsrcSent,trgSent);
  if(result<SMALL_LG_NUM) result=SMALL_LG_NUM;
  return result;
}

//-------------------------
double _incrHmmAligModel::calc_lanjm1ip_anji_num_jg1(PositionIndex ip,
                                                     PositionIndex slen,
                                                     PositionIndex i,
                                                     PositionIndex j,
                                                     const std::vector<WordIndex>& nsrcSent,
                                                     const std::vector<WordIndex>& trgSent)
{
  double result=log_alpha(slen,ip,j-1,nsrcSent,trgSent)+
    cached_logaProb(ip,slen,i,nsrcSent,trgSent)+
    cachedLexLogProbs[i][j]+
    log_beta(slen,i,j,nsrcSent,trgSent);
  if(result<SMALL_LG_NUM) result=SMALL_LG_NUM;
  return result;
}

//-------------------------
void _incrHmmAligModel::getHmmAligInfo(PositionIndex ip,
                                       unsigned int slen,
                                       PositionIndex i,
                                       HmmAligInfo& hmmAligInfo)
{
  hmmAligInfo.validAlig=isValidAlig(ip,slen,i);
  if(hmmAligInfo.validAlig)
  {
    hmmAligInfo.nullAlig=isNullAlig(ip,slen,i);
    hmmAligInfo.modified_ip=getModifiedIp(ip,slen,i);
  }
  else
  {
    hmmAligInfo.nullAlig=false;
    hmmAligInfo.modified_ip=ip;
  }
}

//-------------------------
bool _incrHmmAligModel::isValidAlig(PositionIndex ip,
                                    unsigned int slen,
                                    PositionIndex i)
{
  if(i<=slen) return true;
  else
  {
    if(ip==0) return true;
    i=i-slen;
    if(ip>slen) ip=ip-slen;
    if(i!=ip) return false;
    else return true;
  }
}

//-------------------------
bool _incrHmmAligModel::isNullAlig(PositionIndex ip,
                                   unsigned int slen,
                                   PositionIndex i)
{
  if(i<=slen) return false;
  else
  {
    if(ip==0) return true;
    i=i-slen;
    if(ip>slen) ip=ip-slen;
    if(i!=ip) return false;
    else return true;
  }
}

//-------------------------
PositionIndex _incrHmmAligModel::getModifiedIp(PositionIndex ip,
                                               unsigned int slen,
                                               PositionIndex i)
{
  if(i<=slen && ip>slen)
  {
    return ip-slen;
  }
  else return ip;
}

//-------------------------
double _incrHmmAligModel::log_alpha(PositionIndex /*slen*/,
                                    PositionIndex i,
                                    PositionIndex j,
                                    const std::vector<WordIndex>& /*nsrcSent*/,
                                    const std::vector<WordIndex>& /*trgSent*/)
{
  return alphaMatrix[i][j];
}

//-------------------------
double _incrHmmAligModel::log_beta(PositionIndex /*slen*/,
                                   PositionIndex i,
                                   PositionIndex j,
                                   const std::vector<WordIndex>& /*nsrcSent*/,
                                   const std::vector<WordIndex>& /*trgSent*/)
{
  return betaMatrix[i][j];
}

//-------------------------
void _incrHmmAligModel::updateParsLex(void)
{
        // Update parameters
  for(unsigned int i=0;i<lexAuxVar.size();++i)
  {
    for(LexAuxVarElem::iterator lexAuxVarElemIter=lexAuxVar[i].begin();lexAuxVarElemIter!=lexAuxVar[i].end();++lexAuxVarElemIter)
    {
      WordIndex s=i;
      WordIndex t=lexAuxVarElemIter->first;
      float log_suff_stat_curr=lexAuxVarElemIter->second.first;
      float log_suff_stat_new=lexAuxVarElemIter->second.second;

          // Update parameters only if current and new sufficient statistics
          // are different
      if(log_suff_stat_curr!=log_suff_stat_new)
      {
            // Obtain lexNumer for s,t
        bool found;
        float numer=incrLexTable->getLexNumer(s,t,found);
        if(!found) numer=SMALL_LG_NUM;

            // Obtain lexDenom for s,t
        float denom=incrLexTable->getLexDenom(s,found);
        if(!found) denom=SMALL_LG_NUM;

            // Obtain new sufficient statistics
        float new_numer=obtainLogNewSuffStat(numer,log_suff_stat_curr,log_suff_stat_new);
        float new_denom=denom;
        if(numer!=SMALL_LG_NUM)
          new_denom=MathFuncs::lns_sublog_float(denom,numer);
        new_denom=MathFuncs::lns_sumlog_float(new_denom,new_numer);

            // Set lexical numerator and denominator
        incrLexTable->setLexNumDen(s,t,new_numer,new_denom);
      }
    }
  }
      // Clear auxiliary variables
  lexAuxVar.clear();
}

//-------------------------
void _incrHmmAligModel::updateParsAlig(void)
{
      // Update parameters
  for(AligAuxVar::iterator aligAuxVarIter=aligAuxVar.begin();aligAuxVarIter!=aligAuxVar.end();++aligAuxVarIter)
  {
    aSourceHmm asHmm=aligAuxVarIter->first.first;
    unsigned int i=aligAuxVarIter->first.second;
    float log_suff_stat_curr=aligAuxVarIter->second.first;
    float log_suff_stat_new=aligAuxVarIter->second.second;

        // Update parameters only if current and new sufficient statistics
        // are different
    if(log_suff_stat_curr!=log_suff_stat_new)
    {
          // Obtain aligNumer
      bool found;
      float numer=incrHmmAligTable.getAligNumer(asHmm,i,found);
      if(!found) numer=SMALL_LG_NUM;

          // Obtain aligDenom
      float denom=incrHmmAligTable.getAligDenom(asHmm,found);
      if(!found) denom=SMALL_LG_NUM;

          // Obtain new sufficient statistics
      float new_numer=obtainLogNewSuffStat(numer,log_suff_stat_curr,log_suff_stat_new);
      float new_denom=MathFuncs::lns_sublog_float(denom,numer);
      new_denom=MathFuncs::lns_sumlog_float(new_denom,new_numer);

          // Set lexical numerator and denominator
      incrHmmAligTable.setAligNumDen(asHmm,i,new_numer,new_denom);
    }
  }
      // Clear auxiliary variables
  aligAuxVar.clear();
}

//-------------------------
float _incrHmmAligModel::obtainLogNewSuffStat(float lcurrSuffStat,
                                              float lLocalSuffStatCurr,
                                              float lLocalSuffStatNew)
{
  float lresult=MathFuncs::lns_sublog_float(lcurrSuffStat,lLocalSuffStatCurr);
  lresult=MathFuncs::lns_sumlog_float(lresult,lLocalSuffStatNew);
  return lresult;
}

//-------------------------
Prob _incrHmmAligModel::sentLenProb(unsigned int slen,
                                   unsigned int tlen)
{
  return sentLengthModel.sentLenProb(slen,tlen);
}

//-------------------------
LgProb _incrHmmAligModel::sentLenLgProb(unsigned int slen,
                                        unsigned int tlen)
{
  return sentLengthModel.sentLenLgProb(slen,tlen);
}

//-------------------------
LgProb _incrHmmAligModel::obtainBestAlignmentVecStrCached(std::vector<std::string> srcSentenceVector,
                                                          std::vector<std::string> trgSentenceVector,
                                                          CachedHmmAligLgProb& cached_logap,
                                                          WordAligMatrix& bestWaMatrix)
{
  LgProb lp;
  std::vector<WordIndex> srcSentIndexVector,trgSentIndexVector;

  srcSentIndexVector=strVectorToSrcIndexVector(srcSentenceVector);
  trgSentIndexVector=strVectorToTrgIndexVector(trgSentenceVector);
  lp=obtainBestAlignmentCached(srcSentIndexVector,trgSentIndexVector,cached_logap,bestWaMatrix);

  return lp;
}

//-------------------------
LgProb _incrHmmAligModel::obtainBestAlignment(std::vector<WordIndex> srcSentIndexVector,
                                              std::vector<WordIndex> trgSentIndexVector,
                                              WordAligMatrix& bestWaMatrix)
{
  CachedHmmAligLgProb cached_logap;
  return obtainBestAlignmentCached(srcSentIndexVector,trgSentIndexVector,cached_logap,bestWaMatrix);
}

//-------------------------
LgProb _incrHmmAligModel::obtainBestAlignmentCached(std::vector<WordIndex> srcSentIndexVector,
                                                    std::vector<WordIndex> trgSentIndexVector,
                                                    CachedHmmAligLgProb& cached_logap,
                                                    WordAligMatrix& bestWaMatrix)
{
  if(sentenceLengthIsOk(srcSentIndexVector) && sentenceLengthIsOk(trgSentIndexVector))
  {
        // Obtain extended source vector
    std::vector<WordIndex> nSrcSentIndexVector=extendWithNullWord(srcSentIndexVector);
        // Call function to obtain best lgprob and viterbi alignment
    std::vector<std::vector<double> > vitMatrix;
    std::vector<std::vector<PositionIndex> > predMatrix;
    viterbiAlgorithmCached(nSrcSentIndexVector,
                           trgSentIndexVector,
                           cached_logap,
                           vitMatrix,
                           predMatrix);
    std::vector<PositionIndex> bestAlig;
    LgProb vit_lp=bestAligGivenVitMatrices(srcSentIndexVector.size(),vitMatrix,predMatrix,bestAlig);
        // Obtain best word alignment vector from the Viterbi matrices
    bestWaMatrix.init(srcSentIndexVector.size(),trgSentIndexVector.size());
    bestWaMatrix.putAligVec(bestAlig);

        // Calculate sentence length model lgprob
    LgProb slm_lp=sentLenLgProb(srcSentIndexVector.size(),
                                trgSentIndexVector.size());

    return slm_lp+vit_lp;
  }
  else
  {
    bestWaMatrix.init(srcSentIndexVector.size(),trgSentIndexVector.size());
    return SMALL_LG_NUM;
  }
}

//-------------------------
void _incrHmmAligModel::viterbiAlgorithm(const std::vector<WordIndex>& nSrcSentIndexVector,
                                         const std::vector<WordIndex>& trgSentIndexVector,
                                         std::vector<std::vector<double> >& vitMatrix,
                                         std::vector<std::vector<PositionIndex> >& predMatrix)
{
  CachedHmmAligLgProb cached_logap;
  viterbiAlgorithmCached(nSrcSentIndexVector,trgSentIndexVector,cached_logap,vitMatrix,predMatrix);
}

//-------------------------
void _incrHmmAligModel::viterbiAlgorithmCached(const std::vector<WordIndex>& nSrcSentIndexVector,
                                               const std::vector<WordIndex>& trgSentIndexVector,
                                               CachedHmmAligLgProb& cached_logap,
                                               std::vector<std::vector<double> >& vitMatrix,
                                               std::vector<std::vector<PositionIndex> >& predMatrix)
{
      // Obtain slen
  PositionIndex slen=getSrcLen(nSrcSentIndexVector);

      // Clear matrices
  vitMatrix.clear();
  predMatrix.clear();

      // Make room for matrices
  std::vector<double> dVec;
  dVec.insert(dVec.begin(),trgSentIndexVector.size()+1,SMALL_LG_NUM);
  vitMatrix.insert(vitMatrix.begin(),nSrcSentIndexVector.size()+1,dVec);

  std::vector<PositionIndex> pidxVec;
  pidxVec.insert(pidxVec.begin(),trgSentIndexVector.size()+1,0);
  predMatrix.insert(predMatrix.begin(),nSrcSentIndexVector.size()+1,pidxVec);

      // Initialize data structure to cache lexical log-probs
  std::vector<std::vector<double> > cached_logpts;
  initCachedLexicalLps(nSrcSentIndexVector,trgSentIndexVector,cached_logpts);

      // Fill matrices
  for(PositionIndex j=1;j<=trgSentIndexVector.size();++j)
  {
    for(PositionIndex i=1;i<=nSrcSentIndexVector.size();++i)
    {
      if(j==1)
      {
            // Update cached alignment log-probs if required
        if(!cached_logap.isDefined(0,slen,i))
          cached_logap.set_boundary_check(0,slen,i,logaProb(0,slen,i));

            // Update matrices
        vitMatrix[i][j]=cached_logap.get(0,slen,i)+cached_logpts[i][j];
        predMatrix[i][j]=0;
      }
      else
      {
        for(PositionIndex i_tilde=1;i_tilde<=nSrcSentIndexVector.size();++i_tilde)
        {
              // Update cached alignment log-probs if required
          if(!cached_logap.isDefined(i_tilde,slen,i))
            cached_logap.set_boundary_check(i_tilde,slen,i,logaProb(i_tilde,slen,i));

              // Update matrices
          double lp=vitMatrix[i_tilde][j-1]+
                    cached_logap.get(i_tilde,slen,i)+
                    cached_logpts[i][j];
          if(lp>vitMatrix[i][j])
          {
            vitMatrix[i][j]=lp;
            predMatrix[i][j]=i_tilde;
          }
        }
      }
    }
  }
}

//-------------------------
double _incrHmmAligModel::bestAligGivenVitMatricesRaw(const std::vector<std::vector<double> >& vitMatrix,
                                                      const std::vector<std::vector<PositionIndex> >& predMatrix,
                                                      std::vector<PositionIndex>& bestAlig)
{
  if(vitMatrix.size()<=1 || predMatrix.size()<=1)
  {
        // if vitMatrix.size()==1 or predMatrix.size()==1, then the
        // source or the target sentences respectively were empty, so
        // there is no word alignment to be returned
    bestAlig.clear();
    return 0;
  }
  else
  {
        // Initialize bestAlig
    bestAlig.clear();
    bestAlig.insert(bestAlig.begin(),predMatrix[0].size()-1,0);

        // Find last word alignment
    PositionIndex last_j=predMatrix[1].size()-1;
    double bestLgProb=vitMatrix[1][last_j];
    bestAlig[last_j-1]=1;
    for(unsigned int i=2;i<=vitMatrix.size()-1;++i)
    {
      if(bestLgProb<vitMatrix[i][last_j])
      {
        bestLgProb=vitMatrix[i][last_j];
        bestAlig[last_j-1]=i;
      }
    }

        // Retrieve remaining alignments
    for(unsigned int j=last_j;j>1;--j)
    {
      bestAlig[j-2]=predMatrix[bestAlig[j-1]][j];
    }
      
        // Return best log-probability
    return bestLgProb;
  }
}

//-------------------------
double _incrHmmAligModel::bestAligGivenVitMatrices(PositionIndex slen,
                                                   const std::vector<std::vector<double> >& vitMatrix,
                                                   const std::vector<std::vector<PositionIndex> >& predMatrix,
                                                   std::vector<PositionIndex>& bestAlig)
{
  double LgProb=bestAligGivenVitMatricesRaw(vitMatrix,predMatrix,bestAlig);

      // Set null word alignments appropriately
  for(unsigned int j=0;j<bestAlig.size();++j)
  {
    if(bestAlig[j]>slen)
      bestAlig[j]=NULL_WORD;
  }

  return LgProb;
}

//-------------------------
LgProb _incrHmmAligModel::calcLgProbForAlig(const std::vector<WordIndex>& /*sSent*/,
                                            const std::vector<WordIndex>& /*tSent*/,
                                            WordAligMatrix /*aligMatrix*/,
                                            int /*verbose*/)
{
      // TO-DO (post-thesis)
  return 0;
}

//-------------------------
LgProb _incrHmmAligModel::calcLgProb(const std::vector<WordIndex>& sSent,
                                     const std::vector<WordIndex>& tSent,
                                     int verbose)
{
  if(sentenceLengthIsOk(sSent) && sentenceLengthIsOk(tSent))
  {
        // Calculate sentence length model lgprob
    LgProb slp=sentLenLgProb(sSent.size(),tSent.size());
    
        // Obtain extended source vector
    std::vector<WordIndex> nSrcSentIndexVector=extendWithNullWord(sSent);
    
        // Calculate hmm lgprob
    LgProb flp=forwardAlgorithm(nSrcSentIndexVector,
                                tSent,
                                verbose);

    if(verbose)
      std::cerr<<"lp= "<<slp+flp<<" ; slm_lp= "<<slp<<" ; lp-slm_lp= "<<flp<<std::endl;
    
    return slp+flp;
  }
  else
  {
    return SMALL_LG_NUM;
  }

}

//-------------------------
double _incrHmmAligModel::forwardAlgorithm(const std::vector<WordIndex>& nSrcSentIndexVector,
                                           const std::vector<WordIndex>& trgSentIndexVector,
                                           int verbose)
{
      // Obtain slen
  PositionIndex slen=getSrcLen(nSrcSentIndexVector);

  
      // Make room for matrix
  std::vector<std::vector<double> > forwardMatrix;
  std::vector<double> dVec;
  dVec.insert(dVec.begin(),trgSentIndexVector.size()+1,0.0);
  forwardMatrix.insert(forwardMatrix.begin(),nSrcSentIndexVector.size()+1,dVec);

      // Initialize data structure to cache lexical log-probs
  std::vector<std::vector<double> > cached_logpts;
  initCachedLexicalLps(nSrcSentIndexVector,trgSentIndexVector,cached_logpts);

      // Fill matrix
  for(PositionIndex j=1;j<=trgSentIndexVector.size();++j)
  {
    for(PositionIndex i=1;i<=nSrcSentIndexVector.size();++i)
    {
      if(j==1)
      {
        forwardMatrix[i][j]=logaProb(0,slen,i)+cached_logpts[i][j];
      }
      else
      {
        for(PositionIndex i_tilde=1;i_tilde<=nSrcSentIndexVector.size();++i_tilde)
        {
          double lp=forwardMatrix[i_tilde][j-1]+
            (double)logaProb(i_tilde,slen,i)+
            cached_logpts[i][j];
          if(i_tilde==1)
            forwardMatrix[i][j]=lp;
          else
            forwardMatrix[i][j]=MathFuncs::lns_sumlog(lp,forwardMatrix[i][j]);
        }
      }
    }
  }

      // Obtain lgProb from forward matrix
  double lp=lgProbGivenForwardMatrix(forwardMatrix);

      // Print verbose info
  if(verbose>1)
  {
        // Clear cached values
    for(PositionIndex j=1;j<=trgSentIndexVector.size();++j)
    {
      for(PositionIndex i=1;i<=nSrcSentIndexVector.size();++i)
      {
        std::cerr<<"i="<<i<<",j="<<j<<" "<<forwardMatrix[i][j];
        if(i<nSrcSentIndexVector.size()) std::cerr<<" ; ";
      }
      std::cerr<<std::endl;
    }
  }

      // Return result
  return lp;
}

//-------------------------
double _incrHmmAligModel::lgProbGivenForwardMatrix(const std::vector<std::vector<double> >& forwardMatrix)
{
      // Sum lgprob for each i
  double lp=SMALL_LG_NUM;
  PositionIndex last_j=forwardMatrix[1].size()-1;
  for(unsigned int i=1;i<=forwardMatrix.size()-1;++i)
  {
    if(i==1)
    {
      lp=forwardMatrix[i][last_j];
    }
    else
    {
      lp=MathFuncs::lns_sumlog(lp,forwardMatrix[i][last_j]);
    }
  }

      // Return result
  return lp;
}

//-------------------------
LgProb _incrHmmAligModel::calcLgProbPhr(const std::vector<WordIndex>& sPhr,
                                        const std::vector<WordIndex>& tPhr,
                                        int verbose/*=0*/)
{
//  return calcVitIbm1LgProb(sPhr,tPhr);
//  return calcSumIBM1LgProb(sPhr,tPhr,verbose);
  return noisyOrLgProb(sPhr,tPhr,verbose);
}

//-------------------------
LgProb _incrHmmAligModel::calcVitIbm1LgProb(const std::vector<WordIndex>& srcSentIndexVector,
                                            const std::vector<WordIndex>& trgSentIndexVector)
{
 LgProb aligLgProb;
 LgProb lp;
 LgProb max_lp;
 std::vector<WordIndex> nSrcSentIndexVector=addNullWordToWidxVec(srcSentIndexVector);

 aligLgProb=0;
 for(unsigned int j=0;j<trgSentIndexVector.size();++j)
 {
   max_lp=-FLT_MAX;
   for(unsigned int i=0;i<nSrcSentIndexVector.size();++i)
   {
     lp=log((double)pts(nSrcSentIndexVector[i],trgSentIndexVector[j]));
     if(max_lp<=lp)
     {
       max_lp=lp;
     }
   }
   aligLgProb=aligLgProb+max_lp;
 }

 return aligLgProb;
}

//-------------------------
LgProb _incrHmmAligModel::calcSumIBM1LgProb(const std::vector<WordIndex>& sSent,
                                            const std::vector<WordIndex>& tSent,
                                            int verbose)
{
 LgProb sumlp;
 LgProb lexContrib;
 LgProb lgProb;    
 unsigned int i,j;
 std::vector<WordIndex> nsSent=addNullWordToWidxVec(sSent);

 if(verbose) std::cerr<<"Obtaining Sum IBM Model 1 logprob...\n"; 
     
 lgProb=logaProbIbm1(sSent.size(),tSent.size());
 
 if(verbose) std::cerr<<"- aligLgProb(tlen="<<tSent.size() <<" | slen="<<sSent.size()<<")= "<<logaProbIbm1(sSent.size(),tSent.size())<<std::endl;

 lgProb+=sentLenLgProb(sSent.size(),tSent.size());
 if(verbose) std::cerr<<"- lenLgProb(tlen="<<tSent.size() <<" | slen="<<sSent.size()<<")= "<<sentLenLgProb(sSent.size(),tSent.size())<<std::endl;

 lexContrib=0;
 for(j=0;j<tSent.size();++j)
 {
   for(i=0;i<nsSent.size();++i)
   {
     if(i==0) sumlp=logpts(nsSent[i],tSent[j]);
     else
       sumlp=MathFuncs::lns_sumlog(logpts(nsSent[i],tSent[j]),sumlp);
     if(verbose==2)
       std::cerr<<"log(t( " <<tSent[j] <<" | " <<nsSent[i]<<" ))= "<<logpts(nsSent[i],tSent[j]) <<std::endl;
   }
   lexContrib+=sumlp;  
   if(verbose) std::cerr<<"- log(sumt(j="<<j<<"))= "<<sumlp<<std::endl;
   if(verbose==2) std::cerr<<std::endl;
 }
 if(verbose) std::cerr<<"- Lexical model contribution= "<<lexContrib<<std::endl;
 lgProb+=lexContrib;
 
 return lgProb;
}

//-------------------------
LgProb _incrHmmAligModel::logaProbIbm1(PositionIndex slen,
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
LgProb _incrHmmAligModel::noisyOrLgProb(const std::vector<WordIndex>& sSent,
                                        const std::vector<WordIndex>& tSent,
                                        int verbose)
{
 LgProb lp=0;    
 for(unsigned int j=0;j<tSent.size();++j)
 {
   Prob prob=1;
   for(unsigned int i=0;i<sSent.size();++i)
   {
     prob=prob*(1.0-(double)pts(sSent[i],tSent[j]));
     
     if(verbose==2)
       std::cerr<<"t( " <<tSent[j] <<" | " <<sSent[i]<<" )= "<<pts(sSent[i],tSent[j]) <<std::endl;
   }
   Prob compProb=1.0-(double)prob;
   if((double)compProb==0.0)
     lp=lp+(double)SMALL_LG_NUM;
   else
     lp=lp+compProb.get_lp();
   
   if(verbose) std::cerr<<"- log(1-prod(j="<<j<<"))= "<<lp<<std::endl;
   if(verbose==2) std::cerr<<std::endl;
 }
 return lp;
}

//-------------------------
void _incrHmmAligModel::initPpInfo(unsigned int /*slen*/,
                                   const std::vector<WordIndex>& /*tSent*/,
                                   PpInfo& /*ppInfo*/)
{
      // TO-DO (post-thesis)
}

//-------------------------
void _incrHmmAligModel::partialProbWithoutLen(unsigned int /*srcPartialLen*/,
                                              unsigned int /*slen*/,
                                              const std::vector<WordIndex>& /*s_*/,
                                              const std::vector<WordIndex>& /*tSent*/,
                                              PpInfo& /*ppInfo*/)
{
      // TO-DO (post-thesis)
}

//-------------------------
LgProb _incrHmmAligModel::lpFromPpInfo(const PpInfo& /*ppInfo*/)
{
      // TO-DO (post-thesis)
  return 0;
}

//-------------------------
void _incrHmmAligModel::addHeurForNotAddedWords(int /*numSrcWordsToBeAdded*/,
                                                const std::vector<WordIndex>& /*tSent*/,
                                                PpInfo& /*ppInfo*/)
{
      // TO-DO (post-thesis)
}

//-------------------------
void _incrHmmAligModel::sustHeurForNotAddedWords(int /*numSrcWordsToBeAdded*/,
                                                 const std::vector<WordIndex>& /*tSent*/,
                                                 PpInfo& /*ppInfo*/)
{
      // TO-DO (post-thesis)
}

//-------------------------
bool _incrHmmAligModel::load(const char* prefFileName)
{
  if(prefFileName[0]!=0)
  {
    bool retVal;

    std::cerr<<"Loading incremental HMM Model data..."<<std::endl;

        // Load vocabularies if they exist
    std::string srcVocFileName=prefFileName;
    srcVocFileName=srcVocFileName+".svcb";
    loadGIZASrcVocab(srcVocFileName.c_str());

    std::string trgVocFileName=prefFileName;
    trgVocFileName=trgVocFileName+".tvcb";    
    loadGIZATrgVocab(trgVocFileName.c_str());

        // Load files with source and target sentences
        // Warning: this must be made before reading file with lanji
        // values
    std::string srcsFile=prefFileName;
    srcsFile=srcsFile+".src";
    std::string trgsFile=prefFileName;
    trgsFile=trgsFile+".trg";
    std::string srctrgcFile=prefFileName;
    srctrgcFile=srctrgcFile+".srctrgc";
    std::pair<unsigned int,unsigned int> pui;
    retVal=readSentencePairs(srcsFile.c_str(),trgsFile.c_str(),srctrgcFile.c_str(),pui);
    if(retVal==THOT_ERROR) return THOT_ERROR;

        // Load file with lanji values
    retVal=lanji.load(prefFileName);
    if(retVal==THOT_ERROR) return THOT_ERROR;

        // Load file with lanjm1ip_anji values
    retVal=lanjm1ip_anji.load(prefFileName);
    if(retVal==THOT_ERROR) return THOT_ERROR;

        // Load file with lexical nd values
    std::string lexNumDenFile=prefFileName;
    lexNumDenFile=lexNumDenFile+lexNumDenFileExtension;
    retVal=incrLexTable->load(lexNumDenFile.c_str());
    if(retVal==THOT_ERROR) return THOT_ERROR;

        // Load file with alignment nd values
    std::string aligNumDenFile=prefFileName;
    aligNumDenFile=aligNumDenFile+".hmm_alignd";
    retVal=incrHmmAligTable.load(aligNumDenFile.c_str());
    if(retVal==THOT_ERROR) return THOT_ERROR;

        // Load file with with lexical smoothing interpolation factor
    std::string lsifFile=prefFileName;
    lsifFile=lsifFile+".lsifactor";
    retVal=loadLexSmIntFactor(lsifFile.c_str());
    if(retVal==THOT_ERROR) return THOT_ERROR;

        // Load file with with alignment smoothing interpolation factor
    std::string asifFile=prefFileName;
    asifFile=asifFile+".asifactor";
    retVal=loadAlSmIntFactor(asifFile.c_str());
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
bool _incrHmmAligModel::print(const char* prefFileName)
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

      // Print file with lanji values
  retVal=lanji.print(prefFileName);
  if(retVal==THOT_ERROR) return THOT_ERROR;

      // Print file with lanjm1ip_anji values
  retVal=lanjm1ip_anji.print(prefFileName);
  if(retVal==THOT_ERROR) return THOT_ERROR;

      // Print file with lexical nd values
  std::string lexNumDenFile=prefFileName;
  lexNumDenFile=lexNumDenFile+lexNumDenFileExtension;
  retVal=incrLexTable->print(lexNumDenFile.c_str());
  if(retVal==THOT_ERROR) return THOT_ERROR;

      // Print file with alignment nd values
  std::string aligNumDenFile=prefFileName;
  aligNumDenFile=aligNumDenFile+".hmm_alignd";
  retVal=incrHmmAligTable.print(aligNumDenFile.c_str());
  if(retVal==THOT_ERROR) return THOT_ERROR;

      // Print file with with lexical smoothing interpolation factor
  std::string lsifFile=prefFileName;
  lsifFile=lsifFile+".lsifactor";
  retVal=printLexSmIntFactor(lsifFile.c_str());
  if(retVal==THOT_ERROR) return THOT_ERROR;

      // Print file with with alignment smoothing interpolation factor
  std::string asifFile=prefFileName;
  asifFile=asifFile+".asifactor";
  retVal=printAlSmIntFactor(asifFile.c_str());
  if(retVal==THOT_ERROR) return THOT_ERROR;

      // Print file with sentence length model
  std::string slmodelFile=prefFileName;
  slmodelFile=slmodelFile+".slmodel";
  retVal=sentLengthModel.print(slmodelFile.c_str());
  if(retVal==THOT_ERROR) return THOT_ERROR;

  return THOT_OK;
}

//-------------------------
void _incrHmmAligModel::clear(void)
{
  _swAligModel<std::vector<Prob> >::clear();
  lanji.clear();
  lanji_aux.clear();
  lanjm1ip_anji.clear();
  lanjm1ip_anji_aux.clear();
  alphaMatrix.clear();
  betaMatrix.clear();
  incrLexTable->clear();
  incrHmmAligTable.clear();
  sentLengthModel.clear();
}

//-------------------------
void _incrHmmAligModel::clearTempVars(void)
{
}

//-------------------------
_incrHmmAligModel::~_incrHmmAligModel(void)
{
  delete incrLexTable;
}
