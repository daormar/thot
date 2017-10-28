/*
thot package for statistical machine translation
Copyright (C) 2013-2017 Daniel Ortiz-Mart\'inez, Adam Harasimowicz

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
/* Module: _incrHmmAligModel                                        */
/*                                                                  */
/* Prototype file: _incrHmmAligModel.h                              */
/*                                                                  */
/* Description: Defines the _incrHmmAligModel class.                */
/*              _incrHmmAligModel class allows to generate and      */
/*              access to the data of an Hmm statistical            */
/*              alignment model.                                    */
/*                                                                  */
/* Notes: 100% AC-DC powered                                        */
/*                                                                  */
/********************************************************************/

#ifndef __incrHmmAligModel_h
#define __incrHmmAligModel_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "_incrSwAligModel.h"
#include "WeightedIncrNormSlm.h"
#include "anjiMatrix.h"
#include "anjm1ip_anjiMatrix.h"
#include "aSourceHmm.h"
#include "HmmAligInfo.h"
#include "CachedHmmAligLgProb.h"
#include "DoubleMatrix.h"
#include "_incrLexTable.h"
#include "IncrHmmAligTable.h"
#include "ashPidxPairHashF.h"
#include "LexAuxVar.h"
#include <MathFuncs.h>

#if __GNUC__>2
#include <ext/hash_map>
using __gnu_cxx::hash_map;
#else
#include <hash_map>
#endif

//--------------- Constants ------------------------------------------

#define EXP_VAL_LOG_MAX                   -0.01
#define EXP_VAL_LOG_MIN                   -9
#define DEFAULT_ALIG_SMOOTH_INTERP_FACTOR  0.3
#define DEFAULT_LEX_SMOOTH_INTERP_FACTOR   0.1

//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------

//--------------- Classes --------------------------------------------

//--------------- _incrHmmAligModel class

class _incrHmmAligModel: public _incrSwAligModel<std::vector<Prob> >
{
  public:

   typedef _incrSwAligModel<std::vector<Prob> >::PpInfo PpInfo;
   typedef std::map<WordIndex,Prob> SrcTableNode;

   // Constructor
   _incrHmmAligModel();

   void set_expval_maxnsize(unsigned int _expval_maxnsize);
       // Function to set a maximum size for the matrices of expected
       // values (by default the size is not restricted)

   // Functions to read and add sentence pairs
   unsigned int numSentPairs(void);

   // Functions to train model
   void trainSentPairRange(std::pair<unsigned int,unsigned int> sentPairRange,
                           int verbosity=0);
       // train model for range [uint,uint]
   void trainAllSents(int verbosity=0);
   void efficientBatchTrainingForRange(std::pair<unsigned int,unsigned int> sentPairRange,
                                       int verbosity=0);
   std::pair<double,double> loglikelihoodForPairRange(std::pair<unsigned int,unsigned int> sentPairRange,
                                                 int verbosity=0);
       // Returns log-likelihood. The first double contains the
       // loglikelihood for all sentences, and the second one, the same
       // loglikelihood normalized by the number of sentences
   std::pair<double,double> vitLoglikelihoodForPairRange(std::pair<unsigned int,unsigned int> sentPairRange,
                                                    int verbosity=0);
       // The same as the previous one, but Viterbi alignments are
       // computed
   void clearInfoAboutSentRange(void);
       // clear info about the whole sentence range without clearing
       // information about current model parameters

   // Functions to set model factors

   void setLexSmIntFactor(double _lexSmoothInterpFactor);
       // Sets lexical smoothing interpolation factor
   void setAlSmIntFactor(double _aligSmoothInterpFactor);
       // Sets alignment smoothing interpolation factor

   // Functions to access model parameters

   Prob pts(WordIndex s,WordIndex t);
       // returns p(t|s)
   virtual LgProb logpts(WordIndex s,WordIndex t);
       // returns log(p(t|s))

       // alignment model functions
   Prob aProb(PositionIndex prev_i,
              PositionIndex slen,
              PositionIndex i);
       // Returns p(i|prev_i,slen)
   virtual LgProb logaProb(PositionIndex prev_i,
                           PositionIndex slen,
                           PositionIndex i);
       // Returns log(p(i|prev_i,slen))

   // Sentence length model functions
   Prob sentLenProb(unsigned int slen,unsigned int tlen);
       // returns p(tlen|slen)
   LgProb sentLenLgProb(unsigned int slen,unsigned int tlen);

   // Functions to get translations for word
   bool getEntriesForTarget(WordIndex t,
                            SrcTableNode& srctn);

   // Functions to generate alignments
   virtual LgProb obtainBestAlignmentVecStrCached(std::vector<std::string> srcSentenceVector,
                                                  std::vector<std::string> trgSentenceVector,
                                                  CachedHmmAligLgProb& cached_logap,
                                                  WordAligMatrix& bestWaMatrix);
   LgProb obtainBestAlignment(std::vector<WordIndex> srcSentIndexVector,
                              std::vector<WordIndex> trgSentIndexVector,
                              WordAligMatrix& bestWaMatrix);
   virtual LgProb obtainBestAlignmentCached(std::vector<WordIndex> srcSentIndexVector,
                                            std::vector<WordIndex> trgSentIndexVector,
                                            CachedHmmAligLgProb& cached_logap,
                                            WordAligMatrix& bestWaMatrix);

   // Functions to calculate probabilities for alignments
   LgProb calcLgProbForAlig(const std::vector<WordIndex>& sSent,
                            const std::vector<WordIndex>& tSent,
                            WordAligMatrix aligMatrix,
                            int verbose=0);

   // Scoring functions without giving an alignment
   LgProb calcLgProb(const std::vector<WordIndex>& sSent,
                     const std::vector<WordIndex>& tSent,
                     int verbose=0);
   LgProb calcLgProbPhr(const std::vector<WordIndex>& sPhr,
                        const std::vector<WordIndex>& tPhr,
                        int verbose=0);
       // Scoring function for phrase pairs

   // Partial scoring functions
   void initPpInfo(unsigned int slen,
                   const std::vector<WordIndex>& tSent,
                   PpInfo& ppInfo);
   void partialProbWithoutLen(unsigned int srcPartialLen,
                              unsigned int slen,
                              const std::vector<WordIndex>& s_,
                              const std::vector<WordIndex>& tSent,
                              PpInfo& ppInfo);
   LgProb lpFromPpInfo(const PpInfo& ppInfo);
   void addHeurForNotAddedWords(int numSrcWordsToBeAdded,
                                const std::vector<WordIndex>& tSent,
                                PpInfo& ppInfo);
   void sustHeurForNotAddedWords(int numSrcWordsToBeAdded,
                                 const std::vector<WordIndex>& tSent,
                                 PpInfo& ppInfo);

   // load function
   bool load(const char* prefFileName);

   // print function
   bool print(const char* prefFileName);

   // clear() function
   void clear(void);

   // clearTempVars() function
   void clearTempVars(void);

   // Destructor
   ~_incrHmmAligModel();

  protected:

   anjiMatrix lanji;
   anjiMatrix lanji_aux;
   anjm1ip_anjiMatrix lanjm1ip_anji;
   anjm1ip_anjiMatrix lanjm1ip_anji_aux;
   std::vector<std::vector<double> > alphaMatrix;
   std::vector<std::vector<double> > betaMatrix;
       // Data structures for manipulating expected values

   std::string lexNumDenFileExtension;
       // Extensions for input files for loading

   LexAuxVar lexAuxVar;
   std::vector<std::vector<double> > cachedLexLogProbs;
       // EM algorithm auxiliary variables

   typedef hash_map<std::pair<aSourceHmm,PositionIndex>,std::pair<float,float>,ashPidxPairHashF> AligAuxVar;
   AligAuxVar aligAuxVar;
   CachedHmmAligLgProb cachedAligLogProbs;
       // EM algorithm auxiliary variables

   _incrLexTable* incrLexTable;
       // Pointer to table with lexical parameters

   IncrHmmAligTable incrHmmAligTable;
       // Table with alignment parameters

   WeightedIncrNormSlm sentLengthModel;

   double aligSmoothInterpFactor;
   double lexSmoothInterpFactor;

   // Functions to get sentence pairs
   std::vector<WordIndex> getSrcSent(unsigned int n);
       // get n-th source sentence
   virtual std::vector<WordIndex> extendWithNullWord(const std::vector<WordIndex>& srcWordIndexVec);
       // given a vector with source words, returns a extended vector
       // including extra NULL words
   virtual std::vector<WordIndex> extendWithNullWordAlig(const std::vector<WordIndex>& srcWordIndexVec);
       // the same as the previous one, but it is specific when calculating suff. statistics
       // for the alignment parameters
   PositionIndex getSrcLen(const std::vector<WordIndex>& nsrcWordIndexVec);

   std::vector<WordIndex> getTrgSent(unsigned int n);
       // get n-th target sentence

   bool sentenceLengthIsOk(const std::vector<WordIndex> sentence);

   // Auxiliary functions to load and print models
   bool loadLexSmIntFactor(const char* lexSmIntFactorFile);
   bool printLexSmIntFactor(const char* lexSmIntFactorFile);
   bool loadAlSmIntFactor(const char* alSmIntFactorFile);
   bool printAlSmIntFactor(const char* alSmIntFactorFile);

   // Functions to handle nloglikelihood
   void set_nloglikelihood(unsigned int n,
                           double d);
   double get_nloglikelihood(unsigned int n);

   // Auxiliary scoring functions
   void initCachedLexicalLps(const std::vector<WordIndex>& nSrcSentIndexVector,
                             const std::vector<WordIndex>& trgSentIndexVector,
                             std::vector<std::vector<double> >& cachedLps);
   double unsmoothed_logpts(WordIndex s,
                            WordIndex t);
       // Returns log(p(t|s)) without smoothing
   virtual double unsmoothed_logaProb(PositionIndex prev_i,
                                      PositionIndex slen,
                                      PositionIndex i);
   double cached_logaProb(PositionIndex prev_i,
                          PositionIndex slen,
                          PositionIndex i,
                          const std::vector<WordIndex>& nsrcSent,
                          const std::vector<WordIndex>& trgSent);
   void nullAligSpecialPar(unsigned int ip,
                           unsigned int slen,
                           aSourceHmm& asHmm,
                           unsigned int& i);
       // Given ip and slen values, returns (asHmm,i) pair expressing a
       // valid alignment with the null word

   void viterbiAlgorithm(const std::vector<WordIndex>& nSrcSentIndexVector,
                         const std::vector<WordIndex>& trgSentIndexVector,
                         std::vector<std::vector<double> >& vitMatrix,
                         std::vector<std::vector<PositionIndex> >& predMatrix);
       // Execute the Viterbi algorithm to obtain the best HMM word
       // alignment
   void viterbiAlgorithmCached(const std::vector<WordIndex>& nSrcSentIndexVector,
                               const std::vector<WordIndex>& trgSentIndexVector,
                               CachedHmmAligLgProb& cached_logap,
                               std::vector<std::vector<double> >& vitMatrix,
                               std::vector<std::vector<PositionIndex> >& predMatrix);
       // Cached version of viterbiAlgorithm()

   double bestAligGivenVitMatricesRaw(const std::vector<std::vector<double> >& vitMatrix,
                                      const std::vector<std::vector<PositionIndex> >& predMatrix,
                                      std::vector<PositionIndex>& bestAlig);
       // Obtain best alignment vector from Viterbi algorithm matrices,
       // index of null word depends on how the source index vector is
       // transformed
   double bestAligGivenVitMatrices(PositionIndex slen,
                                   const std::vector<std::vector<double> >& vitMatrix,
                                   const std::vector<std::vector<PositionIndex> >& predMatrix,
                                   std::vector<PositionIndex>& bestAlig);
       // Obtain best alignment vector from Viterbi algorithm matrices,
       // index of null word is zero
   double forwardAlgorithm(const std::vector<WordIndex>& nSrcSentIndexVector,
                           const std::vector<WordIndex>& trgSentIndexVector,
                           int verbose=0);
       // Execute Forward algorithm to obtain the log-probability of a
       // sentence pair
   double lgProbGivenForwardMatrix(const std::vector<std::vector<double> >& forwardMatrix);
   LgProb calcVitIbm1LgProb(const std::vector<WordIndex>& srcSentIndexVector,
                            const std::vector<WordIndex>& trgSentIndexVector);
   virtual LgProb calcSumIBM1LgProb(const std::vector<WordIndex>& sSent,
                                    const std::vector<WordIndex>& tSent,
                                    int verbose);
   LgProb logaProbIbm1(PositionIndex slen,
                       PositionIndex tlen);
   LgProb noisyOrLgProb(const std::vector<WordIndex>& sSent,
                        const std::vector<WordIndex>& tSent,
                        int verbose);

   // EM-related functions
   void calcNewLocalSuffStats(std::pair<unsigned int,unsigned int> sentPairRange,
                              int verbosity=0);
   void calcNewLocalSuffStatsVit(std::pair<unsigned int,unsigned int> sentPairRange,
                                 int verbosity=0);
   void calcAlphaMatrix(unsigned int n,
                        const std::vector<WordIndex>& nsrcSent,
                        const std::vector<WordIndex>& trgSent);
   void calcBetaMatrix(unsigned int n,
                       const std::vector<WordIndex>& nsrcSent,
                       const std::vector<WordIndex>& trgSent);
   void calc_lanji(unsigned int n,
                   const std::vector<WordIndex>& nsrcSent,
                   const std::vector<WordIndex>& trgSent,
                   const Count& weight);
   void calc_lanji_vit(unsigned int n,
                       const std::vector<WordIndex>& nsrcSent,
                       const std::vector<WordIndex>& trgSent,
                       const std::vector<PositionIndex>& bestAlig,
                       const Count& weight);
   void fillEmAuxVarsLex(unsigned int mapped_n,
                         unsigned int mapped_n_aux,
                         PositionIndex i,
                         PositionIndex j,
                         const std::vector<WordIndex>& nsrcSent,
                         const std::vector<WordIndex>& trgSent,
                         const Count& weight);
   void calc_lanjm1ip_anji(unsigned int n,
                           const std::vector<WordIndex>& nsrcSent,
                           const std::vector<WordIndex>& trgSent,
                           const Count& weight);
   void calc_lanjm1ip_anji_vit(unsigned int n,
                               const std::vector<WordIndex>& nsrcSent,
                               const std::vector<WordIndex>& trgSent,
                               const std::vector<PositionIndex>& bestAlig,
                               const Count& weight);
   bool isFirstNullAligPar(PositionIndex ip,
                           unsigned int slen,
                           PositionIndex i);
   double calc_lanji_num(PositionIndex slen,
                         PositionIndex i,
                         PositionIndex j,
                         const std::vector<WordIndex>& nsrcSent,
                         const std::vector<WordIndex>& trgSent);
   double calc_lanjm1ip_anji_num_je1(PositionIndex slen,
                                     PositionIndex i,
                                     const std::vector<WordIndex>& nsrcSent,
                                     const std::vector<WordIndex>& trgSent);
   double calc_lanjm1ip_anji_num_jg1(PositionIndex ip,
                                     PositionIndex slen,
                                     PositionIndex i,
                                     PositionIndex j,
                                     const std::vector<WordIndex>& nsrcSent,
                                     const std::vector<WordIndex>& trgSent);
   void gatherLexSuffStats(unsigned int mapped_n,
                           unsigned int mapped_n_aux,
                           const std::vector<WordIndex>& nsrcSent,
                           const std::vector<WordIndex>& trgSent,
                           const Count& weight);
   void gatherAligSuffStats(unsigned int mapped_n,
                            unsigned int mapped_n_aux,
                            const std::vector<WordIndex>& nsrcSent,
                            const std::vector<WordIndex>& trgSent,
                            const Count& weight);
   void fillEmAuxVarsAlig(unsigned int mapped_n,
                          unsigned int mapped_n_aux,
                          PositionIndex slen,
                          PositionIndex ip,
                          PositionIndex i,
                          PositionIndex j,
                          const Count& weight);
   void getHmmAligInfo(PositionIndex ip,
                       unsigned int slen,
                       PositionIndex i,
                       HmmAligInfo& hmmAligInfo);
   bool isValidAlig(PositionIndex ip,
                    unsigned int slen,
                    PositionIndex i);
   bool isNullAlig(PositionIndex ip,
                   unsigned int slen,
                   PositionIndex i);
   PositionIndex getModifiedIp(PositionIndex ip,
                               unsigned int slen,
                               PositionIndex i);
   double log_alpha(PositionIndex slen,
                    PositionIndex i,
                    PositionIndex j,
                    const std::vector<WordIndex>& nsrcSent,
                    const std::vector<WordIndex>& trgSent);
   double log_beta(PositionIndex slen,
                   PositionIndex i,
                   PositionIndex j,
                   const std::vector<WordIndex>& nsrcSent,
                   const std::vector<WordIndex>& trgSent);
   void updateParsLex(void);
   void updateParsAlig(void);
   virtual float obtainLogNewSuffStat(float lcurrSuffStat,
                                      float lLocalSuffStatCurr,
                                      float lLocalSuffStatNew);
};

#endif
