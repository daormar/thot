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

/********************************************************************/
/*                                                                  */
/* Module: IncrLevelDbHmmP0AligModel                                */
/*                                                                  */
/* Definitions file: IncrLevelDbHmmP0AligModel.cc                   */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "IncrLevelDbHmmP0AligModel.h"

//--------------- IncrLevelDbHmmP0AligModel class function definitions

//-------------------------
IncrLevelDbHmmP0AligModel::IncrLevelDbHmmP0AligModel() : _incrHmmP0AligModel()
{
      // Create table with lexical parameters
  incrLexTable = new IncrLexLevelDbTable();
  lexNumDenFileExtension = "_leveldb_hmm_lexnd";
}
