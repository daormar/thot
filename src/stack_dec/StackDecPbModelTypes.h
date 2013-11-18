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
 
#ifndef _StackDecPbModelTypes_h
#define _StackDecPbModelTypes_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

//--------------- Constants ------------------------------------------

#define UM_ML_PBM               1
#define UM_ML_PBM_LABEL         "Unsmoothed maximum-likelihood phrase model"
#define UM_ML_PBM_NOTES         ""

#define ML_PBM                  2
#define ML_PBM_LABEL            "Maximum-likelihood phrase model"
#define ML_PBM_NOTES            ""

#define CACHE_ML_PBM            3
#define CACHE_ML_PBM_LABEL      "Cached maximum-likelihood phrase model"

#define SWISE_ML_PBM            4
#define SWISE_ML_PBM_LABEL      "Maximum-likelihood phrase model with stepwise updates"
#define SWISE_ML_PBM_NOTES      ""

#define GT_PBM                  5
#define GT_PBM_LABEL            "Good-Turing phrase model"
#define GT_PBM_NOTES            ""

#define GT_LEX_BO_PBM           6
#define GT_LEX_BO_PBM_LABEL     "Good-Turing + Lex backoff phrase model"
#define GT_LEX_BO_PBM_NOTES     ""

#define SD_IPBM                 7
#define SD_IPBM_LABEL           "Simple-discount incremental phrase model"
#define SD_IPBM_NOTES           ""       

#define SD_LEX_BO_IPBM          8
#define SD_LEX_BO_IPBM_LABEL    "Simple-discount + Lex backoff incremental phrase model"
#define SD_LEX_BO_IPBM_NOTES    ""       

#define AD_PBM                  9
#define AD_PBM_LABEL            "Absolute-discounting phrase model"
#define AD_PBM_NOTES            ""

#define KN_PBM                 10
#define KN_PBM_LABEL            "Kneser-Ney discounting phrase model"
#define KN_PBM_NOTES            ""

#define ML_LEX_LI_PBM          11
#define ML_LEX_LI_PBM_LABEL     "Maximum-likelihood + Lex linearly-interpolated phrase model"
#define ML_LEX_LI_PBM_NOTES     ""

#define SD_LEX_LI_PBM          12
#define SD_LEX_LI_PBM_LABEL     "Simple-discount + Lex linearly-interpolated phrase model"
#define SD_LEX_LI_PBM_NOTES     ""

#define GT_LEX_LI_PBM          13
#define GT_LEX_LI_PBM_LABEL     "Good-Turing + Lex linearly-interpolated phrase model"
#define GT_LEX_LI_PBM_NOTES     ""

#define AD_LEX_LI_PBM          14
#define AD_LEX_LI_PBM_LABEL     "Absolut-discounting + Lex linearly-interpolated phrase model"
#define AD_LEX_LI_PBM_NOTES     ""

#define KN_LEX_LI_PBM          15
#define KN_LEX_LI_PBM_LABEL     "Kneser-Ney discounting + Lex linearly-interpolated phrase model"
#define KN_LEX_LI_PBM_NOTES     ""

// Set the phrase-based model used by different statistical machine
// translation models. The valid options are: ML_PBM,
// GT_PBM,... These options can be given before executing the
// configure script.
//
// Example:
// $ export THOT_PBM_TYPE="ML_PBM"
// $ configure --prefix=$PWD
// $ make
// $ make install

#if THOT_PBM_TYPE == UM_ML_PBM
# include <UnsmoothedMlPm.h>
# define THOT_CURR_PBM_TYPE UnsmoothedMlPm
# define THOT_CURR_PBM_LABEL UM_ML_PBM_LABEL
# define THOT_CURR_PBM_NOTES UM_ML_PBM_NOTES

#elif THOT_PBM_TYPE == ML_PBM
# include <WbaIncrPhraseModel.h>
# define THOT_CURR_PBM_TYPE WbaIncrPhraseModel
# define THOT_CURR_PBM_LABEL ML_PBM_LABEL
# define THOT_CURR_PBM_NOTES ML_PBM_NOTES

#elif THOT_PBM_TYPE == CACHE_ML_PBM
# include <CachePtPm.h>
# define THOT_CURR_PBM_TYPE  CachePtPm
# define THOT_CURR_PBM_LABEL CACHE_ML_PBM_LABEL
# define THOT_CURR_PBM_NOTES CACHE_ML_PBM_NOTES

#elif THOT_PBM_TYPE == SWISE_ML_PBM
# include <WbaStepwisePm.h>
# define THOT_CURR_PBM_TYPE WbaStepwisePm
# define THOT_CURR_PBM_LABEL SWISE_ML_PBM_LABEL
# define THOT_CURR_PBM_NOTES SWISE_ML_PBM_NOTES

#elif THOT_PBM_TYPE == GT_PBM
# include <GtPhraseModel.h>
# define THOT_CURR_PBM_TYPE GtPhraseModel
# define THOT_CURR_PBM_LABEL GT_PBM_LABEL
# define THOT_CURR_PBM_NOTES GT_PBM_NOTES

#elif THOT_PBM_TYPE == GT_LEX_BO_PBM
# include <GtLexBoPm.h>
# define THOT_CURR_PBM_TYPE GtLexBoPm
# define THOT_CURR_PBM_LABEL GT_LEX_BO_PBM_LABEL
# define THOT_CURR_PBM_NOTES GT_LEX_BO_PBM_NOTES

#elif THOT_PBM_TYPE == SD_IPBM
# include <SdWipm.h>
# define THOT_CURR_PBM_TYPE SdWipm
# define THOT_CURR_PBM_LABEL SD_IPBM_LABEL
# define THOT_CURR_PBM_NOTES SD_IPBM_NOTES

#elif THOT_PBM_TYPE == SD_LEX_BO_IPBM
# include <SdLexBoWipm.h>
# define THOT_CURR_PBM_TYPE SdLexBoWipm
# define THOT_CURR_PBM_LABEL SD_LEX_BO_IPBM_LABEL
# define THOT_CURR_PBM_NOTES SD_LEX_BO_IPBM_NOTES

#elif THOT_PBM_TYPE == AD_PBM
# include <AdPm.h>
# define THOT_CURR_PBM_TYPE AdPm
# define THOT_CURR_PBM_LABEL AD_PBM_LABEL
# define THOT_CURR_PBM_NOTES AD_PBM_NOTES

#elif THOT_PBM_TYPE == KN_PBM
# include <KnPm.h>
# define THOT_CURR_PBM_TYPE KnPm
# define THOT_CURR_PBM_LABEL KN_PBM_LABEL
# define THOT_CURR_PBM_NOTES KN_PBM_NOTES

#elif THOT_PBM_TYPE == ML_LEX_LI_PBM
# include <MlLexLiPm.h>
# define THOT_CURR_PBM_TYPE MlLexLiPm
# define THOT_CURR_PBM_LABEL ML_LEX_LI_PBM_LABEL
# define THOT_CURR_PBM_NOTES ML_LEX_LI_PBM_NOTES

#elif THOT_PBM_TYPE == SD_LEX_LI_PBM
# include <SdLexLiPm.h>
# define THOT_CURR_PBM_TYPE SdLexLiPm
# define THOT_CURR_PBM_LABEL SD_LEX_LI_PBM_LABEL
# define THOT_CURR_PBM_NOTES SD_LEX_LI_PBM_NOTES

#elif THOT_PBM_TYPE == GT_LEX_LI_PBM
# include <GtLexLiPm.h>
# define THOT_CURR_PBM_TYPE GtLexLiPm
# define THOT_CURR_PBM_LABEL GT_LEX_LI_PBM_LABEL
# define THOT_CURR_PBM_NOTES GT_LEX_LI_PBM_NOTES

#elif THOT_PBM_TYPE == AD_LEX_LI_PBM
# include <AdLexLiPm.h>
# define THOT_CURR_PBM_TYPE AdLexLiPm
# define THOT_CURR_PBM_LABEL AD_LEX_LI_PBM_LABEL
# define THOT_CURR_PBM_NOTES AD_LEX_LI_PBM_NOTES

#elif THOT_PBM_TYPE == KN_LEX_LI_PBM
# include <KnLexLiPm.h>
# define THOT_CURR_PBM_TYPE KnLexLiPm
# define THOT_CURR_PBM_LABEL KN_LEX_LI_PBM_LABEL
# define THOT_CURR_PBM_NOTES KN_LEX_LI_PBM_NOTES

#endif

#endif
