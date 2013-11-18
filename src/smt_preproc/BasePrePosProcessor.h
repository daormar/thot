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
/* Module: BasePrePosProcessor                                      */
/*                                                                  */
/* Prototypes file: BasePrePosProcessor.h                           */
/*                                                                  */
/* Description: Declares the BasePrePosProcessor class,             */
/*              this class implements the interfaz of               */
/*              pre/pos-processing modules                          */
/*                                                                  */
/********************************************************************/

/**
 * @file BasePrePosProcessor.h
 * 
 * @brief Defines the BasePrePosProcessor class, this class implements
 * the interfaz of pre/pos-processing modules
 */

#ifndef _BasePrePosProcessor_h
#define _BasePrePosProcessor_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <string>
#include <SmtPreprocDefs.h>

using namespace std;

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- BasePrePosProcessor template class

/**
 * @brief The BasePrePosProcessor class implements the interface of
 * pre/pos-processing modules
 */

class BasePrePosProcessor
{
 public:
  
  virtual bool loadCapitInfo(const char* filename)=0;

  virtual std::string preprocLine(std::string str,
                                  bool caseconv,
                                  bool keepPreprocInfo)=0;
  
  virtual std::string postprocLine(std::string str,
                                   bool caseconv)=0;

  virtual bool isCategory(std::string word)=0;
  
      // Destructor
  virtual ~BasePrePosProcessor(){};
};
#endif
