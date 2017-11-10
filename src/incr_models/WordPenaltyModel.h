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
/* Module: WordPenaltyModel                                         */
/*                                                                  */
/* Prototype file: WordPenaltyModel.h                               */
/*                                                                  */
/* Description: The WordPenaltyModel class implements a model for   */
/*              the word insertion penalty.                         */
/*                                                                  */
/********************************************************************/

#ifndef _WordPenaltyModel
#define _WordPenaltyModel

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseWordPenaltyModel.h"
#include <string.h>
#include "AwkInputStream.h"
#include "ErrorDefs.h"
#include "LM_Defs.h"
#include "SmtDefs.h"
#include "Prob.h"
#include "MathFuncs.h"
#include <map>
#include <utility>

//--------------- Constants ------------------------------------------

#define USE_LOGARITHMIC 0
#define USE_TRIANG      1
#define USE_GEOM        2

#define DEFAULT_P_GEOM  0.7

//--------------- typedefs -------------------------------------------


//--------------- WordPenaltyModel class

class WordPenaltyModel: public BaseWordPenaltyModel
{
 public:

      // Constructor
  WordPenaltyModel(void);
  
      // returns log(wp(tl=tlen))
  LgProb wordPenaltyScore(unsigned int tlen);
      // returns log(wp(tl>=tlen))
  LgProb sumWordPenaltyScore(unsigned int tlen);

      // clear function
  void clear(void);
  
 protected:

  int mode;
  unsigned int minLen;
      // minLen is the minimum sentence length parameter used in both
      // the logarithmic and the triangular distributions
  unsigned int maxLen;
      // maxLen is the maximum sentence length parameter used in both
      // the logarithmic and the triangular distributions
  double p_geom;
      // p_geom is the probability of success on each trial

  std::vector<LgProb> sum_wlp;
      // Precalculates the sum of the word penalty
  
      // auxiliary function for sumWordPenaltyScore()
  LgProb sumWordPenaltyScoreAux(unsigned int tlen);
      // Word penalty using a logarithmic distribution
  LgProb wordPenaltyScoreLogarithmic(unsigned int tlen);
  Prob sumSentLenProbLogarithmic(unsigned int tlen);
  
      // Word penalty using a triangular distribution
  LgProb wordPenaltyScoreTriang(unsigned int tlen);
  Prob sumSentLenProbTriang(unsigned int tlen);
  
      // Word penalty using a geometric distribution
  LgProb wordPenaltyScoreGeom(unsigned int tlen);
  
      // Functions to read distribution parameters
  bool readLogPars(const char *logParsFileName);
  bool readTriPars(const char *triParsFileName);
  bool readGeomPars(const char *geomParsFileName);
};

#endif
