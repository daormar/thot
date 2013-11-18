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
 
#ifndef _MultiStackTypes_h
#define _MultiStackTypes_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

//--------------- Constants ------------------------------------------

#define MSTACK                1
#define MSTACK_LABEL          "Multiple stack translator"
#define MSTACK_NOTES          "The value of the S parameter is interpreted as the maximum number of hypotheses that can be stored in each stack."

#define MSTACK_GRAN           2
#define MSTACK_GRAN_LABEL     "Multiple stack translator with granularity"
#define MSTACK_GRAN_NOTES     "The value of the S parameter is interpreted as the maximum number of hypotheses that can be stored in the multiple-stack container."

// Set the model type used by different programs such as
// phrase_mstack_trans, etc.  The valid options are: MSTACK and
// MSTACK_GRAN. These options can be given before executing the
// configure script.
//
// Example:
// $ export THOT_MSTACK_TYPE="MSTACK"
// $ configure --prefix=$PWD
// $ make
// $ make install

#if THOT_MSTACK_TYPE == MSTACK
# ifdef THOT_DISABLE_REC
#  include "multi_stack_decoder.h"
#  define CURR_MSTACK_TYPE multi_stack_decoder
# else
#  include "multi_stack_decoder_rec.h"
#  define CURR_MSTACK_TYPE multi_stack_decoder_rec
# endif
# define CURR_MSTACK_LABEL MSTACK_LABEL
# define CURR_MSTACK_NOTES MSTACK_NOTES

#elif THOT_MSTACK_TYPE == MSTACK_GRAN
# define MULTI_STACK_USE_GRAN
# ifdef THOT_DISABLE_REC
#  include "multi_stack_decoder_gran.h"
#  define CURR_MSTACK_TYPE multi_stack_decoder_gran
# else
#  include "multi_stack_decoder_gran_rec.h"
#  define CURR_MSTACK_TYPE multi_stack_decoder_gran_rec
# endif
# define CURR_MSTACK_LABEL MSTACK_GRAN_LABEL
# define CURR_MSTACK_NOTES MSTACK_GRAN_NOTES

#endif

#endif
