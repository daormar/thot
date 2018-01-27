/*
thot package for statistical machine translation
Copyright (C) 2013-2017 Daniel Ortiz-Mart\'inez, Adam Harasimowicz

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
 * @file _incrHmmP0AligModel.cc
 * 
 * @brief Definitions file for _incrHmmP0AligModel.h
 */

//--------------- Include files --------------------------------------

#include "_incrHmmP0AligModel.h"

//--------------- _incrHmmP0AligModel class function definitions

//-------------------------
_incrHmmP0AligModel::_incrHmmP0AligModel(void):_incrHmmAligModel()
{
  hmm_p0=DEFAULT_HMM_P0;
}

//-------------------------
void _incrHmmP0AligModel::set_hmm_p0(Prob _hmm_p0)
{
  hmm_p0=_hmm_p0;
}

//-------------------------
bool _incrHmmP0AligModel::load(const char* prefFileName)
{
  bool retVal=_incrHmmAligModel::load(prefFileName);
  if(retVal==THOT_ERROR) return THOT_ERROR;

      // Load file with hmm p0 value
  std::string hmmP0File=prefFileName;
  hmmP0File=hmmP0File+".hmm_p0";
  retVal=loadHmmP0(hmmP0File.c_str());
  if(retVal==THOT_ERROR) return THOT_ERROR;

  return THOT_OK;
}

//-------------------------
bool _incrHmmP0AligModel::print(const char* prefFileName)
{
  bool retVal=_incrHmmAligModel::print(prefFileName);
  if(retVal==THOT_ERROR) return THOT_ERROR;

      // Print file with hmm p0 value
  std::string hmmP0File=prefFileName;
  hmmP0File=hmmP0File+".hmm_p0";
  retVal=printHmmP0(hmmP0File.c_str());
  if(retVal==THOT_ERROR) return THOT_ERROR;

  return THOT_OK;
}

//-------------------------
void _incrHmmP0AligModel::clear(void)
{
  _incrHmmAligModel::clear();
  hmm_p0=DEFAULT_HMM_P0;
}

//-------------------------
bool _incrHmmP0AligModel::loadHmmP0(const char *hmmP0FileName)
{
  std::cerr<<"Loading file with hmm p0 value from "<<hmmP0FileName<<std::endl;

  AwkInputStream awk;

  if(awk.open(hmmP0FileName)==THOT_ERROR)
  {
    std::cerr<<"Error in file with hmm p0 value, file "<<hmmP0FileName<<" does not exist. Assuming hmm_p0="<<DEFAULT_HMM_P0<<"\n";
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
        std::cerr<<"hmm p0 value has been set to "<<hmm_p0<<std::endl;
        return THOT_OK;
      }
      else
      {
        std::cerr<<"Error: anomalous .hmm_p0 file, "<<hmmP0FileName<<std::endl;
        return THOT_ERROR;
      }
    }
    else
    {
      std::cerr<<"Error: anomalous .hmm_p0 file, "<<hmmP0FileName<<std::endl;
      return THOT_ERROR;
    }
  }
}

//-------------------------
bool _incrHmmP0AligModel::printHmmP0(const char *hmmP0FileName)
{
  std::ofstream outF;
  outF.open(hmmP0FileName,std::ios::out);
  if(!outF)
  {
    std::cerr<<"Error while printing file with hmm p0 value."<<std::endl;
    return THOT_ERROR;
  }
  else
  {
    outF<<hmm_p0<<std::endl;
    return THOT_OK;
  }
}

//-------------------------
std::vector<WordIndex> _incrHmmP0AligModel::extendWithNullWordAlig(const std::vector<WordIndex>& srcWordIndexVec)
{
      // No extra NULL words are added when calculating suff. statistics
      // for alignment parameters, since the alignment with the NULL
      // word is not a free parameter in this model
  return srcWordIndexVec;
}

//-------------------------
double _incrHmmP0AligModel::unsmoothed_logaProb(PositionIndex prev_i,
                                                PositionIndex slen,
                                                PositionIndex i)
{
  HmmAligInfo hmmAligInfo;
  _incrHmmAligModel::getHmmAligInfo(prev_i,slen,i,hmmAligInfo);
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

      numer=_incrHmmAligModel::incrHmmAligTable.getAligNumer(asHmm,i,found);
      if(found)
      {
            // aligNumer for pair asHmm,i exists
        double denom;
        denom=_incrHmmAligModel::incrHmmAligTable.getAligDenom(asHmm,found);
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
