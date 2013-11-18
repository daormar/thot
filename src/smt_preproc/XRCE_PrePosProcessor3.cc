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
/* Module: XRCE_PrePosProcessor3                                    */
/*                                                                  */
/* Definitions file: XRCE_PrePosProcessor3.cc                       */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "XRCE_PrePosProcessor3.h"


//--------------- XRCE_PrePosProcessor3 class functions
//

//---------------------------------------
XRCE_PrePosProcessor3::XRCE_PrePosProcessor3()
{
  
}

//---------------------------------------
bool XRCE_PrePosProcessor3::loadCapitInfo(const char* /*filename*/)
{
  cerr<<"Warning: this module does not capitalize or decapitalize sentences!"<<endl;
  
  return OK;
}

//---------------------------------------
std::string XRCE_PrePosProcessor3::preprocLine(std::string str,
                                               bool /*caseconv*/,
                                               bool /*keepPreprocInfo*/)
{
  return XRCEtokLine(str.c_str(),false,false);
}

//---------------------------------------
std::string XRCE_PrePosProcessor3::postprocLine(std::string str,
                                                bool /*caseconv*/)
{
  return XRCEdetokLine(str.c_str(),false);
}

//---------------------------------------
bool XRCE_PrePosProcessor3::isCategory(std::string /*word*/)
{
  return false;
}

//---------------------------------------
XRCE_PrePosProcessor3::~XRCE_PrePosProcessor3()
{
  
}
