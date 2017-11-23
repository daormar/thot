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
 * @file StrProcUtils.h
 * @brief Defines string processing utilities
 */

#ifndef _StrProcUtils_h
#define _StrProcUtils_h

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>

namespace StrProcUtils
{
  std::vector<std::string> charItemsToVector(const char *ch);
  std::vector<std::string> stringToStringVector(std::string s);
  std::string stringVectorToString(std::vector<std::string> svec);
  std::string stringVectorToStringWithoutSpaces(std::vector<std::string> svec);
  bool isPrefix(std::string str1,std::string str2);
      // returns true if string str1 is a prefix of string str2
  bool isPrefixStrVec(std::vector<std::string> strVec1,
                      std::vector<std::string> strVec2);
      // returns true if string vector strVec1 is a prefix of string
      // vector strVec2
  std::string getLastWord(std::string str);
      // Remove last word contained in string str
  bool lastCharIsBlank(const std::string& str);
      // Returns true if last char of str is blank
  std::string removeLastBlank(std::string str);
      // Remove last blank character of str if exists
  std::string addBlank(std::string str);
      // Add blank character at the end of str
  std::vector<float> strVecToFloatVec(std::vector<std::string> strVec);
      // Convert string vector into a float vector
  std::string expandLibDirIfFound(std::string soFileName);
  std::string getLibDirVarNameValue(void);
  std::string getLibDir(void);
}

#endif
