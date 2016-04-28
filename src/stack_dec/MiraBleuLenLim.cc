


//--------------- Include files --------------------------------------

#include "MiraBleuLenLim.h"
#include "bleu.h"

#include <sstream>
//--------------- KbMiraLlWu class functions

//---------------------------------------
double MiraBleuLenLim::scoreFromStats(Vector<unsigned int>& stats){
  double bp;
  if (stats[0] < stats[1])
    bp = (double)exp((double)1-(double)stats[1]/stats[0]);
  else bp = 1;

  double log_aux = 0;
  for (unsigned int sz=1; sz<=N; sz++) {
    unsigned int prec = stats[sz*2];
    unsigned int total = stats[sz*2+1];
    if (total == 0) log_aux += 1;
    else            log_aux += (double)my_log((double)prec/total);
  }
  log_aux /= N;
  return bp * (double)exp(log_aux);
}

//---------------------------------------
void MiraBleuLenLim::statsForSentence(const Vector<std::string>& candidate_tokens,
                                      const Vector<std::string>& reference_tokens,
                                      Vector<unsigned int>& stats)
{
  stats.clear();

  unsigned int prec, total;
  stats.push_back(candidate_tokens.size());
  stats.push_back(reference_tokens.size());
  for (unsigned int sz=1; sz<=N; sz++) {
    prec_n(reference_tokens, candidate_tokens, sz, prec, total);
    stats.push_back(prec);
    stats.push_back(total);
  }
}

//---------------------------------------
void MiraBleuLenLim::split(const std::string& sentence,
                     Vector<std::string>& tokens)
{
  std::string item;
  std::stringstream ss(sentence);
  while (std::getline(ss, item, ' '))
    tokens.push_back(item);
}

//---------------------------------------
void MiraBleuLenLim::sentBackgroundScore(const std::string& candidate,
                                         const std::string& reference,
                                         double& bleu,
                                         Vector<unsigned int>& sentStats)
{
  Vector<std::string> candidate_tokens, reference_tokens;
  split(candidate, candidate_tokens);
  split(reference, reference_tokens);

  if (candidate.size() > CHARACTER_LIMIT) {
    bleu = 0.0;
    unsigned int nRefTok = reference_tokens.size();
    sentStats.clear();
    sentStats.push_back(0);
    sentStats.push_back(nRefTok);
    for (unsigned int i=0; i<N; i++) {
      sentStats.push_back(0);
      sentStats.push_back(max((unsigned int)0, nRefTok - i));
    }
  }
  else {
    statsForSentence(candidate_tokens, reference_tokens, sentStats);
    Vector<unsigned int> stats;
    for (unsigned int i=0; i<N_STATS; i++)
      stats.push_back(sentStats[i] + backgroundBleu[i]);
    // scale bleu to roughly typical margins
    bleu = scoreFromStats(stats) * reference_tokens.size();
  }
}

//---------------------------------------
void MiraBleuLenLim::Score(const Vector<std::string>& candidates,
                           const Vector<std::string>& references,
                           double& bleu)
{
  bool overLimit = false;
  Vector<unsigned int> corpusStats(N_STATS, 0);
  for (unsigned int i=0; i<candidates.size(); i++) {
    Vector<std::string> candidate_tokens, reference_tokens;
    split(candidates[i], candidate_tokens);
    split(references[i], reference_tokens);

    if (candidates[i].size() > CHARACTER_LIMIT)
      overLimit = true;

    Vector<unsigned int> stats;
    statsForSentence(candidate_tokens, reference_tokens, stats);

    for (unsigned int i=0; i<N_STATS; i++)
      corpusStats[i] += stats[i];
  }

  if (overLimit) {
    bleu = 0.0;
    for (unsigned int i=0; i< N_STATS/2; i++)
      corpusStats[2*i] = 0;
  }
  else
    bleu = scoreFromStats(corpusStats);
}

//---------------------------------------
void MiraBleuLenLim::sentScore(const std::string& candidate,
                               const std::string& reference,
                               double& bleu)
{
  Vector<std::string> candidate_tokens, reference_tokens;
  split(candidate, candidate_tokens);
  split(reference, reference_tokens);

  Vector<unsigned int> stats;
  if (candidate.size() > CHARACTER_LIMIT) {
    bleu = 0.0;
    unsigned int nRefTok = reference_tokens.size();
    stats.clear();
    stats.push_back(0);
    stats.push_back(nRefTok);
    for (unsigned int i=0; i<N; i++) {
      stats.push_back(0);
      stats.push_back(max((unsigned int)0, nRefTok - i));
    }
  }
  else {
    statsForSentence(candidate_tokens, reference_tokens, stats);
    for (unsigned int i=0; i<N_STATS; i++)
      stats[i] += 1;
    bleu = scoreFromStats(stats);
  }
}
