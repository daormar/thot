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
 

#ifndef _BasePbTransModelStats_h
#define _BasePbTransModelStats_h

//--------------- Include files ----------------------------------------------

#include <iostream>
#include <iomanip>
#include "ctimer.h"
#include "Prob.h"

using namespace std;

//--------------- Constants --------------------------------------------------


//--------------- BasePbTransModelStats class: class for calculating statistics of
//--------------- the BasePbTransModel class

class BasePbTransModelStats
{
  public:
   unsigned long transOptions;
   unsigned long getTransCalls;

   BasePbTransModelStats(void)
     {
     }
   void clear(void)
     {
       transOptions=0;
       getTransCalls=0;		
     }
   ostream & print(ostream & outS)
     {
       outS<< " * Translation options mean       : " << (float)transOptions/getTransCalls <<"\n";
       return outS;
     }
};

#endif
