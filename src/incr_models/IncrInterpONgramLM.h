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
/* Module: IncrInterpONgramLM                                       */
/*                                                                  */
/* Prototype file: IncrInterpONgramLM.h                             */
/*                                                                  */
/* Description: Class to manage encoded incremental                 */
/*              interpolated-order ngram language                   */
/*              models p(x|Vector<x>).                              */
/*                                                                  */
/********************************************************************/

#ifndef _IncrInterpONgramLM
#define _IncrInterpONgramLM

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "_incrInterpONgramLM.h"

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------

//--------------- IncrInterpONgramLM class

class IncrInterpONgramLM: public _incrInterpONgramLM<Count,Count>
{
 public:

  typedef _incrInterpONgramLM<Count,Count>::SrcTableNode SrcTableNode;
  typedef _incrInterpONgramLM<Count,Count>::TrgTableNode TrgTableNode;

      // Constructor
  IncrInterpONgramLM():_incrInterpONgramLM<Count,Count>()
    {
          // Set new pointer to table
      this->tablePtr=new vecx_x_incr_cptable<WordIndex,Count,Count>;
    }


      // Destructor
  ~IncrInterpONgramLM();
   
 protected:
};

//---------------


#endif
