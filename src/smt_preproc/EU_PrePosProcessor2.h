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
 * @file EU_PrePosProcessor2.h
 * 
 * @brief Defines the EU_PrePosProcessor2 class, this class implements a
 * pre/pos-processing module for the EU corpus (the one used in the TT2
 * project plus an extra tokenization step)
 */

#ifndef _EU_PrePosProcessor2_h
#define _EU_PrePosProcessor2_h

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

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- EU_PrePosProcessor2 template class

/**
 * @brief The EU_PrePosProcessor2 class implements a pre/pos-processing
 * module for the EU corpus (the one used in the TT2 project plus an
 * extra tokenization step)
 */

class EU_PrePosProcessor2: public BasePrePosProcessor
{
 public:
  
      // Constructor
  EU_PrePosProcessor2();

  bool loadCapitInfo(const char* filename);

  std::string preprocLine(std::string str,
                          bool caseconv,
                          bool keepPreprocInfo);
  
  std::string postprocLine(std::string str,
                           bool caseconv);

  bool isCategory(std::string word);

      // Destructor
  ~EU_PrePosProcessor2();

 private:
  
  std::string lastPreprocStrKept;
};
#endif
