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
/* Module: ThotImtEngine                                            */
/*                                                                  */
/* Definitions file: ThotImtEngine.cc                               */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "ThotImtEngine.h"

//--------------- ThotImtEngine class functions

//--------------------------
ThotImtEngine::ThotImtEngine(UserNameToUserIdMap* _userNameToUserIdMapPtr,
                             ThotDecoderUserPars _tdup,
                             ThotDecoder* _thotDecoderPtr,
                             int _user_id,
                             bool _verbose): userNameToUserIdMapPtr(_userNameToUserIdMapPtr), tdup(_tdup), thotDecoderPtr(_thotDecoderPtr), user_id(_user_id), verbose(_verbose)
{
}

//--------------------------
void ThotImtEngine::translate(const std::vector<std::string> &source,
                              std::vector<std::string> &target)
{
      // Initialize variables
  std::string sourceStr=StrProcUtils::stringVectorToString(source);
  std::string targetStr;

      // Obtain translation
  thotDecoderPtr->translateSentence(user_id,
                                    sourceStr.c_str(),
                                    targetStr,
                                    verbose);
  
      // Set value of target
  target=StrProcUtils::stringToStringVector(targetStr);
}


//--------------------------
void ThotImtEngine::update(const std::vector<std::string> &source,
                           const std::vector<std::string> &target)
{
      // Initialize variables
  std::string sourceStr=StrProcUtils::stringVectorToString(source);
  std::string targetStr=StrProcUtils::stringVectorToString(target);

      // Update parameters
  thotDecoderPtr->onlineTrainSentPair(user_id,
                                      sourceStr.c_str(),
                                      targetStr.c_str(),
                                      verbose);
}

//--------------------------
void ThotImtEngine::validate(const vector<string> &source,
                             const vector<string> &target,
                             const vector<bool> &validated)
{
      // TBD
  cout << "store validated sentence '";
  copy(source.begin(), source.end(), ostream_iterator<string>(cout, " "));
  cout << "' as";
  for (size_t t = 0; t < target.size(); t++)
  {
    cout << " " << target[t] << "(" << validated[t] << ")";
  }
  cout << "\n";
}

//--------------------------
IInteractiveMtSession *ThotImtEngine::newSession(const vector<string> &source)
{
      // Obtain new user id
  std::pair<std::string,int> userNameIdPair=userNameToUserIdMapPtr->genNewUserNameIdPair();

  std::cerr<<"Creating new imt session with id "<<userNameIdPair.second<<std::endl;
    
      // Set user parameters in thotDecoder
  int initParsVerbosity=true;
  if(thotDecoderPtr->user_id_new(userNameIdPair.second))
    thotDecoderPtr->initUserPars(userNameIdPair.second,tdup,initParsVerbosity);

      // Return ThotImtSession instance
  return new ThotImtSession(thotDecoderPtr,userNameIdPair.second,source,verbose);
}

//--------------------------
void ThotImtEngine::deleteSession(IInteractiveMtSession *session)
{
      // Print info about session to be deleted to the error output
  ThotImtSession* thotSessionPtr=dynamic_cast<ThotImtSession*>(session);
  if(thotSessionPtr)
  {
    std::cerr<<"Deleting imt session with id "<<thotSessionPtr->get_uid()<<std::endl;
    thotDecoderPtr->release_user_data(thotSessionPtr->get_uid());
  }
  delete session;
}
