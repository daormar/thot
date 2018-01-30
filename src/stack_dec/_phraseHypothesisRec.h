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
 
/**
 * @file _phraseHypothesisRec.h
 * 
 * @brief Declares the _phraseHypothesisRec abstract template class,
 * this class is derived from the _hypothesisRec class.
 */

#ifndef __phraseHypothesisRec_h
#define __phraseHypothesisRec_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

//--------------- Constants ------------------------------------------

#include "BasePhraseHypothesisRec.h"
#include "PhrHypData.h"

//--------------- Classes --------------------------------------------

//--------------- _phraseHypothesisRec template class

/**
 * @brief The _phraseHypothesisRec abstract template class is derived from the
 * _hypothesisRec class.
 */

template<class SCORE_INFO,class EQCLASS_FUNC,class HYPSTATE>
class _phraseHypothesisRec: public BasePhraseHypothesisRec<SCORE_INFO,PhrHypData,EQCLASS_FUNC,HYPSTATE>
{
 public:

  typedef typename BaseHypothesisRec<SCORE_INFO,PhrHypData,EQCLASS_FUNC,HYPSTATE>::ScoreInfo ScoreInfo;  
  typedef typename BaseHypothesisRec<SCORE_INFO,PhrHypData,EQCLASS_FUNC,HYPSTATE>::DataType DataType;
  typedef typename BaseHypothesisRec<SCORE_INFO,PhrHypData,EQCLASS_FUNC,HYPSTATE>::EqClassFunc EqClassFunc;
  typedef typename BaseHypothesisRec<SCORE_INFO,PhrHypData,EQCLASS_FUNC,HYPSTATE>::HypState HypState;

      // Basic functions
  Score getScore(void)const;
  void setScoreInfo(const ScoreInfo& _scoreInfo);
  ScoreInfo getScoreInfo(void)const;
  void addHeuristic(Score h);
  void subtractHeuristic(Score h);
  PhrHypData getData(void)const;
  void setData(const PhrHypData& _data);

      // Specific functions
  bool isAligned(PositionIndex i)const;
  bool areAligned(PositionIndex i,PositionIndex j)const;
  void getPhraseAlign(SourceSegmentation& sourceSegmentation,
                      std::vector<PositionIndex>& targetSegmentCuts)const;
  Bitset<MAX_SENTENCE_LENGTH_ALLOWED> getKey(void)const;
  std::vector<WordIndex> getPartialTrans(void)const;
  unsigned int partialTransLength(void)const;

      // Destructor
  ~_phraseHypothesisRec();

 protected:
        // Data members
  SCORE_INFO scoreInfo;
  PhrHypData data;
};

//--------------- _phraseHypothesisRec template class method definitions

template<class SCORE_INFO,class EQCLASS_FUNC,class HYPSTATE>
Score _phraseHypothesisRec<SCORE_INFO,EQCLASS_FUNC,HYPSTATE>::getScore(void)const
{
  return scoreInfo.getScore();
}

//---------------------------------------
template<class SCORE_INFO,class EQCLASS_FUNC,class HYPSTATE>
void _phraseHypothesisRec<SCORE_INFO,EQCLASS_FUNC,HYPSTATE>::addHeuristic(Score h)
{
  scoreInfo.addHeuristic(h);  
}

//---------------------------------------
template<class SCORE_INFO,class EQCLASS_FUNC,class HYPSTATE>
void _phraseHypothesisRec<SCORE_INFO,EQCLASS_FUNC,HYPSTATE>::subtractHeuristic(Score h)
{
  scoreInfo.subtractHeuristic(h);    
}

//---------------------------------------
template<class SCORE_INFO,class EQCLASS_FUNC,class HYPSTATE>
PhrHypData _phraseHypothesisRec<SCORE_INFO,EQCLASS_FUNC,HYPSTATE>::getData(void)const
{
  return data;
}

//---------------------------------------
template<class SCORE_INFO,class EQCLASS_FUNC,class HYPSTATE>
void _phraseHypothesisRec<SCORE_INFO,EQCLASS_FUNC,HYPSTATE>::setScoreInfo(const ScoreInfo& _scoreInfo)
{
  scoreInfo=_scoreInfo;
}

//---------------------------------------
template<class SCORE_INFO,class EQCLASS_FUNC,class HYPSTATE>
typename _phraseHypothesisRec<SCORE_INFO,EQCLASS_FUNC,HYPSTATE>::ScoreInfo
_phraseHypothesisRec<SCORE_INFO,EQCLASS_FUNC,HYPSTATE>::getScoreInfo(void)const
{
  return scoreInfo;
}

//---------------------------------------
template<class SCORE_INFO,class EQCLASS_FUNC,class HYPSTATE>
void _phraseHypothesisRec<SCORE_INFO,EQCLASS_FUNC,HYPSTATE>::setData(const PhrHypData& _data)
{
  data=_data;
}

//---------------------------------------
template<class SCORE_INFO,class EQCLASS_FUNC,class HYPSTATE>
bool _phraseHypothesisRec<SCORE_INFO,EQCLASS_FUNC,HYPSTATE>::isAligned(PositionIndex i)const
{
  for(unsigned int k=0;k<this->data.sourceSegmentation.size();k++)
  {
    if(i>=this->data.sourceSegmentation[k].first &&
       i<=this->data.sourceSegmentation[k].second)
      return true;
  }
  return false;  
}

//---------------------------------------
template<class SCORE_INFO,class EQCLASS_FUNC,class HYPSTATE>
bool _phraseHypothesisRec<SCORE_INFO,EQCLASS_FUNC,HYPSTATE>::areAligned(PositionIndex i,
                                                                        PositionIndex j)const
{
  for(unsigned int k=0;k<this->data.sourceSegmentation.size();k++)
  {
    if(i>=this->data.sourceSegmentation[k].first &&
       i<=this->data.sourceSegmentation[k].second)
    {
      if(k==0)
      {
        if(j>=1 && j<=this->data.targetSegmentCuts[k])
          return true;
      }
      else
      {
        if(j>=this->data.targetSegmentCuts[k-1]+1 &&
           j<=this->data.targetSegmentCuts[k])
          return true;
      }
    }
  }
  return false;
}

//---------------------------------------
template<class SCORE_INFO,class EQCLASS_FUNC,class HYPSTATE>
void _phraseHypothesisRec<SCORE_INFO,EQCLASS_FUNC,HYPSTATE>::getPhraseAlign(SourceSegmentation& sourceSegmentation,
                                                                            std::vector<PositionIndex>& targetSegmentCuts)const
{
  sourceSegmentation=data.sourceSegmentation;
  targetSegmentCuts=data.targetSegmentCuts;
}

//---------------------------------------
template<class SCORE_INFO,class EQCLASS_FUNC,class HYPSTATE>
Bitset<MAX_SENTENCE_LENGTH_ALLOWED>
_phraseHypothesisRec<SCORE_INFO,EQCLASS_FUNC,HYPSTATE>::getKey(void)const
{
  unsigned int k,j;
  Bitset<MAX_SENTENCE_LENGTH_ALLOWED> b;

  b.reset();
  for(k=0;k<this->data.sourceSegmentation.size();k++)
  {
    for(j=this->data.sourceSegmentation[k].first;j<=this->data.sourceSegmentation[k].second;j++) 
      b.set( (size_t) j);
  }
  return b;	  
}

//---------------------------------------
template<class SCORE_INFO,class EQCLASS_FUNC,class HYPSTATE>
std::vector<WordIndex>
_phraseHypothesisRec<SCORE_INFO,EQCLASS_FUNC,HYPSTATE>::getPartialTrans(void)const
{
  return this->data.ntarget;
}

//---------------------------------------
template<class SCORE_INFO,class EQCLASS_FUNC,class HYPSTATE>
unsigned int
_phraseHypothesisRec<SCORE_INFO,EQCLASS_FUNC,HYPSTATE>::partialTransLength(void)const
{
  return this->data.ntarget.size()-1;
}

//---------------------------------
template<class SCORE_INFO,class EQCLASS_FUNC,class HYPSTATE>
_phraseHypothesisRec<SCORE_INFO,EQCLASS_FUNC,HYPSTATE>::~_phraseHypothesisRec()
{
  
}

#endif
