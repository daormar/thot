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
/* Module: XRCE_PrePosProcessor2                                    */
/*                                                                  */
/* Prototypes file: XRCE_PrePosProcessor2.h                         */
/*                                                                  */
/* Description: Declares the XRCE_PrePosProcessor2 class,           */
/*              this class implements a pre/pos-processing module   */
/*              for the XRCE corpus (the one used in the TT2 project*/
/*              with an alternative capitalization process)         */
/*                                                                  */
/********************************************************************/

/**
 * @file XRCE_PrePosProcessor2.h
 * 
 * @brief Defines the XRCE_PrePosProcessor2 class, this class implements
 * a pre/pos-processing module for the XRCE corpus (the one used in the
 * TT2 project with an alternative capitalization process)
 */

#ifndef _XRCE_PrePosProcessor2_h
#define _XRCE_PrePosProcessor2_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BasePrePosProcessor.h"
#include <map>
#include <utility>
#include <string>
#include <iostream>
#include "ErrorDefs.h"
#include "StrProcUtils.h"
#include "awkInputStream.h"
#include "preprocess.h"
#include "postprocess.h"
#include <IncrJelMerNgramLM.h>

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- XRCE_PrePosProcessor2 template class

/**
 * @brief The XRCE_PrePosProcessor2 class implements a
 * pre/pos-processing module for the XRCE corpus (the one used in the
 * TT2 project with an alternative capitalization process)
 */

class XRCE_PrePosProcessor2: public BasePrePosProcessor
{
 public:
  
      // Constructor
  XRCE_PrePosProcessor2();

  bool loadCapitInfo(const char* filename);

  std::string preprocLine(std::string str,
                          bool caseconv,
                          bool keepPreprocInfo);
  
  std::string postprocLine(std::string str,
                           bool caseconv);

  bool isCategory(std::string word);

      // Destructor
  ~XRCE_PrePosProcessor2();

 protected:
  
  std::string lastPreprocStr;

      // capitMap stores capitalization options
  std::map<std::string,std::vector<std::string> > capitMap;

      // Language model members
  IncrJelMerNgramLM lmodel;
  std::string languageModelFileName;

  std::string capitalize(std::string str);

  bool loadCapitOptions(const char* filename);
  bool loadLangModelForSmtPreproc(const char* prefixFileName);

  void clear(void);

};
#endif
