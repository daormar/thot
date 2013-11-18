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

#define IBM1_SWM                  1
#define IBM1_SWM_LABEL            "IBM 1 single-word model"

#define IBM2_SWM                  2
#define IBM2_SWM_LABEL            "IBM 2 single-word model"

#define IBM3_SWM                  3
#define IBM3_SWM_LABEL            "IBM 3 single-word model"

#define IBM4_SWM                  4
#define IBM4_SWM_LABEL            "IBM 4 single-word model"

#define MIX_IBM1_SWM              5
#define MIX_IBM1_SWM_LABEL        "Mixture of IBM 1 single-word models"

#define MIX_IBM2_SWM              6
#define MIX_IBM2_SWM_LABEL        "Mixture of IBM 2 single-word models"

#define INCR_IBM1_SWM             7
#define INCR_IBM1_SWM_LABEL       "Incremental IBM 1 single-word model"

#define SINCR_IBM1_SWM            8
#define SINCR_IBM1_SWM_LABEL      "Smoothed incremental IBM 1 single-word model"

#define SSWISE_IBM1_SWM           9
#define SSWISE_IBM1_SWM_LABEL     "Smoothed stepwise IBM 1 single-word model"

#define INCR_IBM2_SWM            10
#define INCR_IBM2_SWM_LABEL       "Incremental IBM 2 single-word model"

#define SINCR_IBM2_SWM           11
#define SINCR_IBM2_SWM_LABEL      "Smoothed incremental IBM 2 single-word model"

#define SSWISE_IBM2_SWM          12
#define SSWISE_IBM2_SWM_LABEL     "Smoothed stepwise IBM 2 single-word model"

#define CSIIBM2_SWM              13
#define CSIIBM2_SWM_LABEL         "Compact smoothed incremental IBM 2 single-word model"

#define GSIIBM2_SWM              14
#define GSIIBM2_SWM_LABEL         "Smoothed incremental IBM 2 single-word model with geometrically distributed alignment model"

#define INCR_HMM_SWM             15
#define INCR_HMM_SWM_LABEL        "Incremental Hmm-based single-word model"

#define INCR_HOM_HMM_SWM         16
#define INCR_HOM_HMM_SWM_LABEL    "Incremental homogeneus Hmm-based single-word model"

#define INCR_HMM_P0_SWM          17
#define INCR_HMM_P0_SWM_LABEL     "Incremental Hmm-based with fixed P0 single-word model"

#define SWISE_HMM_P0_SWM         18
#define SWISE_HMM_P0_SWM_LABEL    "Stepwise Hmm-based with fixed P0 single-word model"

// Set the single-word model used by the calc_sw_lgprob tool. The valid
// options are: IBM1_SWM, IBM2_SWM,... These options can be given before
// executing the configure script.
//
// Example:
// $ export CXXFLAGS="$CXXFLAGS -DTHOT_SWM_TYPE=IBM1_SWM"
// $ configure --prefix=$PWD
// $ make
// $ make install

#if THOT_SWM_TYPE == IBM1_SWM
# include "Ibm1AligModel.h"
# define THOT_CURR_SWM_TYPE Ibm1AligModel
# define THOT_CURR_SWM_LABEL IBM1_SWM_LABEL

#elif THOT_SWM_TYPE == IBM2_SWM
# include "Ibm2AligModel.h"
# define THOT_CURR_SWM_TYPE Ibm2AligModel
# define THOT_CURR_SWM_LABEL IBM2_SWM_LABEL

#elif THOT_SWM_TYPE == IBM3_SWM
# include "Ibm3AligModel.h"
# define THOT_CURR_SWM_TYPE Ibm3AligModel
# define THOT_CURR_SWM_LABEL IBM3_SWM_LABEL

#elif THOT_SWM_TYPE == IBM4_SWM
# include "Ibm4AligModel.h"
# define THOT_CURR_SWM_TYPE Ibm4AligModel
# define THOT_CURR_SWM_LABEL IBM4_SWM_LABEL

#elif THOT_SWM_TYPE == MIX_IBM1_SWM
# include "MixIbm1AligModel.h"
# define THOT_CURR_SWM_TYPE MixIbm1AligModel
# define THOT_CURR_SWM_LABEL MIX_IBM1_SWM_LABEL

#elif THOT_SWM_TYPE == MIX_IBM2_SWM
# include "MixIbm2AligModel.h"
# define THOT_CURR_SWM_TYPE MixIbm2AligModel
# define THOT_CURR_SWM_LABEL MIX_IBM2_SWM_LABEL

#elif THOT_SWM_TYPE == INCR_IBM1_SWM
# include "IncrIbm1AligModel.h"
# define THOT_CURR_SWM_TYPE IncrIbm1AligModel
# define THOT_CURR_SWM_LABEL INCR_IBM1_SWM_LABEL

#elif THOT_SWM_TYPE == SINCR_IBM1_SWM
# include "SmoothedIncrIbm1AligModel.h"
# define THOT_CURR_SWM_TYPE SmoothedIncrIbm1AligModel
# define THOT_CURR_SWM_LABEL SINCR_IBM1_SWM_LABEL

#elif THOT_SWM_TYPE == SSWISE_IBM1_SWM
# include "SmStepwiseIbm1AligModel.h"
# define THOT_CURR_SWM_TYPE SmStepwiseIbm1AligModel
# define THOT_CURR_SWM_LABEL SSWISE_IBM1_SWM_LABEL

#elif THOT_SWM_TYPE == INCR_IBM2_SWM
# include "IncrIbm2AligModel.h"
# define THOT_CURR_SWM_TYPE IncrIbm2AligModel
# define THOT_CURR_SWM_LABEL INCR_IBM2_SWM_LABEL

#elif THOT_SWM_TYPE == SINCR_IBM2_SWM
# include "SmoothedIncrIbm2AligModel.h"
# define THOT_CURR_SWM_TYPE SmoothedIncrIbm2AligModel
# define THOT_CURR_SWM_LABEL SINCR_IBM2_SWM_LABEL

#elif THOT_SWM_TYPE == SSWISE_IBM2_SWM
# include "SmStepwiseIbm2AligModel.h"
# define THOT_CURR_SWM_TYPE SmStepwiseIbm2AligModel
# define THOT_CURR_SWM_LABEL SSWISE_IBM2_SWM_LABEL

#elif THOT_SWM_TYPE == CSIIBM2_SWM
# include "CompactSmIncrIbm2AligModel.h"
# define THOT_CURR_SWM_TYPE CompactSmIncrIbm2AligModel
# define THOT_CURR_SWM_LABEL CSIIBM2_SWM_LABEL

#elif THOT_SWM_TYPE == GSIIBM2_SWM
# include "GeomSmIncrIbm2AligModel.h"
# define THOT_CURR_SWM_TYPE GeomSmIncrIbm2AligModel
# define THOT_CURR_SWM_LABEL GSIIBM2_SWM_LABEL

#elif THOT_SWM_TYPE == INCR_HMM_SWM
# include "IncrHmmAligModel.h"
# define THOT_CURR_SWM_TYPE IncrHmmAligModel
# define THOT_CURR_SWM_LABEL INCR_HMM_SWM_LABEL

#elif THOT_SWM_TYPE == INCR_HOM_HMM_SWM
# include "IncrHomHmmAligModel.h"
# define THOT_CURR_SWM_TYPE IncrHomHmmAligModel
# define THOT_CURR_SWM_LABEL INCR_HOM_HMM_SWM_LABEL

#elif THOT_SWM_TYPE == INCR_HMM_P0_SWM
# include "IncrHmmP0AligModel.h"
# define THOT_CURR_SWM_TYPE IncrHmmP0AligModel
# define THOT_CURR_SWM_LABEL INCR_HMM_P0_SWM_LABEL

#elif THOT_SWM_TYPE == SWISE_HMM_P0_SWM
# include "StepwiseHmmP0AligModel.h"
# define THOT_CURR_SWM_TYPE StepwiseHmmP0AligModel
# define THOT_CURR_SWM_LABEL SWISE_HMM_P0_SWM_LABEL

#endif

#ifndef THOT_SWM_TYPE
        // Default option
#include "Ibm1AligModel.h"
#define THOT_CURR_SWM_TYPE Ibm1AligModel
#define THOT_CURR_SWM_LABEL IBM1_SWM_LABEL
#endif

#endif
