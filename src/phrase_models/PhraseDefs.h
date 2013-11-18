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
/* Module: PhraseDefs                                               */
/*                                                                  */
/* Prototype file: PhraseDefs                                       */
/*                                                                  */
/* Description: Constants, typedefs and basic classes used in the   */
/*              phrase-model classes.                               */
/*                                                                  */
/********************************************************************/

#ifndef _PhraseDefs
#define _PhraseDefs

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "StatModelDefs.h"
#include "SentSegmentation.h"
#include "VecUnsignedIntSortCriterion.h"
#include "PhraseSortCriterion.h"
#include "PhraseId.h"

//--------------- Constants ------------------------------------------

#define PHRASE_PROB_SMOOTH       1e-10
#define LOG_PHRASE_PROB_SMOOTH   log(PHRASE_PROB_SMOOTH)
#define SEGM_SIZE_PROB_SMOOTH    1e-7
#define MAX_SENTENCE_LENGTH    101

//--------------- typedefs -------------------------------------------

typedef off_t PhrIndex;

#endif
