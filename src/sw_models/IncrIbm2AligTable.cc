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
/* Module: IncrIbm2AligTable                                        */
/*                                                                  */
/* Definitions file: IncrIbm2AligTable.cc                           */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "IncrIbm2AligTable.h"

//--------------- Global variables -----------------------------------


//--------------- Function declarations 


//--------------- Constants


//--------------- Classes --------------------------------------------


//--------------- IncrIbm2AligTable class function definitions

//-------------------------
IncrIbm2AligTable::IncrIbm2AligTable(void)
{
}

//-------------------------   
void IncrIbm2AligTable::setAligNumer(aSource as,
                                     PositionIndex i,
                                     float f)
{
      // Grow aligNumer
  AligNumerElem aligNumerElem;
  
  while(aligNumer.size()<=i)
    aligNumer.push_back(aligNumerElem);

      // Insert aligNumer for pair as,i
  aligNumer[i][as]=f;
}

//-------------------------   
float IncrIbm2AligTable::getAligNumer(aSource as,
                                      PositionIndex i,
                                      bool& found)
{
  if(i>=aligNumer.size())
  {
        // entry for i in aligNumer does not exist
    found=false;
    return 0;
  }
  else
  {
        // entry for i in aligNumer exists
    AligNumerElem::iterator aneIter=aligNumer[i].find(as);
    if(aneIter!=aligNumer[i].end())
    {
          // aligNumer for pair as,i exists
      found=true;
      return aneIter->second;
    }
    else
    {
          // aligNumer for pair as,i does not exist
      found=false;
      return 0;
    }
  }
}

//-------------------------   
void IncrIbm2AligTable::setAligDenom(aSource as,
                                     float f)
{
  aligDenom[as]=f;
}

//-------------------------   
float IncrIbm2AligTable::getAligDenom(aSource as,
                                      bool& found)
{
  AligNumerElem::iterator aneIter=aligDenom.find(as);
  if(aneIter!=aligDenom.end())
  {
        // s is stored in aligDenom
    found=true;
    return aneIter->second;
  }
  else
  {
        // s is not stored in aligDenom
    found=false;
    return 0;
  }  
}

//-------------------------   
void IncrIbm2AligTable::setAligNumDen(aSource as,
                                      PositionIndex i,
                                      float num,
                                      float den)
{
  setAligNumer(as,i,num);
  setAligDenom(as,den);
}

//-------------------------
bool IncrIbm2AligTable::load(const char* aligNumDenFile)
{
#ifdef THOT_ENABLE_LOAD_PRINT_TEXTPARS 
  return loadPlainText(aligNumDenFile);
#else
  return loadBin(aligNumDenFile);
#endif
}

//-------------------------
bool IncrIbm2AligTable::loadPlainText(const char* aligNumDenFile)
{
      // Clear data structures
  clear();

  std::cerr<<"Loading alignd file in plain text format from "<<aligNumDenFile<<std::endl;

  AwkInputStream awk;
  if(awk.open(aligNumDenFile)==THOT_ERROR)
  {
    std::cerr<<"Error in alignment nd file, file "<<aligNumDenFile<<" does not exist.\n";
    return THOT_ERROR;
  }
  else
  { 
        // Read entries
    while(awk.getln())
    {
      if(awk.NF==6)
      {
        aSource as;
        as.j=atoi(awk.dollar(1).c_str());
        as.slen=atoi(awk.dollar(2).c_str());
        as.tlen=atoi(awk.dollar(3).c_str());
        PositionIndex i=atoi(awk.dollar(4).c_str());
        float numer=atof(awk.dollar(5).c_str());
        float denom=atof(awk.dollar(6).c_str());
        setAligNumDen(as,i,numer,denom);
      }
    }
    return THOT_OK;
  }
}

//-------------------------
bool IncrIbm2AligTable::loadBin(const char* aligNumDenFile)
{
      // Clear data structures
  clear();

  std::cerr<<"Loading alignd file in binary format from "<<aligNumDenFile<<std::endl;

      // Try to open file  
  std::ifstream inF (aligNumDenFile, std::ios::in | std::ios::binary);
  if(!inF)
  {
    std::cerr<<"Error in alignment nd file, file "<<aligNumDenFile<<" does not exist.\n";
    return THOT_ERROR;    
  }
  else
  {
        // Read register
    bool end=false;
    while(!end)
    {
      aSource as;
      PositionIndex i;
      float numer;
      float denom;
      if(inF.read((char*)&as.j,sizeof(PositionIndex)))
      {
        inF.read((char*)&as.slen,sizeof(PositionIndex));
        inF.read((char*)&as.tlen,sizeof(PositionIndex));
        inF.read((char*)&i,sizeof(PositionIndex));
        inF.read((char*)&numer,sizeof(float));
        inF.read((char*)&denom,sizeof(float));
        setAligNumDen(as,i,numer,denom);
      }
      else end=true;
    }
    return THOT_OK;
  }
}

//-------------------------
bool IncrIbm2AligTable::print(const char* aligNumDenFile)
{
#ifdef THOT_ENABLE_LOAD_PRINT_TEXTPARS 
  return printPlainText(aligNumDenFile);
#else
  return printBin(aligNumDenFile);
#endif
}

//-------------------------
bool IncrIbm2AligTable::printPlainText(const char* aligNumDenFile)
{
  std::ofstream outF;
  outF.open(aligNumDenFile,std::ios::out);
  if(!outF)
  {
    std::cerr<<"Error while printing alignment nd file."<<std::endl;
    return THOT_ERROR;
  }
  else
  {
            // print file with alignment nd values
    for(PositionIndex i=0;i<aligNumer.size();++i)
    {
      AligNumerElem::const_iterator numElemIter;
      for(numElemIter=aligNumer[i].begin();numElemIter!=aligNumer[i].end();++numElemIter)
      {
        bool found;
        outF<<numElemIter->first.j<<" ";
        outF<<numElemIter->first.slen<<" ";
        outF<<numElemIter->first.tlen<<" ";
        outF<<i<<" ";
        outF<<numElemIter->second<<" ";
        float denom=getAligDenom(numElemIter->first,found);
        outF<<denom<<std::endl;
      }
    }
    return THOT_OK;
  }  
}

//-------------------------
bool IncrIbm2AligTable::printBin(const char* aligNumDenFile)
{
  std::ofstream outF;
  outF.open(aligNumDenFile,std::ios::out);
  if(!outF)
  {
    std::cerr<<"Error while printing alignment nd file."<<std::endl;
    return THOT_ERROR;
  }
  else
  {
        // print file with alignment nd values
    for(PositionIndex i=0;i<aligNumer.size();++i)
    {
      AligNumerElem::const_iterator numElemIter;
      for(numElemIter=aligNumer[i].begin();numElemIter!=aligNumer[i].end();++numElemIter)
      {
        bool found;
        outF.write((char*)&numElemIter->first.j,sizeof(PositionIndex));
        outF.write((char*)&numElemIter->first.slen,sizeof(PositionIndex));
        outF.write((char*)&numElemIter->first.tlen,sizeof(PositionIndex));
        outF.write((char*)&i,sizeof(PositionIndex));
        outF.write((char*)&numElemIter->second,sizeof(float));
        float denom=getAligDenom(numElemIter->first,found);
        outF.write((char*)&denom,sizeof(float));
      }
    }
    return THOT_OK;
  }
}

//-------------------------
void IncrIbm2AligTable::clear(void)
{
  aligNumer.clear();
  aligDenom.clear();
}
