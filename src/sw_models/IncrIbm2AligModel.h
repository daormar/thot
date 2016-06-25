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
/* Module: IncrIbm2AligModel                                        */
/*                                                                  */
/* Prototype file: IncrIbm2AligModel.h                              */
/*                                                                  */
/* Description: Defines the IncrIbm2AligModel class.                */
/*              IncrIbm2AligModel class allows to generate and      */
/*              access to the data of an IBM 2 statistical          */
/*              alignment model.                                    */
/*                                                                  */
/********************************************************************/

#ifndef _IncrIbm2AligModel_h
#define _IncrIbm2AligModel_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "IncrIbm1AligModel.h"
#include "aSource.h"
#include "IncrIbm2AligTable.h"

//--------------- Constants ------------------------------------------

#define ARBITRARY_AP 0.1

//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------

//--------------- Classes --------------------------------------------

//--------------- IncrIbm2AligModel class

class IncrIbm2AligModel: public IncrIbm1AligModel
{
  public:

   typedef IncrIbm1AligModel::PpInfo PpInfo;
   typedef IncrIbm1AligModel::SrcTableNode SrcTableNode;

       // Constructor
   IncrIbm2AligModel();

       // Functions to train model
   void efficientBatchTrainingForRange(pair<unsigned int,unsigned int> sentPairRange,
                                       int verbosity=0);

   // Functions to access model parameters

   // alignment model functions
   virtual Prob aProb(PositionIndex j,
                      PositionIndex slen,
                      PositionIndex tlen,
                      PositionIndex i);
       // Returns p(i|j,slen,tlen)
   LgProb logaProb(PositionIndex j,
                   PositionIndex slen,
                   PositionIndex tlen,
                   PositionIndex i);
       // Returns log(p(i|j,slen,tlen))
   // Functions to generate alignments 
   LgProb obtainBestAlignment(Vector<WordIndex> srcSentIndexVector,
                              Vector<WordIndex> trgSentIndexVector,
                              WordAligMatrix& bestWaMatrix);

   LgProb lexAligM2LpForBestAlig(Vector<WordIndex> nSrcSentIndexVector,
                                 Vector<WordIndex> trgSentIndexVector,
                                 Vector<PositionIndex>& bestAlig);

   // Functions to calculate probabilities for alignments
   LgProb calcLgProbForAlig(const Vector<WordIndex>& sSent,
                            const Vector<WordIndex>& tSent,
                            WordAligMatrix aligMatrix,
                            int verbose=0);
   LgProb incrIBM2LgProb(Vector<WordIndex> nsSent,
                         Vector<WordIndex> tSent,
                         Vector<PositionIndex> alig,
                         int verbose=0);

   // Scoring functions without giving an alignment
   LgProb calcLgProb(const Vector<WordIndex>& sSent,
                     const Vector<WordIndex>& tSent,
                     int verbose=0);
   LgProb calcSumIBM2LgProb(Vector<WordIndex> nsSent,
                            Vector<WordIndex> tSent,
                            int verbose=0);

   // Partial scoring functions
   void initPpInfo(unsigned int slen,
                   const Vector<WordIndex>& tSent,
                   PpInfo& ppInfo);
   void partialProbWithoutLen(unsigned int srcPartialLen,
                              unsigned int slen,
                              const Vector<WordIndex>& s_,
                              const Vector<WordIndex>& tSent,
                              PpInfo& ppInfo);

   // load function
   bool load(const char* prefFileName);
   
   // print function
   bool print(const char* prefFileName);

   // clear() function
   void clear(void);
   
   // Destructor
   ~IncrIbm2AligModel();

  protected:
   
   IncrIbm2AligTable incrIbm2AligTable;

   typedef std::map<pair<aSource,PositionIndex>,pair<float,float> > AligAuxVar;
   AligAuxVar aligAuxVar;
       // EM algorithm auxiliary variables

   // Auxiliar scoring functions
   virtual double unsmoothed_aProb(PositionIndex j,
                                   PositionIndex slen,
                                   PositionIndex tlen,
                                   PositionIndex i);
       // Returns p(i|j,slen,tlen) without smoothing
   double unsmoothed_logaProb(PositionIndex j,
                              PositionIndex slen,
                              PositionIndex tlen,
                              PositionIndex i);
         // Returns log(p(i|j,slen,tlen)) without smoothing
     
   // EM-related functions
   double calc_anji_num(const Vector<WordIndex>& nsrcSent,
                        const Vector<WordIndex>& trgSent,
                        unsigned int i,
                        unsigned int j);
   double calc_anji_num_alig(PositionIndex i,
                             PositionIndex j,
                             PositionIndex slen,
                             PositionIndex tlen);
   void fillEmAuxVars(unsigned int mapped_n,
                      unsigned int mapped_n_aux,
                      PositionIndex i,
                      PositionIndex j,
                      const Vector<WordIndex>& nsrcSent,
                      const Vector<WordIndex>& trgSent,
                      const Count& weight);
   void fillEmAuxVarsAlig(unsigned int mapped_n,
                          unsigned int mapped_n_aux,
                          PositionIndex i,
                          PositionIndex j,
                          PositionIndex slen,
                          PositionIndex tlen,
                          const Count& weight);
   void updatePars(void);
   void updateParsAlig(void);

   // Mask for aSource. This function makes it possible to affect the
   // estimation of the alignment probabilities by setting to zero the
   // components of 'as'
   virtual void aSourceMask(aSource &as);
};

#endif
