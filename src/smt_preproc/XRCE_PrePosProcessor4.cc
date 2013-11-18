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
/* Module: XRCE_PrePosProcessor4                                    */
/*                                                                  */
/* Definitions file: XRCE_PrePosProcessor4.cc                       */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "XRCE_PrePosProcessor4.h"


//--------------- XRCE_PrePosProcessor4 class functions
//

//---------------------------------------
XRCE_PrePosProcessor4::XRCE_PrePosProcessor4()
{
}

//---------------------------------------
bool XRCE_PrePosProcessor4::loadCapitInfo(const char* /*filename*/)
{
  cerr<<"Warning: this module does not capitalize or decapitalize sentences!"<<endl;
  
  return OK;
}

//---------------------------------------
std::string XRCE_PrePosProcessor4::preprocLine(std::string str,
                                               bool /*caseconv*/,
                                               bool keepPreprocInfo)
{
      // Store state info
  if(keepPreprocInfo)
     lastPreprocStrKept=str;

  const char* tokstr=XRCEtokLine(str.c_str(),false,keepPreprocInfo);
  return XRCEcategLine(tokstr,keepPreprocInfo);
}

//---------------------------------------
std::string XRCE_PrePosProcessor4::postprocLine(std::string str,
                                                bool /*caseconv*/)
{
      // Restore preprocessing info if necessary
  if(!lastPreprocStrKept.empty())
  {
    const char* tokstr=XRCEtokLine(lastPreprocStrKept.c_str(),false,true);
    XRCEcategLine(tokstr,true);
  }
  
  const char* decategstr=XRCEdecategLine(str.c_str());
  return XRCEdetokLine(decategstr,false);
}

//---------------------------------------
bool XRCE_PrePosProcessor4::isCategory(std::string /*word*/)
{
  return false;
}

//---------------------------------------
XRCE_PrePosProcessor4::~XRCE_PrePosProcessor4()
{
  
}
