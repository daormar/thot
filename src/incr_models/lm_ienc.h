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
/* Module: lm_ienc                                                  */
/*                                                                  */
/* Prototype file: lm_ienc                                          */
/*                                                                  */
/* Description: Encoder for incremental language models.            */
/*                                                                  */
/********************************************************************/

#ifndef _lm_ienc
#define _lm_ienc

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "vecx_x_incr_enc.h"
#include <LM_Defs.h>
#include <string>

using namespace std;

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------

//--------------- lm_ienc class

class lm_ienc: public vecx_x_incr_enc<std::string,WordIndex>
{
  public:

       // Constructor
   lm_ienc();

};

#endif
