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
/* Module: NbSearchStack                                            */
/*                                                                  */
/* Prototypes file: NbSearchStack.h                                 */
/*                                                                  */
/* Description: The NbSearchStack class implements a stack to be    */
/*              used when obtaining n-best lists for a given        */
/*              wordgraph.                                          */
/*                                                                  */
/********************************************************************/

/**
 * @file NbSearchStack.h
 *
 * @brief Declares the NbSearchStack class. This class implements a
 * stack to be used when obtaining n-best lists for a given wordgraph.
 */

#ifndef _NbSearchStack_h
#define _NbSearchStack_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "Score.h"
#include "NbSearchHyp.h"
#include <map>

//--------------- Constants ------------------------------------------

#define UNLIMITED_MAX_NBSTACK_SIZE -1

//--------------- Classes --------------------------------------------

//--------------- NbSearchStack template class

/**
 * @brief The NbSearchStack class implements a word graph for being
 * used in stack decoding.
 */

class NbSearchStack
{
 public:

      // Constructor
  NbSearchStack(void);

      // Basic functions
  void push(Score scr,
            NbSearchHyp nbsHyp);
  std::pair<Score,NbSearchHyp> top(void);
  void pop(void);
  
      // setMaxStackSize() function
  void setMaxStackSize(int _maxStackSize);
  
      // empty(), size() and clear() functions
  bool empty(void);
  std::size_t size(void);
  void clear(void);
  
      // Destructor
  ~NbSearchStack();
  
 protected:

  typedef std::multimap<Score,NbSearchHyp,std::greater<Score> > Container;
  Container container;

  int maxStackSize;

      // Auxiliary functions
  void removeLast(void);
};

#endif
