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
/* Module: ThotMtEngine                                             */
/*                                                                  */
/* Definitions file: ThotMtEngine.cc                                */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "ThotMtEngine.h"

//--------------- ThotMtEngine class functions

//--------------------------
ThotMtEngine::ThotMtEngine(ThotDecoder* _thotDecoderPtr,
                           int _user_id,
                           bool _verbose): thotDecoderPtr(_thotDecoderPtr), user_id(_user_id), verbose(_verbose)
{
}

//--------------------------
void ThotMtEngine::translate(const std::vector<std::string> &source,
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
void ThotMtEngine::update(const std::vector<std::string> &source,
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
