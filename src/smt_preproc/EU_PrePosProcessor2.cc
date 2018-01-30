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
 * @file EU_PrePosProcessor2.cc
 * 
 * @brief Definitions file for EU_PrePosProcessor2.h
 */

//--------------- Include files --------------------------------------

#include "EU_PrePosProcessor2.h"


//--------------- EU_PrePosProcessor2 class functions
//

//---------------------------------------
EU_PrePosProcessor2::EU_PrePosProcessor2()
{
  
}

//---------------------------------------
bool EU_PrePosProcessor2::loadCapitInfo(const char* filename)
{
  std::ifstream *caps=0;
  
  std::cerr<<"Initializing capitalization from file "<<filename<<" ... ";

  caps= new std::ifstream(filename);
  if ( caps )
  {
    EUpostprocInitializeCapitalization(*caps);
    delete caps;
    std::cerr<<"THOT_OK"<<std::endl;
    
    return THOT_OK;
  }
  else
  {
    std::cerr<<"THOT_ERROR"<<std::endl;
    
    return THOT_ERROR;
  }
}

//---------------------------------------
std::string EU_PrePosProcessor2::preprocLine(std::string str,
                                             bool caseconv,
                                             bool keepPreprocInfo)
{
      // Store state info
  if(keepPreprocInfo)
    lastPreprocStrKept=str;

  std::string tok_str=XRCEtokLine(str.c_str(),false,false);
      // The tokenization step is the same used for the XRCE corpus (no
      // specific tokenization step was implemented for the EU corpus in
      // the TT2 project)
  return EUpprocLine(tok_str.c_str(),caseconv,keepPreprocInfo);
      // when last argument of EUpprocLine is true its internal tables
      // are modified
}

//---------------------------------------
std::string EU_PrePosProcessor2::postprocLine(std::string str,
                                              bool caseconv)
{
      // Restore preprocessing info if necessary
  if(!lastPreprocStrKept.empty())
  {
    std::string tok_str=XRCEtokLine(lastPreprocStrKept.c_str(),false,false);
    EUpprocLine(tok_str.c_str(),caseconv,true);
  }

  std::string eupostproc_str=EUpostprocLine(str.c_str(),caseconv);
  return XRCEdetokLine(eupostproc_str.c_str(),false);
      // The detokenization step is the same used for the XRCE corpus
      // (no specific detokenization step was implemented for the EU
      // corpus in the TT2 project)
}

//---------------------------------------
bool EU_PrePosProcessor2::isCategory(std::string word)
{
  if(word.size()==0) return false;
  else
  {
    if(word[0]=='_')
      return true;
    else
      return false;
  }
}

//---------------------------------------
EU_PrePosProcessor2::~EU_PrePosProcessor2()
{
  
}
