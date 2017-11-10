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
/* Module: TrgSegmLenTable                                          */
/*                                                                  */
/* Definitions file: TrgSegmLenTable.cc                             */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "TrgSegmLenTable.h"

//--------------- Global variables -----------------------------------


//--------------- TrgSegmLenTable class function definitions

//-------------------------
TrgSegmLenTable::TrgSegmLenTable(void)
{
  mode=TRGSEGMLEN_UNIFORM;
  avgSrcSegmLen=MAX_SENTENCE_LENGTH/2;
  avgTrgSegmLen=MAX_SENTENCE_LENGTH/2;
}

//-------------------------
LgProb TrgSegmLenTable::trgSegmLenLgProb(unsigned int k,
                                         const SentSegmentation& trgSegm,
                                         unsigned int trgLen,
                                         unsigned int lastSrcSegmLen)
{
  switch(mode)
  {
    case TRGSEGMLEN_UNIFORM: return trgSegmLenLgProbUniform(k,trgSegm,trgLen,lastSrcSegmLen);
    case TRGSEGMLEN_POISSON: return trgSegmLenLgProbPoisson(k,trgSegm,trgLen,lastSrcSegmLen);
    case TRGSEGMLEN_GEOM: return trgSegmLenLgProbGeom(k,trgSegm,trgLen,lastSrcSegmLen);
    default: return 0;
  }
}

//-------------------------
LgProb TrgSegmLenTable::trgSegmLenLgProbUniform(unsigned int k,
                                                const SentSegmentation& trgSegm,
                                                unsigned int trgLen,
                                                unsigned int /*lastSrcSegmLen*/)
{
  unsigned int gapLength;
  unsigned int nextGapStart=trgLen;
  unsigned int trgLeft=trgSegm[k].first;

      // Obtain length of the gap
  if(k>0)
  {
    for(unsigned int j=0;j<=k-1;++j)
    {
      if(trgSegm[j].first>trgLeft && nextGapStart>trgSegm[j].first)
        nextGapStart=trgSegm[j].first;
    }
  }
  gapLength=nextGapStart-trgLeft+1;

      // Assign probability mass uniformly
  return log((double)1.0/(double)gapLength);
}

//-------------------------
LgProb TrgSegmLenTable::trgSegmLenLgProbPoisson(unsigned int k,
                                                const SentSegmentation& trgSegm,
                                                unsigned int /*trgLen*/,
                                                unsigned int lastSrcSegmLen)
{
  unsigned int trgSegmLen=trgSegm[k].second-trgSegm[k].first+1;

      // Assign probability mass using a Poisson distribution
  return MathFuncs::log_poisson(trgSegmLen,(avgTrgSegmLen/avgSrcSegmLen)*lastSrcSegmLen);  
}

//-------------------------
LgProb TrgSegmLenTable::trgSegmLenLgProbGeom(unsigned int k,
                                             const SentSegmentation& trgSegm,
                                             unsigned int /*trgLen*/,
                                             unsigned int lastSrcSegmLen)
{
  unsigned int trgSegmLen=trgSegm[k].second-trgSegm[k].first+1;
  unsigned int diff;

      // Obtain difference in number of words
  if(trgSegmLen<lastSrcSegmLen)
    diff=lastSrcSegmLen-trgSegmLen;
  else
    diff=trgSegmLen-lastSrcSegmLen;

      // Assign probability mass using a geometric distribution
  LgProb result=log((double)TRGSEGMLEN_GEOM_HIT_PROB);
  result+=diff*log((double)1.0-TRGSEGMLEN_GEOM_HIT_PROB);
  return result;  
}

//-------------------------
bool TrgSegmLenTable::load(const char *segmLengthTableFileName)
{
 AwkInputStream awk;
	
 std::cerr<<"Loading target segment length table from file "<<segmLengthTableFileName<<std::endl;
 if(awk.open(segmLengthTableFileName)==THOT_ERROR)
 {
   std::cerr<<"Warning: target segment length tablefile does not exist, target segment length probability will be assumed to be uniform.\n";
   return 1;
 }
 else
 {
   clear();
   if(awk.getln())
   {
     if(strcmp("Uniform",awk.dollar(1).c_str())==0)
     {
       std::cerr<<"Using target segment length model based on a uniform distribution."<<std::endl;
       mode=TRGSEGMLEN_UNIFORM;
     }
     if(strcmp("Poisson",awk.dollar(1).c_str())==0)
     {
       mode=TRGSEGMLEN_POISSON;
       bool ret=readAvgSegmLen(segmLengthTableFileName);
       if(ret==THOT_ERROR)
       {
         avgSrcSegmLen=MAX_SENTENCE_LENGTH/2;
         avgTrgSegmLen=MAX_SENTENCE_LENGTH/2;
         std::cerr<<"Using target segment length model based on a Poisson distribution. Average segment lengths are assumed to be equal."<<std::endl;
       }
       else
         std::cerr<<"Using target segment length model based on a Poisson distribution."<<std::endl;
       return ret;
     }
     if(strcmp("Geometric",awk.dollar(1).c_str())==0)
     {
       std::cerr<<"Using target segment length model based on a geometric distribution."<<std::endl;
       mode=TRGSEGMLEN_GEOM;
     }
   }
 }
 return 0;	
}

//-------------------------
bool TrgSegmLenTable::readAvgSegmLen(const char *avgSegmLenFileName)
{
 AwkInputStream awk;

 std::cerr<<"Reading average segment length file from: "<<avgSegmLenFileName<<std::endl;
 if(awk.open(avgSegmLenFileName)==THOT_ERROR)
 {
   std::cerr<<"Error in average segment length file, file "<<avgSegmLenFileName<<" does not exist.\n";
   return THOT_ERROR;
 }  
 else
 {
   awk.getln();
   if(awk.NF==6)
   {
     avgSrcSegmLen=atof(awk.dollar(6).c_str());
   }
   else
   {
     std::cerr<<"Error in average segment length file: "<<avgSegmLenFileName<<" \n";
     return THOT_ERROR;
   }
   awk.getln();
   if(awk.NF==6)
   {
     avgTrgSegmLen=atof(awk.dollar(6).c_str());
   }
   else
   {
     std::cerr<<"Error in average segment length file: "<<avgSegmLenFileName<<" \n";
     return THOT_ERROR;
   }
 }
 return THOT_OK;  
}

//-------------------------
void TrgSegmLenTable::clear(void)
{
}
