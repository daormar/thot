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
/* Module: client_server_defs                                       */
/*                                                                  */
/* Prototype file: client_server_defs.h                             */
/*                                                                  */
/* Description: Constants, typedefs and basic classes used in the   */
/*              client-server architecture of the translator.       */
/*                                                                  */
/********************************************************************/

#ifndef _client_server_defs
#define _client_server_defs

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

//--------------- Constants ------------------------------------------

#define DEFAULT_USER_ID           0
#define DEFAULT_SERVER_PORT    4550

#define VERIFY_COV                1
#define TRANSLATE_SENT            2
#define TRANSLATE_SENT_HYPINFO    3
#define OL_TRAIN_PAIR             4
#define TRAIN_ECM                 5
#define START_CAT                 6
#define ADD_STR_TO_PREF           7
#define RESET_PREF                8
#define PRINT_MODELS              9
#define END_CLIENT_DIALOG        10
#define END_SERVER               11

#endif
