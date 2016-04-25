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
 
#ifndef _SwModelPars_h
#define _SwModelPars_h

//--------------- Include files --------------------------------------

#include <string>

//--------------- SwModelPars struct

struct SwModelPars
{
  std::string readTablePrefix;

      // sw model without len weight and sentence length model weight
  float swNoLenWeight;
  float lenWeight;
  
      // sw model weights
  float swWeight;

      // Constructor
  SwModelPars(void)
    {
      swNoLenWeight=1.0;
      lenWeight=1.0;
      swWeight=1.0;
    };
};

#endif
