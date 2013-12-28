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
/* Module: StlNgtComponent                                          */
/*                                                                  */
/* Definitions file: StlNgtComponent.cc                             */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "StlNgtComponent.h"

//--------------- Global variables -----------------------------------


//--------------- StlNgtComponent class method definitions

//-------------------------
StlNgtComponent::StlNgtComponent(void)
{
}

//-------------------------
NgIdx StlNgtComponent::updateHistInfo(const Vector<WordIndex>& hist,
                                      Count histCount)
{
  NgIdx histNgIdx=histInfo.size();
  bool idxAlreadyExists;
  NgIdx actualHistNgIdx=updateHistInfoGivenIdx(hist,histNgIdx,histCount,idxAlreadyExists);
  return actualHistNgIdx;
}

//-------------------------
NgIdx StlNgtComponent::updateHistInfoGivenIdx(const Vector<WordIndex>& hist,
                                              NgIdx hngidx,
                                              Count histCount,
                                              bool& idxAlreadyExists)
{
  HistInfo::iterator hiIter=histInfo.find(hist);
  if(hiIter!=histInfo.end())
  {
        // A node for hist already exists
    hiIter->second.second=histCount;
        // Return phrase index
    return hiIter->second.first;
  }
  else
  {
        // Create node for hist in histInfo
    std::pair<NgIdx,Count> ngIdxCountPair;
    ngIdxCountPair.first=hngidx;
    ngIdxCountPair.second=histCount;
    pair<HistInfo::iterator,bool> iterBoolPair=histInfo.insert(make_pair(hist,ngIdxCountPair));

        // Complete info for hist index
    while(histNgIdxInfo.size()<=ngIdxCountPair.first)
    {
      HistInfo::iterator iter=histInfo.end();
      histNgIdxInfo.push_back(iter);
    }
    histNgIdxInfo[ngIdxCountPair.first]=iterBoolPair.first;

        // Return phrase index
    return hngidx;
  }  
}

//-------------------------
std::pair<NgIdx,Count>
StlNgtComponent::getHistInfo(const Vector<WordIndex>& hist,
                             bool& found)
{
  HistInfo::iterator hiIter=histInfo.find(hist);
  if(hiIter!=histInfo.end())
  {
        // A node for hist already exists
    found=true;
    return hiIter->second;
  }
  else
  {
        // There is not a node for hist
    found=false;
    std::pair<NgIdx,Count> ngIdxCountPair;
    ngIdxCountPair.first=0;
    ngIdxCountPair.second=0;
    return ngIdxCountPair;
  }
}

//-------------------------
Count StlNgtComponent::getSrcInfo(const Vector<WordIndex>& srcVec,
                                  bool& found)
{
  std::pair<NgIdx,Count> ngidxCountPair=getHistInfo(srcVec,found);
  return ngidxCountPair.second;
}

//-------------------------
void StlNgtComponent::getHistGivenNgIdx(NgIdx hngidx,
                                        Vector<WordIndex>& hist,
                                        Count& c,
                                        bool& found)
{
  if(hngidx<histNgIdxInfo.size())
  {
        // Obtain iterator to source phrase info
    HistInfo::iterator hiIter=histNgIdxInfo[hngidx];

        // Obtain hist info
    hist=hiIter->first;
    c=hiIter->second.second;

        // Set value of found variable
    found=true;
  }
  else
  {
        // Set hist info
    hist.clear();
    c=0;
        // Set value of found variable
    found=false;
  }
}

//-------------------------
NgIdx StlNgtComponent::updateNgCounts(const Vector<WordIndex>& hist,
                                      WordIndex w,
                                      Count histCount,
                                      Count srcTrgCount)
{
      // Update source phrase count
  NgIdx histNgIdx=updateHistInfo(hist,histCount);
  
      // Update joint count
  updateJointCount(histNgIdx,w,srcTrgCount);

      // Return indices
  return histNgIdx;
}

//-------------------------
void StlNgtComponent::addTableEntry(const Vector<WordIndex>& s,
                                    const WordIndex& t,
                                    im_pair<Count,Count> inf)
{
  NgIdx ngidx=updateNgCounts(s,t,inf.first,inf.second);
}

//-------------------------
NgIdx StlNgtComponent::increaseNgCount(const Vector<WordIndex>& hist,
                                       WordIndex w,
                                       Count c)
{
      // Increase source phrase count
  bool found;
  NgIdx hngidx;
  
  std::pair<NgIdx,Count> ngidxCountPair=getHistInfo(hist,found);
  if(found)
  {
        // Set value of ngidx variable
    hngidx=ngidxCountPair.first;
    
        // Obtain iterator to source phrase info
    HistInfo::iterator hiIter=histNgIdxInfo[hngidx];

        // Increase source phrase count
    hiIter->second.second+=c;
  }
  else
  {
    hngidx=updateHistInfo(hist,c);
  }
  
      // Increase joint count
  NgcNode::iterator nnIter;
  Count jcount=getNgJointCount(hngidx,w,nnIter,found);
  if(found)
  {
    nnIter->second+=c;
  }
  else
  {
    updateJointCount(hngidx,w,c);
  }

      // Return index
  return hngidx;
}

//-------------------------
void StlNgtComponent::updateJointCount(NgIdx histNgIdx,
                                       WordIndex w,
                                       Count jcount)
{
      // Add nodes to ngCounts if required
  NgcNode emptyNgcNode;
  while(ngCounts.size()<=(size_t)histNgIdx)
    ngCounts.push_back(emptyNgcNode);

      // Check if w is contained in the node
  NgcNode::iterator nnIter=ngCounts[histNgIdx].find(w);
  if(nnIter!=ngCounts[histNgIdx].end())
  {
        // Replace count of the existing node
    nnIter->second=jcount;
  }
  else
  {
        // Insert count for w
    ngCounts[histNgIdx][w]=jcount;
  }
}

//-------------------------
std::pair<Count,Count>
StlNgtComponent::getNgCounts(const Vector<WordIndex>& hist,
                             WordIndex w,
                             bool& found)
{
      // Intialize output value
  std::pair<Count,Count> histCountJointCountPair;
  histCountJointCountPair.first=0;
  histCountJointCountPair.second=0;
  
      // Obtain source phrase index and source count
  std::pair<NgIdx,Count> ngidxCountPair=getHistInfo(hist,found);
  if(!found) return histCountJointCountPair;
  histCountJointCountPair.first=ngidxCountPair.second;

      // Obtain joint count
  NgcNode::iterator nnIter;
  histCountJointCountPair.second=getNgJointCount(ngidxCountPair.first,w,nnIter,found);

      // Return result
  return histCountJointCountPair;
}

//-------------------------
Count StlNgtComponent::getSrcTrgInfo(const Vector<WordIndex>& srcVec,
                                     WordIndex t,
                                     bool& found)
{
  std::pair<Count,Count> pcc=getNgCounts(srcVec,t,found);
  return pcc.second;
}

//-------------------------
Count StlNgtComponent::getNgJointCount(NgIdx histNgIdx,
                                       WordIndex w,
                                       NgcNode::iterator& nnIter,
                                       bool& found)
{
      // Check if there is an entry for histNgIdx
  if(ngCounts.size()<=(unsigned int)histNgIdx)
  {
        // There is not any entries for histNgIdx
    found=false;
    return 0;
  }
  else
  {
        // There exists an entry for w
    nnIter=ngCounts[histNgIdx].find(w);
    if(nnIter!=ngCounts[histNgIdx].end())
    {
          // There is a dictionary node for w
      found=true;
      return nnIter->second;
    }
    else
    {
          // There is not a dictionary node for w
      found=false;
      return 0;
    }
  }
}

//-------------------------
bool StlNgtComponent::getWordsForHist(const Vector<WordIndex>& hist,
                                      NgcNode& nnode)
{
      // Check existence of an n-gram index for hist
  bool found;
  std::pair<NgIdx,Count> ngidxCountPair=getHistInfo(hist,found);
  if(!found) return false;

      // Check existence of a dictionary node for ngidx
  if(ngCounts.size()<=(size_t)ngidxCountPair.first)
  {
        // There is not a node for ngidx
    return false;
  }
  else
  {
        // There is a node for ngidx
    nnode=ngCounts[ngidxCountPair.first];
    return true;
  }
}

//-------------------------
bool StlNgtComponent::getEntriesForSource(const Vector<WordIndex>& srcVec,
                                          std::map<WordIndex,im_pair<Count,Count> >& trgtn)
{
      // Check existence of an n-gram index for hist
  bool found;
  std::pair<NgIdx,Count> ngidxCountPair=getHistInfo(srcVec,found);
  if(!found) return false;

      // Check existence of a dictionary node for ngidx
  if(ngCounts.size()<=(size_t)ngidxCountPair.first)
  {
        // There is not a node for ngidx
    return false;
  }
  else
  {
        // There is a node for ngidx
    NgcNode nnode=ngCounts[ngidxCountPair.first];
    
        // Generate result
    trgtn.clear();
    NgcNode::iterator iter;
    for(iter=nnode.begin();iter!=nnode.end();++iter)
    {
      im_pair<Count,Count> imp;
      imp.first=ngidxCountPair.second;
      imp.second=iter->second;
      trgtn[iter->first]=imp;
    }
    return true;
  }
}

//-------------------------
bool StlNgtComponent::nodeForHistHasAtLeastOneWord(const Vector<WordIndex>& hist)
{
      // Check existence of a ngidx for hist
  bool found;
  std::pair<NgIdx,Count> ngidxCountPair=getHistInfo(hist,found);
  if(!found) return false;

      // Check existence of a dictionary node for tpidx
  if(ngCounts.size()<=(size_t)ngidxCountPair.first)
  {
        // There is not a node for tpidx
    return false;
  }
  else
  {
        // There is a node for tpidx
    if(!ngCounts[ngidxCountPair.first].empty())
      return true;
    else
      return false;
  }
}

//-------------------------
size_t StlNgtComponent::size(void)
{
  size_t result=0;
  for(unsigned int i=0;i<ngCounts.size();++i)
    result+=ngCounts[i].size();

  return result;
}

//-------------------------
void StlNgtComponent::clear(void)
{
  histInfo.clear();
  histNgIdxInfo.clear();
  ngCounts.clear();
}

//-------------------------
StlNgtComponent::const_iterator StlNgtComponent::begin(void)const
{
      // TO-DO
  // StlNgtComponent::const_iterator iter(this,phraseDict.begin());
  // return iter;
}

//-------------------------
StlNgtComponent::const_iterator StlNgtComponent::end(void)const
{
      // TO-DO
  // StlNgtComponent::const_iterator iter(this,phraseDict.end());
  // return iter;
}

// const_iterator function definitions
//--------------------------
bool StlNgtComponent::const_iterator::operator++(void) //prefix
{
      // TO-DO
  return false;
}

//--------------------------
bool StlNgtComponent::const_iterator::operator++(int)  //postfix
{
  return operator++();
}

//--------------------------
int StlNgtComponent::const_iterator::operator==(const const_iterator& right)
{
      // TO-DO
  return false;
}

//--------------------------
int StlNgtComponent::const_iterator::operator!=(const const_iterator& right)
{
  return !((*this)==right);
}

//--------------------------
// const PhraseDict::const_iterator&
// StlNgtComponent::const_iterator::operator->(void)const
// {
//   return pdIter;
// }

//-------------------------
