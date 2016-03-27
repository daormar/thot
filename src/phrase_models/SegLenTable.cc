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
/* Module: SegLenTable                                              */
/*                                                                  */
/* Definitions file: SegLenTable.cc                                 */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "SegLenTable.h"

//--------------- Global variables -----------------------------------


//--------------- SegLenTable class method definitions

//-------------------------
SegLenTable::SegLenTable(void)
{
  clear();
}

//-------------------------
Prob SegLenTable::pk_tlen(unsigned int tlen,unsigned int k)
{
  if(tlen>MAX_SENTENCE_LENGTH || k>MAX_SENTENCE_LENGTH)
  {
    return 0;
  }
  else
  {
    if(ksegmLengthCountMargin[tlen]==0)
      return 0;
    else
      return (double) segmLengthCount[tlen][k]/ksegmLengthCountMargin[tlen];
  }
}

//-------------------------
void SegLenTable::constantSegmLengthTable(void)
{
      // Clear data members
  clear();

      // Initialize values
  for(unsigned int i=0;i<MAX_SENTENCE_LENGTH;++i)
  {
    ksegmLengthCountMargin[i]+=1;
    for(unsigned int j=0;j<MAX_SENTENCE_LENGTH;++j)
    {
      segmLengthCount[i][j]=1;  
    }
  }
}

//-------------------------
void SegLenTable::uniformSegmLengthTable(void)
{
      // TODO
}

//-------------------------
void SegLenTable::clear(void)
{
 unsigned int i,j;
	
 for(i=0;i<MAX_SENTENCE_LENGTH;++i)
 {
   ksegmLengthCountMargin[i]=0;
   for(j=0;j<MAX_SENTENCE_LENGTH;++j)
   {
     segmLengthCount[i][j]=0;  
   }	
 }
}

//-------------------------
void SegLenTable::incrCountOf_tlenk(unsigned int tlen,unsigned int k)
{
  ++segmLengthCount[tlen][k];
}

//-------------------------
void SegLenTable::incrCountOf_tlen(unsigned int tlen)
{
  ++ksegmLengthCountMargin[tlen];
}

//-------------------------
bool SegLenTable::load_seglentable(const char *segmLengthTableFileName)
{
 awkInputStream awk;
	
 cerr<<"Loading segmentation length table from file "<<segmLengthTableFileName<<endl;
 if(awk.open(segmLengthTableFileName)==ERROR)
 {
   // cerr<<"Warning: segmentation length tablefile not found, segmentation length probability will be assumed to be constant.\n";
   cerr<<"Segmentation length probability will be assumed to be constant.\n";
   constantSegmLengthTable();	 
   return OK;
 }
 else
 {
   clear();  
   while(awk.getln())
   {
     if(awk.NF==3)
     {
       if(atoi(awk.dollar(1).c_str())<MAX_SENTENCE_LENGTH &&
          atoi(awk.dollar(2).c_str())<MAX_SENTENCE_LENGTH)
       {
         segmLengthCount[atoi(awk.dollar(1).c_str())][atoi(awk.dollar(2).c_str())]=atof(awk.dollar(3).c_str());
         ksegmLengthCountMargin[atoi(awk.dollar(1).c_str())]+=atof(awk.dollar(3).c_str());
       }
       else
       {
         cerr<<"Warning reading seglentable entry, phrase length exceeded!"<<endl;
       }
     }
   }
 }
 
 return OK;
}

//-------------------------
void SegLenTable::printSegmLengthTable(ostream &outS)
{
 unsigned int i,j;

     // segm. table format: f_length k count	                                     
 for(i=0;i<MAX_SENTENCE_LENGTH;++i)
   for(j=0;j<MAX_SENTENCE_LENGTH;++j)
   {
     if(segmLengthCount[i][j]>0)
     {
       outS<< i << " " << j << " " << segmLengthCount[i][j] <<"\n";
     }
   }
}
