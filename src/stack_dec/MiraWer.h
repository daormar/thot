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

#ifndef _MiraWer_h
#define _MiraWer_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseMiraScorer.h"
#include <cassert>
#include <iostream>

class MiraWer : public BaseMiraScorer
{
public:
    // Constructor
  MiraWer() { }

  void resetBackgroundCorpus() { }

  void updateBackgroundCorpus(const Vector<unsigned int>& /*stats*/,
                              double /*decay*/) { }

    // Score for sentence with background corpus stats
  void sentBackgroundScore(const std::string& candidate,
                           const std::string& reference,
                           double& score,
                           Vector<unsigned int>& stats);

    // Score for sentence
  void sentScore(const std::string& candidate,
                 const std::string& reference,
                 double& score);

    // Score for corpus
  void corpusScore(const Vector<std::string>& candidates,
                   const Vector<std::string>& references,
                   double& score);

private:
  int ed(Vector<std::string>& s1, Vector<std::string>& s2);
};

#endif
