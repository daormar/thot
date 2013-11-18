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
 
//--------------- Include files --------------------------------------

#include "SrfNodeKey.h"

//--------------- lt operator
bool operator<(const SrfNodeKey& k1,
               const SrfNodeKey& k2)
{
  if(k1.srcCuts<k2.srcCuts) return 0;
  if(k2.srcCuts<k1.srcCuts) return 1;

  if(k1.trgCuts<k2.trgCuts) return 0;
  if(k2.trgCuts<k1.trgCuts) return 1;

  return 0;  
}
