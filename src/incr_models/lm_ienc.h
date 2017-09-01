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

   bool HighSrc_to_Src(const std::vector<std::string>& hs,
                       std::vector<WordIndex>& s);
       // Given a HSRCDATA object "hs" obtains its corresponding encoded
       // value in "s". Returns true if the encoding was successful
       // ("hs" exists in the vocabulary).  s stores the corresponding
       // code if exists, or a not valid one otherwise
   bool HighTrg_to_Trg(const std::string& ht,WordIndex& t);
       // The same for HX objects

};

#endif
