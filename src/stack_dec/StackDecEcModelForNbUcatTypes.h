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
 
#ifndef _EcModelForNbUcatTypes_h
#define _EcModelForNbUcatTypes_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

//--------------- Constants ------------------------------------------

#define NONPB_ECM_NB_UCAT        1
#define NONPB_ECM_NB_UCAT_LABEL  "Non phrase-based error correcting model for n-best uncoupled cat"
#define NONPB_ECM_NB_UCAT_NOTES  ""

#define EXHPB_ECM_NB_UCAT        2
#define EXHPB_ECM_NB_UCAT_LABEL  "Exhaustive phrase-based error correcting model for n-best uncoupled cat"
#define EXHPB_ECM_NB_UCAT_NOTES  ""

#define RA_ECM_NB_UCAT           3
#define RA_ECM_NB_UCAT_LABEL     "Error correcting model based on recursive alignments for n-best uncoupled cat"
#define RA_ECM_NB_UCAT_NOTES     ""

// Set the error correcting model type used by uncoupled assisted
// translators.  The valid options are: NONPB_ECM_NB_UCAT, EXHPB_ECM_NB_UCAT,
// RA_ECM_NB_UCAT,... These options can be given before executing the
// configure script.
//
// Example:
// $ export THOT_ECM_NB_UCAT_TYPE="NONPB_ECM_NB_UCAT"
// $ configure --prefix=$PWD
// $ make
// $ make install

#if THOT_ECM_NB_UCAT_TYPE == NONPB_ECM_NB_UCAT
# include <NonPbEcModelForNbUcat.h>
# define CURR_ECM_NB_UCAT_TYPE NonPbEcModelForNbUcat
# define CURR_ECM_NB_UCAT_LABEL NONPB_ECM_NB_UCAT_LABEL
# define CURR_ECM_NB_UCAT_NOTES NONPB_ECM_NB_UCAT_NOTES
#elif THOT_ECM_NB_UCAT_TYPE == EXHPB_ECM_NB_UCAT
# include <ExhPbEcModelForNbUcat.h>
# define CURR_ECM_NB_UCAT_TYPE ExhPbEcModelForNbUcat
# define CURR_ECM_NB_UCAT_LABEL EXHPB_ECM_NB_UCAT_LABEL
# define CURR_ECM_NB_UCAT_NOTES EXHPB_ECM_NB_UCAT_NOTES
#elif THOT_ECM_NB_UCAT_TYPE == RA_ECM_NB_UCAT
# include <RecAlignEcModelForNbUcat.h>
# define CURR_ECM_NB_UCAT_TYPE RecAlignEcModelForNbUcat
# define CURR_ECM_NB_UCAT_LABEL RA_ECM_NB_UCAT_LABEL
# define CURR_ECM_NB_UCAT_NOTES RA_ECM_NB_UCAT_NOTES
#endif

#endif
