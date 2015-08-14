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
/* Module: WordPenaltyModel                                          */
/*                                                                   */
/* Definitions file: WordPenaltyModel.cc                             */
/*                                                                   */
/*********************************************************************/


//--------------- Include files ---------------------------------------

#include "WordPenaltyModel.h"

//--------------- Global variables ------------------------------------

//--------------- Function declarations 

//--------------- Constants

//--------------- Classes ---------------------------------------------

//-------------------------
WordPenaltyModel::WordPenaltyModel(void)
{
  minLen=1;
  maxLen=MAX_SENTENCE_LENGTH_ALLOWED+1;
  p_geom=DEFAULT_P_GEOM;
  mode=USE_GEOM;
}

//-------------------------
bool WordPenaltyModel::load(const char *filename)
{
  awkInputStream awk;
  
  clear();
  
  if(awk.open(filename)==ERROR)
  {
    cerr<<"Error in word penalty model file, file "<<filename<<" does not exist.\n";
    return ERROR;
  }
  if(awk.getln())
  {
    if(strcmp("Logarithmic",awk.dollar(1).c_str())==0)
    {
      cerr<<"Using word penalty model based on a logarithmic distribution."<<endl;
      mode=USE_LOGARITHMIC;
      return readLogPars(filename);
    }
    else
    {
      if(strcmp("Triangular",awk.dollar(1).c_str())==0)
      {
        cerr<<"Using word penalty model based on a triangular distribution."<<endl;
        mode=USE_TRIANG;
        return readTriPars(filename);
      }
      else
      {
        if(strcmp("Geometric",awk.dollar(1).c_str())==0)
        {
          cerr<<"Using word penalty model based on a geometric distribution."<<endl;
          mode=USE_GEOM;
          return readGeomPars(filename);
        }
        else
        {
          cerr<<"Anomalous word penalty model file: "<<filename<<"\n";
          return ERROR;
        }
      }
    }
  }
  else
  {
    cerr<<"Anomalous word penalty model file: "<<filename<<"\n";
    return ERROR;
  }
}

//-------------------------
LgProb WordPenaltyModel::wordPenaltyScore(unsigned int tlen)
{
  switch(mode)
  {
    case USE_LOGARITHMIC: return wordPenaltyScoreLogarithmic(tlen);
    case USE_TRIANG: return wordPenaltyScoreTriang(tlen);
    case USE_GEOM: return wordPenaltyScoreGeom(tlen);
    default: return wordPenaltyScoreLogarithmic(tlen);
  }
}

//-------------------------
LgProb WordPenaltyModel::sumWordPenaltyScore(unsigned int tlen)
{
  switch(mode)
  {
    case USE_GEOM: return wordPenaltyScoreGeom(tlen);
    default: return sumWordPenaltyScoreAux(tlen);
  }
}

//-------------------------
LgProb WordPenaltyModel::sumWordPenaltyScoreAux(unsigned int tlen)
{
  if(sum_wlp.size()>tlen)
  {
    return sum_wlp[tlen];
  }
  else
  {    
    if(tlen==0)
    {
      sum_wlp[0]=0;
      return 0;
    }
    else
    {
      LgProb lp=MathFuncs::lns_sublog_float(sumWordPenaltyScore(tlen-1),wordPenaltyScore(tlen-1));
      sum_wlp[tlen]=lp;
      return lp;
    }
  }
}

//-------------------------
LgProb WordPenaltyModel::wordPenaltyScoreLogarithmic(unsigned int tlen)
{
  Prob logprob=sumSentLenProbLogarithmic(tlen)-sumSentLenProbLogarithmic(tlen-1);
  if((double)logprob<LM_PROB_SMOOTH)
    return log(LM_PROB_SMOOTH);
  else return (double)log((double)logprob);
}

//-------------------------
Prob WordPenaltyModel::sumSentLenProbLogarithmic(unsigned int tlen)
{
  return MathFuncs::logarithmic_cdf(minLen,maxLen,tlen+1);
}

//-------------------------
LgProb WordPenaltyModel::wordPenaltyScoreTriang(unsigned int tlen)
{
  Prob logprob=sumSentLenProbTriang(tlen)-sumSentLenProbTriang(tlen-1);
  if((double)logprob<LM_PROB_SMOOTH)
    return log(LM_PROB_SMOOTH);
  else return (double)log((double)logprob);
}

//-------------------------
Prob WordPenaltyModel::sumSentLenProbTriang(unsigned int tlen)
{
  return MathFuncs::triang_cdf(minLen,maxLen,maxLen,tlen+1);
}

//-------------------------
LgProb WordPenaltyModel::wordPenaltyScoreGeom(unsigned int tlen)
{
  return MathFuncs::log_geom(p_geom,tlen);  
}

//-------------------------
bool WordPenaltyModel::readLogPars(const char *logParsFileName)
{
 awkInputStream awk;

 cerr<<"Reading parameters for the logarithmic word penalty model from: "<<logParsFileName<<endl;
 if(awk.open(logParsFileName)==ERROR)
 {
   cerr<<"Word penalty model file "<<logParsFileName<<" does not exist.\n";
   return ERROR;
 }  
 else
 {
   awk.getln(); // Skip first line

   awk.getln();
   if(awk.NF==1)
   {
     minLen=atoi(awk.dollar(1).c_str());
   }
   else
   {
     cerr<<"Error in word penalty model file: "<<logParsFileName<<" \n";
     return ERROR;
   }
   awk.getln();
   if(awk.NF==1)
   {
     maxLen=atoi(awk.dollar(1).c_str())+1;
   }
   else
   {
     cerr<<"Error in word penalty model file: "<<logParsFileName<<" \n";
     return ERROR;
   }
 }
 return OK;    
}

//-------------------------
bool WordPenaltyModel::readTriPars(const char *triParsFileName)
{
 awkInputStream awk;

 cerr<<"Reading parameters for the triangular word penalty model from: "<<triParsFileName<<endl;
 if(awk.open(triParsFileName)==ERROR)
 {
   cerr<<"Word penalty model file, file "<<triParsFileName<<" does not exist.\n";
   return ERROR;
 }  
 else
 {
   awk.getln(); // Skip first line

   awk.getln();
   if(awk.NF==1)
   {
     minLen=atoi(awk.dollar(1).c_str());
   }
   else
   {
     cerr<<"Error in word penalty model file: "<<triParsFileName<<" \n";
     return ERROR;
   }
   awk.getln();
   if(awk.NF==1)
   {
     maxLen=atoi(awk.dollar(1).c_str())+1;
   }
   else
   {
     cerr<<"Error in word penalty model file: "<<triParsFileName<<" \n";
     return ERROR;
   }
 }
 return OK;    
}

//-------------------------
bool WordPenaltyModel::readGeomPars(const char *geomParsFileName)
{
 awkInputStream awk;

 cerr<<"Reading parameters for the geometric word penalty model from: "<<geomParsFileName<<endl;
 if(awk.open(geomParsFileName)==ERROR)
 {
   cerr<<"Word penalty model file, file "<<geomParsFileName<<" does not exist.\n";
   return ERROR;
 }  
 else
 {
   awk.getln(); // Skip first line

   awk.getln();
   if(awk.NF==1)
   {
     p_geom=atof(awk.dollar(1).c_str());
   }
   else
   {
     cerr<<"Error in word penalty model file: "<<geomParsFileName<<" \n";
     return ERROR;
   }
 }
 return OK;    
}

//-------------------------
void WordPenaltyModel::clear(void)
{
  minLen=1;
  maxLen=MAX_SENTENCE_LENGTH_ALLOWED+1;
  p_geom=DEFAULT_P_GEOM;
  mode=USE_GEOM;
  sum_wlp.clear();
}

//-------------------------
