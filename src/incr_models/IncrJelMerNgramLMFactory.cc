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
/* Module: IncrJelMerNgramLMFactory                                 */
/*                                                                  */
/* Definitions file: IncrJelMerNgramLMFactory.cc                    */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "IncrJelMerNgramLM.h"
#include <string>

//--------------- Function definitions

extern "C" BaseNgramLM<std::vector<WordIndex> >* create(std::string /*str*/)
{
  return new IncrJelMerNgramLM;
}

//---------------
extern "C" std::string type_id(void)
{
  return "IncrJelMerNgramLM";
}
