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
 * @file IncrJelMerNgramLM.h
 * 
 * @brief Class to manage encoded incremental Jelinek-Mercer ngram
 * language models p(x|vector<x>).
 */

#ifndef _IncrJelMerNgramLM
#define _IncrJelMerNgramLM

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "_incrJelMerNgramLM.h"

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------

//--------------- IncrJelMerNgramLM class

class IncrJelMerNgramLM: public _incrJelMerNgramLM<Count,Count>
{
 public:

  typedef _incrJelMerNgramLM<Count,Count>::SrcTableNode SrcTableNode;
  typedef _incrJelMerNgramLM<Count,Count>::TrgTableNode TrgTableNode;

      // Constructor
  IncrJelMerNgramLM():_incrJelMerNgramLM<Count,Count>()
    {
          // Set new pointer to table
      this->tablePtr=new vecx_x_incr_cptable<WordIndex,Count,Count>;
    }


      // Destructor
  ~IncrJelMerNgramLM();
   
 protected:
};

//---------------


#endif
