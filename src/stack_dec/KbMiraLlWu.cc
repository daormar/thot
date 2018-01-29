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

/**
 * @file KbMiraLlWu.cc
 * 
 * @brief Definitions file for KbMiraLlWu.h
 */

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
                        const std::vector<std::string>& nblist,
                        const std::vector<std::vector<double> >& scoreCompsVec,
                        const std::vector<double>& currWeightsVec,
                        std::vector<double>& newWeightsVec)
{
  srand(KBMIRA_RANDOM_SEED);

  assert (nblist.size() == scoreCompsVec.size());

  std::vector<double> max_wAvg;
  double quality, max_quality = 0;

  scorer->resetBackgroundCorpus();
  for (unsigned int nReStarts=0; nReStarts<maxRestarts; nReStarts++) {
    double iter_max_quality = 0;
    unsigned int iter_max_j = 0, nUpdates = 1;
    std::vector<double> wt(currWeightsVec);
    std::vector<double> wTotals(currWeightsVec);

    for(unsigned int j=0; j<nIters; j++) {
      HopeFearData hfd;
      HopeFear(reference, nblist, scoreCompsVec, wt, &hfd);
      if (hfd.hopeQuality  > hfd.fearQuality) {
        std::vector<double> diff(hfd.hopeFeatures.size());
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
          double eta = std::min(c, loss/diffNorm);
          for (unsigned int k=0; k<diff.size(); k++) {
            wt[k] += eta*diff[k];
            wTotals[k] += wt[k];
          }
          nUpdates++;
        }
        scorer->updateBackgroundCorpus(hfd.hopeQualityStats, decay);
      }
      // average all seen weight vectors
      std::vector<double> wAvg(wTotals.size(), 0);
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
      //std::cerr << nReStarts << " " << j << " " << iter_max_j << " " << quality << " " << max_quality << std::endl;

      // restart weights if no improvement in X epochs;
      if (j-iter_max_j > epochsToRestart)
        break;
    }
  }
  newWeightsVec = max_wAvg;
}

//---------------------------------------
void KbMiraLlWu::updateClosedCorpus(const std::vector<std::string>& references,
                                    const std::vector<std::vector<std::string> >& nblists,
                                    const std::vector<std::vector<std::vector<double> > >& scoreCompsVecs,
                                    const std::vector<double>& currWeightsVec,
                                    std::vector<double>& newWeightsVec)
{
  srand(KBMIRA_RANDOM_SEED);

  unsigned int nSents = references.size(); // number of examples in the corpus
  assert(nblists.size() == nSents);
  assert(scoreCompsVecs.size() == nSents);

  // //##########################################################################
  // // evaluate bleu of currWeightsVec
  // std::cerr << "CW: [ ";
  // for (unsigned int k=0; k<currWeightsVec.size(); k++)
  //   std::cerr << currWeightsVec[k] << " ";
  // std::cerr << "]" << std::endl;
  // std::string mT;
  // std::vector<std::string> mTs;
  // for (unsigned int i=0; i<nSents; i++) {
  //   MaxTranslation(currWeightsVec, nblists[i], scoreCompsVecs[i], mT);
  //   mTs.push_back(mT);
  //   //std::cerr << i << " " << mT << std::endl;
  // }
  // double bleu;
  // scorer->corpusScore(mTs, references, bleu);
  // std::cerr << bleu << std::endl;
  // //##########################################################################

  std::vector<double> max_wAvg;
  double quality, max_quality = 0;

  scorer->resetBackgroundCorpus();
  for (unsigned int nReStarts=0; nReStarts<maxRestarts; nReStarts++) {
    double iter_max_quality = 0;
    unsigned int iter_max_j = 0, nUpdates = 1;
    std::vector<double> wt(currWeightsVec);
    std::vector<double> wTotals(currWeightsVec);

    for (unsigned int j=0; j<nIters; j++) {
      std::vector<unsigned int> indices(nSents);
      sampleWoReplacement(nSents, indices);
      for (unsigned int z=0; z<nSents; z++) {
        unsigned int i = indices[z];
        assert (nblists[i].size() == scoreCompsVecs[i].size());
        HopeFearData hfd;
        HopeFear(references[i], nblists[i], scoreCompsVecs[i], wt, &hfd);

        // std::cerr << i << " " << hfd.hopeQuality << " " << hfd.fearQuality << std::endl;

        if (hfd.hopeQuality > hfd.fearQuality) {
          std::vector<double> diff(hfd.hopeFeatures.size());
          for (unsigned int k=0; k<diff.size(); k++)
            diff[k] = hfd.hopeFeatures[k] - hfd.fearFeatures[k];
          double delta = hfd.hopeQuality - hfd.fearQuality;
          double diffScore = 0;
          for (unsigned int k=0; k<diff.size(); k++)
            diffScore += wt[k]*diff[k];
          double loss = delta - diffScore;
          
          // std::cerr << " - " << loss << std::endl;

          if (loss > 0) {
            // Update weights
            double diffNorm = 0;
            for (unsigned int k=0; k<diff.size(); k++)
              diffNorm += diff[k]*diff[k];
            double eta = std::min(c, loss/diffNorm);
            for (unsigned int k=0; k<diff.size(); k++) {
              //std::cerr << "WU: " << k << " : " << wt[k];
              wt[k] += eta*diff[k];
              //std::cerr << " ( " << eta << " " << diff[k] << " ) " << wt[k] << std::endl;
              wTotals[k] += wt[k];
            }
            nUpdates++;
          }
          scorer->updateBackgroundCorpus(hfd.hopeQualityStats, decay);
        }
      }

      // average all seen weight vectors
      std::vector<double> wAvg(wTotals.size(), 0);
      for (unsigned int k=0; k<wAvg.size(); k++)
        wAvg[k] = wTotals[k]/nUpdates;

      // std::cerr << "Wavg: [ ";
      // for (unsigned int k=0; k<wAvg.size(); k++)
      //   std::cerr << wAvg[k] << " ";
      // std::cerr << "]" << std::endl;

      // evaluate score of wAvg
      std::string maxTranslation;
      std::vector<std::string> maxTranslations;
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

      //std::cerr << nReStarts << " " << j << " " << iter_max_j << " " << quality << " " << max_quality << std::endl;
      //exit(0);
      // restart weights if no improvement in X epochs;
      if (j-iter_max_j > epochsToRestart)
        break;
    }
  }
  newWeightsVec = max_wAvg;
}

//---------------------------------------
void KbMiraLlWu::MaxTranslation(const std::vector<double>& wv,
                                const std::vector<std::string>& nBest,
                                const std::vector<std::vector<double> >& nScores,
                                std::string& maxTranslation)
{
  double max_score=-DBL_MAX;
  for (unsigned int n=0; n<nBest.size(); n++) {
    double score = 0;
    for (unsigned int k=0; k<wv.size(); k++)
      score += wv[k]*nScores[n][k];
    //cout << " * " << score << " " << nBest[n] << std::endl;
    if (score > max_score) {
        max_score = score;
        maxTranslation = nBest[n];
    }
  }
}

//---------------------------------------
void KbMiraLlWu::HopeFear(const std::string& reference,
                          const std::vector<std::string>& nBest,
                          const std::vector<std::vector<double> >& nScores,
                          const std::vector<double>& wv,
                          HopeFearData* hopeFear)
{
  // Hope / fear decode
  double hope_scale = 1.0;
  double hope_total_score=-DBL_MAX;
  double fear_total_score=-DBL_MAX;

  //std::cerr << "R: " << reference << std::endl;

  for (unsigned int n=0; n<nBest.size(); n++) {
    double score = 0;
    for (unsigned int k=0; k<wv.size(); k++)
      score += wv[k]*nScores[n][k];
    double quality;
    std::vector<unsigned int> qStats;
    scorer->sentBackgroundScore(nBest[n], reference, quality, qStats);

    // std::cerr << nBest[n] << std::endl;
    // std::cerr << reference << std::endl;
    // std::cerr << quality << " : [ ";
    // for (unsigned int k=0; k<qStats.size(); k++)
    //   std::cerr << qStats[k] << " ";
    // std::cerr << "] " << score << " " << quality;
    // std::cerr << " ( " << hope_scale*score + quality << " , " << hope_total_score;
    // std::cerr << " , " << score-quality << " " << fear_total_score << " )" << std::endl;

    // Hope
    if ((hope_scale*score + quality) > hope_total_score) {
      hope_total_score = hope_scale*score + quality;
      // std::cerr << "Hope: " << n << " : " << nBest[n] << score << " " << quality << " " << hope_total_score << std::endl;
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
      // std::cerr << "Fear: " << n << " : " << nBest[n] << score << " " << quality << " " << fear_total_score << std::endl;
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
                                     std::vector<unsigned int>& indices)
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
