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
/* Module: _editDistBasedEcm                                        */
/*                                                                  */
/* Prototypes file: _editDistBasedEcm.h                             */
/*                                                                  */
/* Description: Declares the _editDistBasedEcm class,               */
/*              this class is a base class for error correcting     */
/*              models based on edit distance.                      */
/*                                                                  */
/********************************************************************/

/**
 * @file _editDistBasedEcm.h
 * 
 * @brief Defines the _editDistBasedEcm class, this class is a base
 * class for error correcting models based on edit distance
 */

#ifndef __editDistBasedEcm_h
#define __editDistBasedEcm_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseErrorCorrectionModel.h"
#include "EditDistForVecString.h"
#include "WordAndCharLevelOps.h"

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- _editDistBasedEcm template class

/**
 * @brief The _editDistBasedEcm class is a base class for error
 * correcting models based on edit distance
 */

class _editDistBasedEcm: public BaseErrorCorrectionModel
{    
 protected:

  void correctStrGivenPrefOps(WordAndCharLevelOps wordCharOpsForSegm,
                              Vector<std::string> uncorrStrVec,
                              Vector<std::string> prefStrVec,
                              Vector<std::string>& correctedStrVec);
  void correctWordGivenPrefOps(Vector<unsigned int> charOpsForWord,
                               std::string word,
                               std::string pref,
                               std::string& correctedWord);

};
#endif
