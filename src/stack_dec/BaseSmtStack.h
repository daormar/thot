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
 * @file BaseSmtStack.h
 * 
 * @brief The BaseSmtStack abstract class is a base class for other
 * containers.
 */

#ifndef _BaseSmtStack_h
#define _BaseSmtStack_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- BaseSmtStack template class

/**
 * @brief Base class for other statistical machine translation stack
 * classes.
 */

template<class HYPOTHESIS> 
class BaseSmtStack
{
 public:
  
  virtual void setMaxStackSize(unsigned int _maxStackSize)=0;
  virtual unsigned int getMaxStackSize(void)=0;  
  virtual bool push(const HYPOTHESIS& hyp)=0;
  virtual HYPOTHESIS top(void)=0;
  virtual HYPOTHESIS pop(void)=0;
  virtual HYPOTHESIS last(void)=0;
  virtual void removeLast(void)=0;
  virtual bool empty(void)=0;
  virtual size_t size(void)=0;
  virtual void clear(void)=0;

  virtual ~BaseSmtStack(){};

# ifdef THOT_STATS
  unsigned int discardedPushOpsDueToSize;
  unsigned int discardedPushOpsDueToRec;
# endif

};

#endif
