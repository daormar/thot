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
 
#ifndef _aSource_h
#define _aSource_h

//--------------- Include files --------------------------------------

#include "SwDefs.h"

//--------------- Classes ---------------------------------------------

class aSource
{
 public:
  PositionIndex j;
  PositionIndex slen;
  PositionIndex tlen;
  
  bool operator< (const aSource&  right) const
  {
   if(right.j<j) return 0; if(j<right.j) return 1;
   if(right.slen<slen) return 0; if(slen<right.slen) return 1;    
   if(right.tlen<tlen) return 0; if(tlen<right.tlen) return 1;
   return 0;  
  }

  bool operator== (const aSource&  right) const
  {
    return (j==right.j && slen==right.slen && tlen==right.tlen);
  }

};

std::ostream& operator << (std::ostream &outS,const aSource &aSrc);

#endif
