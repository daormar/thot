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

/**
 * @file Cache_lct_.cc
 * 
 * @brief Definitions file for Cache_lct_.h
 */

//--------------- Include files --------------------------------------

#include "Cache_lct_.h"

//--------------- Function definitions

//-------------------------
Cache_lct_::Cache_lct_(void)
{
  valid=false;
}

//-------------------------
bool Cache_lct_::is_valid(void)
{
  return valid;
}

//-------------------------
bool Cache_lct_::t_present(const std::vector<WordIndex>& _t_)
{
  if(is_valid() && t_==_t_)
    return true;
  else
    return false;
}

//-------------------------
void Cache_lct_::init(const std::vector<WordIndex>& _t_,
                      LogCount _lcount_t_)
{
  valid=true;
  t_=_t_;
  lcount_t_=_lcount_t_;
}

//-------------------------
LogCount Cache_lct_::lct_(void)
{
  if(is_valid())
  {
    return lcount_t_;
  }
  else
  {
    return 0;
  }
}

//-------------------------
void Cache_lct_::clear(void)
{
  valid=false;
}
