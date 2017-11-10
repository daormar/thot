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
/* Module: IncrHmmAligTable                                         */
/*                                                                  */
/* Definitions file: IncrHmmAligTable.cc                            */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "IncrHmmAligTable.h"

//--------------- Global variables -----------------------------------


//--------------- Function declarations 


//--------------- Constants


//--------------- Classes --------------------------------------------


//--------------- IncrHmmAligTable class function definitions

//-------------------------
IncrHmmAligTable::IncrHmmAligTable(void)
{
}

//-------------------------   
void IncrHmmAligTable::setAligNumer(aSourceHmm asHmm,
                                    PositionIndex i,
                                    float f)
{
      // Grow aligNumer
  AligNumerElem aligNumerElem;
  
  while(aligNumer.size()<=i)
    aligNumer.push_back(aligNumerElem);

  std::vector<std::pair<bool,float> > bdpVec;
  while(aligNumer[i].size()<=asHmm.prev_i)
    aligNumer[i].push_back(bdpVec);
  while(aligNumer[i][asHmm.prev_i].size()<=asHmm.slen)
    aligNumer[i][asHmm.prev_i].push_back(std::make_pair(false,0));
  aligNumer[i][asHmm.prev_i][asHmm.slen]=std::make_pair(true,f);
}

//-------------------------   
float IncrHmmAligTable::getAligNumer(aSourceHmm asHmm,
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
    if(aligNumer[i].size()>asHmm.prev_i)
    {
      if(aligNumer[i][asHmm.prev_i].size()>asHmm.slen)
      {
        found=aligNumer[i][asHmm.prev_i][asHmm.slen].first;
        return aligNumer[i][asHmm.prev_i][asHmm.slen].second;
      }
      else
      {
        found=false;
        return 0;
      }
    }
    else
    {
      found=false;
      return 0;
    }
  }
}

//-------------------------   
void IncrHmmAligTable::setAligDenom(aSourceHmm asHmm,
                                    float f)
{
  std::vector<std::pair<bool,float> > bdpVec;
  while(aligDenom.size()<=asHmm.prev_i)
    aligDenom.push_back(bdpVec);
  while(aligDenom[asHmm.prev_i].size()<=asHmm.slen)
    aligDenom[asHmm.prev_i].push_back(std::make_pair(false,0));
  aligDenom[asHmm.prev_i][asHmm.slen]=std::make_pair(true,f);
}

//-------------------------   
float IncrHmmAligTable::getAligDenom(aSourceHmm asHmm,
                                     bool& found)
{
  if(aligDenom.size()>asHmm.prev_i)
  {
    if(aligDenom[asHmm.prev_i].size()>asHmm.slen)
    {
      found=aligDenom[asHmm.prev_i][asHmm.slen].first;
      return aligDenom[asHmm.prev_i][asHmm.slen].second;
    }
    else
    {
      found=false;
      return 0;
    }
  }
  else
  {
    found=false;
    return 0;
  }
}

//-------------------------   
void IncrHmmAligTable::setAligNumDen(aSourceHmm asHmm,
                                     PositionIndex i,
                                     float num,
                                     float den)
{
  setAligNumer(asHmm,i,num);
  setAligDenom(asHmm,den);
}

//-------------------------
bool IncrHmmAligTable::load(const char* aligNumDenFile)
{
#ifdef THOT_ENABLE_LOAD_PRINT_TEXTPARS 
  return loadPlainText(aligNumDenFile);
#else
  return loadBin(aligNumDenFile);
#endif
}

//-------------------------
bool IncrHmmAligTable::loadPlainText(const char* aligNumDenFile)
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
    while(awk.getln())
    {
      if(awk.NF==5)
      {
        aSourceHmm asHmm;
        asHmm.prev_i=atoi(awk.dollar(1).c_str());
        asHmm.slen=atoi(awk.dollar(2).c_str());
        PositionIndex i=atoi(awk.dollar(3).c_str());
        float numer=atof(awk.dollar(4).c_str());
        float denom=atof(awk.dollar(5).c_str());
        setAligNumDen(asHmm,i,numer,denom);
      }
    }
    return THOT_OK;
  }
}

//-------------------------
bool IncrHmmAligTable::loadBin(const char* aligNumDenFile)
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
      aSourceHmm asHmm;
      PositionIndex i;
      float numer;
      float denom;
      if(inF.read((char*)&asHmm.prev_i,sizeof(PositionIndex)))
      {
        inF.read((char*)&asHmm.slen,sizeof(PositionIndex));
        inF.read((char*)&i,sizeof(PositionIndex));
        inF.read((char*)&numer,sizeof(float));
        inF.read((char*)&denom,sizeof(float));
        setAligNumDen(asHmm,i,numer,denom);
      }
      else end=true;
    }
    return THOT_OK;
  }
}

//-------------------------
bool IncrHmmAligTable::print(const char* aligNumDenFile)
{
#ifdef THOT_ENABLE_LOAD_PRINT_TEXTPARS 
  return printPlainText(aligNumDenFile);
#else
  return printBin(aligNumDenFile);
#endif
}

//-------------------------
bool IncrHmmAligTable::printBin(const char* aligNumDenFile)
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
      for(PositionIndex prev_i=0;prev_i<aligNumer[i].size();++prev_i)
      {
        for(PositionIndex slen=0;slen<aligNumer[i][prev_i].size();++slen)
        {
          if(aligNumer[i][prev_i][slen].first)
          {
            bool found;
            outF.write((char*)&prev_i,sizeof(PositionIndex));
            outF.write((char*)&slen,sizeof(PositionIndex));
            outF.write((char*)&i,sizeof(PositionIndex));
            outF.write((char*)&aligNumer[i][prev_i][slen].second,sizeof(float));
            aSourceHmm asHmm;
            asHmm.prev_i=prev_i;
            asHmm.slen=slen;
            float denom=getAligDenom(asHmm,found);
            outF.write((char*)&denom,sizeof(float));            
          }
        }
      }      
    }
    return THOT_OK;
  }
}

//-------------------------
bool IncrHmmAligTable::printPlainText(const char* aligNumDenFile)
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
      for(PositionIndex prev_i=0;prev_i<aligNumer[i].size();++prev_i)
      {
        for(PositionIndex slen=0;slen<aligNumer[i][prev_i].size();++slen)
        {
          if(aligNumer[i][prev_i][slen].first)
          {
            bool found;
            outF<<prev_i<<" ";
            outF<<slen<<" ";
            outF<<i<<" ";
            outF<<aligNumer[i][prev_i][slen].second<<" ";
            aSourceHmm asHmm;
            asHmm.prev_i=prev_i;
            asHmm.slen=slen;
            float denom=getAligDenom(asHmm,found);
            outF<<denom<<std::endl;
          }
        }
      }      
    }
    return THOT_OK;
  }
}

//-------------------------
void IncrHmmAligTable::clear(void)
{
  aligNumer.clear();
  aligDenom.clear();
}
