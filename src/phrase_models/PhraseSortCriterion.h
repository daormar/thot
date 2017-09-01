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
 
#ifndef _PhraseSortCriterion_h
#define _PhraseSortCriterion_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <vector>
#include <utility>
#include "PositionIndex.h"

//--------------- PhraseSortCriterion class

class PhraseSortCriterion
{
 public:
  bool operator() (const std::vector<WordIndex> &a,
                   const std::vector<WordIndex> &b)const
    {
      unsigned int i;
					  
      if(b.size()<a.size()) return 0; if(a.size()<b.size()) return 1;	  
      for(i=0;i<a.size();++i)
      {
        if(b[i]<a[i]) return 0; if(a[i]<b[i]) return 1;
      }
      return 0;  
    }
};

#endif
