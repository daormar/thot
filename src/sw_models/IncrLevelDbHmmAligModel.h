/*
thot package for statistical machine translation
Copyright (C) 2017 Adam Harasimowicz

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
 * @file IncrLevelDbHmmAligModel.h
 * 
 * @brief Defines the IncrLevelDbHmmAligModel class.
 * IncrLevelDbHmmAligModel class allows to generate and access the data
 * of an HMM statistical alignment model which uses LevelDb as a table
 * lexical parameters.
 * 
 */

#ifndef _IncrLevelDbHmmAligModel_h
#define _IncrLevelDbHmmAligModel_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "_incrHmmAligModel.h"
#include <IncrLexLevelDbTable.h>

//--------------- Constants ------------------------------------------

//--------------- typedefs -------------------------------------------

//--------------- function declarations ------------------------------

//--------------- Classes --------------------------------------------

//--------------- IncrLevelDbHmmAligModel class

class IncrLevelDbHmmAligModel : public _incrHmmAligModel
{
  public:

   // Constructor
   IncrLevelDbHmmAligModel();

   // Thread safety related functions
   bool modelReadsAreProcessSafe(void);

   // init function
   bool init(const char* prefFileName);

};

#endif
