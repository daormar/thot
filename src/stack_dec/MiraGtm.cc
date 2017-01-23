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

#include <map>
#include <set>
#include <sstream>
#include <algorithm>    // std::sort


//--------------- KbMiraLlWu class functions


//---------------------------------------
void MiraGtm::split(const std::string& sentence,
                     Vector<std::string>& tokens)
{
  std::string item;
  std::stringstream ss(sentence);
  while (std::getline(ss, item, ' '))
    tokens.push_back(item);
}

//---------------------------------------
double MiraGtm::scoreFromStats(Vector<unsigned int>& stats){
  // stats = [matchings, candidate_len, reference_len]
  double pre, rec, f1;

  pre = double(stats[0])/stats[1];
  rec = double(stats[0])/stats[2];

  f1 = ((1+beta*beta) * prec*rec) / (beta*beta*precision + recall);
  return f1;
}

void MiraGtm::positional_ngrams(const Vector<std::string>& s,
                                std::map<Vector<std::string>,std::set<int> >& ngs) {
  //std::map<Vector<std::string>,std::set<int> > ngs;
  ngs.clear();
  for (unsigned int i=0; i<s.size(); i++) {
    for (unsigned int j=i; j<s.size(); j++) {
      Vector<std::string>::const_iterator first = s.begin() + i;
      Vector<std::string>::const_iterator last = s.begin() + j+1;
      Vector<std::string> ng(first, last); 
      if ( ngs.find(ng) == ngs.end() ) {
        // not found -> initialize
        std::set<int> positions;
        ngs[ng] = positions;
      } 
      for (unsigned int k=i; k<=j; k++)
        ngs[ng].insert(k);
    }
  }
}

//---------------------------------------
void revCompFunction(std::pair<int, std::pair<std::set<int>, std::set<int> > > a,
                     std::pair<int, std::pair<std::set<int>, std::set<int> > > b) {
  return a.first > b.first;
}

bool doIntersect(std::set<int> a, std::set<int> b) {
  std::set<int>::const_iterator it;
  for (it=a.begin(); it!=a.end(); ++it) {
    if (b.find(*it) != b.end())
      return false;
  }
  return true;
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
  int clen += candidate_tokens.size();
  int rlen += reference_tokens.size();

  std::map<Vector<std::string>,std::set<int> > cng, rng;
  positional_ngrams(candidate_tokens, cng)
  positional_ngrams(reference_tokens, rng)

  Vector<std::pair<int, std::pair<std::set<int>, std::set<int> > > > ngs;
  std::map<Vector<std::string>,std::set<int> >::iterator cit = cng.begin();
  while(cit != cng.end()) {
    std::map<Vector<std::string>,std::set<int> >::iterator rit = rng.find(it.first); 
    if ( rit != rng.end() ) {
      std::pair<int, std::pair<std::set<int>, std::set<int> > > entry (cit.first.size(), (cit.second, rit.second));
      ngs.push_back(entry);
    }
  }
  std::sort(ngs.begin(), ngs.end(), revCompFunction); 

  std::set<int> ccover, rcover;
  int matches = 0;
  Vector<std::pair<int, std::pair<std::set<int>, std::set<int> > > >::const_iterator it;
  for (it=ngs.begin(); it!=ngs.end(); ++it) {
    if (!doIntersect(it.second.first, ccover) and !doIntersect(it.second.second, rcover) ) {
      matches += it.first;
      ccover.insert(it.second.first.begin(), it.second.first.end());
      rcover.insert(it.second.second.begin(), it.second.second.end());
    }
  }

  stats[0] = matches;
  stats[1] = clen;
  stats[2] = rlen;
}

//---------------------------------------
void MiraGtm::sentBackgroundScore(const std::string& candidate,
                                  const std::string& reference,
                                  double& score,
                                  Vector<unsigned int>& sentStats)
{
  return sentScore(candidate, reference, score);
}

//---------------------------------------
void MiraGtm::corpusScore(const Vector<std::string>& candidates,
                          const Vector<std::string>& references,
                          double& score)
{
  Vector<unsigned int> corpusStats(N_STATS, 0);
  for (unsigned int i=0; i<candidates.size(); i++) {
    Vector<std::string> candidate_tokens, reference_tokens;
    split(candidates[i], candidate_tokens);
    split(references[i], reference_tokens);

    Vector<unsigned int> stats;
    statsForSentence(candidate_tokens, reference_tokens, stats);

    for (unsigned int i=0; i<N_STATS; i++)
      corpusStats[i] += stats[i];
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
