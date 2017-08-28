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

/********************************************************************/
/*                                                                  */
/* Module: IncrLevelDbHmmP0AligModel                                */
/*                                                                  */
/* Prototype file: IncrLevelDbHmmP0AligModel.h                      */
/*                                                                  */
/* Description: Defines the IncrLevelDbHmmP0AligModel class.        */
/*              IncrLevelDbHmmP0AligModel class allows to generate  */
/*              and access to the data of a HMM statistical         */
/*              alignment model with fixed p0 probability.          */
/*              Model uses LevelDB as a back-end data structure.    */
/*                                                                  */
/********************************************************************/

#ifndef _IncrLevelDbHmmP0AligModel_h
#define _IncrLevelDbHmmP0AligModel_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "_incrHmmP0AligModel.h"
#include "IncrLevelDbHmmAligModel.h"

//--------------- Constants ------------------------------------------

//--------------- typedefs -------------------------------------------

//--------------- function declarations ------------------------------

//--------------- Classes --------------------------------------------

//--------------- IncrLevelDbHmmP0AligModel class

class IncrLevelDbHmmP0AligModel : public _incrHmmP0AligModel<IncrLevelDbHmmAligModel>
{
  public:

      // Constructor
   IncrLevelDbHmmP0AligModel():_incrHmmP0AligModel<IncrLevelDbHmmAligModel>() {};
};

#endif
