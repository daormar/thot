#ifndef _MiraBleuLenLim_h
#define _MiraBleuLenLim_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseMiraScorer.h"
#include <cassert>
#include <iostream>

class MiraBleuLenLim : public BaseMiraScorer
{
public:
    // Constructor
  MiraBleuLenLim(unsigned int length_limitation) {
    N = 4;
    N_STATS = 2 + 2*N; // cand_len, ref_len, (matching, totals) for n 1..4
    CHARACTER_LIMIT = length_limitation; // maximum length allowed (in chars)
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
  void Score(const Vector<std::string>& candidates,
             const Vector<std::string>& references,
             double& score);

    // Score for sentence
  void sentScore(const std::string& candidate,
                 const std::string& reference,
                 double& score);

private:
  unsigned int CHARACTER_LIMIT, N_STATS, N;
  Vector <unsigned int> backgroundBleu; // background corpus stats for BLEU

  double scoreFromStats(Vector<unsigned int>& stats);
  void statsForSentence(const Vector<std::string>& candidate_tokens,
                        const Vector<std::string>& reference_tokens,
                        Vector<unsigned int>& stats);
  void split(const std::string& sentence, Vector<std::string>& tokens);
};

#endif
