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
 
#ifndef _PbTransModelPars_h
#define _PbTransModelPars_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

//--------------- Constants ------------------------------------------

#define PBM_W_DEFAULT          10
#define PBM_A_DEFAULT          10
#define PBM_E_DEFAULT          10
#define PBM_U_DEFAULT          10

//--------------- Classes --------------------------------------------

class PbTransModelPars
{
 public:
  
  float W;                       // Maximum number of translation
                                 // options
  unsigned int A;                // Maximum number of alignments per
                                 // expansion
  unsigned int E;                // Determines the length range for
                                 // reference alignments per expansion:
                                 // [s-E,s+E], where s is the length of
                                 // the source phrase length that is
                                 // being covered
  unsigned int U;                // Maximum number of words jumped

      // Constructor
  PbTransModelPars(void)
  {
    W=PBM_W_DEFAULT;
    A=PBM_A_DEFAULT;
    E=PBM_E_DEFAULT;
    U=PBM_U_DEFAULT;
  };
};

#endif
