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
 * @file BaseSmtMultiStack.h
 * 
 * @brief The BaseSmtMultiStack abstract class is a base class for other
 * container types that use multiple stacks.
 */

#ifndef _BaseSmtMultiStack_h
#define _BaseSmtMultiStack_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseSmtStack.h"

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- BaseSmtMultiStack template class

/**
 * @brief Base class for other container types that use multiple stacks.
 */

template<class HYPOTHESIS_REC> 
class BaseSmtMultiStack: public BaseSmtStack<HYPOTHESIS_REC>
{
 public:

  virtual void set_bf(bool _breadthFirst)=0;
};

#endif
