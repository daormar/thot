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
/* Module: NonPbEcModelForNbUcat                                    */
/*                                                                  */
/* Definitions file: NonPbEcModelForNbUcat.cc                       */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "NonPbEcModelForNbUcat.h"


//--------------- NonPbEcModelForNbUcat class functions
//

//---------------------------------------
NonPbEcModelForNbUcat::NonPbEcModelForNbUcat()
{
}

//---------------------------------------
void NonPbEcModelForNbUcat::link_ecm(BaseErrorCorrectingModel* _ecm_ptr)
{
  ecm_ptr=_ecm_ptr;
}

//---------------------------------------
NbestCorrections
NonPbEcModelForNbUcat::correct(const Vector<std::string>& outputSentVec,
                               const Vector<unsigned int>& /*sourceCuts*/,
                               const Vector<std::string>& prefixVec,
                               unsigned int maxMapSize,
                               int verbose)
{
  MonolingSegmNbest monolingSegmNbest;
  MonolingSegmNbest::iterator nbestIter;
  NbestCorrections result;
  
  monolingSegmNbest=nonPhraseBasedAlign(outputSentVec,prefixVec,maxMapSize,verbose);

  for(nbestIter=monolingSegmNbest.begin();nbestIter!=monolingSegmNbest.end();++nbestIter)
  {
    result.insert(make_pair(nbestIter->first,correctedSent(nbestIter->second)));
  }
  return result;
}

//---------------------------------------
NonPbEcModelForNbUcat::MonolingSegmNbest
NonPbEcModelForNbUcat::nonPhraseBasedAlign(const Vector<std::string>& _outputSentVec,
                                           const Vector<std::string>& _prefixVec,
                                           unsigned int _maxMapSize,
                                           int verbose)
{
      // Init vars
  outputSentVec=_outputSentVec;
  sourceCuts.clear();
  sourceCuts.push_back(outputSentVec.size()-1);
  prefixVec=_prefixVec;
  
  if(verbose) cerr<<"*** Starting prefix aligner"<<endl;

      // Clear n-best corrections list
  monolingSegmNbest.clear();

      // Set maximum map size
  maxMapSize=_maxMapSize;

      // Create vector with source segments
  if(verbose) cerr<<"* Create vector with source segments"<<endl;
  outputSegmVec=obtainVectorWithSegms(outputSentVec,sourceCuts,verbose);

      // Check the prefix length
  if(_prefixVec.size()==0)
  {
    PrefAlignInfo prefAlignInfo;
    
    addSegm(log(1.0),prefAlignInfo);
    
        // return n-best corrections list
    return monolingSegmNbest;
  }
  else
  {
    LgProb sim;
    WordAndCharLevelOps wcOps;
    PrefAlignInfo prefAlignInfo;

        // Obtain similarity
    sim=ecm_ptr->similarityGivenPrefix(_outputSentVec,_prefixVec);

        // Store alignment in the n-best corrections list
    prefAlignInfo.transCuts.push_back(_outputSentVec.size()-1);
    prefAlignInfo.prefCuts.push_back(_prefixVec.size()-1);
    addSegm(sim,prefAlignInfo);

        // return n-best corrections list
    return monolingSegmNbest;
  }
}

//---------------------------------------
Vector<Vector<std::string> >
NonPbEcModelForNbUcat::obtainVectorWithSegms(Vector<std::string> sentVec,
                                             Vector<unsigned int> cuts,
                                             int verbose)
{
  Vector<Vector<std::string> > result;
  
  for(unsigned int i=0;i<cuts.size();++i)
  {
    Vector<std::string> segm;
    unsigned int j;
    
    if(i==0) j=0;
    else j=cuts[i-1]+1;
    for(;j<=cuts[i];++j)
    {
      segm.push_back(sentVec[j]);
    }
    result.push_back(segm);
    if(verbose)
    {
      for(unsigned int j=0;j<segm.size();++j)
        cerr<<segm[j]<<" ";
      cerr<<endl;
    }
  }
  return result;
}

//---------------------------------------
void NonPbEcModelForNbUcat::addSegm(LgProb lp,
                                    PrefAlignInfo& prefAlignInfo)
{
  monolingSegmNbest.insert(pair<float,PrefAlignInfo>(lp,prefAlignInfo));
  if(monolingSegmNbest.size()>maxMapSize)
    removeLastFromMonolingSegmNbest();
}

//---------------------------------------
void NonPbEcModelForNbUcat::removeLastFromMonolingSegmNbest(void)
{
  MonolingSegmNbest::iterator pos;  
	
  if(!monolingSegmNbest.empty())
  {	 
    pos=monolingSegmNbest.end();
    --pos;
    monolingSegmNbest.erase(pos--);
  }
}

//---------------------------------------
Vector<std::string> NonPbEcModelForNbUcat::correctedSent(PrefAlignInfo& prefAlignInfo)
{
  Vector<std::string> lastOutSegm;
  Vector<std::string> lastPrefSegm;
  Vector<std::string> corrSent;
  Vector<std::string> correctedLastSegm;
  unsigned int l;
  unsigned int lastTransCut;
  
      // Determine the value of l
  l=0;
  if(prefAlignInfo.transCuts.size()>0)
  {
    lastTransCut=prefAlignInfo.transCuts.back();
    for(unsigned int i=0;i<sourceCuts.size();++i)
    {
      if(sourceCuts[i]<=lastTransCut) ++l;
      else break;
    }
  }
      // Obtain corrected sentence and the last prefix segment
      // separately
  for(unsigned int i=0;i<prefAlignInfo.prefCuts.size();++i)
  {
    unsigned int j;
    
    if(i==0) j=0;
    else j=prefAlignInfo.prefCuts[i-1]+1;
    for(;j<=prefAlignInfo.prefCuts[i];++j)
    {
      if(i==prefAlignInfo.prefCuts.size()-1)
      {
        lastPrefSegm.push_back(prefixVec[j]);
      }
      else corrSent.push_back(prefixVec[j]);
    }
  }
  if(l>0)
  {
        // Obtain corrected last segment
    getLastOutSegm(outputSentVec,prefAlignInfo.transCuts,lastOutSegm);
    ecm_ptr->correctStrGivenPref(lastOutSegm,lastPrefSegm,correctedLastSegm);
        // Add corrected last segment
    for(unsigned int i=0;i<correctedLastSegm.size();++i)
    {
      corrSent.push_back(correctedLastSegm[i]);
    }
  }
  if(l<outputSegmVec.size())
  {
        // Add unaligned segments
    for(unsigned int j=l;j<outputSegmVec.size();++j)
    {
      for(unsigned int k=0;k<outputSegmVec[j].size();++k)
      {
        corrSent.push_back(outputSegmVec[j][k]);
      }
    }
  }
  else
  {
        // restore last blank if necessary
    if(lastPrefSegm.back()[lastPrefSegm.back().size()-1]==' ')
    {
      if(corrSent[corrSent.size()-1]+' '==lastPrefSegm.back())
        corrSent[corrSent.size()-1]=lastPrefSegm.back();
    }
  }
  
  return corrSent;
}

//---------------------------------------
void NonPbEcModelForNbUcat::getLastOutSegm(Vector<std::string>& x,
                                           Vector<unsigned int>& cuts,
                                           Vector<std::string>& lastOutSegm)
{
  unsigned int left;
  unsigned int right;

  lastOutSegm.clear();
  if(cuts.size()==1) left=0;
  else left=cuts[cuts.size()-2]+1;
  right=cuts.back();
  for(unsigned int i=left;i<=right;++i)
  {
    lastOutSegm.push_back(x[i]);
  }
}

//---------------------------------------
void NonPbEcModelForNbUcat::clear(void)
{
  outputSentVec.clear();
  sourceCuts.clear();
  outputSegmVec.clear();
  prefixVec.clear();
  monolingSegmNbest.clear();
}

//---------------------------------------
void NonPbEcModelForNbUcat::clearTempVars(void)
{
  ecm_ptr->clearTempVars();
}

//---------------------------------------
NonPbEcModelForNbUcat::~NonPbEcModelForNbUcat()
{
}
