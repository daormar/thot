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
/* Module: BatchKbMiraWu                                            */
/*                                                                  */
/* Definitions file: BatchKbMiraWu.cc                               */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "KbMiraLlWu.h"

#include <cassert>
#include <algorithm>

//--------------- KbMiraWu class functions
//

//---------------------------------------
void KbMiraLlWu::update(const std::string& reference,
                        const Vector<std::string>& nblist,
                        const Vector<Vector<Score> >& scoreCompsVec,
                        const Vector<Score>& currWeightsVec,
                        Vector<Score>& newWeightsVec)
{
    //  TO-BE-DONE
}

//---------------------------------------
void KbMiraLlWu::updateClosedCorpus(const Vector<std::string>& references,
                                    const Vector<Vector<std::string> >& nblists,
                                    const Vector<Vector<Vector<Score> > >& scoreCompsVecs,
                                    const Vector<Score>& currWeightsVec,
                                    Vector<Score>& newWeightsVec)
{
  Score c = 0.01;            // Step-size cap C
  Score decay = 0.999;       // Pseudo-corpus decay \gamma
  unsigned int nIters = 60;  // Max epochs J
  unsigned int nSents = references.size(); // number of examples in the corpus
  Vector<unsigned int> bg(10, 1);           // background corpus stats for BLEU
                                            // cand_len, ref_len
                                            // matching, totals for n 1..4

  assert(nblists.size() == nSents);
  assert(scoreCompsVecs.size() == nSents);

  unsigned int nSolutions = 0;
  Vector<Score> wTotals(currWeightsVec);

  Vector<Score> wt(currWeightsVec);
  for(unsigned int j=0; j<nIters; j++) {
    // MIRA train for one epoch
    for(unsigned int i=0; i<nSents; i++) {
      Vector<std::string> nBests(nblists[i]);
      Vector<Vector<Scores> > nScores(scoreCompsVecs[i]);
      assert (nBests.size() == nScores.size());
      HopeFearData hfd;
      HopeFear(nBests, nScores, wt, bg, &hfd);
      // Update weights
      if (hfd.hopeBleu  > hfd.fearBleu) {
        // Vector difference
        Vector<Score> diff(hfd.hopeFeatures.size());
        for(unsigned int k=0; k<diff.size(); k++) {
            diff[i] = hfd.fearFeatures[i] - hfd.hopeFeatures[i]
        }
        // Bleu difference
        Score delta = hfd.hopeBleu - hfd.fearBleu;
        // Loss and update
        Score diffScore = 0;
        for(unsigned int k=0; k<diff.size(); k++) {
          diffScore += wt[k]*diff[k]
        }
        Score loss = delta + diffScore;
        if(verbose) {
          cerr << "Updating sent " << i << endl;
          cerr << "Wght: " << wt << endl;
          cerr << "Hope: " << hfd.hopeFeatures << " BLEU:" << hfd.hopeBleu << " Score:" << hfd.hopeScore << endl;
          cerr << "Fear: " << hfd.fearFeatures << " BLEU:" << hfd.fearBleu << " Score:" << hfd.fearScore << endl;
          cerr << "Diff: " << diff << " BLEU:" << delta << " Score:" << diffScore << endl;
          cerr << "Loss: " << loss <<  " Scale: " << 1 << endl;
          cerr << endl;
        }
        if(loss > 0) {
          diffNorm = 0;
          for(unsigned int k=0; k<diff.size(); k++) {
            diffNorm += diff[i]*diff[i];
          }
          Score eta = min(c, loss/diffNorm);
          for(unsigned int k=0; k<diff.size(); k++) {
            wt[i] = wt[i] + eta*diff[i];
            wTotals[i] += wt[i];
            nSolutions++;
          }
        }
        // Update BLEU statistics
        for(unsigned int k=0; k<bg.size(); k++) {
          bg[k] = decay*bg[k] + hfd.hopeBleuStats[k];
        }
      }
    }
    Vector<Score> wAvg(wTotals.size(), 0);
    for(unsigned int k=0; k<wAvg.size(); k++) {
      wAvg[i] = wTotals[i]/nSolutions;
    }
    // TODO: evaluate bleu of wAvg
 }
}

//---------------------------------------
void KbMiraLlWu::HopeFear(const std::string reference,
                          const Vector<std::string>& nBest,
                          const Vector<Vector<Score> >& nScores,
                          const Vector<Score>& wv,
                          const Vector<unsigned int>& backgroundBleu,
                          HopeFearData* hopeFear)
{
  // Hope / fear decode
  Score hope_scale = 1.0;
  Score hope_total_score, fear_total_score;
  for(unsigned int i=0; i<nBest.size(); i++) {
    Score score = 0;
    for(unsigned int k=0; k<sentScores.size(); k++) {
      score += wv[i]*nScores[i][k];
    }
    Score bleu;
    Vector<unsigned int> stats(backgroundBleu.size());
    backgroundBleu(nBest[i], reference, backgroundBleu, &bleu, stats);
    // Hope
    if(i==0 || (hope_scale*score + bleu) > hope_total_score) {
      hope_total_score = hope_scale*score + bleu;
      hopeFear->hopeScore = score;
      for(unsigned int k=0; k<nScores[i].size(); k++) {
        hopeFear->hopeFeatures.clear();
        hopeFear->hopeFeatures.push_back(nScores[i][k]);
      }
      hopeFear->hopeBleu = bleu;
      for(unsigned int k=0; k<stats.size(); k++) {
        hopeFear->hopeBleuStats.clear();
        hopeFear->hopeBleuStats.push_back(stats[k]);
      }
    }
    // Fear
    if(i==0 || (score - bleu) > fear_total_score) {
      fear_total_score = score - bleu;
      hopeFear->fearScore = score;
      for(unsigned int k=0; k<nScores[i].size(); k++) {
        hopeFear->fearFeatures.clear();
        hopeFear->fearFeatures.push_back(nScores[i][k]);
      }
      hopeFear->fearBleu = bleu;
    }
  }
}

//---------------------------------------
void KbMiraLlWu::backgroundBleu(std::string candidate, std::string reference,
                                Vector<unsigned int>& backgroundBleu,
                                Score* bleu,
                                Vector<unsigned int> stats)
{
  // TODO: get stats for pair of sentences, compute bleu escaled by ref lenght

}
