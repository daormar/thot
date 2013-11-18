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
/* Module: XRCE_PrePosProcessor1                                    */
/*                                                                  */
/* Prototypes file: XRCE_PrePosProcessor1.h                         */
/*                                                                  */
/* Description: Declares the XRCE_PrePosProcessor1 class,           */
/*              this class implements a pre/pos-processing module   */
/*              for the XRCE corpus (the one used in the TT2        */
/*              project)                                            */
/*                                                                  */
/********************************************************************/

/**
 * @file XRCE_PrePosProcessor1.h
 * 
 * @brief Defines the XRCE_PrePosProcessor1 class, this class implements
 * a pre/pos-processing module for the XRCE corpus (the one used in the
 * TT2 project)
 */

#ifndef _XRCE_PrePosProcessor1_h
#define _XRCE_PrePosProcessor1_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BasePrePosProcessor.h"
#include <iostream>
#include <fstream>
#include "ErrorDefs.h"
#include "preprocess.h"
#include "postprocess.h"

using namespace std;

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- XRCE_PrePosProcessor1 template class

/**
 * @brief The XRCE_PrePosProcessor1 class implements a
 * pre/pos-processing module for the XRCE corpus (the one used in the
 * TT2 project)
 */

class XRCE_PrePosProcessor1: public BasePrePosProcessor
{
 public:
  
      // Constructor
  XRCE_PrePosProcessor1();

  bool loadCapitInfo(const char* filename);

  std::string preprocLine(std::string str,
                          bool caseconv,
                          bool keepPreprocInfo);
  
  std::string postprocLine(std::string str,
                           bool caseconv);

  bool isCategory(std::string word);

      // Destructor
  ~XRCE_PrePosProcessor1();

 private:
  
  std::string lastPreprocStrKept;
};
#endif
