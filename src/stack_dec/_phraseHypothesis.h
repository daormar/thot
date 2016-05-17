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
/* Module: _phraseHypothesis                                        */
/*                                                                  */
/* Prototypes file: _phraseHypothesis.h                             */
/*                                                                  */
/* Description: Declares the _phraseHypothesis template class,      */
/*              this class is derived from the BaseHypothesis       */
/*              class.                                              */
/*                                                                  */
/********************************************************************/

/**
 * @file _phraseHypothesis.h
 * 
 * @brief Declares the _phraseHypothesis template class, this class is derived
 * from the BaseHypothesis class.
 */

#ifndef __phraseHypothesis_h
#define __phraseHypothesis_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

//--------------- Constants ------------------------------------------

#include "BasePhraseHypothesis.h"
#include "PhrHypData.h"

//--------------- Classes --------------------------------------------

//--------------- _phraseHypothesis template class

/**
 * @brief The _phraseHypothesis template class is derived from the
 * BasePhraseHypothesis class.
 */

template<class SCORE_INFO,class EQCLASS_FUNC>
class _phraseHypothesis: public BasePhraseHypothesis<SCORE_INFO,PhrHypData,EQCLASS_FUNC>
{
 public:

  typedef typename BasePhraseHypothesis<SCORE_INFO,PhrHypData,EQCLASS_FUNC>::ScoreInfo ScoreInfo;  
  typedef typename BasePhraseHypothesis<SCORE_INFO,PhrHypData,EQCLASS_FUNC>::DataType DataType;
  typedef typename BasePhraseHypothesis<SCORE_INFO,PhrHypData,EQCLASS_FUNC>::EqClassFunc EqClassFunc;

      // Basic functions
  Score getScore(void)const;
  void setScoreInfo(const ScoreInfo& _scoreInfo);
  ScoreInfo getScoreInfo(void)const;
  void addHeuristic(Score h);
  void subtractHeuristic(Score h);
  PhrHypData getData(void)const;
  void setData(const PhrHypData& _data);

      // Specific functions
  bool isAligned(PositionIndex j)const;
  bool areAligned(PositionIndex j,PositionIndex i)const;
  Bitset<MAX_SENTENCE_LENGTH_ALLOWED> getKey(void)const;
  Vector<WordIndex> getPartialTrans(void)const;
  unsigned int partialTransLength(void)const;

      // Destructor
  ~_phraseHypothesis();

 protected:
        // Data members
  SCORE_INFO scoreInfo;
  PhrHypData data;
};

//--------------- _phraseHypothesis template class method definitions

template<class SCORE_INFO,class EQCLASS_FUNC>
Score _phraseHypothesis<SCORE_INFO,EQCLASS_FUNC>::getScore(void)const
{
  return scoreInfo.getScore();
}

//---------------------------------------
template<class SCORE_INFO,class EQCLASS_FUNC>
void _phraseHypothesis<SCORE_INFO,EQCLASS_FUNC>::addHeuristic(Score h)
{
  scoreInfo.addHeuristic(h);
}

//---------------------------------------
template<class SCORE_INFO,class EQCLASS_FUNC>
void _phraseHypothesis<SCORE_INFO,EQCLASS_FUNC>::subtractHeuristic(Score h)
{
  scoreInfo.subtractHeuristic(h);  
}

//---------------------------------------
template<class SCORE_INFO,class EQCLASS_FUNC>
PhrHypData _phraseHypothesis<SCORE_INFO,EQCLASS_FUNC>::getData(void)const
{
  return data;
}

//---------------------------------------
template<class SCORE_INFO,class EQCLASS_FUNC>
void _phraseHypothesis<SCORE_INFO,EQCLASS_FUNC>::setScoreInfo(const ScoreInfo& _scoreInfo)
{
  scoreInfo=_scoreInfo;
}

//---------------------------------------
template<class SCORE_INFO,class EQCLASS_FUNC>
typename _phraseHypothesis<SCORE_INFO,EQCLASS_FUNC>::ScoreInfo
_phraseHypothesis<SCORE_INFO,EQCLASS_FUNC>::getScoreInfo(void)const
{
  return scoreInfo;
}

//---------------------------------------
template<class SCORE_INFO,class EQCLASS_FUNC>
void _phraseHypothesis<SCORE_INFO,EQCLASS_FUNC>::setData(const PhrHypData& _data)
{
  data=_data;
}

//---------------------------------------
template<class SCORE_INFO,class EQCLASS_FUNC>
bool _phraseHypothesis<SCORE_INFO,EQCLASS_FUNC>::isAligned(PositionIndex srcPos)const
{
  for(unsigned int k=0;k<this->data.sourceSegmentation.size();k++)
  {
    if(srcPos>=this->data.sourceSegmentation[k].first &&
       srcPos<=this->data.sourceSegmentation[k].second)
      return true;
  }
  return false;  
}

//---------------------------------------
template<class SCORE_INFO,class EQCLASS_FUNC>
bool _phraseHypothesis<SCORE_INFO,EQCLASS_FUNC>::areAligned(PositionIndex srcPos,
                                                            PositionIndex trgPos)const
{
  for(unsigned int k=0;k<this->data.sourceSegmentation.size();k++)
  {
    if(srcPos>=this->data.sourceSegmentation[k].first &&
       srcPos<=this->data.sourceSegmentation[k].second)
    {
      if(k==0)
      {
        if(trgPos>=1 && trgPos<=this->data.targetSegmentCuts[k])
          return true;
      }
      else
      {
        if(trgPos>=this->data.targetSegmentCuts[k-1]+1 &&
           trgPos<=this->data.targetSegmentCuts[k])
          return true;
      }
    }
  }
  return false;
}

//---------------------------------------
template<class SCORE_INFO,class EQCLASS_FUNC>
Bitset<MAX_SENTENCE_LENGTH_ALLOWED>
_phraseHypothesis<SCORE_INFO,EQCLASS_FUNC>::getKey(void)const
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
template<class SCORE_INFO,class EQCLASS_FUNC>
Vector<WordIndex> _phraseHypothesis<SCORE_INFO,EQCLASS_FUNC>::getPartialTrans(void)const
{
  return this->data.ntarget;
}

//---------------------------------------
template<class SCORE_INFO,class EQCLASS_FUNC>
unsigned int _phraseHypothesis<SCORE_INFO,EQCLASS_FUNC>::partialTransLength(void)const
{
  return this->data.ntarget.size()-1;
}

//---------------------------------
template<class SCORE_INFO,class EQCLASS_FUNC>
_phraseHypothesis<SCORE_INFO,EQCLASS_FUNC>::~_phraseHypothesis()
{
  
}

#endif
