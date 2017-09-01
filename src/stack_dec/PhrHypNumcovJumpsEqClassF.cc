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
/* Module: PhrHypNumcovJumpsEqClassF                                */
/*                                                                  */
/* Definitions file: PhrHypNumcovJumpsEqClassF.cc                   */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "PhrHypNumcovJumpsEqClassF.h"


//--------------- PhrHypNumcovJumpsEqClassF class functions
//

//---------------------------------
PhrHypNumcovJumpsEqClassF::EqClassType
PhrHypNumcovJumpsEqClassF::operator()(const PhrHypData& pbtHypData)
{
  EqClassType eqClass;
  std::vector<unsigned int> uivec;
  
  eqClass.first=0;  // eqClass.first will store the number of covered
                    // words
  
  eqClass.second=0; // eqClass.second will store the number of jumps in
                    // the alignment
  
  for(unsigned int k=0;k<pbtHypData.sourceSegmentation.size();k++)
  {
        // Update first value
    eqClass.first+=pbtHypData.sourceSegmentation[k].second-pbtHypData.sourceSegmentation[k].first+1;

        // Update second value
    if(k==0)
    {
      if(pbtHypData.sourceSegmentation[0].first>1)
        ++eqClass.second;
    }
    else
    {
      if(pbtHypData.sourceSegmentation[k-1].second+1!=pbtHypData.sourceSegmentation[k].first)
        ++eqClass.second;
    }
    
        // Update coverage vector
    for(unsigned int j=pbtHypData.sourceSegmentation[k].first;j<=pbtHypData.sourceSegmentation[k].second;++j)
      set(j-1,uivec);
  }

      // Transform equivalence class (a virtual function is used to
      // easily derive new classes)
  transformRawEqClass(eqClass);
  
  return eqClass;
}

//---------------------------------
void PhrHypNumcovJumpsEqClassF::set(unsigned int j,
                                    std::vector<unsigned int> &uivec)
{
  while(uivec.size()<=j)
  {
    uivec.push_back(0);
  }
  uivec[j]=1;
}

//---------------------------------
void PhrHypNumcovJumpsEqClassF::transformRawEqClass(EqClassType &/*eqc*/)
{
}
