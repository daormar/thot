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
/* Module: PfsmEcmForWg                                             */
/*                                                                  */
/* Prototypes file: PfsmEcmForWg.h                                  */
/*                                                                  */
/* Description: Declares the PfsmEcmForWg class,                    */
/*              this class specializes the PfsmEcm class for        */
/*              implementing the interface given by the             */
/*              BaseEcmForWg class.                                 */
/*                                                                  */
/********************************************************************/

/**
 * @file PfsmEcmForWg.h
 * 
 * @brief Defines the PfsmEcmForWg class, this class specializes the
 * PfsmEcm class for implementing the interface given by the
 * BaseEcmForWg class.
 */

#ifndef _PfsmEcmForWg_h
#define _PfsmEcmForWg_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <PfsmEcm.h>
#include <BaseEcmForWg.h>
#include <PfsmEcmForWgEsi.h>

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- PfsmEcmForWg template class

/**
 * @brief The PfsmEcmForWg class specializes the PfsmEcm class for
 * implementing the interface given by the BaseEcmForWg class.
 */

class PfsmEcmForWg: public PfsmEcm, public BaseEcmForWg<PfsmEcmForWgEsi>
{
 public:

  typedef BaseEcmForWg<PfsmEcmForWgEsi>::EcmScoreInfo EcmScoreInfo;

  void correctStrGivenPrefWg(std::vector<std::string> uncorrStrVec,
                             std::vector<std::string> prefStrVec,
                             std::vector<std::string>& correctedStrVec);
      // Corrects string 'uncorrStrVec' given the prefix 'prefStrVec'
      // storing the results in 'correctedStrVec'. This is a specialized
      // version for word graphs of the correctStrGivenPref function

  EcmScoreInfo constructInitEsi(void);
      // Constructs an initial esi object
  
  EcmScoreInfo constructEsi(const EcmScoreInfo& prevEsi,
                            const std::string& word);
      // Constructs an esi object given a previous one

  void extendInitialEsi(const std::vector<std::string>& prefixDiffVec,
                        const EcmScoreInfo& prevInitEsi,
                        EcmScoreInfo& newInitEsi);
      // Extends initial ecm score info
  
  void extendEsi(const std::vector<std::string>& prefixDiffVec,
                 const EcmScoreInfo& prevEsi,
                 const std::string& word,
                 EcmScoreInfo& newEsi);
      // Extends ecm score info

      // Functions to extract data from a given esi
  std::vector<Score> obtainScrVecFromEsi(const EcmScoreInfo& esi);
  std::vector<int> obtainLastInsPrefWordVecFromEsi(const EcmScoreInfo& esi);

  void updateEsiPositions(const EcmScoreInfo& esi1,
                          const std::vector<unsigned int>& posVec,
                          EcmScoreInfo& esi2);
      // Updates the EcmScoreInfo object "esi2" using the given
      // positions of "esi1".

  unsigned int numberOfPosInEsi(EcmScoreInfo& esi);
      // Return number of positions in the EcmScoreInfo object "esi"
      // (returns size of score vector that is obtained by means of the
      // obtainScrVecFromEsi() function)

  void removeLastPosFromEsi(EcmScoreInfo& esi);
      // Removes last position from the EcmScoreInfo object "esi".

      // Destructor
  ~PfsmEcmForWg();

 protected:

};
#endif
