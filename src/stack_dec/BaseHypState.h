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
 * @file BaseHypState.h
 * 
 * @brief Declares the BaseHypState class is an abstract class for
 * implementing the state of the hypotheses.
 */

#ifndef _BaseHypState_h
#define _BaseHypState_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */



//--------------- Classes --------------------------------------------

/**
 * @brief The BaseHypState class is an abstract class for implementing
 * the state of the hypotheses.
 */

class BaseHypState
{
  public:

       // Note: Derived classes must define the "less" operator: operator<
      
       // Destructor
   virtual ~BaseHypState()=0;
};

#endif
