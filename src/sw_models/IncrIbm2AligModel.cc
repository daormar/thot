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
/* Module: IncrIbm2AligModel                                        */
/*                                                                  */
/* Definitions file: IncrIbm2AligModel.cc                           */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "IncrIbm2AligModel.h"

//--------------- Classes --------------------------------------------


//--------------- IncrIbm2AligModel class function definitions

//-------------------------
IncrIbm2AligModel::IncrIbm2AligModel():IncrIbm1AligModel()
{
}

//-------------------------
void IncrIbm2AligModel::efficientBatchTrainingForRange(pair<unsigned int,unsigned int> sentPairRange,
                                                       int verbosity/*=0*/)
{
      // Set maximum size of the matrices of expected values to zero
  set_expval_maxnsize(0);

      // Train sentence length model
  sentLengthModel.trainSentPairRange(sentPairRange,verbosity);

      // EM algorithm
  calcNewLocalSuffStats(sentPairRange,verbosity);
  incrLexTable.clear();
  incrIbm2AligTable.clear();
  updatePars();
}

//-------------------------   
double IncrIbm2AligModel::calc_anji_num(const Vector<WordIndex>& nsrcSent,
                                        const Vector<WordIndex>& trgSent,
                                        PositionIndex i,
                                        PositionIndex j)
{
  double d;
  
  d=IncrIbm1AligModel::calc_anji_num(nsrcSent,trgSent,i,j);
  d=d*calc_anji_num_alig(i,j,nsrcSent.size()-1,trgSent.size());
  return d;
}


//-------------------------
double IncrIbm2AligModel::calc_anji_num_alig(PositionIndex i,
                                             PositionIndex j,
                                             PositionIndex slen,
                                             PositionIndex tlen)
{
  bool found;
  aSource as;

  as.j=j;
  as.slen=slen;
  as.tlen=tlen;
  aSourceMask(as);
  
  incrIbm2AligTable.getAligNumer(as,i,found);
  if(found)
  {
        // alig. parameter has previously been seen
    return unsmoothed_aProb(as.j,as.slen,as.tlen,i);
  }
  else
  {
        // alig. parameter has never been seen
    return ARBITRARY_AP;
  }
}

//-------------------------   
void IncrIbm2AligModel::fillEmAuxVars(unsigned int mapped_n,
                                      unsigned int mapped_n_aux,
                                      PositionIndex i,
                                      PositionIndex j,
                                      const Vector<WordIndex>& nsrcSent,
                                      const Vector<WordIndex>& trgSent,
                                      const Count& weight)
{
  IncrIbm1AligModel::fillEmAuxVars(mapped_n,mapped_n_aux,i,j,nsrcSent,trgSent,weight);
  fillEmAuxVarsAlig(mapped_n,mapped_n_aux,i,j,nsrcSent.size()-1,trgSent.size(),weight);
}

//-------------------------   
void IncrIbm2AligModel::fillEmAuxVarsAlig(unsigned int mapped_n,
                                          unsigned int mapped_n_aux,
                                          PositionIndex i,
                                          PositionIndex j,
                                          PositionIndex slen,
                                          PositionIndex tlen,
                                          const Count& weight)
{
      // Init vars
  float curr_anji=anji.get_fast(mapped_n,j,i);
  float weighted_curr_anji=0;
  if(curr_anji!=INVALID_ANJI_VAL)
  {
    weighted_curr_anji=(float)weight*curr_anji;
    if(weighted_curr_anji<SMOOTHING_WEIGHTED_ANJI)
      weighted_curr_anji=SMOOTHING_WEIGHTED_ANJI;
  }

  float weighted_new_anji=(float)weight*anji_aux.get_invp_fast(mapped_n_aux,j,i);
  if(weighted_new_anji<SMOOTHING_WEIGHTED_ANJI)
    weighted_new_anji=SMOOTHING_WEIGHTED_ANJI;
  
      // Init aSource data structure
  aSource as;
  as.j=j;
  as.slen=slen;
  as.tlen=tlen;
  aSourceMask(as);

      // Obtain logarithms
  float weighted_curr_lanji;
  if(weighted_curr_anji==0)
    weighted_curr_lanji=SMALL_LG_NUM;
  else
    weighted_curr_lanji=log(weighted_curr_anji);
  
  float weighted_new_lanji=log(weighted_new_anji);

      // Store contributions
  AligAuxVar::iterator aligAuxVarIter=aligAuxVar.find(make_pair(as,i));
  if(aligAuxVarIter!=aligAuxVar.end())
  {
    if(weighted_curr_lanji!=SMALL_LG_NUM)
      aligAuxVarIter->second.first=MathFuncs::lns_sumlog_float(aligAuxVarIter->second.first,weighted_curr_lanji);
    aligAuxVarIter->second.second=MathFuncs::lns_sumlog_float(aligAuxVarIter->second.second,weighted_new_lanji);
  }
  else
  {
    aligAuxVar[make_pair(as,i)]=make_pair(weighted_curr_lanji,weighted_new_lanji);
  }
}

//-------------------------   
void IncrIbm2AligModel::updatePars(void)
{
  IncrIbm1AligModel::updatePars();
  updateParsAlig();  
}

//-------------------------   
void IncrIbm2AligModel::updateParsAlig(void)
{
        // Update parameters
  for(AligAuxVar::iterator aligAuxVarIter=aligAuxVar.begin();aligAuxVarIter!=aligAuxVar.end();++aligAuxVarIter)
  {
    aSource as=aligAuxVarIter->first.first;
    PositionIndex i=aligAuxVarIter->first.second;
    float log_suff_stat_curr=aligAuxVarIter->second.first;
    float log_suff_stat_new=aligAuxVarIter->second.second;

        // Update parameters only if current and new sufficient statistics
        // are different
    if(log_suff_stat_curr!=log_suff_stat_new)
    {
          // Obtain aligNumer
      bool found;
      float numer=incrIbm2AligTable.getAligNumer(as,i,found);
      if(!found) numer=SMALL_LG_NUM;

          // Obtain aligDenom
      float denom=incrIbm2AligTable.getAligDenom(as,found);
      if(!found) denom=SMALL_LG_NUM;

          // Obtain new sufficient statistics
      float new_numer=obtainLogNewSuffStat(numer,log_suff_stat_curr,log_suff_stat_new);
      float new_denom=denom;
      if(numer!=SMALL_LG_NUM)
        new_denom=MathFuncs::lns_sublog_float(denom,numer);
      new_denom=MathFuncs::lns_sumlog_float(new_denom,new_numer);
      
          // Set lexical numerator and denominator
      incrIbm2AligTable.setAligNumDen(as,i,new_numer,new_denom);
    }
  }
      // Clear auxiliary variables
  aligAuxVar.clear();
}

//-------------------------
Prob IncrIbm2AligModel::aProb(PositionIndex j,
                              PositionIndex slen,
                              PositionIndex tlen,
                              PositionIndex i)
{
  return unsmoothed_aProb(j,slen,tlen,i);
}

//-------------------------
LgProb IncrIbm2AligModel::logaProb(PositionIndex j,
                                   PositionIndex slen,
                                   PositionIndex tlen,
                                   PositionIndex i)
{
  return unsmoothed_logaProb(j,slen,tlen,i);
}

//-------------------------
double IncrIbm2AligModel::unsmoothed_aProb(PositionIndex j,
                                           PositionIndex slen,
                                           PositionIndex tlen,
                                           PositionIndex i)
{
  return exp(unsmoothed_logaProb(j,slen,tlen,i));
}

//-------------------------
double IncrIbm2AligModel::unsmoothed_logaProb(PositionIndex j,
                                              PositionIndex slen,
                                              PositionIndex tlen,
                                              PositionIndex i)
{
  bool found;
  double numer;
  aSource as;

  as.j=j;
  as.slen=slen;
  as.tlen=tlen;
  aSourceMask(as);
  
  numer=incrIbm2AligTable.getAligNumer(as,i,found);
  if(found)
  {
        // aligNumer for pair as,i exists
    double denom;
    denom=incrIbm2AligTable.getAligDenom(as,found);
    if(!found) return SMALL_LG_NUM;
    else
    {
      return numer-denom;
    }
  }
  else
  {
        // aligNumer for pair as,i does not exist
    return SMALL_LG_NUM;
  }
}

//-------------------------
LgProb IncrIbm2AligModel::obtainBestAlignment(Vector<WordIndex> srcSentIndexVector,
                                              Vector<WordIndex> trgSentIndexVector,
                                              WordAligMatrix& bestWaMatrix)
{
  Vector<PositionIndex> bestAlig;
  LgProb lgProb=sentLenLgProb(srcSentIndexVector.size(),
                              trgSentIndexVector.size());
  lgProb+=lexAligM2LpForBestAlig(addNullWordToWidxVec(srcSentIndexVector),
                                 trgSentIndexVector,
                                 bestAlig);

  bestWaMatrix.init(srcSentIndexVector.size(),trgSentIndexVector.size());
  bestWaMatrix.putAligVec(bestAlig);

  return lgProb;
}

//-------------------------
LgProb IncrIbm2AligModel::calcLgProbForAlig(const Vector<WordIndex>& sSent,
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
    return ERROR;
  }     
  else
  {
    return incrIBM2LgProb(addNullWordToWidxVec(sSent),tSent,alig,verbose);
  }
}

//-------------------------
LgProb IncrIbm2AligModel::incrIBM2LgProb(Vector<WordIndex> nsSent,
                                         Vector<WordIndex> tSent,
                                         Vector<PositionIndex> alig,
                                         int verbose)
{  
  unsigned int slen=nsSent.size()-1;
  unsigned int tlen=tSent.size();
     
  if(verbose) cerr<<"Obtaining IBM Model 2 logprob...\n"; 
  
  LgProb lgProb=0;
  for(unsigned int j=0;j<alig.size();++j)
  {
    Prob p=pts(nsSent[alig[j]],tSent[j]);
    if(verbose) cerr<<"t("<<tSent[j] <<"|"<<nsSent[alig[j]] <<")= "<<p<<" ; logp="<<(double)log((double)p)<<endl;   
    lgProb=lgProb+(double)log((double)p); 
    
    p=aProb(j+1,slen,tlen,alig[j]);
    lgProb=lgProb+(double)log((double)p); 
  }
  return lgProb;
}

//-------------------------
LgProb IncrIbm2AligModel::calcLgProb(const Vector<WordIndex>& sSent,
                                     const Vector<WordIndex>& tSent,
                                     int verbose)
{
  return calcSumIBM2LgProb(addNullWordToWidxVec(sSent),tSent,verbose);
}

//-------------------------
LgProb IncrIbm2AligModel::calcSumIBM2LgProb(Vector<WordIndex> nsSent,
                                            Vector<WordIndex> tSent,
                                            int verbose)
{
  unsigned int slen=nsSent.size()-1;
  unsigned int tlen=tSent.size();
  Prob sump;
  LgProb lexAligContrib;
 
  if(verbose) cerr<<"Obtaining Sum IBM Model 2 logprob...\n"; 
     
  LgProb lgProb=sentLenLgProb(slen,tlen);
  if(verbose) cerr<<"- lenLgProb(tlen="<<tSent.size() <<" | slen="<<slen <<")= "<<sentLenLgProb(slen,tlen)<<endl;

  lexAligContrib=0;
  for(unsigned int j=0;j<tSent.size();++j)
  {
    sump=0;
    for(unsigned int i=0;i<nsSent.size();++i)
    {
      sump+=pts(nsSent[i],tSent[j])*aProb(j+1,slen,tlen,i);
      if(verbose==2)
      {
        cerr<<"t( " <<tSent[j] <<" | " <<nsSent[i]<<" )= "<<pts(nsSent[i],tSent[j]) <<endl;
        cerr<<"a( "<<i<<"| j="<<j+1<<", slen="<<slen<<", tlen="<<tlen<<")= "<<aProb(j+1,slen,tlen,i)<<endl; 
      }
    }
    lexAligContrib+=(double)log((double)sump);  
    if(verbose)
      cerr<<"- sump(j="<<j+1<<")= "<<sump<<endl;
    if(verbose==2) cerr<<endl;
  }

  if(verbose) cerr<<"- Lexical plus alignment contribution= "<<lexAligContrib<<endl;
  lgProb+=lexAligContrib;
 
  return lgProb;
}

//-------------------------
void IncrIbm2AligModel::initPpInfo(unsigned int slen,
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
  unsigned int tlen=tSent.size();  
  for(unsigned int j=0;j<tSent.size();++j)
  {
    ppInfo[j]+=pts(NULL_WORD,tSent[j])*aProb(j+1,slen,tlen,0);
  }
}

//-------------------------
void IncrIbm2AligModel::partialProbWithoutLen(unsigned int srcPartialLen,
                                              unsigned int slen,
                                              const Vector<WordIndex>& s_,
                                              const Vector<WordIndex>& tSent,
                                              PpInfo& ppInfo)
{
  unsigned int tlen=tSent.size();

  for(unsigned int i=0;i<s_.size();++i)
  {
    for(unsigned int j=0;j<tSent.size();++j)
    {
      ppInfo[j]+=pts(s_[i],tSent[j])*aProb(j+1,slen,tlen,srcPartialLen+i+1);
          // srcPartialLen+i is added 1 because the first source word has index 1
    }
  }
}

//-------------------------
bool IncrIbm2AligModel::load(const char* prefFileName)
{
  if(prefFileName[0]!=0)
  {
    bool retVal;

        // Load IBM 1 Model data
    retVal=IncrIbm1AligModel::load(prefFileName);
    if(retVal==ERROR) return ERROR;
    
    cerr<<"Loading incremental IBM 2 Model data..."<<endl;

        // Load file with alignment nd values
    std::string aligNumDenFile=prefFileName;
    aligNumDenFile=aligNumDenFile+".ibm2_alignd";
    retVal=incrIbm2AligTable.load(aligNumDenFile.c_str());
    if(retVal==ERROR) return ERROR;

    return THOT_OK;
  }
  else return ERROR;
}
   
//-------------------------
bool IncrIbm2AligModel::print(const char* prefFileName)
{
  bool retVal;
  
      // Print IBM 1 Model data
  retVal=IncrIbm1AligModel::print(prefFileName);
  
      // Print file with alignment nd values
  std::string aligNumDenFile=prefFileName;
  aligNumDenFile=aligNumDenFile+".ibm2_alignd";
  retVal=incrIbm2AligTable.print(aligNumDenFile.c_str());
  if(retVal==ERROR) return ERROR;

  return THOT_OK;
}

//-------------------------
void IncrIbm2AligModel::clear(void)
{
  IncrIbm1AligModel::clear();
  incrIbm2AligTable.clear();
}

//-------------------------
LgProb IncrIbm2AligModel::lexAligM2LpForBestAlig(Vector<WordIndex> nSrcSentIndexVector,
                                                 Vector<WordIndex> trgSentIndexVector,
                                                 Vector<PositionIndex>& bestAlig)
{
      // Initialize variables
  unsigned int slen=nSrcSentIndexVector.size()-1;
  unsigned int tlen=trgSentIndexVector.size();
  LgProb aligLgProb=0;
  bestAlig.clear();
  
  for(unsigned int j=0;j<trgSentIndexVector.size();++j)
  {
    unsigned int best_i=0;
    LgProb max_lp=-FLT_MAX;
    for(unsigned int i=0;i<nSrcSentIndexVector.size();++i)
    {
          // lexical logprobability
      LgProb lp=log((double)pts(nSrcSentIndexVector[i],trgSentIndexVector[j]));
          // alignment logprobability
      lp+=log((double)aProb(j+1,slen,tlen,i));

      if(max_lp<=lp)
      {
        max_lp=lp;
        best_i=i;
      }
    }
        // Add contribution
    aligLgProb=aligLgProb+max_lp;
        // Add word alignment
    bestAlig.push_back(best_i);
  }
  return aligLgProb;
}

//-------------------------
void IncrIbm2AligModel::aSourceMask(aSource &/*as*/)
{
      // This function is left void for performing a standard estimation
}

//-------------------------
IncrIbm2AligModel::~IncrIbm2AligModel(void)
{
  
}
