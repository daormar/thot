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
/* Module: HypDebugData                                             */
/*                                                                  */
/* Prototypes file: HypDebugData.h                                  */
/*                                                                  */
/* Description: Class for debugging purposes.                       */
/*                                                                  */
/********************************************************************/

#ifndef _HypDebugData_h
#define _HypDebugData_h

//--------------- Include files --------------------------------------

#include "Score.h"
#include <string.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------


//--------------- HypDebugData class: class for storing debugging
//                                    information of each operation
//                                    made during the expansion process

class HypDebugData
{
 public:

  std::string opCode;
  std::vector<unsigned int> parameters;
  std::vector<Score> partialContribs;
  Score accum;

  void print(std::ostream &outS)const
    {
      outS<<"Op: "<<opCode;
      for(unsigned int i=0;i<parameters.size();++i)
      {
        outS<<" "<<parameters[i];
      }
      outS<<" ;";
      for(unsigned int i=0;i<partialContribs.size();++i)
      {
        outS<<" "<<partialContribs[i];
      }
      outS<<" ; Accum. Score: "<<accum<<std::endl;
    }
};

#endif
