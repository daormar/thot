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
/* Module: ThotImtFactory.h                                         */
/*                                                                  */
/* Prototype file: ThotImtFactory.h                                 */
/*                                                                  */
/* Description: thot imt factory.                                   */
/*                                                                  */
/********************************************************************/

#ifndef _ThotImtFactory_h
#define _ThotImtFactory_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "UserNameToUserIdMap.h"
#include "ThotDecoder.h"
#include "ThotImtEngine.h"
#include "ThotImtFactoryInitPars.h"

using namespace casmacat;

//--------------- Classes --------------------------------------------

class ThotImtFactory: public IInteractiveMtFactory
{
 public:

  // Constructor
  ThotImtFactory(){}
  
  // do not forget to free all allocated resources
  // otherwise define the destructor with an empty body
  virtual ~ThotImtFactory(){}

  // initialize the IMT engine with main-like parameters
  virtual int init(int argc,
                   char *argv[],
                   Context *context = 0);
  
  // Function to return package version
  virtual string getVersion()
  {
    return THOT_PACKAGE_VERSION;
  }

  // Create imt engine instance
  virtual IInteractiveMtEngine* createInstance(const std::string &specialization_id = "");

  // Delete imt engine instance
  virtual void deleteInstance(IInteractiveMtEngine* instancePtr);

 private:
  ThotImtFactoryInitPars pars;
  UserNameToUserIdMap userNameToUserIdMap;
  ThotDecoderUserPars tdup;
  ThotDecoder thotDecoder;

      // Auxiliary functions
  int takeInitPars(int argc,
                   char *argv[],
                   ThotImtFactoryInitPars& pars);
  int checkPars(const ThotImtFactoryInitPars& pars);
};
#endif
