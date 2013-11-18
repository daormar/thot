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
 
#ifndef _SwModelsSlmTypes_h
#define _SwModelsSlmTypes_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

//--------------- Constants ------------------------------------------

#define SLM_POISSON              1
#define SLM_POISSON_LABEL        "Poisson sentence length model"
#define SLM_POISSON_NOTES        ""

#define SLM_GEOM                 2
#define SLM_GEOM_LABEL           "Geometric sentence length model"
#define SLM_GEOM_NOTES           ""

#define SLM_NORM                 3
#define SLM_NORM_LABEL           "Normal sentence length model"
#define SLM_NORM_NOTES           ""

#define ISLM_NORM                4
#define ISLM_NORM_LABEL          "Incremental normal sentence length model"
#define ISLM_NORM_NOTES          ""

#define SSLM_NORM                5
#define SSLM_NORM_LABEL          "Stepwise normal sentence length model"
#define SSLM_NORM_NOTES          ""

#define WISLM_NORM               6
#define WISLM_NORM_LABEL         "Weighted incremental normal sentence length model"
#define WISLM_NORM_NOTES         ""

// Set the sentence length model type used by different programs.  The
// valid options are: SLM_POISSON, SLM_GEOM and SLM_NORM. These options
// can be given before executing the configure script.
//
// Example:
// $ export THOT_SLM_TYPE="SLM_POISSON"
// $ configure --prefix=$PWD
// $ make
// $ make install

#if THOT_SLM_TYPE == SLM_POISSON
# define CURR_SLM_TYPE PoissonSentLengthModel
# define CURR_SLM_LABEL SLM_POISSON_LABEL
# define CURR_SLM_NOTES SLM_POISSON_NOTES
# include <PoissonSentLengthModel.h>

#elif THOT_SLM_TYPE == SLM_GEOM
# define CURR_SLM_TYPE GeomSentLengthModel
# define CURR_SLM_LABEL SLM_GEOM_LABEL
# define CURR_SLM_NOTES SLM_GEOM_NOTES
# include <GeomSentLengthModel.h>

#elif THOT_SLM_TYPE == SLM_NORM
# define CURR_SLM_TYPE NormSentLengthModel
# define CURR_SLM_LABEL SLM_NORM_LABEL
# define CURR_SLM_NOTES SLM_NORM_NOTES
# include <NormSentLengthModel.h>

#elif THOT_SLM_TYPE == ISLM_NORM
# define CURR_SLM_TYPE IncrNormSlm
# define CURR_SLM_LABEL ISLM_NORM_LABEL
# define CURR_SLM_NOTES ISLM_NORM_NOTES
# include <IncrNormSlm.h>

#elif THOT_SLM_TYPE == SSLM_NORM
# define CURR_SLM_TYPE StepwiseNormSlm
# define CURR_SLM_LABEL SSLM_NORM_LABEL
# define CURR_SLM_NOTES SSLM_NORM_NOTES
# include <StepwiseNormSlm.h>

#elif THOT_SLM_TYPE == WISLM_NORM
# define CURR_SLM_TYPE WeightedIncrNormSlm
# define CURR_SLM_LABEL WISLM_NORM_LABEL
# define CURR_SLM_NOTES WISLM_NORM_NOTES
# include <WeightedIncrNormSlm.h>

#endif

#ifndef THOT_SLM_TYPE
# define CURR_SLM_TYPE WeightedIncrNormSlm
# define CURR_SLM_LABEL WISLM_NORM_LABEL
# define CURR_SLM_NOTES WISLM_NORM_NOTES
# include <WeightedIncrNormSlm.h>
#endif

#endif
