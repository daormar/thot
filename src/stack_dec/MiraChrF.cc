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

//--------------- Include files --------------------------------------

#include "chrf.h"
#include "MiraChrF.h"


void MiraChrF::sentBackgroundScore(const std::string& candidate,
                                   const std::string& reference,
                                   double& score,
                                   std::vector<unsigned int>& /*sentStats*/)
{
  std::vector<std::string> reference_tokens;
  reference_tokens = StrProcUtils::stringToStringVector(reference);

  sentScore(candidate, reference, score);
  score *=reference_tokens.size();
}

void MiraChrF::sentScore(const std::string& candidate,
                         const std::string& reference,
                         double& score)
{
    calculate_chrf(reference, candidate, score);
}

void MiraChrF::corpusScore(const std::vector<std::string>& candidates,
                           const std::vector<std::string>& references,
                           double& score)
{
    score = 0;
    for (unsigned int i=0; i<candidates.size(); i++) {
        double sentenceScore;
        sentScore(candidates[i], references[i], sentenceScore);
        score += sentenceScore;
    }

    score /= candidates.size();
}
