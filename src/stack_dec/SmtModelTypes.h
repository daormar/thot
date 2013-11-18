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
 
#ifndef _SmtModelTypes_h
#define _SmtModelTypes_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

//--------------- Constants ------------------------------------------

#define PB                      1
#define PB_LABEL                "Lm + phrase-based model"
#define PB_NOTES                ""

#define PB_COV                  2
#define PB_COV_LABEL            "Lm + phrase-based model, cov. eq. class"
#define PB_COV_NOTES            "Equivalence class for hypotheses is given by the coverage vector"

#define PB_NUMCOVJUMPS          3
#define PB_NUMCOVJUMPS_LABEL    "Lm + phrase-based model, numcov. + jumps eq. class"
#define PB_NUMCOVJUMPS_NOTES    "Equivalence class for hypotheses is given by the number of covered source words and the number of alignment jumps"

#define PB_NUMCOVJUMPS01        4
#define PB_NUMCOVJUMPS01_LABEL  "Lm + phrase-based model, numcov. + jumps 01 eq. class"
#define PB_NUMCOVJUMPS01_NOTES  "Equivalence class for hypotheses is given by the number of covered source words and the existence or not of alignment jumps"

#define PB_NUMCOVJUMPSDIV       5
#define PB_NUMCOVJUMPSDIV_LABEL "Lm + phrase-based model, numcov. + jumps div. eq. class"
#define PB_NUMCOVJUMPSDIV_NOTES "Equivalence class for hypotheses is given by the number of covered source words and the number of alignment jumps, the number of jumps is divided by a constant"

#define PB_LASTSRCPOSCOV        6
#define PB_LASTSRCPOSCOV_LABEL  "Lm + phrase-based model, numcov. + last source position covered"
#define PB_LASTSRCPOSCOV_NOTES  "Equivalence class for hypotheses is given by the number of covered source words and the last covered source position"

#define PB_SRCLENTRGLEN         7
#define PB_SRCLENTRGLEN_LABEL   "Lm + phrase-based model, numcov. + target sentence length"
#define PB_SRCLENTRGLEN_NOTES   "Equivalence class for hypotheses is given by the number of covered source words and the length of target sentence"

#define PB_SRCLENLASTTRGWORD    8
#define PB_SRCLENLASTTRGWORD_LABEL  "Lm + phrase-based model, numcov. + last target word"
#define PB_SRCLENLASTTRGWORD_NOTES  "Equivalence class for hypotheses is given by the number of covered source words and the last target word"

#define PBSWM                   9
#define PBSWM_LABEL             "Lm + phrase-based model + sw model"
#define PBSWM_NOTES             ""

#define PBLSWMLI               10
#define PBLSWMLI_LABEL          "Lm + phrase-based model and local sw model linearly-interpolated"
#define PBLSWMLI_NOTES          ""

#define PBLSWM                 11
#define PBLSWM_LABEL            "Lm + phrase-based model + local sw model"
#define PBLSWM_NOTES            ""

// Set the model type used by different programs such as
// phrase_stack_trans, phrase_mstack_trans, phrase_aligner, etc.  The
// valid options are: PB, PBSWM, PBLSWM, PBLSWMEC... These options
// can be given before executing the configure script.
//
// Example:
// $ export THOT_SMT_MODEL_TYPE="PB"
// $ configure --prefix=$PWD
// $ make
// $ make install

#if THOT_SMT_MODEL_TYPE == PB
# include <PhraseBasedTransModel.h>
# include <PhrHypEqClassF.h>
# define CURR_MODEL_TYPE PhraseBasedTransModel<PhrHypEqClassF>
# define CURR_MODEL_LABEL PB_LABEL
# define CURR_MODEL_NOTES PB_NOTES

#elif THOT_SMT_MODEL_TYPE == PB_COV
# include <PhraseBasedTransModel.h>
# include <PhrHypCovEqClassF.h>
# define CURR_MODEL_TYPE PhraseBasedTransModel<PhrHypCovEqClassF>
# define CURR_MODEL_LABEL PB_COV_LABEL
# define CURR_MODEL_NOTES PB_COV_NOTES

#elif THOT_SMT_MODEL_TYPE == PB_NUMCOVJUMPS
# include <PhraseBasedTransModel.h>
# include <PhrHypNumcovJumpsEqClassF.h>
# define CURR_MODEL_TYPE PhraseBasedTransModel<PhrHypNumcovJumpsEqClassF>
# define CURR_MODEL_LABEL PB_NUMCOVJUMPS_LABEL
# define CURR_MODEL_NOTES PB_NUMCOVJUMPS_NOTES

#elif THOT_SMT_MODEL_TYPE == PB_NUMCOVJUMPSDIV
# include <PhraseBasedTransModel.h>
# include <PhrHypNumcovJumpsDivEqClassF.h>
# define CURR_MODEL_TYPE PhraseBasedTransModel<PhrHypNumcovJumpsDivEqClassF>
# define CURR_MODEL_LABEL PB_NUMCOVJUMPSDIV_LABEL
# define CURR_MODEL_NOTES PB_NUMCOVJUMPSDIV_NOTES

#elif THOT_SMT_MODEL_TYPE == PB_NUMCOVJUMPS01
# include <PhraseBasedTransModel.h>
# include <PhrHypNumcovJumps01EqClassF.h>
# define CURR_MODEL_TYPE PhraseBasedTransModel<PhrHypNumcovJumps01EqClassF>
# define CURR_MODEL_LABEL PB_NUMCOVJUMPS01_LABEL
# define CURR_MODEL_NOTES PB_NUMCOVJUMPS01_NOTES

#elif THOT_SMT_MODEL_TYPE == PB_LASTSRCPOSCOV
# include <PhraseBasedTransModel.h>
# include <PhrHypLastSrcPosCovEqClassF.h>
# define CURR_MODEL_TYPE PhraseBasedTransModel<PhrHypLastSrcPosCovEqClassF>
# define CURR_MODEL_LABEL PB_LASTSRCPOSCOV_LABEL
# define CURR_MODEL_NOTES PB_LASTSRCPOSCOV_NOTES

#elif THOT_SMT_MODEL_TYPE == PB_SRCLENTRGLEN
# include <PhraseBasedTransModel.h>
# include <PhrSrcLenTrgLenEqClassF.h>
# define CURR_MODEL_TYPE PhraseBasedTransModel<PhrSrcLenTrgLenEqClassF>
# define CURR_MODEL_LABEL PB_SRCLENTRGLEN_LABEL
# define CURR_MODEL_NOTES PB_SRCLENTRGLEN_NOTES

#elif THOT_SMT_MODEL_TYPE == PB_SRCLENLASTTRGWORD
# include <PhraseBasedTransModel.h>
# include <PhrSrcLenLastTrgWordEqClassF.h>
# define CURR_MODEL_TYPE PhraseBasedTransModel<PhrSrcLenLastTrgWordEqClassF>
# define CURR_MODEL_LABEL PB_SRCLENLASTTRGWORD_LABEL
# define CURR_MODEL_NOTES PB_SRCLENLASTTRGWORD_NOTES

#elif THOT_SMT_MODEL_TYPE == PBSWM
# include <PhrSwTransModel.h>
# define CURR_MODEL_TYPE PhrSwTransModel
# define CURR_MODEL_LABEL PBSWM_LABEL
# define CURR_MODEL_NOTES PBSWM_NOTES

#elif THOT_SMT_MODEL_TYPE == PBLSWMLI
# include <PhrLocalSwLiTm.h>
# define CURR_MODEL_TYPE PhrLocalSwLiTm
# define CURR_MODEL_LABEL PBLSWMLI_LABEL
# define CURR_MODEL_NOTES PBLSWMLI_NOTES

#elif THOT_SMT_MODEL_TYPE == PBLSWM
# include <PhrLocalSwTransModel.h>
# define CURR_MODEL_TYPE PhrLocalSwTransModel
# define CURR_MODEL_LABEL PBLSWM_LABEL
# define CURR_MODEL_NOTES PBLSWM_NOTES
#endif

#endif
