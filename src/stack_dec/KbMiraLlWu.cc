/*
thot package for statistical machine translation
Copyright (C) 2013 Daniel Ortiz-Mart\'inez

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

/********************************************************************/
/*                                                                  */
/* Module: KbMiraLlWu                                               */
/*                                                                  */
/* Definitions file: KbMiraLlWu.cc                                  */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "KbMiraLlWu.h"

#include "bleu.h"

#include <cstdlib>
#include <cassert>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <iostream>

//--------------- KbMiraLlWu class functions

//---------------------------------------
KbMiraLlWu::KbMiraLlWu(double C,
                       double gamma,
                       unsigned int J,
                       unsigned int epochs_to_restart,
                       unsigned int max_restarts) {
  c = C;
  decay = gamma;
  nIters = J;
  epochsToRestart = epochs_to_restart;
  maxRestarts = max_restarts;
};

//---------------------------------------
void KbMiraLlWu::update(const std::string& reference,
                        const Vector<std::string>& nblist,
                        const Vector<Vector<double> >& scoreCompsVec,
                        const Vector<double>& currWeightsVec,
                        Vector<double>& newWeightsVec)
{
  srand(KBMIRA_RANDOM_SEED);

      // FIXME: this should be changed by a scorer class
  Vector<unsigned int> bg(10, 1);  // background corpus stats for BLEU
                                   // cand_len, ref_len
                                   // matching, totals for n 1..4
  assert (nblist.size() == scoreCompsVec.size());

  unsigned int nUpdates = 1;
  Vector<double> wTotals(currWeightsVec);
  Vector<double> max_wAvg;

  unsigned int nReStarts=0;
  double bleu, iter_max_bleu = 0, iter_max_j = 0, max_bleu = 0;
  Vector<double> wt(currWeightsVec);
  for(unsigned int j=0; j<nIters; j++) {
    // MIRA train for one epoch
    HopeFearData hfd;
    HopeFear(reference, nblist, scoreCompsVec, wt, bg, &hfd);
    if (hfd.hopeBleu  > hfd.fearBleu) {
      Vector<double> diff(hfd.hopeFeatures.size());
      for (unsigned int k=0; k<diff.size(); k++)
        diff[k] = hfd.hopeFeatures[k] - hfd.fearFeatures[k];
      double delta = hfd.hopeBleu - hfd.fearBleu;
      double diffScore = 0;
      for (unsigned int k=0; k<diff.size(); k++)
        diffScore += wt[k]*diff[k];
      double loss = delta - diffScore;

      if (loss > 0) {
        // Update weights
        double diffNorm = 0;
        for (unsigned int k=0; k<diff.size(); k++)
          diffNorm += diff[k]*diff[k];
        double eta = min(c, loss/diffNorm);
        for (unsigned int k=0; k<diff.size(); k++) {
          wt[k] += eta*diff[k];
          wTotals[k] += wt[k];
        }
        nUpdates++;
      }
      // Update BLEU statistics
      for (unsigned int k=0; k<bg.size(); k++)
        bg[k] = decay*bg[k] + hfd.hopeBleuStats[k];
    }

    // average all seen weight vectors
    Vector<double> wAvg(wTotals.size(), 0);
    for (unsigned int k=0; k<wAvg.size(); k++)
      wAvg[k] = wTotals[k]/nUpdates;

    // evaluate bleu of wAvg
    std::string maxTranslation;
    MaxTranslation(wAvg, nblist, scoreCompsVec, maxTranslation);
    sentenceBleu(maxTranslation, reference, bleu);
    if (bleu > iter_max_bleu) {
      iter_max_j = j;
      iter_max_bleu = bleu;
      if (iter_max_bleu > max_bleu) {
        max_bleu = iter_max_bleu;
        max_wAvg = wAvg;
      }
    }

    cerr << nReStarts << " " << j << " " << iter_max_j << " " << bleu << " " << max_bleu << endl;

    // restart weights if no improvement in X epochs;
    if (j-iter_max_j > epochsToRestart) {
      if (nReStarts < maxRestarts) {
        nReStarts++;
        j = 0;
        iter_max_j = 0;
        iter_max_bleu = 0;
        nUpdates = 1;
        for(unsigned int k=0; k<wt.size(); k++) {
          wt[k] = currWeightsVec[k];
          wTotals[k] = currWeightsVec[k];
        }
      }
      else break;
    }
  }
  newWeightsVec = max_wAvg;
}

//---------------------------------------
void KbMiraLlWu::updateClosedCorpus(const Vector<std::string>& references,
                                    const Vector<Vector<std::string> >& nblists,
                                    const Vector<Vector<Vector<double> > >& scoreCompsVecs,
                                    const Vector<double>& currWeightsVec,
                                    Vector<double>& newWeightsVec)
{
  srand(KBMIRA_RANDOM_SEED);

  // FIXME: this should be changed by a scorer class
  Vector<unsigned int> bg(10, 1);          // background corpus stats for BLEU
                                           // cand_len, ref_len
                                           // matching, totals for n 1..4
  unsigned int nSents = references.size(); // number of examples in the corpus
  assert(nblists.size() == nSents);
  assert(scoreCompsVecs.size() == nSents);

  unsigned int nUpdates = 1;
  Vector<double> wTotals(currWeightsVec);
  Vector<double> max_wAvg;

  unsigned int nReStarts = 0;
  double bleu, iter_max_bleu = 0, iter_max_j = 0, max_bleu = 0;
  Vector<double> wt(currWeightsVec);
  for(unsigned int j=0; j<nIters; j++) {
    // MIRA train for one epoch
    Vector<unsigned int> indices(nSents);
    sampleWoReplacement(nSents, indices);
    for (unsigned int z=0; z<nSents; z++) {
      unsigned int i = indices[z];

      assert (nblists[i].size() == scoreCompsVecs[i].size());
      HopeFearData hfd;
      HopeFear(references[i], nblists[i], scoreCompsVecs[i], wt, bg, &hfd);
      if (hfd.hopeBleu  > hfd.fearBleu) {
        Vector<double> diff(hfd.hopeFeatures.size());
        for (unsigned int k=0; k<diff.size(); k++)
          diff[k] = hfd.hopeFeatures[k] - hfd.fearFeatures[k];
        double delta = hfd.hopeBleu - hfd.fearBleu;
        double diffScore = 0;
        for (unsigned int k=0; k<diff.size(); k++)
          diffScore += wt[k]*diff[k];
        double loss = delta - diffScore;

        if (loss > 0) {
          // Update weights
          double diffNorm = 0;
          for (unsigned int k=0; k<diff.size(); k++)
            diffNorm += diff[k]*diff[k];
          double eta = min(c, loss/diffNorm);
          for (unsigned int k=0; k<diff.size(); k++) {
            wt[k] += eta*diff[k];
            wTotals[k] += wt[k];
          }
          nUpdates++;
        }
        // Update BLEU statistics
        for (unsigned int k=0; k<bg.size(); k++)
          bg[k] = decay*bg[k] + hfd.hopeBleuStats[k];
      }
    }

    // average all seen weight vectors
    Vector<double> wAvg(wTotals.size(), 0);
    for (unsigned int k=0; k<wAvg.size(); k++)
      wAvg[k] = wTotals[k]/nUpdates;

    // evaluate bleu of wAvg
    std::string maxTranslation;
    Vector<std::string> maxTranslations;
    for (unsigned int i=0; i<nSents; i++) {
      MaxTranslation(wAvg, nblists[i], scoreCompsVecs[i], maxTranslation);
      maxTranslations.push_back(maxTranslation);
    }
    Bleu(maxTranslations, references, bleu);
    if (bleu > iter_max_bleu) {
      iter_max_j = j;
      iter_max_bleu = bleu;
      if (iter_max_bleu > max_bleu) {
        max_bleu = iter_max_bleu;
        max_wAvg = wAvg;
      }
    }

    cerr << nReStarts << " " << j << " " << iter_max_j << " " << bleu << " " << max_bleu << endl;

    // restart weights if no improvement in X epochs;
    if (j-iter_max_j > epochsToRestart) {
      if (nReStarts < maxRestarts) {
        nReStarts++;
        j = 0;
        iter_max_j = 0;
        iter_max_bleu = 0;
        nUpdates = 1;
        for(unsigned int k=0; k<wt.size(); k++) {
          wt[k] = currWeightsVec[k];
          wTotals[k] = currWeightsVec[k];
        }
      }
      else break;
    }
  }
  cerr << "FW: [ ";
  for (unsigned int k=0; k<max_wAvg.size(); k++)
    cerr << max_wAvg[k] << " ";
  cerr << "]" << endl;
  newWeightsVec = max_wAvg;
}

//---------------------------------------
void KbMiraLlWu::MaxTranslation(const Vector<double>& wv,
                                const Vector<std::string>& nBest,
                                const Vector<Vector<double> >& nScores,
                                std::string& maxTranslation)
{
  double max_score=-DBL_MAX;
  for (unsigned int n=0; n<nBest.size(); n++) {
    double score = 0;
    for (unsigned int k=0; k<wv.size(); k++)
      score += wv[k]*nScores[n][k];
    //cout << " * " << score << " " << nBest[n] << endl;
    if (score > max_score) {
        max_score = score;
        maxTranslation = nBest[n];
    }
  }
}

//---------------------------------------
void KbMiraLlWu::HopeFear(const std::string& reference,
                          const Vector<std::string>& nBest,
                          const Vector<Vector<double> >& nScores,
                          const Vector<double>& wv,
                          const Vector<unsigned int>& backgroundBleu,
                          HopeFearData* hopeFear)
{
  // Hope / fear decode
  double hope_scale = 1.0;
  double hope_total_score=-DBL_MAX;
  double fear_total_score=-DBL_MAX;

  //cout << "R: " << reference << endl;

  for (unsigned int n=0; n<nBest.size(); n++) {
    double score = 0;
    for (unsigned int k=0; k<wv.size(); k++)
      score += wv[k]*nScores[n][k];
    double bleu;
    Vector<unsigned int> stats(backgroundBleu.size());
    sentBckgrndBleu(nBest[n], reference, backgroundBleu, bleu, stats);

    // Hope
    if ((hope_scale*score + bleu) > hope_total_score) {
      //cout << "H (" << hope_scale*score+bleu << ") " << nBest[n] << " " << hope_scale*score << " " << bleu << endl;
      hope_total_score = hope_scale*score + bleu;
      hopeFear->hopeScore = score;
      hopeFear->hopeFeatures.clear();
      for (unsigned int k=0; k<nScores[n].size(); k++)
        hopeFear->hopeFeatures.push_back(nScores[n][k]);
      hopeFear->hopeBleu = bleu;
      hopeFear->hopeBleuStats.clear();
      for (unsigned int k=0; k<stats.size(); k++)
        hopeFear->hopeBleuStats.push_back(stats[k]);
    }
    // Fear
    if ((score - bleu) > fear_total_score) {
      //cout << "F (" << score-bleu << ") " << nBest[n] << " " << score << " " << bleu << endl;
      fear_total_score = score - bleu;
      hopeFear->fearScore = score;
      hopeFear->fearFeatures.clear();
      for (unsigned int k=0; k<nScores[n].size(); k++)
        hopeFear->fearFeatures.push_back(nScores[n][k]);
      hopeFear->fearBleu = bleu;
    }
  }
}

//---------------------------------------
// FIXME: substitute by scoring class
void KbMiraLlWu::sentBckgrndBleu(const std::string& candidate,
                                 const std::string& reference,
                                 const Vector<unsigned int>& backgroundBleu,
                                 double& bleu,
                                 Vector<unsigned int>& stats)
{
  Vector<std::string> candidate_tokens, reference_tokens;
  std::string item;

  std::stringstream ssc(candidate);
  while (std::getline(ssc, item, ' '))
    candidate_tokens.push_back(item);

  std::stringstream ssr(reference);
  while (std::getline(ssr, item, ' '))
    reference_tokens.push_back(item);

  unsigned int prec, total;
  stats[0] = candidate_tokens.size();
  stats[1] = reference_tokens.size();
  for (unsigned int sz=1; sz<=4; sz++) {
    prec_n(reference_tokens, candidate_tokens, sz, prec, total);
    stats[2*sz] = prec;
    stats[2*sz+1] = total;
  }
  // calculate brevity penalty
  double bp;
  unsigned int rLen, cLen;
  cLen = stats[0] + backgroundBleu[0];
  rLen = stats[1] + backgroundBleu[1];
  if (cLen < rLen)
    bp = (double)exp(1-(double)rLen/cLen);
  else bp = 1;

  // calculate bleu
  double log_aux = 0;
  for (unsigned int sz=1; sz<=4; sz++) {
    prec = stats[sz*2] + backgroundBleu[sz*2];
    total = stats[sz*2+1] + backgroundBleu[sz*2+1];
    if (total == 0) log_aux += 1;
    else            log_aux += my_log((double)prec/total);
  }
  log_aux /= 4;
  bleu = bp * (double)exp(log_aux);
  bleu *= rLen; // scale bleu to roughly typical margins
}

//---------------------------------------
// FIXME: substitute by scoring class
void KbMiraLlWu::Bleu(const Vector<std::string>& candidates,
                      const Vector<std::string>& references,
                      double& bleu)
{
  Vector<unsigned int> stats(10, 0);
  unsigned int prec, total;
  for (unsigned int i=0; i<candidates.size(); i++) {
    Vector<std::string> candidate_tokens, reference_tokens;
    std::string item;

    std::stringstream ssc(candidates[i]);
    while (std::getline(ssc, item, ' '))
      candidate_tokens.push_back(item);

    std::stringstream ssr(references[i]);
    while (std::getline(ssr, item, ' '))
      reference_tokens.push_back(item);

    stats[0] += candidate_tokens.size();
    stats[1] += reference_tokens.size();
    for (unsigned int sz=1; sz<=4; sz++) {
      prec_n(reference_tokens, candidate_tokens, sz, prec, total);
      stats[2*sz] += prec;
      stats[2*sz+1] += total;
    }
  }
  // calculate brevity penalty
  double bp;
  if (stats[0] < stats[1])
    bp = (double)exp((double)1-(double)stats[1]/stats[0]);
  else bp = 1;

  // calculate bleu
  double log_aux = 0;
  for (unsigned int sz=1; sz<=4; sz++) {
    prec = stats[sz*2];
    total = stats[sz*2+1];
    if (total == 0) log_aux += 1;
    else            log_aux += (double)my_log((double)prec/total);
  }
  log_aux /= 4;
  bleu = bp * (double)exp(log_aux);
}

//---------------------------------------
// FIXME: substitute by scoring class
void KbMiraLlWu::sentenceBleu(const std::string& candidate,
                              const std::string& reference,
                              double& bleu)
{
  Vector<unsigned int> stats(10, 1);
  unsigned int prec, total;
  Vector<std::string> candidate_tokens, reference_tokens;
  std::string item;

  std::stringstream ssc(candidate);
  while (std::getline(ssc, item, ' '))
    candidate_tokens.push_back(item);

  std::stringstream ssr(reference);
  while (std::getline(ssr, item, ' '))
    reference_tokens.push_back(item);

  stats[0] += candidate_tokens.size();
  stats[1] += reference_tokens.size();
  for (unsigned int sz=1; sz<=4; sz++) {
    prec_n(reference_tokens, candidate_tokens, sz, prec, total);
    stats[2*sz] += prec;
    stats[2*sz+1] += total;
  }

  // calculate brevity penalty
  double bp;
  if (stats[0] < stats[1])
    bp = (double)exp((double)1-(double)stats[1]/stats[0]);
  else bp = 1;

  // calculate bleu
  double log_aux = 0;
  for (unsigned int sz=1; sz<=4; sz++) {
    prec = stats[sz*2];
    total = stats[sz*2+1];
    if (total == 0) log_aux += 1;
    else            log_aux += (double)my_log((double)prec/total);
  }
  log_aux /= 4;
  bleu = bp * (double)exp(log_aux);
}


//---------------------------------------
void KbMiraLlWu::sampleWoReplacement(unsigned int nSamples,
                                     vector<unsigned int>& indices)
{
  // create indices array
  for (unsigned int k=0; k<nSamples; k++)
    indices[k] = k;
  // shuffle
  int tmp, swap_k;
  for (int k=nSamples-1; k>0; k--) {
    swap_k = rand() % k;
    tmp = indices[k];
    indices[k] = indices[swap_k];
    indices[swap_k] = tmp;
  }
}
