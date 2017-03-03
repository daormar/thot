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

#include "MiraWer.h"

//--------------- MiraWer class functions

//---------------------------------------
void MiraWer::sentBackgroundScore(const std::string& candidate,
                                  const std::string& reference,
                                  double& score,
                                  Vector<unsigned int>& sentStats)
{
  Vector<std::string> candidate_tokens, reference_tokens;
  split(candidate, candidate_tokens);
  split(reference, reference_tokens);

  if (reference_tokens.size() == 0)
    score = 0.0;
  else {
    int nedits = ed(candidate_tokens, reference_tokens);
    int nwords = reference_tokens.size();
    // Scale score for mira
    score = (1.0 - double(nedits)/nwords) * nwords;
  }
}

//---------------------------------------
void MiraWer::sentScore(const std::string& candidate,
                        const std::string& reference,
                        double& score)
{
  Vector<std::string> candidate_tokens, reference_tokens;
  split(candidate, candidate_tokens);
  split(reference, reference_tokens);

  if (reference_tokens.size() == 0)
    score = 0.0;
  else {
    int nedits = ed(candidate_tokens, reference_tokens);
    int nwords = reference_tokens.size();
    score = 1.0 - double(nedits)/nwords;
  }
}

//---------------------------------------
void MiraWer::corpusScore(const Vector<std::string>& candidates,
                          const Vector<std::string>& references,
                          double& score)
{
  int nedits = 0, nwords = 0;
  for (unsigned int i=0; i<candidates.size(); i++) {
    Vector<std::string> candidate_tokens, reference_tokens;
    split(candidates[i], candidate_tokens);
    split(references[i], reference_tokens);

    nedits += ed(candidate_tokens, reference_tokens);
    nwords += reference_tokens.size();
  }
  if (nwords == 0)
    score = 0.0;
  else
    score = 1.0 - double(nedits)/nwords; 
}

//---------------------------------------
int MiraWer::ed(Vector<std::string>& s1, Vector<std::string>& s2) 
{
  const std::size_t len1 = s1.size(), len2 = s2.size();
  std::vector<unsigned int> col(len2+1), prevCol(len2+1);
  
  for (unsigned int i = 0; i < prevCol.size(); i++)
    prevCol[i] = i;
  for (unsigned int i = 0; i < len1; i++) {
    col[0] = i+1;
    for (unsigned int j = 0; j < len2; j++)
                        // note that std::min({arg1, arg2, arg3}) works only in C++11,
                        // for C++98 use std::min(std::min(arg1, arg2), arg3)
      //col[j+1] = std::min({ prevCol[1 + j] + 1, col[j] + 1, prevCol[j] + (s1[i]==s2[j] ? 0 : 1) });
      col[j+1] = std::min(std::min(prevCol[1+j] + 1, col[j] + 1), prevCol[j]+( s1[i]==s2[j] ? 0 : 1) );
    col.swap(prevCol);
  }
  return prevCol[len2];
}

