/*
thot package for statistical machine translation
Copyright (C) 2013-2017 Daniel Ortiz-Mart\'inez, Adam Harasimowicz

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
 * @file IncrHmmP0AligModel.h
 * 
 * @brief Defines the IncrHmmP0AligModel class.  IncrHmmP0AligModel
 * class allows to generate and access to the data of a Hmm statistical
 * alignment model with fixed p0 probability.
 */

#ifndef _IncrHmmP0AligModel_h
#define _IncrHmmP0AligModel_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "_incrHmmP0AligModel.h"
#include "IncrLexTable.h"

//--------------- Constants ------------------------------------------

//--------------- typedefs -------------------------------------------

//--------------- function declarations ------------------------------

//--------------- Classes --------------------------------------------

//--------------- IncrHmmP0AligModel class

class IncrHmmP0AligModel : public _incrHmmP0AligModel
{
  public:

      // Constructor
   IncrHmmP0AligModel();
};

#endif
