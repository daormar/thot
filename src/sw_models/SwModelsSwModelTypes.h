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
 
#ifndef _SwModelsSwModelTypes_h
#define _SwModelsSwModelTypes_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

//--------------- Constants ------------------------------------------

#define THOT_IBM1_SWM                  1
#define THOT_IBM1_SWM_LABEL            "IBM 1 single-word model"

#define THOT_IBM2_SWM                  2
#define THOT_IBM2_SWM_LABEL            "IBM 2 single-word model"

#define THOT_IBM3_SWM                  3
#define THOT_IBM3_SWM_LABEL            "IBM 3 single-word model"

#define THOT_IBM4_SWM                  4
#define THOT_IBM4_SWM_LABEL            "IBM 4 single-word model"

#define THOT_MIX_IBM1_SWM              5
#define THOT_MIX_IBM1_SWM_LABEL        "Mixture of IBM 1 single-word models"

#define THOT_MIX_IBM2_SWM              6
#define THOT_MIX_IBM2_SWM_LABEL        "Mixture of IBM 2 single-word models"

#define THOT_INCR_IBM1_SWM             7
#define THOT_INCR_IBM1_SWM_LABEL       "Incremental IBM 1 single-word model"

#define THOT_SINCR_IBM1_SWM            8
#define THOT_SINCR_IBM1_SWM_LABEL      "Smoothed incremental IBM 1 single-word model"

#define THOT_SSWISE_IBM1_SWM           9
#define THOT_SSWISE_IBM1_SWM_LABEL     "Smoothed stepwise IBM 1 single-word model"

#define THOT_INCR_IBM2_SWM            10
#define THOT_INCR_IBM2_SWM_LABEL       "Incremental IBM 2 single-word model"

#define THOT_SINCR_IBM2_SWM           11
#define THOT_SINCR_IBM2_SWM_LABEL      "Smoothed incremental IBM 2 single-word model"

#define THOT_SSWISE_IBM2_SWM          12
#define THOT_SSWISE_IBM2_SWM_LABEL     "Smoothed stepwise IBM 2 single-word model"

#define THOT_CSIIBM2_SWM              13
#define THOT_CSIIBM2_SWM_LABEL         "Compact smoothed incremental IBM 2 single-word model"

#define THOT_GSIIBM2_SWM              14
#define THOT_GSIIBM2_SWM_LABEL         "Smoothed incremental IBM 2 single-word model with geometrically distributed alignment model"

#define THOT_INCR_HMM_SWM             15
#define THOT_INCR_HMM_SWM_LABEL        "Incremental Hmm-based single-word model"

#define THOT_INCR_HOM_HMM_SWM         16
#define THOT_INCR_HOM_HMM_SWM_LABEL    "Incremental homogeneus Hmm-based single-word model"

#define THOT_INCR_HMM_P0_SWM          17
#define THOT_INCR_HMM_P0_SWM_LABEL     "Incremental Hmm-based with fixed P0 single-word model"

#define THOT_SWISE_HMM_P0_SWM         18
#define THOT_SWISE_HMM_P0_SWM_LABEL    "Stepwise Hmm-based with fixed P0 single-word model"

// Set the single-word model used by the calc_sw_lgprob tool. The valid
// options are: IBM1_SWM, IBM2_SWM,... These options can be given before
// executing the configure script.
//
// Example:
// $ export CXXFLAGS="$CXXFLAGS -DTHOT_SWM_TYPE=THOT_IBM1_SWM"
// $ configure --prefix=$PWD
// $ make
// $ make install

#if THOT_SWM_TYPE == THOT_IBM1_SWM
# include "Ibm1AligModel.h"
# define CURR_SWM_TYPE Ibm1AligModel
# define CURR_SWM_LABEL THOT_IBM1_SWM_LABEL

#elif THOT_SWM_TYPE == THOT_IBM2_SWM
# include "Ibm2AligModel.h"
# define CURR_SWM_TYPE Ibm2AligModel
# define CURR_SWM_LABEL THOT_IBM2_SWM_LABEL

#elif THOT_SWM_TYPE == THOT_IBM3_SWM
# include "Ibm3AligModel.h"
# define CURR_SWM_TYPE Ibm3AligModel
# define CURR_SWM_LABEL THOT_IBM3_SWM_LABEL

#elif THOT_SWM_TYPE == THOT_IBM4_SWM
# include "Ibm4AligModel.h"
# define CURR_SWM_TYPE Ibm4AligModel
# define CURR_SWM_LABEL THOT_IBM4_SWM_LABEL

#elif THOT_SWM_TYPE == THOT_MIX_IBM1_SWM
# include "MixIbm1AligModel.h"
# define CURR_SWM_TYPE MixIbm1AligModel
# define CURR_SWM_LABEL THOT_MIX_IBM1_SWM_LABEL

#elif THOT_SWM_TYPE == THOT_MIX_IBM2_SWM
# include "MixIbm2AligModel.h"
# define CURR_SWM_TYPE MixIbm2AligModel
# define CURR_SWM_LABEL THOT_MIX_IBM2_SWM_LABEL

#elif THOT_SWM_TYPE == THOT_INCR_IBM1_SWM
# include "IncrIbm1AligModel.h"
# define CURR_SWM_TYPE IncrIbm1AligModel
# define CURR_SWM_LABEL THOT_INCR_IBM1_SWM_LABEL

#elif THOT_SWM_TYPE == THOT_SINCR_IBM1_SWM
# include "SmoothedIncrIbm1AligModel.h"
# define CURR_SWM_TYPE SmoothedIncrIbm1AligModel
# define CURR_SWM_LABEL THOT_SINCR_IBM1_SWM_LABEL

#elif THOT_SWM_TYPE == THOT_SSWISE_IBM1_SWM
# include "SmStepwiseIbm1AligModel.h"
# define CURR_SWM_TYPE SmStepwiseIbm1AligModel
# define CURR_SWM_LABEL THOT_SSWISE_IBM1_SWM_LABEL

#elif THOT_SWM_TYPE == THOT_INCR_IBM2_SWM
# include "IncrIbm2AligModel.h"
# define CURR_SWM_TYPE IncrIbm2AligModel
# define CURR_SWM_LABEL THOT_INCR_IBM2_SWM_LABEL

#elif THOT_SWM_TYPE == THOT_SINCR_IBM2_SWM
# include "SmoothedIncrIbm2AligModel.h"
# define CURR_SWM_TYPE SmoothedIncrIbm2AligModel
# define CURR_SWM_LABEL THOT_SINCR_IBM2_SWM_LABEL

#elif THOT_SWM_TYPE == THOT_SSWISE_IBM2_SWM
# include "SmStepwiseIbm2AligModel.h"
# define CURR_SWM_TYPE SmStepwiseIbm2AligModel
# define CURR_SWM_LABEL THOT_SSWISE_IBM2_SWM_LABEL

#elif THOT_SWM_TYPE == THOT_CSIIBM2_SWM
# include "CompactSmIncrIbm2AligModel.h"
# define CURR_SWM_TYPE CompactSmIncrIbm2AligModel
# define CURR_SWM_LABEL THOT_CSIIBM2_SWM_LABEL

#elif THOT_SWM_TYPE == THOT_GSIIBM2_SWM
# include "GeomSmIncrIbm2AligModel.h"
# define CURR_SWM_TYPE GeomSmIncrIbm2AligModel
# define CURR_SWM_LABEL THOT_GSIIBM2_SWM_LABEL

#elif THOT_SWM_TYPE == THOT_INCR_HMM_SWM
# include "IncrHmmAligModel.h"
# define CURR_SWM_TYPE IncrHmmAligModel
# define CURR_SWM_LABEL THOT_INCR_HMM_SWM_LABEL

#elif THOT_SWM_TYPE == THOT_INCR_HOM_HMM_SWM
# include "IncrHomHmmAligModel.h"
# define CURR_SWM_TYPE IncrHomHmmAligModel
# define CURR_SWM_LABEL THOT_INCR_HOM_HMM_SWM_LABEL

#elif THOT_SWM_TYPE == THOT_INCR_HMM_P0_SWM
# include "IncrHmmP0AligModel.h"
# define CURR_SWM_TYPE IncrHmmP0AligModel
# define CURR_SWM_LABEL THOT_INCR_HMM_P0_SWM_LABEL

#elif THOT_SWM_TYPE == THOT_SWISE_HMM_P0_SWM
# include "StepwiseHmmP0AligModel.h"
# define CURR_SWM_TYPE StepwiseHmmP0AligModel
# define CURR_SWM_LABEL THOT_SWISE_HMM_P0_SWM_LABEL

#endif

#ifndef THOT_SWM_TYPE
        // Default option
# include "IncrHmmP0AligModel.h"
# define CURR_SWM_TYPE IncrHmmP0AligModel
# define CURR_SWM_LABEL THOT_INCR_HMM_P0_SWM_LABEL
/* #include "Ibm1AligModel.h" */
/* #define CURR_SWM_TYPE Ibm1AligModel */
/* #define CURR_SWM_LABEL THOT_IBM1_SWM_LABEL */
#endif

#endif
