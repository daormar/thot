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
/* Module: SrcSegmLenTable                                          */
/*                                                                  */
/* Definitions file: SrcSegmLenTable.cc                             */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "SrcSegmLenTable.h"

//--------------- Global variables -----------------------------------


//--------------- SrcSegmLenTable class function definitions

//-------------------------
SrcSegmLenTable::SrcSegmLenTable(void)
{
  mode=SRCSEGMLEN_UNIFORM;  
}

//-------------------------
LgProb SrcSegmLenTable::srcSegmLenLgProb(unsigned int x_k,
                                         unsigned int x_km1,
                                         unsigned int srcLen)
{
  switch(mode)
  {
    case SRCSEGMLEN_UNIFORM: return srcSegmLenLgProbUniform(x_k,x_km1,srcLen);
    case SRCSEGMLEN_GEOM: return srcSegmLenLgProbGeom(x_k,x_km1,srcLen);
    default: return 0;
  }
}

//-------------------------
LgProb SrcSegmLenTable::srcSegmLenLgProbUniform(unsigned int /*x_k*/,
                                                unsigned int x_km1,
                                                unsigned int srcLen)
{
  if(srcLen>0)
  {
    return log(1.0)-log((float)srcLen-(float)x_km1);
  }
  else
  {
        // if srcLen=0 then generate constant log-prob
    return log(1.0)-log((float) MAX_SENTENCE_LENGTH);
  }
}

//-------------------------
LgProb SrcSegmLenTable::srcSegmLenLgProbGeom(unsigned int x_k,
                                             unsigned int x_km1,
                                             unsigned int /*srcLen*/)
{
  unsigned int srcSegmLen=x_k-x_km1;

  LgProb result=log((double)SRCSEGMLEN_GEOM_HIT_PROB);
  result+=srcSegmLen*(log((double)1.0-SRCSEGMLEN_GEOM_HIT_PROB));
  return result;
}

//-------------------------
bool SrcSegmLenTable::load(const char *segmLengthTableFileName)
{
 awkInputStream awk;
	
 cerr<<"Loading source segment length table from file "<<segmLengthTableFileName<<endl;
 if(awk.open(segmLengthTableFileName)==ERROR)
 {
   cerr<<"Warning: source segment length tablefile does not exist, source segment length probability will be assumed to be uniform.\n";
   return 1;
 }
 else
 {
   clear();
   if(awk.getline())
   {
     if(strcmp("Uniform",awk.dollar(1).c_str())==0)
     {
       cerr<<"Using source segment length model based on a uniform distribution."<<endl;
       mode=SRCSEGMLEN_UNIFORM;
     }
     if(strcmp("Geometric",awk.dollar(1).c_str())==0)
     {
       cerr<<"Using source segment length model based on a geometric distribution."<<endl;
       mode=SRCSEGMLEN_GEOM;
     }
   }
 }
 return 0;	
}

//-------------------------
void SrcSegmLenTable::clear(void)
{
}
