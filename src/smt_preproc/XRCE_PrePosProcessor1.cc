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
/* Module: XRCE_PrePosProcessor1                                    */
/*                                                                  */
/* Definitions file: XRCE_PrePosProcessor1.cc                       */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "XRCE_PrePosProcessor1.h"


//--------------- XRCE_PrePosProcessor1 class functions
//

//---------------------------------------
XRCE_PrePosProcessor1::XRCE_PrePosProcessor1()
{
}

//---------------------------------------
bool XRCE_PrePosProcessor1::loadCapitInfo(const char* filename)
{
  std::ifstream *caps=0;
  
  cerr<<"Initializing capitalization from file "<<filename<<" ... ";

  caps= new std::ifstream(filename);
  if ( caps )
  {
    XRCEpostprocInitializeCapitalization(*caps);
    delete caps;
    cerr<<"THOT_OK"<<endl;
    
    return THOT_OK;
  }
  else
  {
    cerr<<"ERROR"<<endl;
    
    return ERROR;
  }
}

//---------------------------------------
std::string XRCE_PrePosProcessor1::preprocLine(std::string str,
                                               bool caseconv,
                                               bool keepPreprocInfo)
{
      // Store state info
  if(keepPreprocInfo)
    lastPreprocStrKept=str;

      // Return preprocessed sentence
  return XRCEpprocLine(str.c_str(),caseconv,keepPreprocInfo);
      // when last argument of XRCEpprocLine is true its internal tables
      // are modified
}

//---------------------------------------
std::string XRCE_PrePosProcessor1::postprocLine(std::string str,
                                                bool caseconv)
{
      // Restore preprocessing info if necessary
  if(!lastPreprocStrKept.empty())
    XRCEpprocLine(lastPreprocStrKept.c_str(),caseconv,true);
  
  return XRCEpostprocLine(str.c_str(),caseconv);
}

//---------------------------------------
bool XRCE_PrePosProcessor1::isCategory(std::string word)
{
  if(word.size()==0) return false;
  else
  {
    if(word[word.size()-1]=='>')
      return true;
    else
      return false;
  }
}

//---------------------------------------
XRCE_PrePosProcessor1::~XRCE_PrePosProcessor1()
{
  
}
