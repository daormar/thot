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
/* Module: KbMiraLlWu                                               */
/*                                                                  */
/* Prototypes file: KbMiraLlWu.h                                    */
/*                                                                  */
/* Description: Declares the KbMiraLlWu class implementing          */
/*              the K-best MIRA algorithm for weight                */
/*              updating.                                           */
/*                                                                  */
/********************************************************************/

/**
 * @file KbMiraLlWu.h
 *
 * @brief Declares the KbMiraLlWu class implementing the K-best MIRA
 * algorithm for weight updating.
 */

#ifndef _KbMiraLlWu_h
#define _KbMiraLlWu_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseLogLinWeightUpdater.h"
#include "BaseMiraScorer.h"
#include <float.h>

//--------------- Constants ------------------------------------------

#define KBMIRA_RANDOM_SEED 31415

//--------------- typedefs -------------------------------------------
struct HopeFearData {
  Vector<double> hopeFeatures, fearFeatures;
  Vector<unsigned int> hopeQualityStats;
  double hopeScore, hopeQuality;
  double fearScore, fearQuality;
};

//--------------- Classes --------------------------------------------

//--------------- KbMiraLlWu template class

/**
 * @brief Class implementing the K-best MIRA algorithm.
 */

class KbMiraLlWu: public BaseLogLinWeightUpdater
{
 public:
  KbMiraLlWu(double C = 0.01,
             double gamma = 0.999,
             unsigned int J = 30,
             unsigned int epochs_to_restart = 8,
             unsigned int max_restarts = 2);
  ~KbMiraLlWu();

      // Compute new weights for an individual sentence
  void update(const std::string& reference,
              const Vector<std::string>& nblist,
              const Vector<Vector<double> >& scoreCompsVec,
              const Vector<double>& currWeightsVec,
              Vector<double>& newWeightsVec);

      // Compute new weights for a closed corpus
  void updateClosedCorpus(const Vector<std::string>& references,
                          const Vector<Vector<std::string> >& nblists,
                          const Vector<Vector<Vector<double> > >& scoreCompsVecs,
                          const Vector<double>& currWeightsVec,
                          Vector<double>& newWeightsVec);
 private:
  double c;              // Step-size cap C
  double decay;          // Pseudo-corpus decay \gamma
  unsigned int nIters;  // Max epochs J
  unsigned int epochsToRestart; // epochs without improvement before re-start
  unsigned int maxRestarts;     // max number of re-starts
  BaseMiraScorer *scorer;

     // Compute max scoring translaiton according to w
  void MaxTranslation(const Vector<double>& w,
                      const Vector<std::string>& nBest,
                      const Vector<Vector<double> >& nScores,
                      std::string &maxTranslation);

     // Compute hope/fear translations and stores info in hopeFear
  void HopeFear(const std::string& reference,
                const Vector<std::string>& nBest,
                const Vector<Vector<double> >& nScores,
                const Vector<double>& wv,
                HopeFearData* hopeFear);

   //get permutation indices
  void sampleWoReplacement(unsigned int nSamples,
                           vector<unsigned int>& indices);
};

#endif
