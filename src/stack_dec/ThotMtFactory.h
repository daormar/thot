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
/* Module: ThotMtFactory.h                                          */
/*                                                                  */
/* Prototype file: ThotMtFactory.h                                  */
/*                                                                  */
/* Description: thot mt factory.                                    */
/*                                                                  */
/********************************************************************/

#ifndef _ThotMtFactory_h
#define _ThotMtFactory_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "UserNameToUserIdMap.h"
#include "ThotDecoder.h"
#include "ThotMtEngine.h"
#include "ThotMtFactoryInitPars.h"

using namespace casmacat;

//--------------- Classes --------------------------------------------

class ThotMtFactory: public IMtFactory
{
 public:

  // Constructor
  ThotMtFactory(){}
  
  // do not forget to free all allocated resources
  // otherwise define the destructor with an empty body
  virtual ~ThotMtFactory(){}

  // initialize the MT engine with main-like parameters
  virtual int init(int argc,
                   char *argv[],
                   Context *context = 0);

  // Function to return package version
  virtual string getVersion()
  {
    return THOT_PACKAGE_VERSION;
  }

  // Create mt engine instance
  virtual IMtEngine* createInstance(const std::string &specialization_id = "");

  // Delete mt engine instance
  virtual void deleteInstance(IMtEngine* instancePtr);

 private:
  ThotMtFactoryInitPars pars;
  UserNameToUserIdMap userNameToUserIdMap;
  ThotDecoderUserPars tdup;
  ThotDecoder thotDecoder;

      // Auxiliary functions
  int takeInitPars(int argc,
                   char *argv[],
                   ThotMtFactoryInitPars& pars);
  int checkPars(const ThotMtFactoryInitPars& pars);
};
#endif
