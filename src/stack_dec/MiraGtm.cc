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

#include "MiraGtm.h"

#include <cmath>
#include <sstream>
#include <algorithm>    // std::sort


//--------------- KbMiraLlWu class functions


//---------------------------------------
void MiraGtm::split(const std::string& sentence,
           Vector<std::string>& tokens)
{
  std::string item;
  std::stringstream ss(sentence);
  tokens.clear();
  while (std::getline(ss, item, ' '))
    tokens.push_back(item);
}

//---------------------------------------
bool MiraGtm::revCompFunction(std::pair<int, std::pair<std::pair<int,int>, std::pair<int,int> > > a,
                              std::pair<int, std::pair<std::pair<int,int>, std::pair<int,int> > > b) {
  return a.first > b.first;
}

bool MiraGtm::doIntersect(std::pair<int,int> a, std::set<int> b) {
  for (int it=a.first; it<=a.second; ++it) {
    if (b.find(it) != b.end())
      return true;
  }
  return false;
}

//---------------------------------------
void MiraGtm::sorted_common_ngrams(const Vector<std::string>& s1,
                                   const Vector<std::string>& s2,
                                   Vector<std::pair<int, std::pair<std::pair<int,int>, std::pair<int,int> > > >& ngs) {

  ngs.clear();
  if(!s1.empty() && !s2.empty()) {
    int *curr = new int [s2.size()];
    int *prev = new int [s2.size()];
    int *swap;

    for (unsigned int i=0; i<s1.size(); ++i) {
      for (unsigned int j=0; j<s2.size(); ++j) {
        if (s1[i]!=s2[j])
          curr[j] = 0;
        else {
          if (i==0 || j==0)
            curr[j] = 1;
          else
            curr[j] = 1 + prev[j-1];

          // new common ngram s1[i-z+1..i] == s2[j-z+1..j]
          // add all subngrams
          for (int sz=curr[j]; sz>0; --sz) {
            std::pair<int,int> s1_pos(i-sz+1, i);
            std::pair<int,int> s2_pos(j-sz+1, j);

            //for (unsigned int k=i-sz+1; k<=i; ++k) s1_pos.insert(k);
            //for (unsigned int k=j-sz+1; k<=j; ++k) s2_pos.insert(k);

            std::pair<std::pair<int,int>, std::pair<int,int> > info(s1_pos, s2_pos);
            std::pair<int, std::pair<std::pair<int,int>, std::pair<int,int> > > entry(sz, info);
            ngs.push_back(entry);
          }
        }
      }
      swap=curr;
      curr=prev;
      prev=swap;
    }
    delete [] curr;
    delete [] prev;
    std::sort(ngs.begin(), ngs.end(), revCompFunction);
  }
}

//---------------------------------------
void MiraGtm::statsForSentence(const Vector<std::string>& candidate_tokens,
                               const Vector<std::string>& reference_tokens,
                               Vector<unsigned int>& stats)
{
  stats.clear();
  for (unsigned int i=0; i<N_STATS; i++) {
    stats.push_back(0);
  }

  // compute mms, candidate_len and ref_len
  int clen = candidate_tokens.size();
  int rlen = reference_tokens.size();

  // cerr << endl << "Can:";
  // for (int i=0; i<clen; ++i)
  //   cerr << " " << candidate_tokens[i];
  // cerr << endl;
  // cerr << "Ref:";
  // for (int i=0; i<rlen; ++i)
  //   cerr << " " << reference_tokens[i];
  // cerr << endl;

  // std::sort(ngs.begin(), ngs.end(), revCompFunction);
  Vector<std::pair<int, std::pair<std::pair<int,int>, std::pair<int,int> > > > ngs;
  sorted_common_ngrams(candidate_tokens, reference_tokens, ngs);

  // Vector<std::pair<int, std::pair<std::pair<int,int>, std::pair<int,int> > > >::iterator itt;
  // std::set<int>::iterator sit;
  // for (itt=ngs.begin(); itt!=ngs.end(); ++itt) {
  //   cerr << itt->first << endl << " - " << itt->second.first.first << " <-> " << itt->second.first.second;
  //   cerr << endl << " - " << itt->second.second.first << " <-> " << itt->second.second.second;
  //   cerr << endl;
  // }
  // cerr << endl;


  std::set<int> ccover, rcover;
  int matches = 0;
  Vector<std::pair<int, std::pair<std::pair<int,int>, std::pair<int,int> > > >::const_iterator it;
  for (it=ngs.begin(); it!=ngs.end(); ++it) {
    if (!doIntersect(it->second.first, ccover) && !doIntersect(it->second.second, rcover) ) {
      matches += it->first;
      for(int i=it->second.first.first; i<=it->second.first.second; ++i) ccover.insert(i);
      for(int i=it->second.second.first; i<=it->second.second.second; ++i) rcover.insert(i);
      // ccover.insert(it->second.first.begin(), it->second.first.end());
      // rcover.insert(it->second.second.begin(), it->second.second.end());

      // cerr << it->first << endl << " - " << it->second.first.first << " <-> " << it->second.first.second;
      // cerr << endl << " - " << it->second.second.first << " <-> " << it->second.second.second;
      // cerr << endl;
    }
  }

  stats[0] = matches;
  stats[1] = clen;
  stats[2] = rlen;

  // cerr << stats[0] << " " << stats[1] << " " << stats[2] << endl;
}

//---------------------------------------
double MiraGtm::scoreFromStats(Vector<unsigned int>& stats){
  // stats = [matchings, candidate_len, reference_len]
  double pre, rec, f1;

  pre = double(stats[0])/stats[1];
  rec = double(stats[0])/stats[2];

  f1 = ((1+beta*beta) * pre*rec) / (beta*beta*pre + rec);
  return f1;
}

//---------------------------------------
void MiraGtm::sentBackgroundScore(const std::string& candidate,
                                  const std::string& reference,
                                  double& score,
                                  Vector<unsigned int>& sentStats)
{
  Vector<std::string> candidate_tokens, reference_tokens;
  split(candidate, candidate_tokens);
  split(reference, reference_tokens);

  Vector<unsigned int> stats;
  statsForSentence(candidate_tokens, reference_tokens, stats);

  // scale score for Mira
  score = scoreFromStats(stats)*stats[2];
}

//---------------------------------------
void MiraGtm::corpusScore(const Vector<std::string>& candidates,
                          const Vector<std::string>& references,
                          double& score)
{
  Vector<unsigned int> corpusStats(N_STATS, 0);
  Vector<std::string> candidate_tokens, reference_tokens;
  Vector<unsigned int> stats;
  for (unsigned int i=0; i<candidates.size(); i++) {
    split(candidates[i], candidate_tokens);
    split(references[i], reference_tokens);

    statsForSentence(candidate_tokens, reference_tokens, stats);
    for (unsigned int j=0; j<N_STATS; j++)
      corpusStats[j] += stats[j];
  }
  // cerr << "CS: [";
  // for(unsigned int k=0; k<N_STATS; k++)
  //   cerr << corpusStats[k] << " ";
  // cerr << "]" << endl;
  score = scoreFromStats(corpusStats);
}

//---------------------------------------
void MiraGtm::sentScore(const std::string& candidate,
                        const std::string& reference,
                        double& score)
{
  Vector<std::string> candidate_tokens, reference_tokens;
  split(candidate, candidate_tokens);
  split(reference, reference_tokens);

  Vector<unsigned int> stats;
  statsForSentence(candidate_tokens, reference_tokens, stats);

  score = scoreFromStats(stats);
}
