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
/* Module: PhrNbestTransTablePrefKey                                */
/*                                                                  */
/* Definitions file: PhrNbestTransTablePrefKey.cc                   */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "PhrNbestTransTablePrefKey.h"

//--------------- PhrNbestTransTablePrefKey class functions

bool PhrNbestTransTablePrefKey::operator< (const PhrNbestTransTablePrefKey &right)const
{
  if(srcLeft < right.srcLeft) return 0; if(right.srcLeft < srcLeft) return 1;
  if(srcRight < right.srcRight) return 0; if(right.srcRight < srcRight) return 1;
  if(ntrgSize < right.ntrgSize) return 0; if(right.ntrgSize < ntrgSize) return 1;
  if(lastGap < right.lastGap) return 0; if(right.lastGap < lastGap) return 1;
  return 0;
}
