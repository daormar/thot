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

//--------------- KbMiraLlWu class functions

//---------------------------------------
KbMiraLlWu::KbMiraLlWu(double C,
                       double gamma,
                       unsigned int J,
                       unsigned int epochs_to_restart,
                       unsigned int max_restarts)
{
  c = C;
  decay = gamma;
  nIters = J;
  epochsToRestart = epochs_to_restart;
  maxRestarts = max_restarts;
}

//---------------------------------------
KbMiraLlWu::~KbMiraLlWu()
{
}

//---------------------------------------
bool KbMiraLlWu::link_scorer(BaseScorer* baseScorerPtr)
{
  scorer=dynamic_cast<BaseMiraScorer*>(baseScorerPtr);
  if(scorer)
    return true;
  else
    return false;
}

//---------------------------------------
void KbMiraLlWu::update(const std::string& reference,
                        const Vector<std::string>& nblist,
                        const Vector<Vector<double> >& scoreCompsVec,
                        const Vector<double>& currWeightsVec,
                        Vector<double>& newWeightsVec)
{
  srand(KBMIRA_RANDOM_SEED);

  assert (nblist.size() == scoreCompsVec.size());

  Vector<double> max_wAvg;
  double quality, max_quality = 0;

  scorer->resetBackgroundCorpus();
  for (unsigned int nReStarts=0; nReStarts<maxRestarts; nReStarts++) {
    double iter_max_quality = 0;
    unsigned int iter_max_j = 0, nUpdates = 1;
    Vector<double> wt(currWeightsVec);
    Vector<double> wTotals(currWeightsVec);

    for(unsigned int j=0; j<nIters; j++) {
      HopeFearData hfd;
      HopeFear(reference, nblist, scoreCompsVec, wt, &hfd);
      if (hfd.hopeQuality  > hfd.fearQuality) {
        Vector<double> diff(hfd.hopeFeatures.size());
        for (unsigned int k=0; k<diff.size(); k++)
          diff[k] = hfd.hopeFeatures[k] - hfd.fearFeatures[k];
        double delta = hfd.hopeQuality - hfd.fearQuality;
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
        scorer->updateBackgroundCorpus(hfd.hopeQualityStats, decay);
      }
      // average all seen weight vectors
      Vector<double> wAvg(wTotals.size(), 0);
      for (unsigned int k=0; k<wAvg.size(); k++)
        wAvg[k] = wTotals[k]/nUpdates;

      // evaluate bleu of wAvg
      std::string maxTranslation;
      MaxTranslation(wAvg, nblist, scoreCompsVec, maxTranslation);
      scorer->sentScore(maxTranslation, reference, quality);
      if (quality > iter_max_quality) {
        iter_max_j = j;
        iter_max_quality = quality;
        if (iter_max_quality > max_quality) {
          max_quality = iter_max_quality;
          max_wAvg = wAvg;
        }
      }
      cerr << nReStarts << " " << j << " " << iter_max_j << " " << quality << " " << max_quality << endl;

      // restart weights if no improvement in X epochs;
      if (j-iter_max_j > epochsToRestart)
        break;
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

  unsigned int nSents = references.size(); // number of examples in the corpus
  assert(nblists.size() == nSents);
  assert(scoreCompsVecs.size() == nSents);

  // //##########################################################################
  // // evaluate bleu of currWeightsVec
  // cerr << "CW: ";
  // for (unsigned int k=0; k<currWeightsVec.size(); k++)
  //   cerr << currWeightsVec[k] << " ";
  // cerr << "]" << endl;
  // std::string mT;
  // Vector<std::string> mTs;
  // for (unsigned int i=0; i<nSents; i++) {
  //   MaxTranslation(currWeightsVec, nblists[i], scoreCompsVecs[i], mT);
  //   mTs.push_back(mT);
  //   //cerr << i << " " << mT << endl;
  // }
  // double bleu;
  // scorer->corpusScore(mTs, references, bleu);
  // cerr << bleu << endl;
  // //##########################################################################

  Vector<double> max_wAvg;
  double quality, max_quality = 0;

  scorer->resetBackgroundCorpus();
  for (unsigned int nReStarts=0; nReStarts<maxRestarts; nReStarts++) {
    double iter_max_quality = 0;
    unsigned int iter_max_j = 0, nUpdates = 1;
    Vector<double> wt(currWeightsVec);
    Vector<double> wTotals(currWeightsVec);

    for (unsigned int j=0; j<nIters; j++) {
      Vector<unsigned int> indices(nSents);
      sampleWoReplacement(nSents, indices);
      for (unsigned int z=0; z<nSents; z++) {
        unsigned int i = indices[z];
        assert (nblists[i].size() == scoreCompsVecs[i].size());
        HopeFearData hfd;
        HopeFear(references[i], nblists[i], scoreCompsVecs[i], wt, &hfd);
        if (hfd.hopeQuality  > hfd.fearQuality) {
          Vector<double> diff(hfd.hopeFeatures.size());
          for (unsigned int k=0; k<diff.size(); k++)
            diff[k] = hfd.hopeFeatures[k] - hfd.fearFeatures[k];
          double delta = hfd.hopeQuality - hfd.fearQuality;
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
              //cerr << k << " : " << wt[k];
              wt[k] += eta*diff[k];
              //cerr << " ( " << eta << " " << diff[k] << " ) " << wt[k] << endl;
              wTotals[k] += wt[k];
            }
            nUpdates++;
          }
          scorer->updateBackgroundCorpus(hfd.hopeQualityStats, decay);
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
      scorer->corpusScore(maxTranslations, references, quality);
      if (quality > iter_max_quality) {
        iter_max_j = j;
        iter_max_quality = quality;
        if (iter_max_quality > max_quality) {
          max_quality = iter_max_quality;
          max_wAvg = wAvg;
        }
      }

      cerr << nReStarts << " " << j << " " << iter_max_j << " " << quality << " " << max_quality << endl;

      // restart weights if no improvement in X epochs;
      if (j-iter_max_j > epochsToRestart)
        break;
    }
  }
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
                          HopeFearData* hopeFear)
{
  // Hope / fear decode
  double hope_scale = 1.0;
  double hope_total_score=-DBL_MAX;
  double fear_total_score=-DBL_MAX;

  //cerr << "R: " << reference << endl;

  for (unsigned int n=0; n<nBest.size(); n++) {
    double score = 0;
    for (unsigned int k=0; k<wv.size(); k++)
      score += wv[k]*nScores[n][k];
    double quality;
    Vector<unsigned int> qStats;
    scorer->sentBackgroundScore(nBest[n], reference, quality, qStats);

    // cerr << nBest[n] << endl;
    // cerr << reference << endl;
    // cerr << quality << " : [ ";
    // for (unsigned int k=0; k<qStats.size(); k++)
    //   cerr << qStats[k] << " ";
    // cerr << "] " << score << " " << quality;
    // cerr << "( " << hope_scale*score + quality << " " << hope_total_score;
    // cerr << " , " << score-quality << " " << fear_total_score << " )" << endl;

    // Hope
    if ((hope_scale*score + quality) > hope_total_score) {
      hope_total_score = hope_scale*score + quality;
      // cerr << n << " : " << nBest[n] << score << " " << quality << " " << hope_total_score << endl;
      hopeFear->hopeScore = score;
      hopeFear->hopeFeatures.clear();
      for (unsigned int k=0; k<nScores[n].size(); k++)
        hopeFear->hopeFeatures.push_back(nScores[n][k]);
      hopeFear->hopeQuality = quality;
      hopeFear->hopeQualityStats = qStats;
    }
    // Fear
    if ((score - quality) > fear_total_score) {
      fear_total_score = score - quality;
      // cerr << n << " : " << nBest[n] << score << " " << quality << " " << fear_total_score << endl;
      hopeFear->fearScore = score;
      hopeFear->fearFeatures.clear();
      for (unsigned int k=0; k<nScores[n].size(); k++)
        hopeFear->fearFeatures.push_back(nScores[n][k]);
      hopeFear->fearQuality = quality;
    }
  }
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
