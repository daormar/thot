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
/* Module: BasePhraseModel                                          */
/*                                                                  */
/* Definitions file: BasePhraseModel.cc                             */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "BasePhraseModel.h"

//--------------- Function definitions


//-------------------------
Prob BasePhraseModel::strPt_s_(const Vector<string>& s,
                               const Vector<string>& t)
{
  return exp((double)strLogpt_s_(s,t));
}

//-------------------------
LgProb BasePhraseModel::strLogpt_s_(const Vector<string>& s,
                                    const Vector<string>& t)
{
  Vector<WordIndex> wIndex_s,wIndex_t;

  for(unsigned int i=0;i<s.size();++i)
    wIndex_s.push_back(stringToSrcWordIndex(s[i]));

  for(unsigned int i=0;i<t.size();++i)
    wIndex_t.push_back(stringToTrgWordIndex(t[i]));

  return logpt_s_(wIndex_s,wIndex_t);
}

//-------------------------
Prob BasePhraseModel::pt_s_(const Vector<WordIndex>& s,
                            const Vector<WordIndex>& t)
{
  return exp((double)logpt_s_(s,t));
}

//-------------------------
Prob BasePhraseModel::strPs_t_(const Vector<string>& s,
                               const Vector<string>& t)
{
  return exp((double)strLogps_t_(s,t));
}

//-------------------------
LgProb BasePhraseModel::strLogps_t_(const Vector<string>& s,
                                    const Vector<string>& t)
{
  Vector<WordIndex> wIndex_s,wIndex_t;

  for(unsigned int i=0;i<s.size();++i)
    wIndex_s.push_back(stringToSrcWordIndex(s[i]));

  for(unsigned int i=0;i<t.size();++i)
    wIndex_t.push_back(stringToTrgWordIndex(t[i]));

  return logps_t_(wIndex_s,wIndex_t);
}

//-------------------------
Prob BasePhraseModel::ps_t_(const Vector<WordIndex>& s,
                            const Vector<WordIndex>& t)
{
  return exp((double)logps_t_(s,t));
}
    
//-------------------------
bool BasePhraseModel::strGetTransFor_s_(const Vector<string>& s,
                                        TrgTableNode& trgtn)
{
  Vector<WordIndex> wIndex_s;

  for(unsigned int i=0;i<s.size();++i)
    wIndex_s.push_back(stringToSrcWordIndex(s[i]));

  return getTransFor_s_(wIndex_s,trgtn);  
}

//-------------------------
bool BasePhraseModel::strGetTransFor_t_(const Vector<string>& t,
                                        SrcTableNode& srctn)
{
  Vector<WordIndex> wIndex_t;

  for(unsigned int i=0;i<t.size();++i)
    wIndex_t.push_back(stringToTrgWordIndex(t[i]));

  return getTransFor_t_(wIndex_t,srctn);
}

//-------------------------
bool BasePhraseModel::strGetNbestTransFor_s_(const Vector<string>& s,
                                             NbestTableNode<PhraseTransTableNodeData>& nbt)
{
  Vector<WordIndex> wIndex_s;

  for(unsigned int i=0;i<s.size();++i)
    wIndex_s.push_back(stringToSrcWordIndex(s[i]));

  return getNbestTransFor_s_(wIndex_s,nbt);
}

//-------------------------
bool BasePhraseModel::strGetNbestTransFor_t_(const Vector<string>& t,
                                             NbestTableNode<PhraseTransTableNodeData>& nbt,
                                             int N/*=-1*/)
{
  Vector<WordIndex> wIndex_t;

  for(unsigned int i=0;i<t.size();++i)
    wIndex_t.push_back(stringToTrgWordIndex(t[i]));

  return getNbestTransFor_t_(wIndex_t,nbt,N);
}

//-------------------------
int BasePhraseModel::trainSentPair(const Vector<std::string>& /*srcSentStrVec*/,
                                   const Vector<std::string>& /*trgSentStrVec*/,
                                   Count /*c*/,
                                   int /*verbose*/)
{
  cerr<<"Warning: Phrase-based model training of a sentence pair was requested, but such functionality is not provided!"<<endl;
  return ERROR;
}

//-------------------------
int BasePhraseModel::trainBilPhrases(const Vector<Vector<std::string> >& /*srcPhrVec*/,
                                     const Vector<Vector<std::string> >& /*trgPhrVec*/,
                                     Count /*c*/,
                                     Count /*lowerBound*/,
                                     int /*verbose*/)
{
  cerr<<"Warning: Phrase-based model training of bilingual phrases was requested, but such functionality is not provided!"<<endl;
  return ERROR;
}

//-------------------------
