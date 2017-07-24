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
/* Module: PfsmEcm                                                  */
/*                                                                  */
/* Definitions file: PfsmEcm.cc                                     */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "PfsmEcm.h"

//--------------- PfsmEcm class functions
//

//---------------------------------------
PfsmEcm::PfsmEcm()
{
      // Initialize ecModelPars variable
  ecModelPars.push_back(DEFAULT_VOC_SIZE);
  ecModelPars.push_back(DEFAULT_HIT_PROB);
  ecModelPars.push_back(DEFAULT_INS_FACTOR);
  ecModelPars.push_back(DEFAULT_SUBST_FACTOR);
  ecModelPars.push_back(DEFAULT_DEL_FACTOR);

      // Set error model
  setErrorModel(ecModelPars[0],ecModelPars[1],ecModelPars[2],ecModelPars[3],ecModelPars[4],false);
}

//---------------------------------------
Score PfsmEcm::similarityGivenPrefix(Vector<std::string> x,
                                     Vector<std::string> y)
{
  return simGivenPrefPfsm(x,y);
}

//---------------------------------------
void PfsmEcm::correctStrGivenPref(Vector<std::string> uncorrStrVec,
                                  Vector<std::string> prefStrVec,
                                  Vector<std::string>& correctedStrVec)
{
  WordAndCharLevelOps wcOps;
  
  editDistForVecStr.calculateEditDistPrefixOps(uncorrStrVec,prefStrVec,wcOps.first,wcOps.second);
  
  correctStrGivenPrefOps(wcOps,uncorrStrVec,prefStrVec,correctedStrVec);
}

//---------------------------------------
void PfsmEcm::setWeights(Vector<float> wVec)
{
  if(wVec.size()==5)
  {
        // Check if hProb is greater or equal to one (one or greater
        // probabilities are not allowed since they don't define a valid
        // error model)
    bool hprob_ge_one=false;
    if(wVec[1]>=1)
    {
      hprob_ge_one=true;
    }
    
        // Check if model weights are lower or equal to zero (negative
        // weights produce negative probabilities, zero weights are not
        // allowed)
    bool weights_le_zero=false;
    for(unsigned int i=0;i<wVec.size();++i)
    {
      if(wVec[i]<=0)
      {
        weights_le_zero=true;
        break;
      }
    }

        // Set weights
    if(!hprob_ge_one && !weights_le_zero)
    {
      ecModelPars=wVec;
      setErrorModel(wVec[0],wVec[1],wVec[2],wVec[3],wVec[4],true);
    }
    else
    {
      if(hprob_ge_one)
        cerr<<"Warning: hProb parameter cannot be equal or above one"<<endl;
      if(weights_le_zero)
        cerr<<"Warning: error correction model weights cannot be negative or zero"<<endl;
      cerr<<"Error correction model weights were not changed"<<endl;
    }
  }
}

//---------------------------------------
unsigned int PfsmEcm::getNumWeights(void)
{
  return 5;
}

//---------------------------------------
void PfsmEcm::printWeights(ostream &outS)
{
  if(ecModelPars.size()==5)
  {
    outS<<"vocSize: "<<ecModelPars[0]<<" , ";
    outS<<"hProb: "<<ecModelPars[1]<<" , ";
    outS<<"iFactor: "<<ecModelPars[2]<<" , ";
    outS<<"sFactor: "<<ecModelPars[3]<<" , ";
    outS<<"dFactor: "<<ecModelPars[4];
  }
}

//---------------------------------------
bool PfsmEcm::load(const char */*prefix*/)
{
//  cerr<<"Warning: This error correction model has not any parameters to load from file"<<endl;
  return THOT_OK;
}

//---------------------------------------
bool PfsmEcm::print(const char */*prefix*/)
{
//  cerr<<"Warning: This error correction model has not any parameters to print to file"<<endl;
  return THOT_OK;
}

//---------------------------------------
void PfsmEcm::setErrorModel(float vocsize,
                            float hprob,
                            float ifactor,
                            float sfactor,
                            float dfactor,
                            int verbose/*=0*/)
{
  float ip;
  float sp;
  float dp;

      // Obtain error probabilities
  obtainErrorProbsFromParams(vocsize,hprob,ifactor,sfactor,dfactor,ip,sp,dp);

      // Print error log-probs that will be used when calculating edit
      // distance
  if(verbose)
    cerr<<"ECM error log-probs changed to: log(hp)= "<<log(hprob)<<" , log(ip)= "<<log(ip)<<" , log(sp)= "<<log(sp)<<" , log(dp)= "<<log(dp)<<endl;

      // Set error model for the edit distance module
  editDistForVecStr.setErrorModel(-log(hprob),
                                  -log(ip),
                                  -log(sp),
                                  -log(dp));
}

//---------------------------------------
void PfsmEcm::obtainErrorProbsFromParams(float vocsize,
                                         float hprob,
                                         float ifactor,
                                         float sfactor,
                                         float dfactor,
                                         float& ip,
                                         float& sp,
                                         float& dp)
{
      // Calculate constant
  float e;
  if(vocsize==0)
    e=(1-(hprob))/(ifactor+sfactor+dfactor);
  else
    e=(1-(hprob))/((ifactor*vocsize)+(sfactor*(vocsize-1))+dfactor*1);

      // Set probabilities for the edit distance operations
  ip=e*ifactor;
  sp=e*sfactor;
  dp=e*dfactor;
}

//---------------------------------------
Score PfsmEcm::similarity(Vector<std::string> x,
                          Vector<std::string> y)
{
  return simPfsm(x,y);
}

//---------------------------------------
Score PfsmEcm::simPfsm(Vector<std::string> x,
                       Vector<std::string> y)
{
  return -editDistForVecStr.calculateEditDist(x,y);
}

//---------------------------------------
Score PfsmEcm::simGivenPrefPfsm(Vector<std::string> x,
                                Vector<std::string> y)
{
  return -editDistForVecStr.calculateEditDistPrefix(x,y);
}

//---------------------------------------
Score PfsmEcm::simGivenPrefOpsPfsm(Vector<std::string> x,
                                   Vector<std::string> y,
                                   WordAndCharLevelOps& wcOps)
{
  return -editDistForVecStr.calculateEditDistPrefixOps(x,y,wcOps.first,wcOps.second);
}

//---------------------------------------
PfsmEcm::~PfsmEcm()
{
  
}
