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
 
#ifndef _StackDecSwModelTypes_h
#define _StackDecSwModelTypes_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

//--------------- Constants ------------------------------------------

#define THOT_MIX_IBM1_SWM             1
#define THOT_MIX_IBM1_SWM_LABEL       "Mixture of IBM 1 single-word models"
#define THOT_MIX_IBM1_SWM_NOTES       ""

#define THOT_MIX_IBM2_SWM             2
#define THOT_MIX_IBM2_SWM_LABEL       "Mixture of IBM 2 single-word models"
#define THOT_MIX_IBM2_SWM_NOTES       ""

#define THOT_SINCR_IBM1_SWM           3
#define THOT_SINCR_IBM1_SWM_LABEL     "Smoothed incremental IBM 1 single-word model"
#define THOT_SINCR_IBM1_SWM_NOTES     ""

#define THOT_SSWISE_IBM1_SWM          4
#define THOT_SSWISE_IBM1_SWM_LABEL    "Smoothed stepwise IBM 1 single-word model"
#define THOT_SSWISE_IBM1_SWM_NOTES    ""

#define THOT_SINCR_IBM2_SWM           5
#define THOT_SINCR_IBM2_SWM_LABEL     "Smoothed incremental IBM 2 single-word model"
#define THOT_SINCR_IBM2_SWM_NOTES     ""

#define THOT_SSWISE_IBM2_SWM          6
#define THOT_SSWISE_IBM2_SWM_LABEL    "Smoothed stepwise IBM 2 single-word model"
#define THOT_SSWISE_IBM2_SWM_NOTES    ""

#define THOT_INCR_HMM_SWM             7
#define THOT_INCR_HMM_SWM_LABEL       "Incremental HMM single-word model"
#define THOT_INCR_HMM_SWM_NOTES       ""

#define THOT_INCR_HOM_HMM_SWM         8
#define THOT_INCR_HOM_HMM_SWM_LABEL   "Incremental homogeneus HMM single-word model"
#define THOT_INCR_HOM_HMM_SWM_NOTES   ""

#define THOT_INCR_HMM_P0_SWM          9
#define THOT_INCR_HMM_P0_SWM_LABEL    "Incremental Hmm-based with fixed P0 single-word model"
#define THOT_INCR_HMM_P0_SWM_NOTES    ""

#define THOT_SWISE_HMM_P0_SWM        10
#define THOT_SWISE_HMM_P0_SWM_LABEL   "Stepwise Hmm-based with fixed P0 single-word model"
#define THOT_SWISE_HMM_P0_SWM_NOTES   ""

// Set the model type used by different programs such as
// phrase_stack_trans, phrase_mstack_trans, phrase_aligner, etc.  The
// valid options are: THOT_MIX_IBM1_SWM,... These options
// can be given before executing the configure script.
//
// Example:
// $ export THOT_SWM_TYPE="THOT_MIX_IBM1_SWM"
// $ configure --prefix=$PWD
// $ make
// $ make install

#if THOT_SWM_TYPE == THOT_MIX_IBM1_SWM
# include <MixIbm1AligModel.h>
# define CURR_SWM_TYPE MixIbm1AligModel
# define CURR_SWM_LABEL THOT_MIX_IBM1_SWM_LABEL
# define CURR_SWM_NOTES THOT_MIX_IBM1_SWM_NOTES
typedef CURR_SWM_TYPE::PpInfo PpInfo;

#elif THOT_SWM_TYPE == THOT_MIX_IBM2_SWM
# include <MixIbm2AligModel.h>
# define CURR_SWM_TYPE MixIbm2AligModel
# define CURR_SWM_LABEL THOT_MIX_IBM2_SWM_LABEL
# define CURR_SWM_NOTES THOT_MIX_IBM2_SWM_NOTES
typedef CURR_SWM_TYPE::PpInfo PpInfo;

#elif THOT_SWM_TYPE == THOT_SINCR_IBM1_SWM
# include <SmoothedIncrIbm1AligModel.h>
# define CURR_SWM_TYPE SmoothedIncrIbm1AligModel
# define CURR_SWM_LABEL THOT_SINCR_IBM1_SWM_LABEL
# define CURR_SWM_NOTES THOT_SINCR_IBM1_SWM_NOTES
typedef CURR_SWM_TYPE::PpInfo PpInfo;

#elif THOT_SWM_TYPE == THOT_SSWISE_IBM1_SWM
# include <SmStepwiseIbm1AligModel.h>
# define CURR_SWM_TYPE SmStepwiseIbm1AligModel
# define CURR_SWM_LABEL THOT_SSWISE_IBM1_SWM_LABEL
# define CURR_SWM_NOTES THOT_SSWISE_IBM1_SWM_NOTES
typedef CURR_SWM_TYPE::PpInfo PpInfo;

#elif THOT_SWM_TYPE == THOT_SINCR_IBM2_SWM
# include <SmoothedIncrIbm2AligModel.h>
# define CURR_SWM_TYPE SmoothedIncrIbm2AligModel
# define CURR_SWM_LABEL THOT_SINCR_IBM2_SWM_LABEL
# define CURR_SWM_NOTES THOT_SINCR_IBM2_SWM_NOTES
typedef CURR_SWM_TYPE::PpInfo PpInfo;

#elif THOT_SWM_TYPE == THOT_SSWISE_IBM2_SWM
# include <SmStepwiseIbm2AligModel.h>
# define CURR_SWM_TYPE SmStepwiseIbm2AligModel
# define CURR_SWM_LABEL THOT_SSWISE_IBM2_SWM_LABEL
# define CURR_SWM_NOTES THOT_SSWISE_IBM2_SWM_NOTES
typedef CURR_SWM_TYPE::PpInfo PpInfo;

#elif THOT_SWM_TYPE == THOT_INCR_HMM_SWM
# include <IncrHmmAligModel.h>
# define CURR_SWM_TYPE IncrHmmAligModel
# define CURR_SWM_LABEL THOT_INCR_HMM_SWM_LABEL
# define CURR_SWM_NOTES THOT_INCR_HMM_SWM_NOTES
typedef CURR_SWM_TYPE::PpInfo PpInfo;

#elif THOT_SWM_TYPE == THOT_INCR_HOM_HMM_SWM
# include <IncrHomHmmAligModel.h>
# define CURR_SWM_TYPE IncrHomHmmAligModel
# define CURR_SWM_LABEL THOT_INCR_HOM_HMM_SWM_LABEL
# define CURR_SWM_NOTES THOT_INCR_HOM_HMM_SWM_NOTES
typedef CURR_SWM_TYPE::PpInfo PpInfo;

#elif THOT_SWM_TYPE == THOT_INCR_HMM_P0_SWM
# include <IncrHmmP0AligModel.h>
# define CURR_SWM_TYPE IncrHmmP0AligModel
# define CURR_SWM_LABEL THOT_INCR_HMM_P0_SWM_LABEL
# define CURR_SWM_NOTES THOT_INCR_HMM_P0_SWM_NOTES
typedef CURR_SWM_TYPE::PpInfo PpInfo;

#elif THOT_SWM_TYPE == THOT_SWISE_HMM_P0_SWM
# include <StepwiseHmmP0AligModel.h>
# define CURR_SWM_TYPE StepwiseHmmP0AligModel
# define CURR_SWM_LABEL THOT_SWISE_HMM_P0_SWM_LABEL
# define CURR_SWM_NOTES THOT_SWISE_HMM_P0_SWM_NOTES
typedef CURR_SWM_TYPE::PpInfo PpInfo;

#endif

#endif
