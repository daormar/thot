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
 * @file lt_op_vec.h
 * 
 * @brief defines the less-than operator for vectors.
 */

#ifndef _lt_op_vec_h
#define _lt_op_vec_h

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <vector>

template<class VEC_ELEM>
bool operator<(const std::vector<VEC_ELEM> &left,
               const std::vector<VEC_ELEM> &right)
{
  if(left.size() < right.size()) return 0;
  if(right.size() < left.size()) return 1;
  for(unsigned int i=0;i<left.size();++i)
  {
    if((left[i])<right[i]) return 0;
    if(right[i]<left[i]) return 1;
  }
  return 0;
}

#endif
