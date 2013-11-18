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
/* Module: Cache_ct_                                                */
/*                                                                  */
/* Prototype file: Cache_ct_                                        */
/*                                                                  */
/* Description: Data structure to cache c(t_) counts.               */
/*                                                                  */
/********************************************************************/

#ifndef _Cache_ct_
#define _Cache_ct_

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BasePhraseTable.h"

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------
 

//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------


//--------------- Cache_ct_ class
class Cache_ct_ 
{
 public:

  Cache_ct_(void);
  bool is_valid(void);
  bool t_present(const Vector<WordIndex>& _t_);
  void init(const Vector<WordIndex>& _t_,
            Count _count_t_);
  Count ct_(void);
  void clear(void);

 protected:
  
  bool valid;
  Vector<WordIndex> t_;
  Count count_t_;
};

#endif
