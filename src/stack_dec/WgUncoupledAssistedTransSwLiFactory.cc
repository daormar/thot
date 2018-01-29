/*
error_correction package for statistical machine translation
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
 * @file WgUncoupledAssistedTransSwLiFactory.cc
 * 
 * @brief Factory for WgUncoupledAssistedTransSwLi objects.
 */

//--------------- Include files --------------------------------------

#include "PhrLocalSwLiTm.h"
#include "WgUncoupledAssistedTrans.h"
#include <string>

//--------------- Function definitions

extern "C" BaseAssistedTrans<PhrLocalSwLiTm>* create(const char* /*str*/)
{
  return new WgUncoupledAssistedTrans<PhrLocalSwLiTm>;
}

//---------------
extern "C" const char* type_id(void)
{
  return "WgUncoupledAssistedTrans<PhrLocalSwLiTm>";
}
