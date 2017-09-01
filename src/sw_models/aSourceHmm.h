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
 
#ifndef _aSourceHmm_h
#define _aSourceHmm_h

//--------------- Include files --------------------------------------

#include "SwDefs.h"

//--------------- Classes ---------------------------------------------

class aSourceHmm
{
 public:
  PositionIndex prev_i;
  PositionIndex slen;

  bool operator== (const aSourceHmm&  right) const
  {
   if(right.prev_i!=prev_i) return 0; 
   if(right.slen!=slen) return 0;
   return 1;
  }
  
  bool operator< (const aSourceHmm&  right) const
  {
   if(right.prev_i<prev_i) return 0; if(prev_i<right.prev_i) return 1;
   if(right.slen<slen) return 0; if(slen<right.slen) return 1;    
   return 0;  
  }
};

std::ostream& operator << (std::ostream &outS,const aSourceHmm &aSrcHmm);

#endif
