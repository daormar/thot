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
 * @file multi_stack_decoder_rec__swli_factory.cc
 * 
 * @brief Factory for multi_stack_decoder_rec objects.
 */

//--------------- Include files --------------------------------------

#include "PhrLocalSwLiTm.h"
#include "multi_stack_decoder_rec.h"
#include <string>

//--------------- Function definitions

extern "C" BaseStackDecoder<PhrLocalSwLiTm>* create(const char* /*str*/)
{
  return new multi_stack_decoder_rec<PhrLocalSwLiTm>;
}

//---------------
extern "C" const char* type_id(void)
{
  return "multi_stack_decoder_rec<PhrLocalSwLiTm>";
}
