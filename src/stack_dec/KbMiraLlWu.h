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
#include <float.h>

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------
struct HopeFearData {
  Vector<double> hopeFeatures, hopeBleuStats;
  Vector<double> fearFeatures;
  double hopeScore, hopeBleu;
  double fearScore, fearBleu;
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
             unsigned int J = 60);

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
                const Vector<unsigned int>& backgroundBleu,
                HopeFearData* hopeFear);

    // Computes background Bleu for candidate and background corpus
  void sentBckgrndBleu(const std::string& candidate,
                       const std::string& reference,
                       const Vector<unsigned int>& backgroundBleu,
                       double& bleu,
                       Vector<unsigned int>& stats);

    // Bleu for corpus
  void Bleu(const Vector<std::string>& candidates,
            const Vector<std::string>& references,
            double& bleu);

    // Bleu for sentence
  void sentenceBleu(const std::string& candidate,
                    const std::string& reference,
                    double& bleu);

   //get permutation indices
  void sampleWoReplacement(unsigned int nSamples,
                           vector<unsigned int>& indices);
};

#endif
