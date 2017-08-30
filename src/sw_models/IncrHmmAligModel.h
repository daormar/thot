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
/* Module: IncrHmmAligModel                                         */
/*                                                                  */
/* Prototype file: IncrHmmAligModel.h                               */
/*                                                                  */
/* Description: Defines the IncrHmmAligModel class.                 */
/*              IncrHmmAligModel class allows to generate and       */
/*              access to the data of an HMM statistical            */
/*              alignment model.                                    */
/*                                                                  */
/* Notes: 100% AC-DC powered                                        */
/*                                                                  */
/********************************************************************/

#ifndef _IncrHmmAligModel_h
#define _IncrHmmAligModel_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "_incrHmmAligModel.h"
#include "IncrLexTable.h"

//--------------- Constants ------------------------------------------

//--------------- typedefs -------------------------------------------

//--------------- function declarations ------------------------------

//--------------- Classes --------------------------------------------

//--------------- IncrHmmAligModel class

class IncrHmmAligModel : public _incrHmmAligModel
{
  public:
  
   // Constructor
   IncrHmmAligModel();

};

#endif
