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
 
#ifndef _SwModelsShTypes_h
#define _SwModelsShTypes_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

//--------------- Constants ------------------------------------------

#define SENT_HANDLER              1
#define SENT_HANDLER_LABEL        "Sentence handler"
#define SENT_HANDLER_NOTES        ""

#define LSENT_HANDLER             2
#define LSENT_HANDLER_LABEL       "Light sentence handler"
#define LSENT_HANDLER_NOTES       ""

// Set the sentence handler type used by different programs.  The valid
// options are: SENT_HANDLER and LSENT_HANDLER. These options can be
// given before executing the configure script.
//
// Example:
// $ export THOT_SH_TYPE="SENT_HANDLER"
// $ configure --prefix=$PWD
// $ make
// $ make install

#if THOT_SH_TYPE == SENT_HANDLER
# define CURR_SH_TYPE SentenceHandler
# define CURR_SH_LABEL SENT_HANDLER_LABEL
# define CURR_SH_NOTES SENT_HANDLER_NOTES
# include <SentenceHandler.h>

#elif THOT_SH_TYPE == LSENT_HANDLER
# define CURR_SH_TYPE LightSentenceHandler
# define CURR_SH_LABEL LSENT_HANDLER_LABEL
# define CURR_SH_NOTES LSENT_HANDLER_NOTES
# include <LightSentenceHandler.h>

#endif

#ifndef THOT_SH_TYPE
# define CURR_SH_TYPE LightSentenceHandler
# define CURR_SH_LABEL LSENT_HANDLER_LABEL
# define CURR_SH_NOTES LSENT_HANDLER_NOTES
# include <LightSentenceHandler.h>
#endif

#endif
