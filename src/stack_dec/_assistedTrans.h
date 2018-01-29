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
 * @file _assistedTrans.h
 *
 * @brief Declares the _assistedTrans abstract template class, this is a
 * predecessor class useful to derive new classes that implement
 * assisted translators.
 */

#ifndef __assistedTrans_h
#define __assistedTrans_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseAssistedTrans.h"

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------


//--------------- _assistedTrans template class: interface for
//                assisted translators

/**
 * @brief The _assistedTrans abstract template class is a predecessor
 * class useful to derive new classes that implement assisted
 * translators.
 */

template<class SMT_MODEL>
class _assistedTrans: public BaseAssistedTrans<SMT_MODEL>
{
 public:

      // Constructor
  _assistedTrans(void);
  

 protected:

};

//--------------- _assistedTrans template class method definitions

//---------------------------------------
template<class SMT_MODEL>
_assistedTrans<SMT_MODEL>::_assistedTrans(void)
{
}

#endif
