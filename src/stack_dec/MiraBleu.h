/*
thot package for statistical machine translation

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

#ifndef _MiraBleu_h
#define _MiraBleu_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseMiraScorer.h"
#include <cassert>
#include <iostream>

class MiraBleu : public BaseMiraScorer
{
public:
    // Constructor
  MiraBleu() {
    N_STATS = 10; // cand_len, ref_len, (matching, totals) for n 1..4
    resetBackgroundCorpus();
  }

  void resetBackgroundCorpus() {
    backgroundBleu.clear();
    for (unsigned int i=0; i<N_STATS; i++)
      backgroundBleu.push_back(1);
  }

  void updateBackgroundCorpus(const Vector<unsigned int>& stats,
                              double decay) {
    assert (stats.size() == N_STATS);
    for (unsigned int i=0; i<N_STATS; i++)
      backgroundBleu[i] = decay*backgroundBleu[i] + stats[i];
  }

    // Score for sentence with background corpus stats
  void sentBackgroundScore(const std::string& candidate,
                           const std::string& reference,
                           double& score,
                           Vector<unsigned int>& stats);

    // Score for corpus
  void corpusScore(const Vector<std::string>& candidates,
                   const Vector<std::string>& references,
                   double& score);

    // Score for sentence
  void sentScore(const std::string& candidate,
                 const std::string& reference,
                 double& score);

private:
  unsigned int N_STATS;
  Vector <double> backgroundBleu; // background corpus stats for BLEU

  double scoreFromStats(Vector<unsigned int>& stats);
  void statsForSentence(const Vector<std::string>& candidate_tokens,
                        const Vector<std::string>& reference_tokens,
                        Vector<unsigned int>& stats);
  void split(const std::string& sentence, Vector<std::string>& tokens);
};

#endif
