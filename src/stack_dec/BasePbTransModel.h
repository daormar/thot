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
/* Module: BasePbTransModel                                         */
/*                                                                  */
/* Prototypes file: BasePbTransModel.h                              */
/*                                                                  */
/* Description: Declares the BasePbTransModel class.                */
/*              This class is a predecessor of the                  */
/*              _phraseBasedTransModel class.                       */
/*                                                                  */
/********************************************************************/

/**
 * @file BasePbTransModel.h
 *
 * @brief Declares the BasePbTransModel class.  This class is a
 * predecessor of the _phraseBasedTransModel class. 
 */

#ifndef _BasePbTransModel_h
#define _BasePbTransModel_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "StrProcUtils.h"
#include "SmtDefs.h"
#include "_smtModel.h"
#include "_phraseHypothesis.h"
#include "_phraseHypothesisRec.h"
#include "BasePbTransModelStats.h"

//--------------- Constants ------------------------------------------

#define PBM_W_DEFAULT          10
#define PBM_A_DEFAULT          10
#define PBM_E_DEFAULT          10
#define PBM_U_DEFAULT          10

//--------------- Classes --------------------------------------------

//--------------- BasePbTransModel class

/**
 * @brief The BasePbTransModel class is a predecessor of the
 * _phraseBasedTransModel class. In this class it is assumed that the
 * template parameter HYPOTHESIS is a class derived from the
 * BasePhraseHypothesis or the BasePhraseHypothesisRec classes.
 */

template<class HYPOTHESIS>
class BasePbTransModel: public _smtModel<HYPOTHESIS>
{
 public:

  typedef typename _smtModel<HYPOTHESIS>::Hypothesis Hypothesis;
  typedef typename _smtModel<HYPOTHESIS>::HypScoreInfo HypScoreInfo;
  typedef typename _smtModel<HYPOTHESIS>::HypDataType HypDataType;

  // class functions

      // Constructor
  BasePbTransModel(void);
  
  
  // class methods

      // Init language and alignment models
  virtual bool loadLangModel(const char* prefixFileName)=0;
  virtual bool loadAligModel(const char* prefixFileName)=0;

      // Print models
  virtual bool printLangModel(std::string printPrefix)=0;
  virtual bool printAligModel(std::string printPrefix)=0;

  virtual void clear(void)=0;

  ////// Hypotheses-related functions

        // Heuristic-related functions
  virtual void setHeuristic(unsigned int _heuristicId)=0;
  virtual void addHeuristicToHyp(Hypothesis& hyp)=0;
  virtual void sustractHeuristicToHyp(Hypothesis& hyp)=0;

      // Expansion-related functions
  virtual void expand(const Hypothesis& hyp,
                      Vector<Hypothesis>& hypVec,
                      Vector<Vector<Score> >& scrCompVec);
  virtual void expand_ref(const Hypothesis& hyp,
                          Vector<Hypothesis>& hypVec,
                          Vector<Vector<Score> >& scrCompVec);
  virtual void expand_ver(const Hypothesis& hyp,
                          Vector<Hypothesis>& hypVec,
                          Vector<Vector<Score> >& scrCompVec);
  virtual void expand_prefix(const Hypothesis& hyp,
                             Vector<Hypothesis>& hypVec,
                             Vector<Vector<Score> >& scrCompVec);

      // Misc. operations with hypothesis
  unsigned int distToNullHyp(const Hypothesis& hyp);
  virtual void aligMatrix(const Hypothesis& hyp,
                          Vector<pair<PositionIndex,PositionIndex> >& amatrix);
      // Returns an alignment matrix for the hypothesis hyp. This
      // function is required for assisted translation purposes

      // Printing functions and data conversion
  unsigned int partialTransLength(const Hypothesis& hyp)const;

      // Expansion-related parameters
  void set_W_par(float W_par);
  void set_A_par(unsigned int A_par);
  void set_E_par(unsigned int E_par);
  void set_U_par(unsigned int U_par);
  bool monotoneSearch(void);
      // Returns true if the search is monotone

      // Specific phrase-based functions
  virtual void extendHypData(PositionIndex srcLeft,
                             PositionIndex srcRight,
                             const Vector<std::string>& trgPhrase,
                             HypDataType& hypd)=0;

      // Set verbosity level
  void setVerbosity(int _verbosity);

      // Utility functions
  void getPhraseAlignment(const Vector<pair<PositionIndex,PositionIndex> >& amatrix,
                          SourceSegmentation& sourceSegmentation,
                          Vector<PositionIndex>& targetSegmentCuts);
  Vector<Vector<std::string> > getSrcPhrases(const Vector<std::string>& srcSentVec,
                                             const Hypothesis& hyp);
  Vector<Vector<std::string> > getTrgPhrases(const Hypothesis& hyp);
  
      // Destructor
  ~BasePbTransModel();

# ifdef THOT_STATS
  virtual ostream & printStats(ostream &outS);
  virtual void clearStats(void);
  BasePbTransModelStats basePbTmStats;
# endif

 protected:

  float W;                       // Maximum number of translation
                                 // options
  unsigned int A;                // Maximum number of alignments per
                                 // expansion
  unsigned int E;                // Determines the length range for
                                 // reference alignments per expansion:
                                 // [s-E,s+E], where s is the length of
                                 // the source phrase length that is
                                 // being covered
  unsigned int U;                // Maximum number of words jumped
  
  int verbosity;                 // Verbosity level

  ////// Hypotheses-related functions

      // Expansion-related functions
  void extract_gaps(const Hypothesis& hyp,
                    Vector<pair<PositionIndex,PositionIndex> >&  gaps);
  void extract_gaps(const Bitset<MAX_SENTENCE_LENGTH_ALLOWED>& hypKey,
                    Vector<pair<PositionIndex,PositionIndex> >&  gaps);
  unsigned int get_num_gaps(const Bitset<MAX_SENTENCE_LENGTH_ALLOWED>& hypKey);
  virtual bool getHypDataVecForGap(const Hypothesis& hyp,
                                   PositionIndex srcLeft,
                                   PositionIndex srcRight,
                                   Vector<HypDataType>& hypDataTypeVec,
                                   float N)=0;
      // Get N-best translations for a subphrase of the source sentence
      // to be translated .  If N is between 0 and 1 then N represents a
      // threshold. 
  virtual bool getHypDataVecForGapRef(const Hypothesis& hyp,
                                      PositionIndex srcLeft,
                                      PositionIndex srcRight,
                                      Vector<HypDataType>& hypDataTypeVec,
                                      float N)=0;
      // This function is identical to the previous function but is to
      // be used when the translation process is conducted by a given
      // reference sentence
  virtual bool getHypDataVecForGapVer(const Hypothesis& hyp,
                                      PositionIndex srcLeft,
                                      PositionIndex srcRight,
                                      Vector<HypDataType>& hypDataTypeVec,
                                      float N)=0;
      // This function is identical to the previous function but is to
      // be used when the translation process is performed to verify the
      // coverage of the model given a reference sentence
  virtual bool getHypDataVecForGapPref(const Hypothesis& hyp,
                                       PositionIndex srcLeft,
                                       PositionIndex srcRight,
                                       Vector<HypDataType>& hypDataTypeVec,
                                       float N)=0;
      // This function is identical to the previous function but is to
      // be used when the translation process is conducted by a given
      // prefix

      // Misc. operations with hypothesis
  virtual unsigned int numberOfUncoveredSrcWords(const Hypothesis& hyp)const;
  virtual unsigned int
    numberOfUncoveredSrcWordsHypData(const HypDataType& hypd)const=0;
};

//--------------- BasePbTransModel class functions
//

//---------------------------------
template<class HYPOTHESIS>
BasePbTransModel<HYPOTHESIS>::BasePbTransModel(void):_smtModel<HYPOTHESIS>()
{
  W=PBM_W_DEFAULT;
  A=PBM_A_DEFAULT;
  E=PBM_E_DEFAULT;
  U=PBM_U_DEFAULT;
      // Set verbosity level
  verbosity=0;
}

//---------------------------------
template<class HYPOTHESIS>
void BasePbTransModel<HYPOTHESIS>::getPhraseAlignment(const Vector<pair<PositionIndex,PositionIndex> >& amatrix,
                                                      SourceSegmentation& sourceSegmentation,
                                                      Vector<PositionIndex>& targetSegmentCuts)
{
  sourceSegmentation.clear();
  targetSegmentCuts.clear();
  
  if(amatrix.size()>0)
  {
    Vector<pair<PositionIndex,PositionIndex> > temp;
    pair<PositionIndex,PositionIndex> pip;

        // Create temporary data structure 'temp' from 'amatrix'
    for(unsigned int i=0;i<amatrix.size();++i)
    {
      unsigned int j=amatrix[i].second;
      while(temp.size()<=j)
        temp.push_back(make_pair(MAX_SENTENCE_LENGTH_ALLOWED+1,0));
      if(temp[j].first>amatrix[i].first)
        temp[j].first=amatrix[i].first;
      if(temp[j].second<amatrix[i].first)
        temp[j].second=amatrix[i].first;
    }
        // Set contents of 'sourceSegmentation' and 'targetSegmentCuts'
        // data structures from 'temp'
    pip=temp[1];
    for(unsigned int j=1;j<temp.size();++j)
    {
      if(j==temp.size()-1) 
      {
        sourceSegmentation.push_back(temp[j]);
        targetSegmentCuts.push_back(j);
      }
      else
      {
        if(pip!=temp[j+1])
        {
          sourceSegmentation.push_back(temp[j]);
          targetSegmentCuts.push_back(j);
          pip=temp[j+1];
        }
      }
    }
  }
}

//---------------------------------------
template<class HYPOTHESIS>
Vector<Vector<std::string> > BasePbTransModel<HYPOTHESIS>::getSrcPhrases(const Vector<std::string>& srcSentVec,
                                                                         const Hypothesis& hyp)
{
  Vector<pair<PositionIndex,PositionIndex> > amatrix;
  SourceSegmentation sourceSegmentation;
  Vector<PositionIndex> targetSegmentCuts;
  Vector<Vector<std::string> > srcPhrases;
  Vector<std::string> srcPhrase;
  
  aligMatrix(hyp,amatrix);

  getPhraseAlignment(amatrix,sourceSegmentation,targetSegmentCuts);

  for(unsigned int i=0;i<sourceSegmentation.size();++i)
  {
    srcPhrase.clear();
    for(unsigned int j=sourceSegmentation[i].first;j<=sourceSegmentation[i].second;++j)
    {
      srcPhrase.push_back(srcSentVec[j-1]);
    }
    srcPhrases.push_back(srcPhrase);
  }
  
  return srcPhrases;
}

//---------------------------------------
template<class HYPOTHESIS>
Vector<Vector<std::string> > BasePbTransModel<HYPOTHESIS>::getTrgPhrases(const Hypothesis& hyp)
{
  Vector<pair<PositionIndex,PositionIndex> > amatrix;
  SourceSegmentation sourceSegmentation;
  Vector<PositionIndex> targetSegmentCuts;
  Vector<Vector<std::string> > trgPhrases;
  Vector<std::string> trgPhrase;
  Vector<std::string> trgSentVec=getTransInPlainTextVec(hyp);
    
  aligMatrix(hyp,amatrix);

  getPhraseAlignment(amatrix,sourceSegmentation,targetSegmentCuts);

  for(unsigned int i=0;i<targetSegmentCuts.size();++i)
  {
    trgPhrase.clear();
    unsigned int phrStart=1;
    if(i>0) phrStart=targetSegmentCuts[i-1]+1;
    for(unsigned int j=phrStart;j<=targetSegmentCuts[i];++j)
    {
      trgPhrase.push_back(trgSentVec[j-1]);
    }
    trgPhrases.push_back(trgPhrase);
  }

  return trgPhrases;
}

//---------------------------------------
template<class HYPOTHESIS>
void BasePbTransModel<HYPOTHESIS>::set_W_par(float W_par)
{
  W=W_par;
}

//---------------------------------------
template<class HYPOTHESIS>
void BasePbTransModel<HYPOTHESIS>::set_A_par(unsigned int A_par)
{
  A=A_par;
}

//---------------------------------------
template<class HYPOTHESIS>
void BasePbTransModel<HYPOTHESIS>::set_E_par(unsigned int E_par)
{
  E=E_par;
}

//---------------------------------------
template<class HYPOTHESIS>
void BasePbTransModel<HYPOTHESIS>::set_U_par(unsigned int U_par)
{
  U=U_par;
}

//---------------------------------------
template<class HYPOTHESIS>
bool BasePbTransModel<HYPOTHESIS>::monotoneSearch(void)
{
  if(U==0) return true;
  else return false;	 
}

//---------------------------------
template<class HYPOTHESIS>
void BasePbTransModel<HYPOTHESIS>::setVerbosity(int _verbosity)
{
  verbosity=_verbosity;
}

//---------------------------------
template<class HYPOTHESIS>
BasePbTransModel<HYPOTHESIS>::~BasePbTransModel()
{
}

//--------------- BasePbTransModel class methods
//

//---------------------------------
template<class HYPOTHESIS>
void BasePbTransModel<HYPOTHESIS>::expand(const Hypothesis& hyp,
                                          Vector<Hypothesis>& hypVec,
                                          Vector<Vector<Score> >& scrCompVec)
{
  Vector<pair<PositionIndex,PositionIndex> > gaps;
  Vector<WordIndex> s_;
  Hypothesis extHyp;
  Vector<HypDataType> hypDataVec;
  Vector<Score> scoreComponents;
  
  hypVec.clear();
  scrCompVec.clear();
  
      // Extract gaps	
  extract_gaps(hyp,gaps);
  if(verbosity>=2)
  {
    cerr<<"  gaps: "<<gaps.size()<<endl;
  }
   
      // Generate new hypotheses completing the gaps
  for(unsigned int k=0;k<gaps.size();++k)
  {
    unsigned int gap_length=gaps[k].second-gaps[k].first+1;
    for(unsigned int x=0;x<gap_length;++x)
    {
      s_.clear();
      if(x<=U) // x should be lower than U, which is the maximum
               // number of words that can be jUmped
      {
        for(unsigned int y=x;y<gap_length;++y)
        {
          unsigned int segmRightMostj=gaps[k].first+y;
          unsigned int segmLeftMostj=gaps[k].first+x;
              // Verify that the source phrase length does not exceed
              // the limit
          if((segmRightMostj-segmLeftMostj)+1 > A) 
            break;
              // Obtain hypothesis data vector
          getHypDataVecForGap(hyp,segmLeftMostj,segmRightMostj,hypDataVec,W);
          if(hypDataVec.size()!=0)
          {
            for(unsigned int i=0;i<hypDataVec.size();++i)
            {
              this->incrScore(hyp,hypDataVec[i],extHyp,scoreComponents);
              hypVec.push_back(extHyp);
              scrCompVec.push_back(scoreComponents);
            }
#           ifdef THOT_STATS    
            basePbTmStats.transOptions+=hypDataVec.size();
            ++basePbTmStats.getTransCalls;
#           endif    
          }
        }
      }
    }
  }   
}

//---------------------------------
template<class HYPOTHESIS>
void BasePbTransModel<HYPOTHESIS>::expand_ref(const Hypothesis& hyp,
                                              Vector<Hypothesis>& hypVec,
                                              Vector<Vector<Score> >& scrCompVec)
{
  Vector<pair<PositionIndex,PositionIndex> > gaps;
  Vector<WordIndex> s_;
  Hypothesis extHyp;
  Vector<HypDataType> hypDataVec;
  Vector<Score> scoreComponents;

  hypVec.clear();
  scrCompVec.clear();
  
      // Extract gaps	
  extract_gaps(hyp,gaps);
   
      // Generate new hypotheses completing the gaps
  for(unsigned int k=0;k<gaps.size();++k)
  {
    unsigned int gap_length=gaps[k].second-gaps[k].first+1;
    for(unsigned int x=0;x<gap_length;++x)
    {
      s_.clear();
      if(x<=U) // x should be lower than U, which is the maximum
               // number of words that can be jUmped
      {
        for(unsigned int y=x;y<gap_length;++y)
        {
          unsigned int segmRightMostj=gaps[k].first+y;
          unsigned int segmLeftMostj=gaps[k].first+x;
              // Verify that the source phrase length does not exceed
              // the limit
          if((segmRightMostj-segmLeftMostj)+1 > A) 
            break;
              // Obtain hypothesis data vector
          getHypDataVecForGapRef(hyp,segmLeftMostj,segmRightMostj,hypDataVec,W);
          if(hypDataVec.size()!=0)
          {
            for(unsigned int i=0;i<hypDataVec.size();++i)
            {
              this->incrScore(hyp,hypDataVec[i],extHyp,scoreComponents);
              hypVec.push_back(extHyp);
              scrCompVec.push_back(scoreComponents);
            }
#           ifdef THOT_STATS    
            ++basePbTmStats.getTransCalls;
            basePbTmStats.transOptions+=hypDataVec.size();
#           endif    
          }
        }
      }
    }
  }     
}

//---------------------------------
template<class HYPOTHESIS>
void BasePbTransModel<HYPOTHESIS>::expand_ver(const Hypothesis& hyp,
                                              Vector<Hypothesis>& hypVec,
                                              Vector<Vector<Score> >& scrCompVec)
{
  Vector<pair<PositionIndex,PositionIndex> > gaps;
  Vector<WordIndex> s_;
  Hypothesis extHyp;
  Vector<HypDataType> hypDataVec;
  Vector<Score> scoreComponents;

  hypVec.clear();
  scrCompVec.clear();
  
      // Extract gaps	
  extract_gaps(hyp,gaps);
   
      // Generate new hypotheses completing the gaps
  for(unsigned int k=0;k<gaps.size();++k)
  {
    unsigned int gap_length=gaps[k].second-gaps[k].first+1;
    for(unsigned int x=0;x<gap_length;++x)
    {
      s_.clear();
      if(x<=U) // x should be lower than U, which is the maximum
               // number of words that can be jUmped
      {
        for(unsigned int y=x;y<gap_length;++y)
        {
          unsigned int segmRightMostj=gaps[k].first+y;
          unsigned int segmLeftMostj=gaps[k].first+x;
              // Verify that the source phrase length does not exceed
              // the limit
          if((segmRightMostj-segmLeftMostj)+1 > A) 
            break;
              // Obtain hypothesis data vector
          getHypDataVecForGapVer(hyp,segmLeftMostj,segmRightMostj,hypDataVec,W);
          if(hypDataVec.size()!=0)
          {
            for(unsigned int i=0;i<hypDataVec.size();++i)
            {
              this->incrScore(hyp,hypDataVec[i],extHyp,scoreComponents);
              hypVec.push_back(extHyp);
              scrCompVec.push_back(scoreComponents);
            }
#           ifdef THOT_STATS    
            ++basePbTmStats.getTransCalls;
            basePbTmStats.transOptions+=hypDataVec.size();
#           endif    
          }
        }
      }
    }
  }       
}

//---------------------------------
template<class HYPOTHESIS>
void BasePbTransModel<HYPOTHESIS>::expand_prefix(const Hypothesis& hyp,
                                                 Vector<Hypothesis>& hypVec,
                                                 Vector<Vector<Score> >& scrCompVec)
{
  Vector<pair<PositionIndex,PositionIndex> > gaps;
  Vector<WordIndex> s_;
  Hypothesis extHyp;
  Vector<HypDataType> hypDataVec;
  Vector<Score> scoreComponents;

  hypVec.clear();
  scrCompVec.clear();

      // Extract gaps	
  extract_gaps(hyp,gaps);
   
      // Generate new hypotheses completing the gaps
  for(unsigned int k=0;k<gaps.size();++k)
  {
    unsigned int gap_length=gaps[k].second-gaps[k].first+1;
    for(unsigned int x=0;x<gap_length;++x)
    {
      s_.clear();
      if(x<=U) // x should be lower than U, which is the maximum
               // number of words that can be jUmped
      {
        for(unsigned int y=x;y<gap_length;++y)
        {
          unsigned int segmRightMostj=gaps[k].first+y;
          unsigned int segmLeftMostj=gaps[k].first+x;
              // Verify that the source phrase length does not exceed
              // the limit
          if((segmRightMostj-segmLeftMostj)+1 > A) 
            break;
              // Obtain hypothesis data vector
          getHypDataVecForGapPref(hyp,segmLeftMostj,segmRightMostj,hypDataVec,W);
          if(hypDataVec.size()!=0)
          {
            for(unsigned int i=0;i<hypDataVec.size();++i)
            {
              this->incrScore(hyp,hypDataVec[i],extHyp,scoreComponents);
              hypVec.push_back(extHyp);
              scrCompVec.push_back(scoreComponents);
            }
#           ifdef THOT_STATS    
            ++basePbTmStats.getTransCalls;
            basePbTmStats.transOptions+=hypDataVec.size();
#           endif    
          }
        }
      }
    }
  }       
}

//---------------------------------
template<class HYPOTHESIS>
void BasePbTransModel<HYPOTHESIS>::aligMatrix(const Hypothesis& hyp,
                                              Vector<pair<PositionIndex,PositionIndex> >& amatrix)
{
  Hypothesis nullHyp=this->nullHypothesis();
  unsigned int numSrcWords=numberOfUncoveredSrcWords(nullHyp);
  unsigned int numTrgWords=hyp.partialTransLength();

  amatrix.clear();
  for(unsigned int i=0;i<=numSrcWords;++i)
  {
    for(unsigned int j=0;j<=numTrgWords;++j)
    {
      if(hyp.areAligned(i,j))
        amatrix.push_back(make_pair(i,j));
    }
  }
}

//---------------------------------
template<class HYPOTHESIS>
unsigned int BasePbTransModel<HYPOTHESIS>::distToNullHyp(const Hypothesis& hyp)
{
  return numberOfUncoveredSrcWordsHypData(this->nullHypothesisHypData())-numberOfUncoveredSrcWordsHypData(hyp.getData());
}

//---------------------------------
template<class HYPOTHESIS>
unsigned int BasePbTransModel<HYPOTHESIS>::partialTransLength(const Hypothesis& hyp)const
{
  return hyp.partialTransLength();
}

//---------------------------------
template<class HYPOTHESIS>
unsigned int BasePbTransModel<HYPOTHESIS>::numberOfUncoveredSrcWords(const Hypothesis& hyp)const
{
  HypDataType dataType;
 
  dataType=hyp.getData();
  return numberOfUncoveredSrcWordsHypData(dataType);
}

//---------------------------------
template<class HYPOTHESIS>
void BasePbTransModel<HYPOTHESIS>::extract_gaps(const Hypothesis& hyp,
                                                Vector<pair<PositionIndex,PositionIndex> >&  gaps)
{
  extract_gaps(hyp.getKey(),gaps);
}

//---------------------------------
template<class HYPOTHESIS>
void BasePbTransModel<HYPOTHESIS>::extract_gaps(const Bitset<MAX_SENTENCE_LENGTH_ALLOWED>& hypKey,
                                                Vector<pair<PositionIndex,PositionIndex> >&  gaps)
{
      // Extract all uncovered gaps
  pair<PositionIndex,PositionIndex> gap;
  unsigned int srcSentLen=numberOfUncoveredSrcWordsHypData(this->nullHypothesisHypData());
  unsigned int j;
  
      // Extract gaps
  gaps.clear();
  bool crossing_a_gap=false;	
	
  for(j=1;j<=srcSentLen;++j)
  {
    if(crossing_a_gap==false && hypKey.test(j)==0)
    {
      crossing_a_gap=true;
      gap.first=j;
    }
	
    if(crossing_a_gap==true && hypKey.test(j)==1)
    {
      crossing_a_gap=false;
      gap.second=j-1;
      gaps.push_back(gap);	
    }	
  }
  if(crossing_a_gap==true)
  {
    gap.second=j-1;
    gaps.push_back(gap);
  }
}

//---------------------------------
template<class HYPOTHESIS>
unsigned int
BasePbTransModel<HYPOTHESIS>::get_num_gaps(const Bitset<MAX_SENTENCE_LENGTH_ALLOWED>& hypKey)
{
      // Count all uncovered gaps
  unsigned int result=0;
  unsigned int j;
  bool crossing_a_gap;
  unsigned int srcSentLen;
  HypDataType nullHypData=this->nullHypothesisHypData();

  srcSentLen=numberOfUncoveredSrcWordsHypData(nullHypData);
  
      // count gaps	
  crossing_a_gap=false;	
	
  for(j=1;j<=srcSentLen;++j)
  {
    if(crossing_a_gap==false && hypKey.test(j)==0)
    {
      crossing_a_gap=true;
    }
	
    if(crossing_a_gap==true && hypKey.test(j)==1)
    {
      crossing_a_gap=false;
      ++result;	
    }	
  }
  if(crossing_a_gap==true)
  {
    ++result;
  }
  return result;
}

# ifdef THOT_STATS
//---------------------------------
template<class HYPOTHESIS>
ostream & BasePbTransModel<HYPOTHESIS>::printStats(ostream &outS)
{
  return basePbTmStats.print(outS);
}

//---------------------------------
template<class HYPOTHESIS>
void BasePbTransModel<HYPOTHESIS>::clearStats(void)
{
  basePbTmStats.clear();
}

# endif

#endif
