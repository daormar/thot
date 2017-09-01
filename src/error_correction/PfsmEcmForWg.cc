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
/* Module: PfsmEcmForWg                                             */
/*                                                                  */
/* Definitions file: PfsmEcmForWg.cc                                */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "PfsmEcmForWg.h"

//--------------- PfsmEcmForWg class functions
//

//---------------------------------------
void PfsmEcmForWg::correctStrGivenPrefWg(std::vector<std::string> uncorrStrVec,
                                         std::vector<std::string> prefStrVec,
                                         std::vector<std::string>& correctedStrVec)
{
  WordAndCharLevelOps wcOps;
  
  editDistForVecStr.calculateEditDistPrefixOpsNoPrefDel(uncorrStrVec,prefStrVec,wcOps.first,wcOps.second);
  correctStrGivenPrefOps(wcOps,uncorrStrVec,prefStrVec,correctedStrVec);
}

//---------------------------------------
PfsmEcmForWg::EcmScoreInfo
PfsmEcmForWg::constructInitEsi(void)
{
  EcmScoreInfo esi;

  std::vector<std::string> xVec;
  std::vector<std::string> yVec;

  Score scr=editDistForVecStr.calculateEditDist(xVec,yVec);
  esi.scrVec.push_back(scr);
  
  return esi;  
}

//---------------------------------------
PfsmEcmForWg::EcmScoreInfo
PfsmEcmForWg::constructEsi(const EcmScoreInfo& prevEsi,
                           const std::string& word)
{
  EcmScoreInfo esi;

  std::vector<std::string> xVec;
  xVec.push_back(word);
  std::vector<std::string> yVec;

      // Create score vector
  Score scr=editDistForVecStr.calculateEditDist(xVec,yVec);
  esi.scrVec.push_back(prevEsi.scrVec[0]+scr);

      // Create predecessor for prefix word vector
  esi.opIdVec.push_back(NONE_OP);
  
  return esi;
}

//---------------------------------------
void PfsmEcmForWg::extendInitialEsi(const std::vector<std::string>& prefixDiffVec,
                                    const EcmScoreInfo& prevInitEsi,
                                    EcmScoreInfo& newInitEsi)
{
      // Extend score vector
  editDistForVecStr.incrEditDistPrefixFirstRow(prefixDiffVec,
                                               prevInitEsi.scrVec,
                                               newInitEsi.scrVec);
}
  
//---------------------------------------
void PfsmEcmForWg::extendEsi(const std::vector<std::string>& prefixDiffVec,
                             const EcmScoreInfo& prevEsi,
                             const std::string& word,
                             EcmScoreInfo& newEsi)
{
      // Extend score vector
  std::vector<int> opIdVec;
  editDistForVecStr.incrEditDistPrefix(word,
                                       prefixDiffVec,
                                       prevEsi.scrVec,
                                       newEsi.scrVec,
                                       opIdVec);

      // Extend predecessor vector
  for(unsigned int i=0;i<opIdVec.size();++i)
    newEsi.opIdVec.push_back(opIdVec[i]);
}

//---------------------------------------
std::vector<Score> PfsmEcmForWg::obtainScrVecFromEsi(const EcmScoreInfo& esi)
{
  std::vector<Score> scrVec;

  for(unsigned int i=0;i<esi.scrVec.size();++i)
    scrVec.push_back(-esi.scrVec[i]);
  
  return scrVec;
}

//---------------------------------------
std::vector<int> PfsmEcmForWg::obtainLastInsPrefWordVecFromEsi(const EcmScoreInfo& esi)
{
      // Extract info from esi
  std::vector<int> lastInsPrefWordVec(esi.opIdVec.size(),0);
  int j_aux;
  for(int j=esi.opIdVec.size()-1;j>=0;--j)
  {
    switch(esi.opIdVec[j])
    {
      case HIT_OP: lastInsPrefWordVec[j]=j-1;
        break;
      case INS_OP:
        j_aux=j;
        while(j_aux>=0 && esi.opIdVec[j_aux]==INS_OP) --j_aux;
        if(esi.opIdVec[j_aux]==HIT_OP || esi.opIdVec[j_aux]==SUBST_OP) --j_aux;
        lastInsPrefWordVec[j]=j_aux;
        break;
      case DEL_OP: lastInsPrefWordVec[j]=j;
        break;
      case SUBST_OP: lastInsPrefWordVec[j]=j-1;
        break;
      case NONE_OP: lastInsPrefWordVec[j]=0;
        break;
    }
  }
      // Return result
  return lastInsPrefWordVec;
}

//---------------------------------------
void PfsmEcmForWg::updateEsiPositions(const EcmScoreInfo& esi1,
                                      const std::vector<unsigned int>& posVec,
                                      EcmScoreInfo& esi2)
{
      // Update score vector
  while(esi2.scrVec.size()<esi1.scrVec.size())
    esi2.scrVec.push_back(0);

  for(unsigned int i=0;i<posVec.size();++i)
    esi2.scrVec[posVec[i]]=esi1.scrVec[posVec[i]];

      // Update predecessor vector
  while(esi2.opIdVec.size()<esi1.opIdVec.size())
    esi2.opIdVec.push_back(0);

  for(unsigned int i=0;i<posVec.size();++i)
  {
    if(esi1.opIdVec.size()>i)
      esi2.opIdVec[posVec[i]]=esi1.opIdVec[posVec[i]];
  }
}

//---------------------------------------
unsigned int PfsmEcmForWg::numberOfPosInEsi(EcmScoreInfo& esi)
{
  return esi.scrVec.size();
}

//---------------------------------------
void PfsmEcmForWg::removeLastPosFromEsi(EcmScoreInfo& esi)
{
      // Remove info for last word (esi.vecOfLgProbVec[0] stores
      // initialization info)
  if(esi.scrVec.size()>1)
  {
    esi.scrVec.pop_back();
  }

  if(esi.opIdVec.size()>1)
  {
    esi.opIdVec.pop_back();
  }
}

//---------------------------------------
PfsmEcmForWg::~PfsmEcmForWg()
{
}
