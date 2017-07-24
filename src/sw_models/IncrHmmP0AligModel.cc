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
/* Module: IncrHmmP0AligModel                                       */
/*                                                                  */
/* Definitions file: IncrHmmP0AligModel.cc                          */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "IncrHmmP0AligModel.h"

//--------------- IncrHmmP0AligModel class function definitions

//-------------------------
IncrHmmP0AligModel::IncrHmmP0AligModel(void):IncrHmmAligModel()
{
  hmm_p0=DEFAULT_HMM_P0;
}

//-------------------------
void IncrHmmP0AligModel::set_hmm_p0(Prob _hmm_p0)
{
  hmm_p0=_hmm_p0;
}

//-------------------------
bool IncrHmmP0AligModel::load(const char* prefFileName)
{
  bool retVal=IncrHmmAligModel::load(prefFileName);
  if(retVal==THOT_ERROR) return THOT_ERROR;

      // Load file with hmm p0 value
  std::string hmmP0File=prefFileName;
  hmmP0File=hmmP0File+".hmm_p0";
  retVal=loadHmmP0(hmmP0File.c_str());
  if(retVal==THOT_ERROR) return THOT_ERROR;

  return THOT_OK;
}
   
//-------------------------
bool IncrHmmP0AligModel::print(const char* prefFileName)
{
  bool retVal=IncrHmmAligModel::print(prefFileName);
  if(retVal==THOT_ERROR) return THOT_ERROR;

      // Print file with hmm p0 value
  std::string hmmP0File=prefFileName;
  hmmP0File=hmmP0File+".hmm_p0";
  retVal=printHmmP0(hmmP0File.c_str());
  if(retVal==THOT_ERROR) return THOT_ERROR;

  return THOT_OK;  
}

//-------------------------
void IncrHmmP0AligModel::clear(void)
{
  IncrHmmAligModel::clear();
  hmm_p0=DEFAULT_HMM_P0;  
}

//-------------------------
bool IncrHmmP0AligModel::loadHmmP0(const char *hmmP0FileName)
{
  cerr<<"Loading file with hmm p0 value from "<<hmmP0FileName<<endl;
  
  awkInputStream awk;
    
  if(awk.open(hmmP0FileName)==THOT_ERROR)
  {
    cerr<<"Error in file with hmm p0 value, file "<<hmmP0FileName<<" does not exist. Assuming hmm_p0="<<DEFAULT_HMM_P0<<"\n";
    hmm_p0=DEFAULT_HMM_P0;
    return THOT_OK;
  }  
  else
  {
    if(awk.getln())
    {
      if(awk.NF==1)
      {
        hmm_p0=(Prob)atof(awk.dollar(1).c_str());
        cerr<<"hmm p0 value has been set to "<<hmm_p0<<endl;
        return THOT_OK;
      }
      else
      {
        cerr<<"Error: anomalous .hmm_p0 file, "<<hmmP0FileName<<endl;
        return THOT_ERROR;
      }
    }
    else
    {
      cerr<<"Error: anomalous .hmm_p0 file, "<<hmmP0FileName<<endl;
      return THOT_ERROR;
    }
  }
}

//-------------------------
bool IncrHmmP0AligModel::printHmmP0(const char *hmmP0FileName)
{
  ofstream outF;
  outF.open(hmmP0FileName,ios::out);
  if(!outF)
  {
    cerr<<"Error while printing file with hmm p0 value."<<endl;
    return THOT_ERROR;
  }
  else
  {
    outF<<hmm_p0<<endl;
    return THOT_OK;
  }
}

//-------------------------
Vector<WordIndex> IncrHmmP0AligModel::extendWithNullWordAlig(const Vector<WordIndex>& srcWordIndexVec)
{
      // No extra NULL words are added when calculating suff. statistics
      // for alignment parameters, since the alignment with the NULL
      // word is not a free parameter in this model
  return srcWordIndexVec;
}

//-------------------------
double IncrHmmP0AligModel::unsmoothed_logaProb(PositionIndex prev_i,
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
    if(hmmAligInfo.nullAlig)
    {
      if(prev_i==0)
      {
        return log((double)hmm_p0)-log((double)slen);
      }
      else
        return log((double)hmm_p0);
    }
    else
    {
      bool found;
      double numer;
      aSourceHmm asHmm;
      asHmm.prev_i=hmmAligInfo.modified_ip;
      asHmm.slen=slen;

      numer=incrHmmAligTable.getAligNumer(asHmm,i,found);
      if(found)
      {
            // aligNumer for pair asHmm,i exists
        double denom;
        denom=incrHmmAligTable.getAligDenom(asHmm,found);
        if(!found) return SMALL_LG_NUM;
        else
        {
          LgProb lp=numer-denom;
          return lp+log((double)1.0-(double)hmm_p0);
        }
      }
      else
      {
            // aligNumer for pair asHmm,i does not exist
        return SMALL_LG_NUM;
      }
    }
  }
}
