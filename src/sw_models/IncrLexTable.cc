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
/* Module: IncrLexTable                                             */
/*                                                                  */
/* Definitions file: IncrLexTable.cc                                */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "IncrLexTable.h"

//--------------- Global variables -----------------------------------


//--------------- Function declarations 


//--------------- Constants


//--------------- Classes --------------------------------------------


//--------------- IncrLexTable class function definitions

//-------------------------
IncrLexTable::IncrLexTable(void)
{
}

//-------------------------   
void IncrLexTable::setLexNumer(WordIndex s,
                               WordIndex t,
                               float f)
{
      // Grow lexNumer
  LexNumerElem lexNumerElem;
  
  while(lexNumer.size()<=t)
    lexNumer.push_back(lexNumerElem);

      // Insert lexNumer for pair s,t
  lexNumer[t][s]=f;
}

//-------------------------   
float IncrLexTable::getLexNumer(WordIndex s,
                                WordIndex t,
                                bool& found)
{
  LexNumerElem::iterator lexNumerElemIter;

  if(t>=lexNumer.size())
  {
        // entry for t in lexNumer does not exist
    found=false;
    return 0;
  }
  else
  {
        // entry for t in lexNumer exists
    lexNumerElemIter=lexNumer[t].find(s);
    if(lexNumerElemIter!=lexNumer[t].end())
    {
          // lexNumer for pair s,t exists
      found=true;
      return lexNumerElemIter->second;
    }
    else
    {
          // lexNumer for pair s,t does not exist
      found=false;
      return 0;
    }
  }
}
   
//-------------------------   
void IncrLexTable::setLexDenom(WordIndex s,
                               float d)
{
  while(lexDenom.size()<=s)
    lexDenom.push_back(std::make_pair(false,0));
  lexDenom[s]=std::make_pair(true,d);  
}

//-------------------------   
float IncrLexTable::getLexDenom(WordIndex s,
                                bool& found)
{
  if(lexDenom.size()>s)
  {
    found=lexDenom[s].first;
    return lexDenom[s].second;
  }
  else
  {
    found=false;
    return 0;
  }
}

//-------------------------
bool IncrLexTable::getTransForTarget(WordIndex t,
                                     std::set<WordIndex>& transSet)
{
  transSet.clear();
  
  if(t>=lexNumer.size())
    return false;
  else
  {
    LexNumerElem::const_iterator numElemIter;
    for(numElemIter=lexNumer[t].begin();numElemIter!=lexNumer[t].end();++numElemIter)
    {
      WordIndex s=numElemIter->first;
      transSet.insert(s);
    }
    return true;
  }
}

//-------------------------
void IncrLexTable::setLexNumDen(WordIndex s,
                                WordIndex t,
                                float num,
                                float den)
{
  setLexDenom(s,den);
  setLexNumer(s,t,num);
}

//-------------------------
bool IncrLexTable::load(const char* lexNumDenFile)
{
#ifdef THOT_ENABLE_LOAD_PRINT_TEXTPARS 
  return loadPlainText(lexNumDenFile);
#else
  return loadBin(lexNumDenFile);
#endif
}

//-------------------------
bool IncrLexTable::loadBin(const char* lexNumDenFile)
{
      // Clear data structures
  clear();

  std::cerr<<"Loading lexnd file in binary format from "<<lexNumDenFile<<std::endl;

      // Try to open file  
  std::ifstream inF (lexNumDenFile, std::ios::in | std::ios::binary);
  if (!inF)
  {
    std::cerr<<"Error in lexical nd file, file "<<lexNumDenFile<<" does not exist.\n";
    return THOT_ERROR;    
  }
  else
  {
        // Read register
    bool end=false;
    while(!end)
    {
      WordIndex s;
      WordIndex t;
      float numer;
      float denom;
      if(inF.read((char*)&s,sizeof(WordIndex)))
      {
        inF.read((char*)&t,sizeof(WordIndex));
        inF.read((char*)&numer,sizeof(float));
        inF.read((char*)&denom,sizeof(float));
        setLexNumDen(s,t,numer,denom);
      }
      else end=true;
    }
    return THOT_OK;
  }
}

//-------------------------
bool IncrLexTable::loadPlainText(const char* lexNumDenFile)
{
      // Clear data structures
  clear();

  std::cerr<<"Loading lexnd file in plain text format from "<<lexNumDenFile<<std::endl;

  AwkInputStream awk;
  if(awk.open(lexNumDenFile)==THOT_ERROR)
  {
    std::cerr<<"Error in file with lexical parameters, file "<<lexNumDenFile<<" does not exist.\n";
    return THOT_ERROR;
  }
  else
  { 
        // Read gaussian parameters
    while(awk.getln())
    {
      if(awk.NF==4)
      {
        WordIndex s=atoi(awk.dollar(1).c_str());
        WordIndex t=atoi(awk.dollar(2).c_str());
        float numer=atof(awk.dollar(3).c_str());
        float denom=atof(awk.dollar(4).c_str());
        setLexNumDen(s,t,numer,denom);
      }
    }
    return THOT_OK;
  }
}

//-------------------------
bool IncrLexTable::print(const char* lexNumDenFile)
{
#ifdef THOT_ENABLE_LOAD_PRINT_TEXTPARS 
  return printPlainText(lexNumDenFile);
#else
  return printBin(lexNumDenFile);
#endif
}

//-------------------------
bool IncrLexTable::printBin(const char* lexNumDenFile)
{
  std::ofstream outF;
  outF.open(lexNumDenFile,std::ios::out);
  if(!outF)
  {
    std::cerr<<"Error while printing lexical nd file."<<std::endl;
    return THOT_ERROR;
  }
  else
  {
        // print file with lexical nd values
    for(WordIndex t=0;t<lexNumer.size();++t)
    {
      LexNumerElem::const_iterator numElemIter;
      for(numElemIter=lexNumer[t].begin();numElemIter!=lexNumer[t].end();++numElemIter)
      {
        bool found;
        outF.write((char*)&numElemIter->first,sizeof(WordIndex));
        outF.write((char*)&t,sizeof(WordIndex));
        outF.write((char*)&numElemIter->second,sizeof(float));
        float denom=getLexDenom(numElemIter->first,found);
        outF.write((char*)&denom,sizeof(float));
      }
    }
    return THOT_OK;
  }
}

//-------------------------
bool IncrLexTable::printPlainText(const char* lexNumDenFile)
{
  std::ofstream outF;
  outF.open(lexNumDenFile,std::ios::out);
  if(!outF)
  {
    std::cerr<<"Error while printing lexical nd file."<<std::endl;
    return THOT_ERROR;
  }
  else
  {
        // print file with lexical nd values
    for(WordIndex t=0;t<lexNumer.size();++t)
    {
      LexNumerElem::const_iterator numElemIter;
      for(numElemIter=lexNumer[t].begin();numElemIter!=lexNumer[t].end();++numElemIter)
      {
        bool found;
        outF<<numElemIter->first<<" ";
        outF<<t<<" ";
        outF<<numElemIter->second<<" ";
        float denom=getLexDenom(numElemIter->first,found);
        outF<<denom<<std::endl;;
      }
    }
    return THOT_OK;
  }
}

//-------------------------
void IncrLexTable::clear(void)
{
  lexNumer.clear();
  lexDenom.clear();
}

//-------------------------
IncrLexTable::~IncrLexTable(void)
{
  // Nothing to do
}
