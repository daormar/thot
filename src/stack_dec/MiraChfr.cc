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

#include "MiraChfr.h"
#include "StrProcUtils.h"

//--------------- MiraChfr class functions

//---------------------------------------
void MiraChfr::sentBackgroundScore(const std::string& candidate,
                                   const std::string& reference,
                                   double& score,
                                   Vector<unsigned int>& /*sentStats*/)
{
  Vector<std::string> reference_tokens = StrProcUtils::stringToStringVector(reference);
  int nwords = reference_tokens.size();
  double sentenceScore;
  sentScore(candidate, reference, sentenceScore);
  score=sentenceScore * nwords;
}

//---------------------------------------
void MiraChfr::sentScore(const std::string& candidate,
                         const std::string& reference,
                         double& score)
{
      // TO-BE-DONE
}

//---------------------------------------
void MiraChfr::corpusScore(const Vector<std::string>& candidates,
                           const Vector<std::string>& references,
                           double& score)
{
      // TO-BE-DONE
}
