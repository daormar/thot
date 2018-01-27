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
 * @file PfsmEcm.h
 * 
 * @brief Defines the PfsmEcm class, this class implements a statistical
 * error correcting model for strings which uses a probabilistic finite
 * state machine
 */

#ifndef _PfsmEcm_h
#define _PfsmEcm_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <math.h>
#include <iostream>
#include <iomanip>
#include "EditDistForVecString.h"
#include "_editDistBasedEcm.h"

//--------------- Constants ------------------------------------------

// Constants to be used in the calculation of the similarities by means
// of probabilistic finite-state machines
#define DEFAULT_VOC_SIZE     128
#define DEFAULT_HIT_PROB       0.8
#define DEFAULT_INS_FACTOR     1
#define DEFAULT_SUBST_FACTOR   1
#define DEFAULT_DEL_FACTOR     1

//--------------- Classes --------------------------------------------

//--------------- PfsmEcm template class

/**
 * @brief The PfsmEcm class aligns the output sentences given by the
 * translator with a prefix given by the user
 */

class PfsmEcm: public _editDistBasedEcm
{
 public:
  
      // Constructor
  PfsmEcm();

      // Basic functions
  Score similarity(std::vector<std::string> x,
                   std::vector<std::string> y);
      // Calculates similarity between x and y
  Score similarityGivenPrefix(std::vector<std::string> x,
                              std::vector<std::string> y);
      // Calculates similarity between x and y, where y is taken as a
      // prefix
  void correctStrGivenPref(std::vector<std::string> uncorrStrVec,
                           std::vector<std::string> prefStrVec,
                           std::vector<std::string>& correctedStrVec);
      // Corrects string 'uncorrStrVec' given the prefix 'prefStrVec'
      // storing the results in 'correctedStrVec'

      // Model weights functions
  void setWeights(std::vector<float> wVec);
  unsigned int getNumWeights(void);
  void printWeights(std::ostream &outS);

      // load() and print() functions
  bool load(const char *prefix);
  bool print(const char *prefix);
    
      // Destructor
  ~PfsmEcm();
  
 protected:

   EditDistForVecString editDistForVecStr;
   std::vector<float> ecModelPars;

   void setErrorModel(float vocsize,
                      float hprob,
                      float ifactor,
                      float sfactor,
                      float dfactor,
                      int verbose=0);
       // Sets the numbers which determines the probabilities of the
       // error model. If vocsize=0 then each state of the model only has
       // four transtitions: "insertion", "substitution", "deletion" and
       // "hit"

   void obtainErrorProbsFromParams(float vocsize,
                                   float hprob,
                                   float ifactor,
                                   float sfactor,
                                   float dfactor,
                                   float& ip,
                                   float& sp,
                                   float& dp);
   Score simPfsm(std::vector<std::string> x,
                 std::vector<std::string> y);
       // Calculates similarity by means of a probabilistic finite-state
       // machine (pfsm)
   Score simGivenPrefPfsm(std::vector<std::string> x,
                          std::vector<std::string> y);
       // Calculates similarity given a prefix by means of a
       // probabilistic finite-state machine (pfsm)
   Score simGivenPrefOpsPfsm(std::vector<std::string> x,
                             std::vector<std::string> y,
                             WordAndCharLevelOps& wcOps);
       // The same as the previous function but also returns word- and
       // character-level edit operations
};
#endif
