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
 * @file BaseEcmForWg.h
 * 
 * @brief Defines the BaseEcmForWg class, this class is a base class for
 * implementing error correcting model for word-graphs
 */

#ifndef _BaseEcmForWg_h
#define _BaseEcmForWg_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <string>
#include <vector>
#include <StrProcUtils.h>
#include <ErrorDefs.h>
#include <Score.h>
#include <limits.h>

//--------------- Constants ------------------------------------------

#define INVALID_ESI_POS UINT_MAX

//--------------- Classes --------------------------------------------

//--------------- BaseEcmForWg template class

/**
 * @brief The BaseEcmForWg class is a base class for implementing error
 * correcting models for word-graphs
 */

template<class ECM_SCORE_INFO>
class BaseEcmForWg
{
 public:

  typedef ECM_SCORE_INFO EcmScoreInfo;
      // The EcmScoreInfo data type stores information about the score of a
      // word-graph state

  // Basic functionality
  
  virtual void correctStrGivenPrefWg(std::vector<std::string> uncorrStrVec,
                                     std::vector<std::string> prefStrVec,
                                     std::vector<std::string>& correctedStrVec)=0;
      // Corrects string 'uncorrStrVec' given the prefix 'prefStrVec'
      // storing the results in 'correctedStrVec'. This is a specialized
      // version for word graphs of the correctStrGivenPref function

  virtual EcmScoreInfo constructInitEsi(void)=0;
      // Constructs an initial esi object

  virtual EcmScoreInfo constructEsi(const EcmScoreInfo& prevEsi,
                                    const std::string& word)=0;
      // Constructs an esi object given a previous one
    
  virtual void extendInitialEsi(const std::vector<std::string>& prefixDiffVec,
                                const EcmScoreInfo& prevInitEsi,
                                EcmScoreInfo& newInitEsi)=0;
      // Extends initial ecm score info
  
  virtual void extendEsi(const std::vector<std::string>& prefixDiffVec,
                         const EcmScoreInfo& prevEsi,
                         const std::string& word,
                         EcmScoreInfo& newEsi)=0;
      // Extends ecm score info
  
  virtual std::vector<Score> obtainScrVecFromEsi(const EcmScoreInfo& esi)=0;
      // Returns a vector of error correcting scores for the
      // EcmScoreInfo object esi. The last score of the vector
      // corresponds to the score of a complete correction of the
      // prefix.

  virtual void updateEsiPositions(const EcmScoreInfo& esi1,
                                  const std::vector<unsigned int>& posVec,
                                  EcmScoreInfo& esi2)=0;
      // Updates the EcmScoreInfo object "esi2" using the given
      // positions of "esi1".

  virtual unsigned int numberOfPosInEsi(EcmScoreInfo& esi)=0;
      // Returns number of positions in the EcmScoreInfo object "esi"
      // (returns size of score vector that is obtained by means of the
      // obtainScrVecFromEsi() function)

  virtual void removeLastPosFromEsi(EcmScoreInfo& esi)=0;
      // Removes last position from the EcmScoreInfo object "esi".
  
      // Destructor
  virtual ~BaseEcmForWg(){};
};

#endif
