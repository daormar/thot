/*
error_correction package for statistical machine translation
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
/* Module: DynClassFileHandler                                      */
/*                                                                  */
/* Prototypes file: DynClassFileHandler.h                           */
/*                                                                  */
/* Description: The DynClassFileHandler class is used to read       */
/*              files with information for dynamic class loading.   */
/*                                                                  */
/********************************************************************/

/**
 * @file DynClassFileHandler.h
 *
 * @brief Declares the DynClassFileHandler class. The
 * DynClassFileHandler class is used to read files with information for
 * dynamic class loading.
 */

#ifndef _DynClassFileHandler_h
#define _DynClassFileHandler_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "awkInputStream.h"
#include "ErrorDefs.h"
#include <algorithm>
#include <map>
#include <string>
#include "myVector.h"

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- DynClassFileHandler class

/**
 * @brief The DynClassFileHandler class is used to read files with
 * information for dynamic class loading.
 */

class DynClassFileHandler
{
 public:

      // Constructor
  DynClassFileHandler(void);

      // Functions to load sentence-wordgraph info
  bool load(std::string _fileName,int verbose=1);

  int getInfoForBaseClass(std::string baseClassName,
                          std::string& soFileName,
                          std::string& initPars);
  
      // clear() function
  void clear(void);

  ~DynClassFileHandler();
  
 protected:
  typedef std::pair<std::string,std::string> DynClassInfo;
  typedef std::map<std::string,DynClassInfo> DynClassInfoMap;

  std::string fileName;
  DynClassInfoMap dynClassInfoMap;
};

#endif
