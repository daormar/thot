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
/* Module: _smtStack                                                */
/*                                                                  */
/* Prototypes file: _smtStack.h                                     */
/*                                                                  */
/* Description: The _smtStack class is a predecessor class for      */
/*              implementing a stack to be used in stack            */
/*              decoding.                                           */
/*                                                                  */
/********************************************************************/

/**
 * @file _smtStack.h
 *
 * @brief The _smtStack class is a predecessor class for implementing a
 * stack to be used in stack decoding.
 */

#ifndef __smtStack_h
#define __smtStack_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <set>
#include "BaseSmtStack.h"
#include "HypSortCriterion.h"

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- _smtStack template class

/**
 * @brief A predecessor class for implementing a stack to be used in
 * stack decoding.
 */
template<class HYPOTHESIS> 
class _smtStack: public BaseSmtStack<HYPOTHESIS>
{
 public:

  typedef std::multiset<HYPOTHESIS,HypSortCriterion<HYPOTHESIS> > Container;

      // constructor
  _smtStack(void);

      // stack size related functions
  void setMaxStackSize(unsigned int _maxStackSize);
  unsigned int getMaxStackSize(void);

      // basic functionality
  HYPOTHESIS top(void);
  HYPOTHESIS last(void);
  bool empty(void);
  size_t size(void);
  void clear(void);

#ifdef THOT_STATS
  unsigned int numDiscardedPushOpsDueToSize(void);
#endif
  
 protected:

  unsigned int maxStackSize;
  Container container;

      // auxiliary functions
  void truncateQueue(unsigned int maxNumOfHyps);
};

//--------------- _smtStack template class function definitions

template<class HYPOTHESIS> 
_smtStack<HYPOTHESIS>::_smtStack(void)
{
# ifdef THOT_STATS
  this->discardedPushOpsDueToSize=0;
  this->discardedPushOpsDueToRec=0;
# endif

  setMaxStackSize(1024);
}

//---------------------------------------
template<class HYPOTHESIS> 
void _smtStack<HYPOTHESIS>::setMaxStackSize(unsigned int _maxStackSize)
{
  maxStackSize=_maxStackSize;
}

//---------------------------------------
template<class HYPOTHESIS> 
unsigned int _smtStack<HYPOTHESIS>::getMaxStackSize(void)
{
  return maxStackSize;
}

//---------------------------------------
template<class HYPOTHESIS> 
HYPOTHESIS _smtStack<HYPOTHESIS>::top(void)
{
  return *container.begin();
}

//---------------------------------------
template<class HYPOTHESIS> 
HYPOTHESIS _smtStack<HYPOTHESIS>::last(void)
{
  typename Container::iterator pos;

  pos=container.end();
  --pos;
  return *pos;
}

//---------------------------------------
template<class HYPOTHESIS> 
bool _smtStack<HYPOTHESIS>::empty(void)
{
  if(container.size()==0) return true;
  else return false;
}

//---------------------------------------
template<class HYPOTHESIS> 
size_t _smtStack<HYPOTHESIS>::size(void)
{
  return container.size();  
}

//---------------------------------------
template<class HYPOTHESIS> 
void _smtStack<HYPOTHESIS>::clear(void)
{
# ifdef THOT_STATS
  this->discardedPushOpsDueToSize=0;
  this->discardedPushOpsDueToRec=0;
# endif

  container.clear();
}

//---------------------------------------
template<class HYPOTHESIS> 
void _smtStack<HYPOTHESIS>::truncateQueue(unsigned int /*maxNumOfHyps*/)
{
  typename Container::iterator pos;  
	
  if(!container.empty())
  {
    this->removeLast();
  }
}

#endif
