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
/* Module: Cache_lct_                                               */
/*                                                                  */
/* Prototype file: Cache_lct_                                       */
/*                                                                  */
/* Description: Data structure to cache lc(t_) logcounts.           */
/*                                                                  */
/********************************************************************/

#ifndef _Cache_lct_
#define _Cache_lct_

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BasePhraseTable.h"

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------
 

//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------


//--------------- Cache_lct_ class
class Cache_lct_ 
{
 public:

  Cache_lct_(void);
  bool is_valid(void);
  bool t_present(const Vector<WordIndex>& _t_);
  void init(const Vector<WordIndex>& _t_,
            LogCount _lcount_t_);
  LogCount lct_(void);
  void clear(void);

 protected:
  
  bool valid;
  Vector<WordIndex> t_;
  LogCount lcount_t_;
};

#endif
