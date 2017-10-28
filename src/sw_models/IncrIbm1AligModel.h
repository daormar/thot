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
/* Module: IncrIbm1AligModel                                        */
/*                                                                  */
/* Prototype file: IncrIbm1AligModel.h                              */
/*                                                                  */
/* Description: Defines the IncrIbm1AligModel class.                */
/*              IncrIbm1AligModel class allows to generate and      */
/*              access to the data of an IBM 1 statistical          */
/*              alignment model.                                    */
/*                                                                  */
/********************************************************************/

#ifndef _IncrIbm1AligModel_h
#define _IncrIbm1AligModel_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "_incrSwAligModel.h"
#include "WeightedIncrNormSlm.h"
#include "anjiMatrix.h"
#include "IncrLexTable.h"
#include "BestLgProbForTrgWord.h"
#include "LexAuxVar.h"

//--------------- Constants ------------------------------------------

#define ARBITRARY_PTS            0.1
#define SMOOTHING_ANJI_NUM       1e-6
#define SMOOTHING_WEIGHTED_ANJI  1e-6

//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------

//--------------- Classes --------------------------------------------

//--------------- IncrIbm1AligModel class

class IncrIbm1AligModel: public _incrSwAligModel<std::vector<Prob> >
{
  public:

   typedef _incrSwAligModel<std::vector<Prob> >::PpInfo PpInfo;
   typedef std::map<WordIndex,Prob> SrcTableNode;
  
   // Constructor
   IncrIbm1AligModel();

   void set_expval_maxnsize(unsigned int _anji_maxnsize);
       // Function to set a maximum size for the vector of expected
       // values anji (by default the size is not restricted)

   // Functions to read and add sentence pairs
   unsigned int numSentPairs(void);

   // Functions to train model
   void trainSentPairRange(std::pair<unsigned int,unsigned int> sentPairRange,
                           int verbosity=0);
       // train model for range [uint,uint]. Returns log-likelihood
   void trainAllSents(int verbosity=0);
   void efficientBatchTrainingForRange(std::pair<unsigned int,unsigned int> sentPairRange,
                                       int verbosity=0);
   std::pair<double,double> loglikelihoodForPairRange(std::pair<unsigned int,unsigned int> sentPairRange,
                                                 int verbosity=0);
        // Returns log-likelihood. The first double contains the
        // loglikelihood for all sentences, and the second one, the same
        // loglikelihood normalized by the number of sentences
   void clearInfoAboutSentRange(void);
       // clear info about the whole sentence range without clearing
       // information about current model parameters
   
   // Functions to access model parameters

   // lexical model functions
   virtual Prob pts(WordIndex s,WordIndex t);
       // returns p(t|s)
   LgProb logpts(WordIndex s,WordIndex t);
       // returns log(p(t|s))

   // alignment model functions
   Prob aProbIbm1(PositionIndex slen,
                  PositionIndex tlen);
   LgProb logaProbIbm1(PositionIndex slen,
                       PositionIndex tlen);

   // Sentence length model functions
   Prob sentLenProb(unsigned int slen,unsigned int tlen);
       // returns p(tlen|slen)
   LgProb sentLenLgProb(unsigned int slen,unsigned int tlen);

   // Functions to get translations for word
   bool getEntriesForTarget(WordIndex t,
                            SrcTableNode& srctn);
   
   // Functions to generate alignments 
   LgProb obtainBestAlignment(std::vector<WordIndex> srcSentIndexVector,
                              std::vector<WordIndex> trgSentIndexVector,
                              WordAligMatrix& bestWaMatrix);

   LgProb lexM1LpForBestAlig(std::vector<WordIndex> nSrcSentIndexVector,
                             std::vector<WordIndex> trgSentIndexVector,
                             std::vector<PositionIndex>& bestAlig);

   // Functions to calculate probabilities for alignments
   LgProb calcLgProbForAlig(const std::vector<WordIndex>& sSent,
                            const std::vector<WordIndex>& tSent,
                            WordAligMatrix aligMatrix,
                            int verbose=0);
   LgProb incrIBM1LgProb(std::vector<WordIndex> nsSent,
                         std::vector<WordIndex> tSent,
                         std::vector<PositionIndex> alig,
                         int verbose=0);

   // Scoring functions without giving an alignment
   LgProb calcLgProb(const std::vector<WordIndex>& sSent,
                     const std::vector<WordIndex>& tSent,
                     int verbose=0);
   LgProb calcSumIBM1LgProb(const char *sSent,
                            const char *tSent,
                            int verbose=0);
   LgProb calcSumIBM1LgProb(std::vector<std::string> nsSent,
                            std::vector<std::string> tSent,
                            int verbose=0);
   LgProb calcSumIBM1LgProb(std::vector<WordIndex> nsSent,
                            std::vector<WordIndex> tSent,
                            int verbose=0);

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
   ~IncrIbm1AligModel();

  protected:
      
   WeightedIncrNormSlm sentLengthModel;

   anjiMatrix anji;
   anjiMatrix anji_aux;
       // Data structures for manipulating expected values

   LexAuxVar lexAuxVar;
       // EM algorithm auxiliary variables
   
   IncrLexTable incrLexTable;

       // Structures to calculate loglikelihood
   std::vector<double> nloglikelihood;     
   double loglikelihood;

   BestLgProbForTrgWord bestLgProbForTrgWord;

   // Functions to get sentence pairs
   std::vector<WordIndex> getSrcSent(unsigned int n);
       // get n-th source sentence
   std::vector<WordIndex> extendWithNullWord(const std::vector<WordIndex>& srcWordIndexVec);
       // given a vector with source words, returns a extended vector
       // including extra NULL words

   std::vector<WordIndex> getTrgSent(unsigned int n);   
       // get n-th target sentence

   bool sentenceLengthIsOk(const std::vector<WordIndex> sentence);

   // Functions to handle nloglikelihood
   void set_nloglikelihood(unsigned int n,
                           double d);
   double get_nloglikelihood(unsigned int n);

   // Auxiliar scoring functions
   double unsmoothed_pts(WordIndex s,WordIndex t);
       // Returns p(t|s) without smoothing
   double unsmoothed_logpts(WordIndex s,
                            WordIndex t);
       // Returns log(p(t|s)) without smoothing

   // EM-related functions
   void calcNewLocalSuffStats(std::pair<unsigned int,unsigned int> sentPairRange,
                              int verbosity=0);
   void calc_anji(unsigned int n,
                  const std::vector<WordIndex>& nsrcSent,
                  const std::vector<WordIndex>& trgSent,
                  const Count& weight);
   virtual double calc_anji_num(const std::vector<WordIndex>& nsrcSent,
                                const std::vector<WordIndex>& trgSent,
                                unsigned int i,
                                unsigned int j);
   virtual void fillEmAuxVars(unsigned int mapped_n,
                              unsigned int mapped_n_aux,
                              PositionIndex i,
                              PositionIndex j,
                              const std::vector<WordIndex>& nsrcSent,
                              const std::vector<WordIndex>& trgSent,
                              const Count& weight);
   virtual void updatePars(void);
   virtual float obtainLogNewSuffStat(float lcurrSuffStat,
                                      float lLocalSuffStatCurr,
                                      float lLocalSuffStatNew);
   
   // Functions to update log-likelihood
   void update_loglikelihood(std::pair<unsigned int,unsigned int> sentPairRange,
                             int verbosity=0);

   // Partial prob. auxiliary functions
   LgProb lgProbOfBestTransForTrgWord(WordIndex t);
};

#endif
