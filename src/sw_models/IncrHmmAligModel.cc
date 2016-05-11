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
/* Module: IncrHmmAligModel                                         */
/*                                                                  */
/* Definitions file: IncrHmmAligModel.cc                            */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "IncrHmmAligModel.h"

//--------------- IncrHmmAligModel class function definitions

//-------------------------
IncrHmmAligModel::IncrHmmAligModel()
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
void IncrHmmAligModel::set_expval_maxnsize(unsigned int _expval_maxnsize)
{
  lanji.set_maxnsize(_expval_maxnsize);
  lanjm1ip_anji.set_maxnsize(_expval_maxnsize);
}

//-------------------------   
unsigned int IncrHmmAligModel::numSentPairs(void)
{
  return sentenceHandler.numSentPairs();
}

//-------------------------
void IncrHmmAligModel::trainSentPairRange(pair<unsigned int,unsigned int> sentPairRange,
                                          int verbosity)
{
      // Train sentence length model
  sentLengthModel.trainSentPairRange(sentPairRange,verbosity);
      
      // EM algorithm
  calcNewLocalSuffStats(sentPairRange,verbosity);
  updateParsLex();
  updateParsAlig();
}

//-------------------------   
void IncrHmmAligModel::trainAllSents(int verbosity)
{
  trainSentPairRange(make_pair(0,numSentPairs()-1),verbosity);
}

//-------------------------
void IncrHmmAligModel::efficientBatchTrainingForRange(pair<unsigned int,unsigned int> sentPairRange,
                                                      int verbosity/*=0*/)
{
      // Set maximum size of the matrices of expected values to zero
  set_expval_maxnsize(0);

      // Train sentence length model
  sentLengthModel.trainSentPairRange(sentPairRange,verbosity);

      // EM algorithm
  calcNewLocalSuffStats(sentPairRange,verbosity);
  incrLexTable.clear();
  incrHmmAligTable.clear();
  updateParsLex();
  updateParsAlig();
}

//-------------------------
pair<double,double> IncrHmmAligModel::loglikelihoodForPairRange(pair<unsigned int,unsigned int> sentPairRange,
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
    if(!nthSrcSent.empty() && !nthTrgSent.empty())
    {
      loglikelihood+=(double)calcLgProb(nthSrcSent,nthTrgSent,verbosity);
      ++numSents;
    }
  }
  return make_pair(loglikelihood,loglikelihood/(double) numSents);
}

//-------------------------
void IncrHmmAligModel::clearInfoAboutSentRange(void)
{
      // Clear info about sentence range
  sentenceHandler.clear();
  lanji.clear();
  lanji_aux.clear();
  lanjm1ip_anji.clear();
  lanjm1ip_anji_aux.clear();
}

//-------------------------
void IncrHmmAligModel::setLexSmIntFactor(double _lexSmoothInterpFactor)
{
  lexSmoothInterpFactor=_lexSmoothInterpFactor;
  cerr<<"Lexical smoothing interpolation factor has been set to "<<lexSmoothInterpFactor<<endl;
}

//-------------------------
Prob IncrHmmAligModel::pts(WordIndex s,
                           WordIndex t)
{
  return exp((double)logpts(s,t));  
}

//-------------------------   
double IncrHmmAligModel::unsmoothed_logpts(WordIndex s,
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
double IncrHmmAligModel::cached_logpts(PositionIndex i,
                                       PositionIndex j,
                                       const Vector<WordIndex>& nsrcSent,
                                       const Vector<WordIndex>& trgSent)
{
  double d=cachedLogProbtsDm.get(i,j);
  if(d!=INVALID_DM_VAL)
    return d;
  else
  {
    d=(double) logpts(nsrcSent[i-1],trgSent[j-1]);
    cachedLogProbtsDm.set(i,j,d);
    return d;
  }
}

//-------------------------
LgProb IncrHmmAligModel::logpts(WordIndex s,
                                WordIndex t)
{
  LgProb lexLgProb=(LgProb)log(1.0-lexSmoothInterpFactor)+unsmoothed_logpts(s,t);
  LgProb smoothLgProb=log(lexSmoothInterpFactor)+log(1.0/(double)(getTrgVocabSize()));
  return MathFuncs::lns_sumlog(lexLgProb,smoothLgProb);
}

//-------------------------
void IncrHmmAligModel::setAlSmIntFactor(double _aligSmoothInterpFactor)
{
  aligSmoothInterpFactor=_aligSmoothInterpFactor;
  cerr<<"Alignment smoothing interpolation factor has been set to "<<aligSmoothInterpFactor<<endl;
}

//-------------------------
Prob IncrHmmAligModel::aProb(PositionIndex prev_i,
                             PositionIndex slen,
                             PositionIndex i)
{
  return exp((double)logaProb(prev_i,slen,i));
}

//-------------------------
double IncrHmmAligModel::unsmoothed_logaProb(PositionIndex prev_i,
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
double IncrHmmAligModel::cached_logaProb(PositionIndex prev_i,
                                         PositionIndex slen,
                                         PositionIndex i,
                                         const Vector<WordIndex>& /*nsrcSent*/,
                                         const Vector<WordIndex>& /*trgSent*/)
{
  double d=cachedLogaProbDm.get(prev_i,i);
  if(d!=INVALID_DM_VAL)
    return d;
  else
  {
    d=(double)logaProb(prev_i,slen,i);
    cachedLogaProbDm.set(prev_i,i,d);
    return d;
  }
}

//-------------------------
void IncrHmmAligModel::nullAligSpecialPar(unsigned int ip,
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
LgProb IncrHmmAligModel::logaProb(PositionIndex prev_i,
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
Vector<WordIndex> IncrHmmAligModel::getSrcSent(unsigned int n)
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
Vector<WordIndex> IncrHmmAligModel::extendWithNullWord(const Vector<WordIndex>& srcWordIndexVec)
{
      // Initialize result using srcWordIndexVec
  Vector<WordIndex> result=srcWordIndexVec;
  
      // Add NULL words
  WordIndex nullWidx=stringToSrcWordIndex(NULL_WORD_STR);
  for(unsigned int i=0;i<srcWordIndexVec.size();++i)
    result.push_back(nullWidx);

  return result;
}

//-------------------------
Vector<WordIndex> IncrHmmAligModel::extendWithNullWordAlig(const Vector<WordIndex>& srcWordIndexVec)
{
  return extendWithNullWord(srcWordIndexVec);
}

//-------------------------
PositionIndex IncrHmmAligModel::getSrcLen(const Vector<WordIndex>& nsrcWordIndexVec)
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
Vector<WordIndex> IncrHmmAligModel::getTrgSent(unsigned int n)
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
bool IncrHmmAligModel::loadLexSmIntFactor(const char* lexSmIntFactorFile)
{
  cerr<<"Loading file with lexical smoothing interpolation factor from "<<lexSmIntFactorFile<<endl;
  
  awkInputStream awk;
    
  if(awk.open(lexSmIntFactorFile)==ERROR)
  {
    cerr<<"Error in file with lexical smoothing interpolation factor, file "<<lexSmIntFactorFile<<" does not exist. Assuming default value."<<"\n";
    setLexSmIntFactor(DEFAULT_LEX_SMOOTH_INTERP_FACTOR); 
    return OK;
  }
  else
  {
    if(awk.getln())
    {
      if(awk.NF==1)
      {
        setLexSmIntFactor((Prob)atof(awk.dollar(1).c_str())); 
        return OK;
      }
      else
      {
        cerr<<"Error: anomalous .lsifactor file, "<<lexSmIntFactorFile<<endl;
        return ERROR;
      }
    }
    else
    {
      cerr<<"Error: anomalous .lsifactor file, "<<lexSmIntFactorFile<<endl;
      return ERROR;
    }
  }  
}

//-------------------------   
bool IncrHmmAligModel::printLexSmIntFactor(const char* lexSmIntFactorFile)
{
  ofstream outF;
  outF.open(lexSmIntFactorFile,ios::out);
  if(!outF)
  {
    cerr<<"Error while printing file with lexical smoothing interpolation factor."<<endl;
    return ERROR;
  }
  else
  {
    outF<<lexSmoothInterpFactor<<endl;
    return OK;
  }
}

//-------------------------   
bool IncrHmmAligModel::loadAlSmIntFactor(const char* alSmIntFactorFile)
{
  cerr<<"Loading file with alignment smoothing interpolation factor from "<<alSmIntFactorFile<<endl;
  
  awkInputStream awk;
    
  if(awk.open(alSmIntFactorFile)==ERROR)
  {
    cerr<<"Error in file with alignment smoothing interpolation factor, file "<<alSmIntFactorFile<<" does not exist. Assuming default value."<<"\n";
    setAlSmIntFactor(DEFAULT_ALIG_SMOOTH_INTERP_FACTOR); 
    return OK;
  }  
  else
  {
    if(awk.getln())
    {
      if(awk.NF==1)
      {
        setAlSmIntFactor((Prob)atof(awk.dollar(1).c_str())); 
        return OK;
      }
      else
      {
        cerr<<"Error: anomalous .asifactor file, "<<alSmIntFactorFile<<endl;
        return ERROR;
      }
    }
    else
    {
      cerr<<"Error: anomalous .asifactor file, "<<alSmIntFactorFile<<endl;
      return ERROR;
    }
  }  
}

//-------------------------   
bool IncrHmmAligModel::printAlSmIntFactor(const char* alSmIntFactorFile)
{
  ofstream outF;
  outF.open(alSmIntFactorFile,ios::out);
  if(!outF)
  {
    cerr<<"Error while printing file with alignment smoothing interpolation factor."<<endl;
    return ERROR;
  }
  else
  {
    outF<<aligSmoothInterpFactor<<endl;
    return OK;
  }
}

//-------------------------   
void IncrHmmAligModel::calcNewLocalSuffStats(pair<unsigned int,unsigned int> sentPairRange,
                                             int /*verbosity*/)
{
      // Iterate over the training samples
  for(unsigned int n=sentPairRange.first;n<=sentPairRange.second;++n)
  {
        // Init vars for n'th sample
    Vector<WordIndex> srcSent=getSrcSent(n);
    Vector<WordIndex> trgSent=getTrgSent(n);

        // Process sentence pair only if both sentences are not empty
    if(!srcSent.empty() && !trgSent.empty())
    {
      Count weight;
      sentenceHandler.getCount(n,weight);
    
          // Calculate suff. stats. for anji values
      calc_lanji(n,extendWithNullWord(srcSent),trgSent,weight);
    
          // Calculate suff. stats. for anjm1ip_anji values
      calc_lanjm1ip_anji(n,extendWithNullWordAlig(srcSent),trgSent,weight);

          // Clear cached alpha and beta values
      alpha_values.clear();
      beta_values.clear();

          // Clear auxiliary log prob double matrices
      cachedLogProbtsDm.clear();
      cachedLogaProbDm.clear();
    }
  }
}

//-------------------------   
void IncrHmmAligModel::calc_lanji(unsigned int n,
                                  const Vector<WordIndex>& nsrcSent,
                                  const Vector<WordIndex>& trgSent,
                                  const Count& weight)
{
  unsigned int np=1;

  PositionIndex slen=getSrcLen(nsrcSent);

      // Calculate new estimation of lanji
  for(unsigned int j=1;j<=trgSent.size();++j)
  {
        // Obtain sum_lanji_num_forall_s
    double sum_lanji_num_forall_s=INVALID_ANJI_VAL;
    Vector<double> numVec;
    numVec.push_back(0);
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
      numVec.push_back(d);
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
      lanji_aux.set(np,j,i,lanji_val);
    }
  }

      // Gather lexical sufficient statistics
  for(unsigned int j=1;j<=trgSent.size();++j)
  {
    for(unsigned int i=1;i<=nsrcSent.size();++i)
    {
          // Reestimate lexical parameters
      fillEmAuxVarsLex(n,np,i,j,nsrcSent,trgSent,weight);
      
          // Update lanji
      lanji.set(n,j,i,lanji_aux.get_invlogp(np,j,i));
    }
  }
  
      // clear lanji_aux data structure
  lanji_aux.clear();
}

//-------------------------   
void IncrHmmAligModel::calc_lanjm1ip_anji(unsigned int n,
                                          const Vector<WordIndex>& nsrcSent,
                                          const Vector<WordIndex>& trgSent,
                                          const Count& weight)
{
  unsigned int np=1;

  PositionIndex slen=getSrcLen(nsrcSent);

      // Calculate new estimation of lanjm1ip_anji
  for(unsigned int j=1;j<=trgSent.size();++j)
  {
        // Obtain sum_lanjm1ip_anji_num_forall_i_ip
    double sum_lanjm1ip_anji_num_forall_i_ip=INVALID_ANJM1IP_ANJI_VAL;
    Vector<Vector<double> > numVecVec;
    Vector<double> numVec;
    numVecVec.push_back(numVec);
    numVec.push_back(0);
    for(unsigned int i=1;i<=nsrcSent.size();++i)
    {
      numVecVec.push_back(numVec);
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
          numVecVec[i].push_back(d);
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
        lanjm1ip_anji_aux.set(np,j,i,0,lanjm1ip_anji_val);
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
            lanjm1ip_anji_aux.set(np,j,i,ip,lanjm1ip_anji_val);
          }
        }
      }
    }
  }
      // Gather alignment sufficient statistics
  gatherAligSuffStats(n,np,nsrcSent,trgSent,weight);

      // clear lanjm1ip_anji_aux data structure
  lanjm1ip_anji_aux.clear();
}

//-------------------------   
void IncrHmmAligModel::gatherAligSuffStats(unsigned int n,
                                           unsigned int np,
                                           const Vector<WordIndex>& nsrcSent,
                                           const Vector<WordIndex>& trgSent,
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
            // Obtain information about alignment
            // Reestimate alignment parameters
        fillEmAuxVarsAlig(n,np,slen,0,i,j,weight);
            
            // Update lanjm1ip_anji
        lanjm1ip_anji.set(n,j,i,0,lanjm1ip_anji_aux.get_invlogp(np,j,i,0));
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
            fillEmAuxVarsAlig(n,np,slen,ip,i,j,weight);
                // Update lanjm1ip_anji
            lanjm1ip_anji.set(n,j,i,ip,lanjm1ip_anji_aux.get_invlogp(np,j,i,ip));
          }
        }
      }
    }
  }
}

//-------------------------   
bool IncrHmmAligModel::isFirstNullAligPar(PositionIndex ip,
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
double IncrHmmAligModel::calc_lanji_num(PositionIndex slen,
                                        PositionIndex i,
                                        PositionIndex j,
                                        const Vector<WordIndex>& nsrcSent,
                                        const Vector<WordIndex>& trgSent)
{
  double result=log_alpha(slen,i,j,nsrcSent,trgSent)+log_beta(slen,i,j,nsrcSent,trgSent);
  if(result<SMALL_LG_NUM) result=SMALL_LG_NUM;
  return result;
}

//-------------------------   
double IncrHmmAligModel::calc_lanjm1ip_anji_num_je1(PositionIndex slen,
                                                    PositionIndex i,                                                    
                                                    const Vector<WordIndex>& nsrcSent,
                                                    const Vector<WordIndex>& trgSent)
{
  double result=(double)cached_logaProb(0,slen,i,nsrcSent,trgSent)+
    (double)cached_logpts(i,1,nsrcSent,trgSent)+
    log_beta(slen,i,1,nsrcSent,trgSent);
  if(result<SMALL_LG_NUM) result=SMALL_LG_NUM;
  return result;
}

//-------------------------   
double IncrHmmAligModel::calc_lanjm1ip_anji_num_jg1(PositionIndex ip,
                                                    PositionIndex slen,
                                                    PositionIndex i,
                                                    PositionIndex j,
                                                    const Vector<WordIndex>& nsrcSent,
                                                    const Vector<WordIndex>& trgSent)
{
  double result=log_alpha(slen,ip,j-1,nsrcSent,trgSent)+
    (double)cached_logaProb(ip,slen,i,nsrcSent,trgSent)+
    (double)cached_logpts(i,j,nsrcSent,trgSent)+
    log_beta(slen,i,j,nsrcSent,trgSent);
  if(result<SMALL_LG_NUM) result=SMALL_LG_NUM;
  return result;
}

//-------------------------   
void IncrHmmAligModel::getHmmAligInfo(PositionIndex ip,
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
bool IncrHmmAligModel::isValidAlig(PositionIndex ip,
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
bool IncrHmmAligModel::isNullAlig(PositionIndex ip,
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
PositionIndex IncrHmmAligModel::getModifiedIp(PositionIndex ip,
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
double IncrHmmAligModel::log_alpha(PositionIndex slen,
                                   PositionIndex i,
                                   PositionIndex j,
                                   const Vector<WordIndex>& nsrcSent,
                                   const Vector<WordIndex>& trgSent)
{
  double cached_alpha=alpha_values.get(i,j);
  if(cached_alpha!=INVALID_DM_VAL)
  {
    return cached_alpha;
  }
  else
  {
    double d=log_alpha_rec(slen,i,j,nsrcSent,trgSent);
    alpha_values.set(i,j,d);
    return d;
  }
}

//-------------------------   
double IncrHmmAligModel::log_alpha_rec(PositionIndex slen,
                                       PositionIndex i,
                                       PositionIndex j,
                                       const Vector<WordIndex>& nsrcSent,
                                       const Vector<WordIndex>& trgSent)
{
  if(j==1)
  {
    return cached_logaProb(0,slen,i,nsrcSent,trgSent)+cached_logpts(i,j,nsrcSent,trgSent);
  }
  else
  {
    LgProb result;
    for(PositionIndex i_tilde=1;i_tilde<=nsrcSent.size();++i_tilde)
    {
      LgProb lp=log_alpha(slen,i_tilde,j-1,nsrcSent,trgSent)+
        (double)cached_logaProb(i_tilde,slen,i,nsrcSent,trgSent)+
        (double)cached_logpts(i,j,nsrcSent,trgSent);
      if(i_tilde==1) result=lp;
      else result=MathFuncs::lns_sumlog(lp,result);
    }
    return result;
  }
}

//-------------------------   
double IncrHmmAligModel::log_beta(PositionIndex slen,
                                  PositionIndex i,
                                  PositionIndex j,
                                  const Vector<WordIndex>& nsrcSent,
                                  const Vector<WordIndex>& trgSent)
{
  double cached_beta=beta_values.get(i,j);
  if(cached_beta!=INVALID_DM_VAL)
  {
    return cached_beta;
  }
  else
  {
    double d=log_beta_rec(slen,i,j,nsrcSent,trgSent);
    beta_values.set(i,j,d);
    return d;
  }
}

//-------------------------   
double IncrHmmAligModel::log_beta_rec(PositionIndex slen,
                                      PositionIndex i,
                                      PositionIndex j,
                                      const Vector<WordIndex>& nsrcSent,
                                      const Vector<WordIndex>& trgSent)
{
  if(j==trgSent.size())
  {
    return log(1.0);
  }
  else
  {
    LgProb result;
    for(PositionIndex i_tilde=1;i_tilde<=nsrcSent.size();++i_tilde)
    {
      LgProb lp=(double)cached_logaProb(i,slen,i_tilde,nsrcSent,trgSent)+
        (double)cached_logpts(i_tilde,j+1,nsrcSent,trgSent)+
        log_beta(slen,i_tilde,j+1,nsrcSent,trgSent);
      if(i_tilde==1) result=lp;
      else result=MathFuncs::lns_sumlog(lp,result);
    }
    return result;
  }
}

//-------------------------   
void IncrHmmAligModel::fillEmAuxVarsLex(unsigned int n,
                                        unsigned int np,
                                        PositionIndex i,
                                        PositionIndex j,
                                        const Vector<WordIndex>& nsrcSent,
                                        const Vector<WordIndex>& trgSent,
                                        const Count& weight)
{
      // Init vars
  float curr_lanji=lanji.get(n,j,i);
  float weighted_curr_lanji=SMALL_LG_NUM;
  if(curr_lanji!=INVALID_ANJI_VAL)
  {
    weighted_curr_lanji=(float)log((float)weight)+curr_lanji;
    if(weighted_curr_lanji<SMALL_LG_NUM)
      weighted_curr_lanji=SMALL_LG_NUM;
  }

  float weighted_new_lanji=(float)log((float)weight)+lanji_aux.get_invlogp(np,j,i);
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
    lexAuxVar[s][t]=make_pair(weighted_curr_lanji,weighted_new_lanji);
  }
}

//-------------------------   
void IncrHmmAligModel::updateParsLex(void)
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
void IncrHmmAligModel::fillEmAuxVarsAlig(unsigned int n,
                                         unsigned int np,
                                         PositionIndex slen,
                                         PositionIndex ip,
                                         PositionIndex i,
                                         PositionIndex j,
                                         const Count& weight)
{
      // Init vars
  float curr_lanjm1ip_anji=lanjm1ip_anji.get(n,j,i,ip);
  float weighted_curr_lanjm1ip_anji=SMALL_LG_NUM;
  if(curr_lanjm1ip_anji!=INVALID_ANJM1IP_ANJI_VAL)
  {
    weighted_curr_lanjm1ip_anji=(float)log((float)weight)+curr_lanjm1ip_anji;
    if(weighted_curr_lanjm1ip_anji<SMALL_LG_NUM)
      weighted_curr_lanjm1ip_anji=SMALL_LG_NUM;
  }

  float weighted_new_lanjm1ip_anji=(float)log((float)weight)+lanjm1ip_anji_aux.get_invlogp(np,j,i,ip);
  if(weighted_new_lanjm1ip_anji<SMALL_LG_NUM)
    weighted_new_lanjm1ip_anji=SMALL_LG_NUM;

      // Init aSourceHmm data structure
  aSourceHmm asHmm;
  asHmm.prev_i=ip;
  asHmm.slen=slen;

      // Gather local suff. stats.
  AligAuxVar::iterator aligAuxVarIter=aligAuxVar.find(make_pair(asHmm,i));
  if(aligAuxVarIter!=aligAuxVar.end())
  {
    if(weighted_curr_lanjm1ip_anji!=SMALL_LG_NUM)
      aligAuxVarIter->second.first=MathFuncs::lns_sumlog_float(aligAuxVarIter->second.first,weighted_curr_lanjm1ip_anji);
   aligAuxVarIter->second.second=MathFuncs::lns_sumlog_float(aligAuxVarIter->second.second,weighted_new_lanjm1ip_anji);
  }
  else
  {
    aligAuxVar[make_pair(asHmm,i)]=make_pair(weighted_curr_lanjm1ip_anji,weighted_new_lanjm1ip_anji);
  }
}

//-------------------------   
void IncrHmmAligModel::updateParsAlig(void)
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
float IncrHmmAligModel::obtainLogNewSuffStat(float lcurrSuffStat,
                                             float lLocalSuffStatCurr,
                                             float lLocalSuffStatNew)
{
  float lresult=MathFuncs::lns_sublog_float(lcurrSuffStat,lLocalSuffStatCurr);
  lresult=MathFuncs::lns_sumlog_float(lresult,lLocalSuffStatNew);
  return lresult;
}

//-------------------------
Prob IncrHmmAligModel::sentLenProb(unsigned int slen,
                                   unsigned int tlen)
{
  return sentLengthModel.sentLenProb(slen,tlen);
}

//-------------------------
LgProb IncrHmmAligModel::sentLenLgProb(unsigned int slen,
                                       unsigned int tlen)
{
  return sentLengthModel.sentLenLgProb(slen,tlen);
}

//-------------------------
LgProb IncrHmmAligModel::obtainBestAlignment(Vector<WordIndex> srcSentIndexVector,
                                             Vector<WordIndex> trgSentIndexVector,
                                             WordAligMatrix& bestWaMatrix)
{
  if(srcSentIndexVector.empty() || trgSentIndexVector.empty())
  {
    bestWaMatrix.init(srcSentIndexVector.size(),trgSentIndexVector.size());    
    return SMALL_LG_NUM;
  }
  else
  {
        // Obtain extended source vector
    Vector<WordIndex> nSrcSentIndexVector=extendWithNullWord(srcSentIndexVector);
        // Call function to obtain best lgprob and viterbi alignment
    Vector<Vector<LgProb> > vitMatrix;
    Vector<Vector<PositionIndex> > predMatrix;
    viterbiAlgorithm(nSrcSentIndexVector,
                     trgSentIndexVector,
                     vitMatrix,
                     predMatrix);
    Vector<PositionIndex> bestAlig;
    LgProb vit_lp=bestAligGivenVitMatrices(srcSentIndexVector.size(),vitMatrix,predMatrix,bestAlig);
        // Obtain best word alignment vector from the Viterbi matrices
    bestWaMatrix.init(srcSentIndexVector.size(),trgSentIndexVector.size());
    bestWaMatrix.putAligVec(bestAlig);

        // Calculate sentence length model lgprob
    LgProb slm_lp=sentLenLgProb(srcSentIndexVector.size(),
                                trgSentIndexVector.size());

    return slm_lp+vit_lp;
  }
}

//-------------------------
void IncrHmmAligModel::viterbiAlgorithm(const Vector<WordIndex>& nSrcSentIndexVector,
                                        const Vector<WordIndex>& trgSentIndexVector,
                                        Vector<Vector<LgProb> >& vitMatrix,
                                        Vector<Vector<PositionIndex> >& predMatrix)
{
      // Obtain slen
  PositionIndex slen=getSrcLen(nSrcSentIndexVector);

      // Clear matrices
  vitMatrix.clear();
  predMatrix.clear();

      // Make room for matrices
  Vector<LgProb> dVec;
  dVec.insert(dVec.begin(),trgSentIndexVector.size()+1,SMALL_LG_NUM);
  vitMatrix.insert(vitMatrix.begin(),nSrcSentIndexVector.size()+1,dVec);
  
  Vector<PositionIndex> pidxVec;
  pidxVec.insert(pidxVec.begin(),trgSentIndexVector.size()+1,0);
  predMatrix.insert(predMatrix.begin(),nSrcSentIndexVector.size()+1,pidxVec);
  
      // Fill matrices
  for(PositionIndex j=1;j<=trgSentIndexVector.size();++j)
  {
    for(PositionIndex i=1;i<=nSrcSentIndexVector.size();++i)
    {
      if(j==1)
      {
        vitMatrix[i][j]=logaProb(0,slen,i)+logpts(nSrcSentIndexVector[i-1],trgSentIndexVector[j-1]);
        predMatrix[i][j]=0;
      }
      else
      {
        for(PositionIndex i_tilde=1;i_tilde<=nSrcSentIndexVector.size();++i_tilde)
        {
          LgProb lp=vitMatrix[i_tilde][j-1]+
                    logaProb(i_tilde,slen,i)+
                    logpts(nSrcSentIndexVector[i-1],trgSentIndexVector[j-1]);
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
LgProb IncrHmmAligModel::bestAligGivenVitMatrices(PositionIndex slen,
                                                  const Vector<Vector<LgProb> >& vitMatrix,
                                                  const Vector<Vector<PositionIndex> >& predMatrix,
                                                  Vector<PositionIndex>& bestAlig)
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

//   for(unsigned int j=0;j<vitMatrix.size();++j)
//   {
//     for(unsigned int i=0;i<vitMatrix[j].size();++i)
//     {
//       cerr<<vitMatrix[i][j]<<" ";
//     }
//     cerr<<endl;
//   }
//   cerr<<endl;
//   for(unsigned int j=0;j<predMatrix.size();++j)
//   {
//     for(unsigned int i=0;i<predMatrix[j].size();++i)
//     {
//       cerr<<predMatrix[i][j]<<" ";
//     }
//     cerr<<endl;
//   }
  
        // Find last word alignment
    PositionIndex last_j=predMatrix[1].size()-1;
    LgProb bestLgProb=vitMatrix[1][last_j];
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

//   for(unsigned int j=0;j<bestAlig.size();++j)
//   {
//     cerr<<bestAlig[j]<<" ";
//   }
//   cerr<<endl;
  
        // Set null word alignments appropriately
    for(unsigned int j=0;j<bestAlig.size();++j)
    {
      if(bestAlig[j]>slen)
        bestAlig[j]=NULL_WORD;
    }
    
        // Return best log-probability
    return bestLgProb;
  }
}

//-------------------------
LgProb IncrHmmAligModel::calcLgProbForAlig(const Vector<WordIndex>& /*sSent*/,
                                           const Vector<WordIndex>& /*tSent*/,
                                           WordAligMatrix /*aligMatrix*/,
                                           int /*verbose*/)
{
      // TO-DO (post-thesis)
  return 0;
}

//-------------------------
LgProb IncrHmmAligModel::calcLgProb(const Vector<WordIndex>& sSent,
                                    const Vector<WordIndex>& tSent,
                                    int verbose)
{
  if(sSent.empty() || tSent.empty())
  {
    return SMALL_LG_NUM;
  }
  else
  {
        // Calculate sentence length model lgprob
    LgProb slp=sentLenLgProb(sSent.size(),tSent.size());
    
        // Obtain extended source vector
    Vector<WordIndex> nSrcSentIndexVector=extendWithNullWord(sSent);
    
        // Calculate hmm lgprob
    LgProb flp=forwardAlgorithm(nSrcSentIndexVector,
                                tSent,
                                verbose);

    if(verbose)
      cerr<<"lp= "<<slp+flp<<" ; slm_lp= "<<slp<<" ; lp-slm_lp= "<<flp<<endl;
    
    return slp+flp;
  }
}

//-------------------------
LgProb IncrHmmAligModel::forwardAlgorithm(const Vector<WordIndex>& nSrcSentIndexVector,
                                          const Vector<WordIndex>& trgSentIndexVector,
                                          int verbose)
{
      // Obtain slen
  PositionIndex slen=getSrcLen(nSrcSentIndexVector);

      // Make room for matrix
  Vector<Vector<LgProb> > forwardMatrix;
  Vector<LgProb> dVec;
  dVec.insert(dVec.begin(),trgSentIndexVector.size()+1,0.0);
  forwardMatrix.insert(forwardMatrix.begin(),nSrcSentIndexVector.size()+1,dVec);
  
      // Fill matrix
  for(PositionIndex j=1;j<=trgSentIndexVector.size();++j)
  {
    for(PositionIndex i=1;i<=nSrcSentIndexVector.size();++i)
    {
      if(j==1)
      {
        forwardMatrix[i][j]=logaProb(0,slen,i)+logpts(nSrcSentIndexVector[i-1],trgSentIndexVector[j-1]);
      }
      else
      {
        for(PositionIndex i_tilde=1;i_tilde<=nSrcSentIndexVector.size();++i_tilde)
        {
          LgProb lp=forwardMatrix[i_tilde][j-1]+
                    logaProb(i_tilde,slen,i)+
                    logpts(nSrcSentIndexVector[i-1],trgSentIndexVector[j-1]);
          if(i_tilde==1)
            forwardMatrix[i][j]=lp;
          else
            forwardMatrix[i][j]=MathFuncs::lns_sumlog((double)lp,(double)forwardMatrix[i][j]);
        }
      }
    }
  }

      // Obtain lgProb from forward matrix
  LgProb lp=lgProbGivenForwardMatrix(forwardMatrix);

      // Print verbose info
  if(verbose>1)
  {
        // Clear cached alpha and beta values
    alpha_values.clear();
    beta_values.clear();
    for(PositionIndex j=1;j<=trgSentIndexVector.size();++j)
    {
      for(PositionIndex i=1;i<=nSrcSentIndexVector.size();++i)
      {
        cerr<<"i="<<i<<",j="<<j<<" "<<forwardMatrix[i][j];
        cerr<<" "<<log_alpha(slen,i,j,nSrcSentIndexVector,trgSentIndexVector)<<" "<<log_beta(slen,i,j,nSrcSentIndexVector,trgSentIndexVector);
        if(i<nSrcSentIndexVector.size()) cerr<<" ; ";
      }
      cerr<<endl;
    }
  }

      // Return result
  return lp;
}

//-------------------------
LgProb IncrHmmAligModel::lgProbGivenForwardMatrix(const Vector<Vector<LgProb> >& forwardMatrix)
{
      // Sum lgprob for each i
  LgProb lp;
  PositionIndex last_j=forwardMatrix[1].size()-1;
  for(unsigned int i=1;i<=forwardMatrix.size()-1;++i)
  {
    if(i==1)
    {
      lp=forwardMatrix[i][last_j];
    }
    else
    {
      lp=MathFuncs::lns_sumlog((double)lp,(double)forwardMatrix[i][last_j]);
    }
  }

      // Return result
  return lp;
}

//-------------------------
LgProb IncrHmmAligModel::calcLgProbPhr(const Vector<WordIndex>& sPhr,
                                       const Vector<WordIndex>& tPhr,
                                       int verbose/*=0*/)
{
//  return calcSumIBM1LgProb(sPhr,tPhr,verbose);
  return noisyOrLgProb(sPhr,tPhr,verbose);
}

//-------------------------
LgProb IncrHmmAligModel::calcSumIBM1LgProb(const Vector<WordIndex>& sSent,
                                           const Vector<WordIndex>& tSent,
                                           int verbose)
{
 LgProb sumlp;
 LgProb lexContrib;
 LgProb lgProb;    
 unsigned int i,j;
 Vector<WordIndex> nsSent=addNullWordToWidxVec(sSent);

 if(verbose) cerr<<"Obtaining Sum IBM Model 1 logprob...\n"; 
     
 lgProb=logaProbIbm1(sSent.size(),tSent.size());
 
 if(verbose) cerr<<"- aligLgProb(tlen="<<tSent.size() <<" | slen="<<sSent.size()<<")= "<<logaProbIbm1(sSent.size(),tSent.size())<<endl;

 lgProb+=sentLenLgProb(sSent.size(),tSent.size());
 if(verbose) cerr<<"- lenLgProb(tlen="<<tSent.size() <<" | slen="<<sSent.size()<<")= "<<sentLenLgProb(sSent.size(),tSent.size())<<endl;

 lexContrib=0;
 for(j=0;j<tSent.size();++j)
 {
   for(i=0;i<nsSent.size();++i)
   {
     if(i==0) sumlp=logpts(nsSent[i],tSent[j]);
     else
       sumlp=MathFuncs::lns_sumlog(logpts(nsSent[i],tSent[j]),sumlp);
     if(verbose==2)
       cerr<<"log(t( " <<tSent[j] <<" | " <<nsSent[i]<<" ))= "<<logpts(nsSent[i],tSent[j]) <<endl;
   }
   lexContrib+=sumlp;  
   if(verbose) cerr<<"- log(sumt(j="<<j<<"))= "<<sumlp<<endl;
   if(verbose==2) cerr<<endl;
 }
 if(verbose) cerr<<"- Lexical model contribution= "<<lexContrib<<endl;
 lgProb+=lexContrib;
 
 return lgProb;
}

//-------------------------
LgProb IncrHmmAligModel::logaProbIbm1(PositionIndex slen,
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
LgProb IncrHmmAligModel::noisyOrLgProb(const Vector<WordIndex>& sSent,
                                       const Vector<WordIndex>& tSent,
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
       cerr<<"t( " <<tSent[j] <<" | " <<sSent[i]<<" )= "<<pts(sSent[i],tSent[j]) <<endl;
   }
   Prob compProb=1.0-(double)prob;
   if((double)compProb==0.0)
     lp=lp+(double)SMALL_LG_NUM;
   else
     lp=lp+compProb.get_lp();
   
   if(verbose) cerr<<"- log(1-prod(j="<<j<<"))= "<<lp<<endl;
   if(verbose==2) cerr<<endl;
 }
 return lp;
}

//-------------------------
void IncrHmmAligModel::initPpInfo(unsigned int /*slen*/,
                                  const Vector<WordIndex>& /*tSent*/,
                                  PpInfo& /*ppInfo*/)
{
      // TO-DO (post-thesis)
}

//-------------------------
void IncrHmmAligModel::partialProbWithoutLen(unsigned int /*srcPartialLen*/,
                                             unsigned int /*slen*/,
                                             const Vector<WordIndex>& /*s_*/,
                                             const Vector<WordIndex>& /*tSent*/,
                                             PpInfo& /*ppInfo*/)
{
      // TO-DO (post-thesis)
}

//-------------------------
LgProb IncrHmmAligModel::lpFromPpInfo(const PpInfo& /*ppInfo*/)
{
      // TO-DO (post-thesis)
  return 0;
}

//-------------------------
void IncrHmmAligModel::addHeurForNotAddedWords(int /*numSrcWordsToBeAdded*/,
                                               const Vector<WordIndex>& /*tSent*/,
                                               PpInfo& /*ppInfo*/)
{
      // TO-DO (post-thesis)
}

//-------------------------
void IncrHmmAligModel::sustHeurForNotAddedWords(int /*numSrcWordsToBeAdded*/,
                                                const Vector<WordIndex>& /*tSent*/,
                                                PpInfo& /*ppInfo*/)
{
      // TO-DO (post-thesis)
}

//-------------------------
bool IncrHmmAligModel::load(const char* prefFileName)
{
  if(prefFileName[0]!=0)
  {
    bool retVal;

    cerr<<"Loading incremental HMM Model data..."<<endl;

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
    pair<unsigned int,unsigned int> pui;
    retVal=readSentencePairs(srcsFile.c_str(),trgsFile.c_str(),srctrgcFile.c_str(),pui);
    if(retVal==ERROR) return ERROR;

        // Load file with lanji values
    retVal=lanji.load(prefFileName);
    if(retVal==ERROR) return ERROR;

        // Load file with lanjm1ip_anji values
    retVal=lanjm1ip_anji.load(prefFileName);
    if(retVal==ERROR) return ERROR;

        // Load file with lexical nd values
    std::string lexNumDenFile=prefFileName;
    lexNumDenFile=lexNumDenFile+".hmm_lexnd";
    retVal=incrLexTable.load(lexNumDenFile.c_str());
    if(retVal==ERROR) return ERROR;

        // Load file with alignment nd values
    std::string aligNumDenFile=prefFileName;
    aligNumDenFile=aligNumDenFile+".hmm_alignd";
    retVal=incrHmmAligTable.load(aligNumDenFile.c_str());
    if(retVal==ERROR) return ERROR;

        // Load file with with lexical smoothing interpolation factor
    std::string lsifFile=prefFileName;
    lsifFile=lsifFile+".lsifactor";
    retVal=loadLexSmIntFactor(lsifFile.c_str());
    if(retVal==ERROR) return ERROR;

        // Load file with with alignment smoothing interpolation factor
    std::string asifFile=prefFileName;
    asifFile=asifFile+".asifactor";
    retVal=loadAlSmIntFactor(asifFile.c_str());
    if(retVal==ERROR) return ERROR;

        // Load average sentence lengths
    std::string slmodelFile=prefFileName;
    slmodelFile=slmodelFile+".slmodel";
    retVal=sentLengthModel.load(slmodelFile.c_str());
    if(retVal==ERROR) return ERROR;

    return OK;
  }
  else return ERROR;
}
   
//-------------------------
bool IncrHmmAligModel::print(const char* prefFileName)
{
  bool retVal;

      // Print vocabularies 
  std::string srcVocFileName=prefFileName;
  srcVocFileName=srcVocFileName+".svcb";
  printGIZASrcVocab(srcVocFileName.c_str());

  std::string trgVocFileName=prefFileName;
  trgVocFileName=trgVocFileName+".tvcb";    
  printGIZATrgVocab(trgVocFileName.c_str());
  
      // Print files with source and target sentences
  std::string srcsFile=prefFileName;
  srcsFile=srcsFile+".src";
  std::string trgsFile=prefFileName;
  trgsFile=trgsFile+".trg";
  std::string srctrgcFile=prefFileName;
  srctrgcFile=srctrgcFile+".srctrgc";
  retVal=printSentPairs(srcsFile.c_str(),trgsFile.c_str(),srctrgcFile.c_str());
  if(retVal==ERROR) return ERROR;

      // Print file with lanji values
  retVal=lanji.print(prefFileName);
  if(retVal==ERROR) return ERROR;

      // Print file with lanjm1ip_anji values
  retVal=lanjm1ip_anji.print(prefFileName);
  if(retVal==ERROR) return ERROR;

      // Print file with lexical nd values
  std::string lexNumDenFile=prefFileName;
  lexNumDenFile=lexNumDenFile+".hmm_lexnd";
  retVal=incrLexTable.print(lexNumDenFile.c_str());
  if(retVal==ERROR) return ERROR;

      // Print file with alignment nd values
  std::string aligNumDenFile=prefFileName;
  aligNumDenFile=aligNumDenFile+".hmm_alignd";
  retVal=incrHmmAligTable.print(aligNumDenFile.c_str());
  if(retVal==ERROR) return ERROR;

      // Print file with with lexical smoothing interpolation factor
  std::string lsifFile=prefFileName;
  lsifFile=lsifFile+".lsifactor";
  retVal=printLexSmIntFactor(lsifFile.c_str());
  if(retVal==ERROR) return ERROR;

      // Print file with with alignment smoothing interpolation factor
  std::string asifFile=prefFileName;
  asifFile=asifFile+".asifactor";
  retVal=printAlSmIntFactor(asifFile.c_str());
  if(retVal==ERROR) return ERROR;

      // Print file with sentence length model
  std::string slmodelFile=prefFileName;
  slmodelFile=slmodelFile+".slmodel";
  retVal=sentLengthModel.print(slmodelFile.c_str());
  if(retVal==ERROR) return ERROR;

  return OK;
}

//-------------------------
void IncrHmmAligModel::clear(void)
{
  _swAligModel<Vector<Prob> >::clear();
  lanji.clear();
  lanji_aux.clear();
  lanjm1ip_anji.clear();
  lanjm1ip_anji_aux.clear();
  alpha_values.clear();
  beta_values.clear();
  incrLexTable.clear();
  incrHmmAligTable.clear();
  sentLengthModel.clear();
}

//-------------------------
void IncrHmmAligModel::clearTempVars(void)
{
}

//-------------------------
IncrHmmAligModel::~IncrHmmAligModel(void)
{
  
}
