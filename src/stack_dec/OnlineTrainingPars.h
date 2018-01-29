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
 * @file OnlineTrainingPars.h
 * 
 * @brief Class to store online training parameters.
 */

#ifndef _OnlineTrainingPars_h
#define _OnlineTrainingPars_h

//--------------- Include files --------------------------------------


//--------------- Constants ------------------------------------------

#define BASIC_INCR_TRAINING            0
#define MINIBATCH_TRAINING             1
#define BATCH_RETRAINING               2
#define FIXED_LEARNING_RATE_POL        0
#define LIANG_LEARNING_RATE_POL        1
#define OWN_LEARNING_RATE_POL          2
#define WER_BASED_LEARNING_RATE_POL    3
#define DEFAULT_INTERLACED_TRAIN_E_PAR 1
#define DEFAULT_INTERLACED_TRAIN_R_PAR 0

//--------------- OnlineTrainingPars template class

class OnlineTrainingPars
{
 public:

  unsigned int onlineLearningAlgorithm;
  unsigned int learningRatePolicy;
  float learnStepSize;
  unsigned int emIters;
  unsigned int E_par;
  unsigned int R_par;
  
  OnlineTrainingPars()
  {
    default_values();
  }

  void default_values(void)
  {
    onlineLearningAlgorithm=BASIC_INCR_TRAINING;
    learningRatePolicy=FIXED_LEARNING_RATE_POL;
    learnStepSize=1;
    emIters=5;
    E_par=DEFAULT_INTERLACED_TRAIN_E_PAR;
    R_par=DEFAULT_INTERLACED_TRAIN_R_PAR;
  }
};

#endif
