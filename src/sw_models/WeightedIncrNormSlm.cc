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
 
/*********************************************************************/
/*                                                                   */
/* Module: WeightedIncrNormSlm                                       */
/*                                                                   */
/* Definitions file: WeightedIncrNormSlm.cc                          */
/*                                                                   */
/*********************************************************************/


//--------------- Include files ---------------------------------------

#include "WeightedIncrNormSlm.h"

//--------------- Global variables ------------------------------------

//--------------- Function declarations 

//--------------- Constants

//--------------- Classes ---------------------------------------------

//-------------------------
WeightedIncrNormSlm::WeightedIncrNormSlm(void)
{
  numSents=0;
  slenSum=0;
  tlenSum=0;
}

//-------------------------
bool WeightedIncrNormSlm::load(const char *filename)
{
  awkInputStream awk;

  clear();
  
  if(awk.open(filename)==ERROR)
  {
    cerr<<"Error in sentence length model file, file "<<filename<<" does not exist.\n";
    return ERROR;
  }
  if(awk.getln())
  {
    if(strcmp("Weighted",awk.dollar(1).c_str())==0)
    {
      return readNormalPars(filename);
    }
    else
    {
      cerr<<"Anomalous sentence length model file: "<<filename<<"\n";
      return ERROR;
    }
  }
  else
  {
    cerr<<"Warning: empty sentence length model file: "<<filename<<"\n";
    clear();
    return THOT_OK;
  }
}

//-------------------------
bool WeightedIncrNormSlm::print(const char* filename)
{
  ofstream outF;

  outF.open(filename,ios::out);
  if(!outF)
  {
    cerr<<"Error while printing sentence length model."<<endl;
    return ERROR;
  }
  else
  {
    print(outF);
    outF.close();	
    return THOT_OK;
  }   
}

//-------------------------
ostream& WeightedIncrNormSlm::print(ostream &outS)
{
      // print header
  outS<<"Weighted incr. gaussian sentence length model...\n";
      // print source and target average sentence info
  outS<<"numsents: "<<numSents<<" ; slensum: "<<slenSum<<" ; tlensum: "<<tlenSum<<endl;

      // Set float precision.
  outS.setf( ios::fixed, ios::floatfield );
  outS.precision(8);

      // Print values for each source sentence length
  for(unsigned int i=0;i<kVec.size();++i)
  {
    if(kVec[i]!=0)
      outS<<i<<" "<<kVec[i]<<" "<<swkVec[i]<<" "<<mkVec[i]<<" "<<skVec[i]<<endl;
  }
  return outS;
}

//-------------------------
Prob WeightedIncrNormSlm::sentLenProb(unsigned int slen,
                                      unsigned int tlen)
{
  return (double)exp((double)sentLenLgProb(slen,tlen));
}

//-------------------------
LgProb WeightedIncrNormSlm::sentLenLgProb(unsigned int slen,
                                          unsigned int tlen)
{
  return sentLenLgProbNorm(slen,tlen);
}

//-------------------------
Prob WeightedIncrNormSlm::sumSentLenProb(unsigned int slen,
                                         unsigned int tlen)
{
  return sumSentLenProbNorm(slen,tlen);
}

//-------------------------
LgProb WeightedIncrNormSlm::sumSentLenLgProb(unsigned int slen,
                                             unsigned int tlen)
{
  return (double)log((double)sumSentLenProb(slen,tlen));  
}

//-------------------------
LgProb WeightedIncrNormSlm::sentLenLgProbNorm(unsigned int slen,
                                              unsigned int tlen)
{
  Prob normprob=sumSentLenProbNorm(slen,tlen)-sumSentLenProbNorm(slen,tlen-1);
  if((double)normprob<SW_PROB_SMOOTH)
    return log(SW_PROB_SMOOTH);
  else return (double)log((double)normprob);
}

//-------------------------
Prob WeightedIncrNormSlm::sumSentLenProbNorm(unsigned int slen,
                                             unsigned int tlen)
{
  float mean;
  float stddev;
  
  bool found=get_mean_stddev(slen,mean,stddev);
  if(found)
  {
    return MathFuncs::norm_cdf(mean,stddev,tlen+0.5);
  }
  else
  {
        // Calculate mean difference between source and target sentence
        // lengths
    float diff;
    if(numSents!=0)
    {
      float avgSrcLen=(float)slenSum/numSents;
      float avgTrgLen=(float)tlenSum/numSents;
      diff=avgTrgLen-avgSrcLen;
    }
    else diff=0;

    mean=slen+diff;
    stddev=(mean)*(0.25);
    return MathFuncs::norm_cdf(mean,stddev,tlen+0.5);
  }
}

//-------------------------
bool WeightedIncrNormSlm::get_mean_stddev(unsigned int slen,
                                          float& mean,
                                          float& stddev)
{
  bool found;
  unsigned int k=get_k(slen,found);
  if(!found || k<2) return false;
  else
  {
    mean=get_mk(slen);
    float swk=get_swk(slen);
    float sk=get_sk(slen);
    stddev=sqrt((sk*k)/((k-1)*swk));
    return true;
  }
}

//-------------------------
bool WeightedIncrNormSlm::readNormalPars(const char *normParsFileName)
{
 awkInputStream awk;

 cerr<<"Reading sentence length model file from: "<<normParsFileName<<" , using a weighted incremental normal distribution"<<endl;
 if(awk.open(normParsFileName)==ERROR)
 {
   cerr<<"Error in sentence length model file, file "<<normParsFileName<<" does not exist.\n";
   return ERROR;
 }
 else
 {
   awk.getln(); // Skip first line
   
       // Read average lengths from second line
   awk.getln();
   if(awk.NF!=8)
   {
     cerr<<"Anomalous sentence length model file!"<<endl;
     return ERROR;
   }
   numSents=atoi(awk.dollar(2).c_str());
   slenSum=atoi(awk.dollar(5).c_str());
   tlenSum=atoi(awk.dollar(8).c_str());

       // Read gaussian parameters
   while(awk.getln())
   {
     if(awk.NF==5)
     {
       unsigned int slen=atoi(awk.dollar(1).c_str());
       unsigned int k_slen=atoi(awk.dollar(2).c_str());
       double swk_slen=atof(awk.dollar(3).c_str());
       double mk_slen=atof(awk.dollar(4).c_str());
       double sk_slen=atof(awk.dollar(5).c_str());

       set_k(slen,k_slen);
       set_swk(slen,swk_slen);
       set_mk(slen,mk_slen);
       set_sk(slen,sk_slen);
     }
   }
   return THOT_OK;
 }
}

//-------------------------
unsigned int WeightedIncrNormSlm::get_k(unsigned int slen,
                                        bool& found)
{
  if(slen>=kVec.size())
  {
    found=false;
    return 0;
  }
  else
  {
    if(kVec[slen]==0)
    {
      found=false;
      return 0;
    }
    else
    {
      found=true;
      return kVec[slen];
    }
  }
}

//-------------------------
void WeightedIncrNormSlm::set_k(unsigned int slen,
                                unsigned int k_val)
{
  while(kVec.size()<=slen)
    kVec.push_back(0);
  kVec[slen]=k_val;
}

//-------------------------
float WeightedIncrNormSlm::get_swk(unsigned int slen)
{
  if(slen<swkVec.size()) return swkVec[slen];
  else return 0;
}

//-------------------------
void WeightedIncrNormSlm::set_swk(unsigned int slen,
                                  float swk_val)
{
  while(swkVec.size()<=slen)
    swkVec.push_back(0);
  swkVec[slen]=swk_val;
}

//-------------------------
float WeightedIncrNormSlm::get_mk(unsigned int slen)
{
  if(slen<mkVec.size()) return mkVec[slen];
  else return 0;
}

//-------------------------
void WeightedIncrNormSlm::set_mk(unsigned int slen,
                                 float mk_val)
{
  while(mkVec.size()<=slen)
    mkVec.push_back(0);
  mkVec[slen]=mk_val;
}

//-------------------------
float WeightedIncrNormSlm::get_sk(unsigned int slen)
{
  if(slen<skVec.size()) return skVec[slen];
  else return 0;
}

//-------------------------
void WeightedIncrNormSlm::set_sk(unsigned int slen,
                                 float sk_val)
{
  while(skVec.size()<=slen)
    skVec.push_back(0);
  skVec[slen]=sk_val;
}

//-------------------------
void WeightedIncrNormSlm::trainSentPair(Vector<std::string> srcSentVec,
                                        Vector<std::string> trgSentVec,
                                        Count c)
{
  unsigned int slen=srcSentVec.size();
  unsigned int tlen=trgSentVec.size();
  bool found;

  ++numSents;
  slenSum+=slen;
  tlenSum+=tlen;

  unsigned int k_slen=get_k(slen,found);
  float swk;
  float mk;
  float sk;
  if(!found)
  {
    k_slen=1;
    set_k(slen,k_slen);
    sk=0;
    set_sk(slen,sk);
    mk=tlen;
    set_mk(slen,mk);
    swk=c;
    set_swk(slen,swk);
  }
  else
  {
    ++k_slen;
    set_k(slen,k_slen);
    swk=get_swk(slen);
    float temp=(float)c+swk;
    mk=get_mk(slen);
    sk=get_sk(slen);
    sk=sk+(swk*(float)c*((tlen-mk)*(tlen-mk)))/temp;
    set_sk(slen,sk);
    mk=mk+((tlen-mk)*(float)c)/temp;
    set_mk(slen,mk);
    swk=temp;
    set_swk(slen,swk);
  }
}

//-------------------------
void WeightedIncrNormSlm::clear(void)
{
  numSents=0;
  slenSum=0;
  tlenSum=0;
  kVec.clear();
  swkVec.clear();
  mkVec.clear();
  skVec.clear();
}

//-------------------------
