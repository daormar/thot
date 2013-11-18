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
/* Module: ThotImtSession                                           */
/*                                                                  */
/* Definitions file: ThotImtSession.cc                              */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "ThotImtSession.h"

//--------------- ThotImtSession class functions

//--------------------------
ThotImtSession::ThotImtSession(ThotDecoder* _thotDecoderPtr,
                               int _user_id,
                               const vector<string> &_source,
                               bool _verbose): thotDecoderPtr(_thotDecoderPtr), user_id (_user_id), source(_source), verbose(_verbose)
{
      // Start IMT session
  std::string sourceStr=StrProcUtils::stringVectorToString(source);
  std::string catResult;
  thotDecoderPtr->startCat(user_id,sourceStr.c_str(),catResult,verbose);
}

//--------------------------
int ThotImtSession::get_uid(void)
{
  return user_id;
}

//--------------------------
void ThotImtSession::setPartialValidation(const vector<string>& partial_translation,
                                          const vector<bool>& validated,
                                          vector<string>& corrected_translation,
                                          vector<bool>& corrected_validated)
{
      // TBD
  corrected_translation = partial_translation;
  corrected_validated = validated;
  for (size_t t = 0; t < corrected_translation.size(); t++)
  {
    // if (not corrected_validated[t] and (rand() / double(RAND_MAX)) > 0.5)
    // {
    //   corrected_translation[t] = random_string_imt();
    // }
  }
}

//--------------------------
void ThotImtSession::setPrefix(const vector<string>& prefix,
                               const vector<string>& suffix,
                               const bool last_token_is_partial,
                               vector<string>& corrected_translation_out)
{
      // Initialize variables
  prevPrefStr=prefix;  
  rejectedWords.clear();
  
      // Invoke setPrefixAux() function
  setPrefixAux(prefix,
               suffix,
               last_token_is_partial,
               rejectedWords,
               corrected_translation_out);
}

//--------------------------
void ThotImtSession::rejectSuffix(const vector<string>& prefix,
                                  const vector<string>& suffix,
                                  const bool last_token_is_partial,
                                  vector<string>& corrected_translation_out)
{
  if(last_token_is_partial)
  {
        // Revise set of rejected words
    if(prevPrefStr!=prefix)
    {
          // The current prefix is different to the previous one, so the
          // set of rejected words has to be cleared
      rejectedWords.clear();
    }
        // Determine accepted prefix and rejected suffix
    std::string accepted_prefix;
    if(!prefix.empty()) accepted_prefix=prefix.back();
    
    std::string rejected_suffix;
    if(!suffix.empty()) rejected_suffix=suffix[0];
    
    rejectedWords.insert(make_pair(accepted_prefix,rejected_suffix));

    // cerr<<"Accepted prefix: "<<accepted_prefix<<" , Rejected suffix: "<<rejected_suffix<<endl;
    
        // Update prevPrefStr variable
    prevPrefStr=prefix;

        // Set current prefix and suffix
    vector<string> currPrefix=prefix;
    if(!currPrefix.empty()) currPrefix.pop_back();
      
    vector<string> currSuffix=suffix;
    if(!currSuffix.empty())
      currSuffix[0]=accepted_prefix+rejected_suffix;

        // Invoke setPrefixAux() function
    setPrefixAux(currPrefix,
                 currSuffix,
                 last_token_is_partial,
                 rejectedWords,
                 corrected_translation_out);
  }
  else
  {
    // Revise set of rejected words
    if(prevPrefStr!=prefix)
    {
          // The current prefix is different to the previous one, so the
          // set of rejected words has to be cleared
      rejectedWords.clear();
    }
        // Determine rejected word
    if(!suffix.empty())
    {
      std::string rejectedWord=suffix[0];
      // cerr<<"New rejected word: "<<rejectedWord<<endl;

          // Add word to the set of rejected words (if the word is not null)
      if(!rejectedWord.empty())
        rejectedWords.insert(make_pair("",rejectedWord));
    }
        
        // Update prevPrefStr variable
    prevPrefStr=prefix;  

        // Invoke setPrefixAux() function
    setPrefixAux(prefix,
                 suffix,
                 last_token_is_partial,
                 rejectedWords,
                 corrected_translation_out);
  }
}

//--------------------------
void ThotImtSession::setPrefixAux(const vector<string>& prefix,
                                  const vector<string>& suffix,
                                  const bool last_token_is_partial,
                                  RejectedWordsSet& rejectedWords,
                                  vector<string>& corrected_translation_out)
{
      // Initialize variables
  std::string prefixStr=StrProcUtils::stringVectorToString(prefix);
  std::string catResult;
  
      // Add blank character to prefix if required
  if(!last_token_is_partial)
    prefixStr=prefixStr+" ";

      // Invoke setPref() function
  thotDecoderPtr->setPref(user_id,prefixStr.c_str(),rejectedWords,catResult,verbose);

      // Set value of corrected_translation_out
  corrected_translation_out=StrProcUtils::stringToStringVector(catResult);
  
  // vector<string> partial_translation(prefix), corrected_translation;
  
  // vector<bool> validated, corrected_validated;
  // validated.resize(prefix.size(), true);
  
  // if (not suffix.empty())
  // {
  //   partial_translation.insert(partial_translation.end(), suffix.begin(), suffix.end());
  //   validated.resize(prefix.size() + suffix.size(), false);
  // }
  // else
  // {
  //   vector<string> simulated_suffix(max(source.size() - prefix.size(), size_t(0)), "<nothing>");
  //   partial_translation.insert(partial_translation.end(), simulated_suffix.begin(), simulated_suffix.end());
  //   validated.resize(prefix.size() + simulated_suffix.size(), false);
  // }

  // setPartialValidation(partial_translation, validated, corrected_translation, corrected_validated);

  // corrected_translation_out.clear();
  // corrected_translation_out.insert(corrected_translation_out.end(), corrected_translation.begin() + prefix.size(), corrected_translation.end());
}
