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
 
/********************************************************************/
/*                                                                  */
/* Module: _incrHmmP0AligModel                                      */
/*                                                                  */
/* Prototype file: _incrHmmP0AligModel.h                            */
/*                                                                  */
/* Description: Defines the _incrHmmP0AligModel class.              */
/*              _incrHmmP0AligModel class allows to generate and    */
/*              access to the data of a Hmm statistical             */
/*              alignment model with fixed p0 probability.          */
/*                                                                  */
/********************************************************************/

#ifndef __incrHmmP0AligModel_h
#define __incrHmmP0AligModel_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "_incrHmmAligModel.h"

//--------------- Constants ------------------------------------------

#define DEFAULT_HMM_P0 0.1

//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------

//--------------- Classes --------------------------------------------

//--------------- _incrHmmP0AligModel class

template<class ALIG_MODEL>
class _incrHmmP0AligModel: public ALIG_MODEL
{
  public:

      // Constructor
   _incrHmmP0AligModel();

      // Set hmm p0 value
   void set_hmm_p0(Prob _hmm_p0);

      // load function
   bool load(const char* prefFileName);

      // print function
   bool print(const char* prefFileName);

      // clear() function
   void clear(void);

  protected:

   Prob hmm_p0;

   bool loadHmmP0(const char *hmmP0FileName);
   bool printHmmP0(const char *hmmP0FileName);

   Vector<WordIndex> extendWithNullWordAlig(const Vector<WordIndex>& srcWordIndexVec);
   double unsmoothed_logaProb(PositionIndex prev_i,
                              PositionIndex slen,
                              PositionIndex i);
};

//--------------- Template function definitions

//-------------------------
template<class ALIG_MODEL>
_incrHmmP0AligModel<ALIG_MODEL>::_incrHmmP0AligModel(void):ALIG_MODEL()
{
  hmm_p0=DEFAULT_HMM_P0;
}

//-------------------------
template<class ALIG_MODEL>
void _incrHmmP0AligModel<ALIG_MODEL>::set_hmm_p0(Prob _hmm_p0)
{
  hmm_p0=_hmm_p0;
}

//-------------------------
template<class ALIG_MODEL>
bool _incrHmmP0AligModel<ALIG_MODEL>::load(const char* prefFileName)
{
  bool retVal=ALIG_MODEL::load(prefFileName);
  if(retVal==THOT_ERROR) return THOT_ERROR;

      // Load file with hmm p0 value
  std::string hmmP0File=prefFileName;
  hmmP0File=hmmP0File+".hmm_p0";
  retVal=loadHmmP0(hmmP0File.c_str());
  if(retVal==THOT_ERROR) return THOT_ERROR;

  return THOT_OK;
}
  
//-------------------------
template<class ALIG_MODEL>
bool _incrHmmP0AligModel<ALIG_MODEL>::print(const char* prefFileName)
{
  bool retVal=ALIG_MODEL::print(prefFileName);
  if(retVal==THOT_ERROR) return THOT_ERROR;

      // Print file with hmm p0 value
  std::string hmmP0File=prefFileName;
  hmmP0File=hmmP0File+".hmm_p0";
  retVal=printHmmP0(hmmP0File.c_str());
  if(retVal==THOT_ERROR) return THOT_ERROR;

  return THOT_OK;  
}

//-------------------------
template<class ALIG_MODEL>
void _incrHmmP0AligModel<ALIG_MODEL>::clear(void)
{
  ALIG_MODEL::clear();
  hmm_p0=DEFAULT_HMM_P0;  
}

//-------------------------
template<class ALIG_MODEL>
bool _incrHmmP0AligModel<ALIG_MODEL>::loadHmmP0(const char *hmmP0FileName)
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
template<class ALIG_MODEL>
bool _incrHmmP0AligModel<ALIG_MODEL>::printHmmP0(const char *hmmP0FileName)
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
template<class ALIG_MODEL>
Vector<WordIndex> _incrHmmP0AligModel<ALIG_MODEL>::extendWithNullWordAlig(const Vector<WordIndex>& srcWordIndexVec)
{
      // No extra NULL words are added when calculating suff. statistics
      // for alignment parameters, since the alignment with the NULL
      // word is not a free parameter in this model
  return srcWordIndexVec;
}

//-------------------------
template<class ALIG_MODEL>
double _incrHmmP0AligModel<ALIG_MODEL>::unsmoothed_logaProb(PositionIndex prev_i,
                                                            PositionIndex slen,
                                                            PositionIndex i)
{
  HmmAligInfo hmmAligInfo;
  ALIG_MODEL::getHmmAligInfo(prev_i,slen,i,hmmAligInfo);
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

      numer=ALIG_MODEL::incrHmmAligTable.getAligNumer(asHmm,i,found);
      if(found)
      {
            // aligNumer for pair asHmm,i exists
        double denom;
        denom=ALIG_MODEL::incrHmmAligTable.getAligDenom(asHmm,found);
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

#endif
