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
/* Module: NonPbEcModelForNbUcat                                    */
/*                                                                  */
/* Prototypes file: NonPbEcModelForNbUcat.h                         */
/*                                                                  */
/* Description: Declares the NonPbEcModelForNbUcat class,           */
/*              this class implements a non phrase-based error      */
/*              correcting model for uncoupled computer assisted    */
/*              translation based on n-best lists.                  */
/*                                                                  */
/********************************************************************/

/**
 * @file NonPbEcModelForNbUcat.h
 * 
 * @brief Defines the NonPbEcModelForNbUcat class, this class implements
 * a non phrase-based error correcting model for uncoupled computer
 * assisted translation based on n-best lists.
 */

#ifndef _NonPbEcModelForNbUcat_h
#define _NonPbEcModelForNbUcat_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <iostream>
#include <iomanip>
#include "myVector.h"
#include <map>
#include <string>
#include "PrefAlignInfo.h"
#include "Prob.h"
#include "BaseEcModelForNbUcat.h"
#include "BaseErrorCorrectionModel.h"

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- NonPbEcModelForNbUcat template class

/**
 * @brief The NonPbEcModelForNbUcat class aligns implements a non
 * phrase-based error correcting model for uncoupled computer assisted
 * translation based on n-best lists.
 */

class NonPbEcModelForNbUcat: public BaseEcModelForNbUcat
{
 public:
    
      // Constructor
  NonPbEcModelForNbUcat();


      // Link error correcting model with the error correcting model for
      // uncoupled cat
  void link_ecm(BaseErrorCorrectionModel* _ecm_ptr);

  NbestCorrections correct(const Vector<std::string>& outputSentVec,
                           const Vector<unsigned int>& sourceCuts,
                           const Vector<std::string>& prefixVec,
                           unsigned int _maxMapSize,
                           int verbose=0);
      // Correct sentence given in outputSentVec using prefixVec. The
      // basic units for the corrections can be restricted by means of
      // sourceCuts, which contains a vector of cuts for
      // outputSentVec. Set one cut per word for unrestricted correction  

      // clear() function
  void clear(void);

      // Clear temporary variables
  void clearTempVars(void);
  
      // Destructor
  ~NonPbEcModelForNbUcat();
  
 protected:

  typedef std::multimap<LgProb,PrefAlignInfo,greater<LgProb> > MonolingSegmNbest;

  Vector<std::string> outputSentVec;
  Vector<unsigned int> sourceCuts;
  Vector<Vector<std::string> > outputSegmVec;
  Vector<std::string> prefixVec;
  unsigned int maxMapSize;
  MonolingSegmNbest monolingSegmNbest;

  BaseErrorCorrectionModel *ecm_ptr;
  
  MonolingSegmNbest nonPhraseBasedAlign(const Vector<std::string>& _outputSentVec,
                                        const Vector<std::string>& _prefixVec,
                                        unsigned int _maxMapSize,
                                        int verbose=0);
  Vector<std::string> correctedSent(PrefAlignInfo& prefAlignInfo);
  Vector<Vector<std::string> > obtainVectorWithSegms(Vector<std::string> sentVec,
                                                     Vector<unsigned int> cuts,
                                                     int verbose=0);
  void addSegm(LgProb lp,PrefAlignInfo& prefAlignInfo);
  void removeLastFromMonolingSegmNbest(void);
  void getLastOutSegm(Vector<std::string>& x,
                      Vector<unsigned int>& cuts,
                      Vector<std::string>& lastOutSegm);
};
#endif
