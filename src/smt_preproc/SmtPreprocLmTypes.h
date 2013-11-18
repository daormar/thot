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
 * @file SmtPreprocLmTypes.h
 * 
 * @brief Include files, typedefs and function definitions needed for
 * simplifying the work with statistical language models.
 */

#ifndef _SmtPreprocLmTypes_h
#define _SmtPreprocLmTypes_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

//--------------- Constants ------------------------------------------

#define THOT_BACKOFF_LM               1
// BACKOFF_LM_LABEL y BACKOFF_LM_NOTES are not necessary since the
// corresponding library already defines a label and notes for this
// model

#define THOT_INTERP_LM                2
#define THOT_INTERP_LM_LABEL          "Interpolated n-gram language model"
#define THOT_INTERP_LM_NOTES          ""

#define THOT_INCR_INTERP_LM           3
#define THOT_INCR_INTERP_LM_LABEL     "Incremental interpolated n-gram language model"
#define THOT_INCR_INTERP_LM_NOTES     ""

// Set the language model used by different statistical machine
// translation models The valid options are: THOT_BACKOFF_LM,
// THOT_INTERP_LM... These options can be given before executing
// the configure script.
//
// Example:
// $ export THOT_LM_TYPE="THOT_BACKOFF_LM"
// $ configure --prefix=$PWD
// $ make
// $ make install

// include files and typedefs
#if THOT_LM_TYPE == THOT_BACKOFF_LM
#include <ArpaLmTypes.h>
#define THOT_CURR_LM_TYPE ARPA_LM_CURR_LM_TYPE
#define THOT_CURR_LM_LABEL ARPA_LM_CURR_LM_LABEL
#define THOT_CURR_LM_NOTES ARPA_LM_CURR_LM_NOTES

#elif THOT_LM_TYPE == THOT_INTERP_LM
#include <lt_op_vec.h> // provides an ordering relationship for vectors
#include <InterpONgramLM.h>
#define THOT_CURR_LM_TYPE InterpONgramLM
#define THOT_CURR_LM_LABEL INTERP_LM_LABEL
#define THOT_CURR_LM_NOTES INTERP_LM_NOTES

#elif THOT_LM_TYPE == THOT_INCR_INTERP_LM
#include <lt_op_vec.h> // provides an ordering relationship for vectors
#include <IncrInterpONgramLM.h>
#define THOT_CURR_LM_TYPE IncrInterpONgramLM
#define THOT_CURR_LM_LABEL THOT_INCR_INTERP_LM_LABEL
#define THOT_CURR_LM_NOTES THOT_INCR_INTERP_LM_NOTES

#endif

#endif
