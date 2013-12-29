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
 * @file StackDecLmTypes.h
 * 
 * @brief Include files, typedefs and function definitions needed for
 * simplifying the work with statistical language models.
 */

#ifndef _StackDecLmTypes_h
#define _StackDecLmTypes_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

//--------------- Constants ------------------------------------------

#define THOT_BACKOFF_LM               1
// THOT_BACKOFF_LM_LABEL and THOT_BACKOFF_LM_NOTES are not
// necessary since the corresponding library already defines a label and
// notes for this model

#define THOT_INCR_INTERP_LM           2
#define THOT_INCR_INTERP_LM_LABEL     "Interpolation of incremental n-gram language models"
#define THOT_INCR_INTERP_LM_NOTES     ""

#define THOT_INCR_INTERPO_LM          3 // This constant is
                                             // temporarily defined, the
                                             // correct name should be
                                             // THOT_INCR_JEL_MER_LM

#define THOT_INCR_JEL_MER_LM          3
#define THOT_INCR_JEL_MER_LM_LABEL    "Incremental Jelinek-Mercer n-gram language model"
#define THOT_INCR_JEL_MER_LM_NOTES    ""

#define THOT_CINCR_JEL_MER_LM         4
#define THOT_CINCR_JEL_MER_LM_LABEL   "Cached incremental jelinek-mercer n-gram language model"
#define THOT_CINCR_JEL_MER_LM_NOTES   ""

#define THOT_SWISE_JEL_MER_LM         5
#define THOT_SWISE_JEL_MER_LM_LABEL   "Stepwise interpolated-order n-gram language model"
#define THOT_SWISE_JEL_MER_LM_NOTES   ""

#define THOT_INCR_KN_LM               6
#define THOT_INCR_KN_LM_LABEL         "Incremental Kneser-Ney n-gram language model"
#define THOT_INCR_KN_LM_NOTES         ""

// Set the language model used by different statistical machine
// translation models. The valid options are: THOT_BACKOFF_LM,
// THOT_JEL_MER_LM... These options can be given before executing the
// configure script.
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
typedef THOT_CURR_LM_TYPE::LM_State LM_State;

#elif THOT_LM_TYPE == THOT_INCR_INTERP_LM
#include <lt_op_vec.h> // provides an ordering relationship for vectors
#include <IncrInterpNgramLM.h>
#define THOT_CURR_LM_TYPE IncrInterpNgramLM
#define THOT_CURR_LM_LABEL THOT_INCR_INTERP_LM_LABEL
#define THOT_CURR_LM_NOTES THOT_INCR_INTERP_LM_NOTES
typedef THOT_CURR_LM_TYPE::LM_State LM_State;

#elif THOT_LM_TYPE == THOT_INCR_JEL_MER_LM
#include <lt_op_vec.h> // provides an ordering relationship for vectors
#include <IncrJelMerNgramLM.h>
#define THOT_CURR_LM_TYPE IncrJelMerNgramLM
#define THOT_CURR_LM_LABEL THOT_INCR_JEL_MER_LM_LABEL
#define THOT_CURR_LM_NOTES THOT_INCR_JEL_MER_LM_NOTES
typedef THOT_CURR_LM_TYPE::LM_State LM_State;

#elif THOT_LM_TYPE == THOT_CINCR_JEL_MER_LM
#include <lt_op_vec.h> // provides an ordering relationship for vectors
#include <CacheIncrJelMerNgramLM.h>
#define THOT_CURR_LM_TYPE CacheIncrJelMerNgramLM
#define THOT_CURR_LM_LABEL THOT_CINCR_JEL_MER_LM_LABEL
#define THOT_CURR_LM_NOTES THOT_CINCR_JEL_MER_LM_NOTES
typedef THOT_CURR_LM_TYPE::LM_State LM_State;

#elif THOT_LM_TYPE == THOT_SWISE_JEL_MER_LM
#include <lt_op_vec.h> // provides an ordering relationship for vectors
#include <StepwiseJelMerNgramLM.h>
#define THOT_CURR_LM_TYPE StepwiseJelMerNgramLM
#define THOT_CURR_LM_LABEL THOT_SWISE_JEL_MER_LM_LABEL
#define THOT_CURR_LM_NOTES THOT_SWISE_JEL_MER_LM_NOTES
typedef THOT_CURR_LM_TYPE::LM_State LM_State;

#elif THOT_LM_TYPE == THOT_INCR_KN_LM
#include <lt_op_vec.h> // provides an ordering relationship for vectors
#include <IncrKnNgramLM.h>
#define THOT_CURR_LM_TYPE IncrKnNgramLM
#define THOT_CURR_LM_LABEL THOT_INCR_KN_LM_LABEL
#define THOT_CURR_LM_NOTES THOT_INCR_KN_LM_NOTES
typedef THOT_CURR_LM_TYPE::LM_State LM_State;

#endif

#endif
