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
 
#ifndef _EcModelTypes_h
#define _EcModelTypes_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

//--------------- Constants ------------------------------------------

#define ED_ECM                1
#define ED_ECM_LABEL          "Edit distance based error correcting model"
#define ED_ECM_NOTES          ""
#define ED_ECM_VALID_FOR_WG   0

#define PE_ECM                2
#define PE_ECM_LABEL          "Post edition based error correcting model"
#define PE_ECM_NOTES          ""
#define PE_ECM_VALID_FOR_WG   0

#define PFSM_ECM              3
#define PFSM_ECM_LABEL        "Statistical error correcting model using a probabilistic finite state machine"
#define PFSM_ECM_NOTES        ""
#define PFSM_ECM_VALID_FOR_WG 0

#define IBM1_ECM              4
#define IBM1_ECM_LABEL        "Statistical error correcting model using an IBM 1 Model"
#define IBM1_ECM_NOTES        ""
#define IBM1_ECM_VALID_FOR_WG 0

#define IBM2_ECM              5
#define IBM2_ECM_LABEL        "Statistical error correcting model using an IBM 2 Model"
#define IBM2_ECM_NOTES        "This model is to be combined with the NonPbEcModelForNbUcat class due to its alignment model estimated at sentence level."
#define IBM2_ECM_VALID_FOR_WG 0

#define PFSM_FOR_WG_ECM       6
#define PFSM_FOR_WG_ECM_LABEL "Statistical error correcting model for word-graphs using a probabilistic finite state machine"
#define PFSM_FOR_WG_ECM_NOTES ""
#define PFSM_FOR_WG_ECM_VALID_FOR_WG 1

#define IBM1_FOR_WG_ECM       7
#define IBM1_FOR_WG_ECM_LABEL "Statistical error correcting model for word-graphs using an IBM 1 Model"
#define IBM1_FOR_WG_ECM_NOTES ""
#define IBM1_FOR_WG_ECM_VALID_FOR_WG 1

// Set the error correcting model type used by uncoupled assisted
// translators.  The valid options are: ED_ECM, PE_ECM,
// PFSM_ECM,... These options can be given before executing the
// configure script.
//
// Example:
// $ export THOT_ECM_TYPE="ED_ECM"
// $ configure --prefix=$PWD
// $ make
// $ make install

#if THOT_ECM_TYPE == ED_ECM
# include <EditDistEcm.h>
# define CURR_ECM_TYPE EditDistEcm
# define CURR_ECM_LABEL ED_ECM_LABEL
# define CURR_ECM_NOTES ED_ECM_NOTES
# define CURR_ECM_VALID_FOR_WG ED_ECM_VALID_FOR_WG

#elif THOT_ECM_TYPE == PE_ECM
# include <PostEditEcm.h>
# define CURR_ECM_TYPE PostEditEcm
# define CURR_ECM_LABEL PE_ECM_LABEL
# define CURR_ECM_NOTES PE_ECM_NOTES
# define CURR_ECM_VALID_FOR_WG PE_ECM_VALID_FOR_WG

#elif THOT_ECM_TYPE == PFSM_ECM
# include <PfsmEcm.h>
# define CURR_ECM_TYPE PfsmEcm
# define CURR_ECM_LABEL PFSM_ECM_LABEL
# define CURR_ECM_NOTES PFSM_ECM_NOTES
# define CURR_ECM_VALID_FOR_WG PFSM_ECM_VALID_FOR_WG

#elif THOT_ECM_TYPE == IBM1_ECM
# include <Ibm1Ecm.h>
# define CURR_ECM_TYPE Ibm1Ecm
# define CURR_ECM_LABEL IBM1_ECM_LABEL
# define CURR_ECM_NOTES IBM1_ECM_NOTES
# define CURR_ECM_VALID_FOR_WG IBM1_ECM_VALID_FOR_WG

#elif THOT_ECM_TYPE == IBM2_ECM
# include <Ibm2Ecm.h>
# define CURR_ECM_TYPE Ibm2Ecm
# define CURR_ECM_LABEL IBM2_ECM_LABEL
# define CURR_ECM_NOTES IBM2_ECM_NOTES
# define CURR_ECM_VALID_FOR_WG IBM2_ECM_VALID_FOR_WG

#elif THOT_ECM_TYPE == PFSM_FOR_WG_ECM
# include <PfsmEcmForWg.h>
# define CURR_ECM_TYPE PfsmEcmForWg
# define CURR_ECM_LABEL PFSM_FOR_WG_ECM_LABEL
# define CURR_ECM_NOTES PFSM_FOR_WG_ECM_NOTES
# define CURR_ECM_VALID_FOR_WG PFSM_FOR_WG_ECM_VALID_FOR_WG

#elif THOT_ECM_TYPE == IBM1_FOR_WG_ECM
# include <Ibm1EcmForWg.h>
# define CURR_ECM_TYPE Ibm1EcmForWg
# define CURR_ECM_LABEL IBM1_FOR_WG_ECM_LABEL
# define CURR_ECM_NOTES IBM1_FOR_WG_ECM_NOTES
# define CURR_ECM_VALID_FOR_WG IBM1_FOR_WG_ECM_VALID_FOR_WG

#endif

#endif
