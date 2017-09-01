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
 
#ifndef _DoubleMatrix_h
#define _DoubleMatrix_h

//--------------- Include files --------------------------------------

#include "SwDefs.h"
#include <vector>
#include <float.h>

//--------------- Constants ------------------------------------------

#define INVALID_DM_VAL FLT_MAX

//--------------- Classes ---------------------------------------------

class DoubleMatrix
{
 public:

  void set(unsigned int i,
           unsigned int j,
           double d);
  
  inline double get(unsigned int i,
                    unsigned int j)
  {
        // Check boundaries
    if(doubleVecVec.size()<=i) return INVALID_DM_VAL;
    if(doubleVecVec[i].size()<=j) return INVALID_DM_VAL;
        // doubleVecVec[i][j] is defined
    return doubleVecVec[i][j];
  }
  
  unsigned int i_size(void);

  unsigned int ij_size(unsigned int i);

  void clear(void);
  
 protected:

  std::vector<std::vector<double> > doubleVecVec;
    
};

#endif
