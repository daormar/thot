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
 * @file PhrHypEqClassF.h
 * 
 * @brief Defines the PhrHypEqClassF class, this class is a function
 * object that for each hypothesis returns its equivalence class.
 */

#ifndef _PhrHypEqClassF_h
#define _PhrHypEqClassF_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "PhrHypData.h"
#include <uiHashF.h>

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- PhrHypEqClassF class

/**
 * @brief The PhrHypEqClassF class is a function object that for each
 * hypothesis returns its equivalence class.
 */

class PhrHypEqClassF
{
 public:
  typedef unsigned int EqClassType;
  typedef uiHashF EqClassTypeHashF;
    
  EqClassType operator()(const PhrHypData& pbtHypData);
};

#endif
