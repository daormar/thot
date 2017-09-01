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
/* Module: WeightedIncrNormSlm                                      */
/*                                                                  */
/* Prototype file: WeightedIncrNormSlm.h                            */
/*                                                                  */
/* Description: The WeightedIncrNormSlm class implements a weighted */
/*              incremental gaussian sentence length model.         */
/*                                                                  */
/********************************************************************/

#ifndef _WeightedIncrNormSlm
#define _WeightedIncrNormSlm

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "_sentLengthModel.h"
#include "awkInputStream.h"
#include "MathFuncs.h"
#include <map>
#include <utility>
#include <string.h>

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- WeightedIncrNormSlm class

class WeightedIncrNormSlm: public _sentLengthModel
{
 public:

      // Constructor
  WeightedIncrNormSlm(void);
  
      // Load model parameters
  bool load(const char* filename);

      // Print model parameters
  bool print(const char* filename);

      // Sentence length model functions
  Prob sentLenProb(unsigned int slen,unsigned int tlen);
      // returns p(tl=tlen|sl=slen)
  LgProb sentLenLgProb(unsigned int slen,unsigned int tlen);

      // Sum sentence length model functions
  Prob sumSentLenProb(unsigned int slen,unsigned int tlen);
      // returns p(tl<=tlen|sl=slen)
  LgProb sumSentLenLgProb(unsigned int slen,unsigned int tlen);

      // Functions to train the sentence length model 
  void trainSentPair(std::vector<std::string> srcSentVec,
                     std::vector<std::string> trgSentVec,
                     Count c=1);

      // clear function
  void clear(void);
  
 protected:

  unsigned int numSents;
  unsigned int slenSum;
  unsigned int tlenSum;
  std::vector<unsigned int> kVec;
  std::vector<float> swkVec;
  std::vector<float> mkVec;
  std::vector<float> skVec;

      // Auxiliary functions
  std::ostream& print(std::ostream &outS);
  LgProb sentLenLgProbNorm(unsigned int slen,
                           unsigned int tlen);
  Prob sumSentLenProbNorm(unsigned int slen,
                          unsigned int tlen);
  bool readNormalPars(const char *normParsFileName);
  bool get_mean_stddev(unsigned int slen,
                       float& mean,
                       float& stddev);
  unsigned int get_k(unsigned int slen,
                     bool& found);
  void set_k(unsigned int slen,
             unsigned int k_val);
  float get_swk(unsigned int slen);
  void set_swk(unsigned int slen,
               float swk_val);
  float get_mk(unsigned int slen);
  void set_mk(unsigned int slen,
              float mk_val);
  float get_sk(unsigned int slen);
  void set_sk(unsigned int slen,
              float sk_val);
};

#endif
