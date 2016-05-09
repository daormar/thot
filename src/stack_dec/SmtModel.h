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
 
#ifndef _CurrSmtModel_h
#define _CurrSmtModel_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "PhrLocalSwLiTm.h"
#include "WordIndex.h"
#include "myVector.h"

//--------------- Constants ------------------------------------------

// Set the SMT model type used by different programs such as
// thot_ms_dec, thot_ms_alig, thot_server, etc. 

#define SMT_MODEL_TYPE_NAME "PhrLocalSwLiTm"
#define SMT_MODEL_DESC      "Lm + phrase-based model and local sw model linearly-interpolated"

//--------------- User defined types ---------------------------------

typedef PhrLocalSwLiTm SmtModel;

#endif
