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
 
#ifndef _AssistedTransTypes_h
#define _AssistedTransTypes_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

//--------------- Constants ------------------------------------------

#define NB_UNCTRANS               1
#define NB_UNCTRANS_LABEL         "N-best uncoupled assisted translator"
#define NB_UNCTRANS_NOTES         ""

#define WG_UNCTRANS               2
#define WG_UNCTRANS_LABEL         "Uncoupled assisted translator based on word-graphs"
#define WG_UNCTRANS_NOTES         ""

#define SM_COUPTRANS              3
#define SM_COUPTRANS_LABEL        "Coupled assisted translator based on smoothing techniques"
#define SM_COUPTRANS_NOTES        ""

// Set the assisted translation type used by stack_trans_server.  The
// valid options are: NB_UNCTRANS,WG_UNCTRANS,... These options can
// be given before executing the configure script.
//
// Example:
// $ export THOT_AT_TYPE="NB_UNCTRANS"
// $ configure --prefix=$PWD
// $ make
// $ make install

#if THOT_AT_TYPE == NB_UNCTRANS
# include <NbUncoupledAssistedTrans.h>
# define CURR_AT_TYPE NbUncoupledAssistedTrans
# define CURR_AT_LABEL NB_UNCTRANS_LABEL
# define CURR_AT_NOTES NB_UNCTRANS_NOTES

#elif THOT_AT_TYPE == WG_UNCTRANS
# include <WgUncoupledAssistedTrans.h>
# define CURR_AT_TYPE WgUncoupledAssistedTrans
# define CURR_AT_LABEL WG_UNCTRANS_LABEL
# define CURR_AT_NOTES WG_UNCTRANS_NOTES

#elif THOT_AT_TYPE == SM_COUPTRANS
# include <SmCoupledAssistedTrans.h>
# define CURR_AT_TYPE SmCoupledAssistedTrans
# define CURR_AT_LABEL SM_COUPTRANS_LABEL
# define CURR_AT_NOTES SM_COUPTRANS_NOTES
#endif

#endif
