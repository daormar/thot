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
/* Module: ThotImtSession.h                                         */
/*                                                                  */
/* Prototype file: ThotImtEngine.h                                  */
/*                                                                  */
/* Description: thot imt session.                                   */
/*                                                                  */
/********************************************************************/

#ifndef _ThotImtSession_h
#define _ThotImtSession_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "ThotDecoder.h"
#include <casmacat/IImtEngine.h>
#include <StrProcUtils.h>
#include <vector>

using namespace std;
using namespace casmacat;

//--------------- Classes --------------------------------------------

//--------------------------
class ThotImtSession: public IInteractiveMtSession
{
 public:

  // Constructor
  //ThotImtSession();
  ThotImtSession(ThotDecoder* _thotDecoderPtr,
                 int _user_id,
                 const vector<string>& _source,
                 bool verbose);

  // Destructor
  virtual ~ThotImtSession(){};
  
  // Return user id
  int get_uid(void);

  /* Set partial validation of a translation */
  virtual void setPartialValidation(const vector<string>& partial_translation,
                                    const vector<bool>& validated,
                                    vector<string>& corrected_translation,
                                    vector<bool>& corrected_validated);
  
  /* Set prefix of a translation */
  virtual void setPrefix(const vector<string>& prefix,
                         const vector<string>& suffix,
                         const bool last_token_is_partial,
                         vector<string>& corrected_translation_out);

  /* Reject suffix of a translation */
  virtual void rejectSuffix(const std::vector<std::string> &prefix,
                            const std::vector<std::string> &suffix,
                            const bool last_token_is_partial,
                            std::vector<std::string> &corrected_translation_out);

 private:
  ThotDecoder* const thotDecoderPtr;
  const int user_id;
  const vector<string> source;
  const bool verbose;
  vector<string> prevPrefStr;
  RejectedWordsSet rejectedWords;

      // Auxiliary functions
  void setPrefixAux(const vector<string>& prefix,
                    const vector<string>& suffix,
                    const bool last_token_is_partial,
                    RejectedWordsSet& rejectedWords,
                    vector<string>& corrected_translation_out);
};
#endif
