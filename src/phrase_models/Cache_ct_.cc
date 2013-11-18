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
/* Module: Cache_ct_                                                */
/*                                                                  */
/* Definitions file: Cache_ct_.cc                                   */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "Cache_ct_.h"

//--------------- Function definitions

//-------------------------
Cache_ct_::Cache_ct_(void)
{
  valid=false;
}

//-------------------------
bool Cache_ct_::is_valid(void)
{
  return valid;
}

//-------------------------
bool Cache_ct_::t_present(const Vector<WordIndex>& _t_)
{
  if(is_valid() && t_==_t_)
    return true;
  else
    return false;
}

//-------------------------
void Cache_ct_::init(const Vector<WordIndex>& _t_,
                     Count _count_t_)
{
  valid=true;
  t_=_t_;
  count_t_=_count_t_;
}

//-------------------------
Count Cache_ct_::ct_(void)
{
  if(is_valid())
  {
    return count_t_;
  }
  else
  {
    return 0;
  }
}

//-------------------------
void Cache_ct_::clear(void)
{
  valid=false;
}
