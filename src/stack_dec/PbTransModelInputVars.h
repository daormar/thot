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

/**
 * @file PbTransModelInputVars.h
 * 
 * @brief Class to store input variables for phrase-based translation
 * model classes.
 */

#ifndef _PbTransModelInputVars_h
#define _PbTransModelInputVars_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "WordIndex.h"
#include <string>
#include <vector>

//--------------- Classes --------------------------------------------

class PbTransModelInputVars
{
 public:
      // Variables to store the source sentence during each translation
      // process
  std::vector<std::string> srcSentVec;
  std::vector<WordIndex> srcSentIdVec;
  std::vector<WordIndex> nsrcSentIdVec;

      // Variables to store the reference sentence during each
      // translation process
  std::vector<std::string> refSentVec;
  std::vector<WordIndex> nrefSentIdVec;
    
      // Variables to store the prefix sentence during each translation
      // process
  bool lastCharOfPrefIsBlank;
  std::vector<std::string> prefSentVec;
  std::vector<WordIndex> nprefSentIdVec;

      // Function to clear variables
  void clear(void)
  {
    srcSentVec.clear();
    srcSentIdVec.clear();
    nsrcSentIdVec.clear();
    refSentVec.clear();
    nrefSentIdVec.clear();
    lastCharOfPrefIsBlank=false;
    prefSentVec.clear();
    nprefSentIdVec.clear();
  };
};

#endif
