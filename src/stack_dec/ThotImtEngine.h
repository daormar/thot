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
/* Module: ThotImtEngine.h                                          */
/*                                                                  */
/* Prototype file: ThotImtEngine.h                                  */
/*                                                                  */
/* Description: thot imt engine.                                    */
/*                                                                  */
/********************************************************************/

#ifndef _ThotImtEngine_h
#define _ThotImtEngine_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "UserNameToUserIdMap.h"
#include <casmacat/IImtEngine.h>
#include "ThotDecoder.h"
#include "ThotImtSession.h"
#include "ThotImtEngine.h"
#include <StrProcUtils.h>
#include <vector>

using namespace casmacat;

//--------------- Classes --------------------------------------------

//--------------------------
class ThotImtEngine: public IInteractiveMtEngine
{
 public:
  
  // Constructor
  ThotImtEngine(UserNameToUserIdMap* _userNameToUserIdMapPtr,
                ThotDecoderUserPars _tdup,
                ThotDecoder* _thotDecoderPtr,
                int _user_id,
                bool _verbose);

  // Destructor
  virtual ~ThotImtEngine(){};

  /* Generate translation for source */
  virtual void translate(const std::vector<std::string> &source,
                         std::vector<std::string> &target);


  /* Update translation models with source/target pair (total or partial translation) */
  virtual void update(const std::vector<std::string> &source,
                      const std::vector<std::string> &target);

  /* Update translation models with source/target pair (total or partial translation) */
  virtual void validate(const vector<string> &source,
                        const vector<string> &target,
                        const vector<bool> &validated);

  // initialize IMT session
  virtual IInteractiveMtSession *newSession(const vector<string> &source);

  // delete IMT session
  virtual void deleteSession(IInteractiveMtSession *session);

 private:
  UserNameToUserIdMap* const userNameToUserIdMapPtr;
  const ThotDecoderUserPars tdup;
  ThotDecoder* const thotDecoderPtr;
  const int user_id;
  const bool verbose;
};
#endif
