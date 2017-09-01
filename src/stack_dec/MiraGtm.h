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

#ifndef _MiraGtm_h
#define _MiraGtm_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseMiraScorer.h"

#include <map>
#include <set>
#include <cassert>
#include <iostream>

class MiraGtm : public BaseMiraScorer
{
public:
    // Constructor
  MiraGtm() { 
    beta = 1.0;
    d = 1.0;
    N_STATS = 3;
  }

  void resetBackgroundCorpus() {  }

  void updateBackgroundCorpus(const std::vector<unsigned int>& /*stats*/,
                              double /*decay*/) {  }

    // Score for sentence with background corpus stats
  void sentBackgroundScore(const std::string& candidate,
                           const std::string& reference,
                           double& score,
                           std::vector<unsigned int>& stats);

    // Score for sentence
  void sentScore(const std::string& candidate,
                 const std::string& reference,
                 double& score);

    // Score for corpus
  void corpusScore(const std::vector<std::string>& candidates,
                   const std::vector<std::string>& references,
                   double& score);

private:
  double beta, d;
  unsigned int N_STATS;

  static bool revCompFunction(std::pair<int, std::pair<std::pair<int,int>, std::pair<int,int> > > a,
                              std::pair<int, std::pair<std::pair<int,int>, std::pair<int,int> > > b);
  bool doIntersect(std::pair<int,int> a, std::set<int> b);

  double scoreFromStats(std::vector<unsigned int>& stats);
  void sorted_common_ngrams(const std::vector<std::string>& s1,
                            const std::vector<std::string>& s2,
                            std::vector<std::pair<int, std::pair<std::pair<int, int>, std::pair<int, int> > > >& ngs);
  void statsForSentence(const std::vector<std::string>& candidate_tokens,
                        const std::vector<std::string>& reference_tokens,
                        std::vector<unsigned int>& stats);
};

#endif
