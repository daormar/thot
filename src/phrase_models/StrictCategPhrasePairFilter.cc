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
 * @file StrictCategPhrasePairFilter.cc
 * 
 * @brief Definitions file for StrictCategPhrasePairFilter.h
 */

//--------------- Include files --------------------------------------

#include "StrictCategPhrasePairFilter.h"

//--------------- Function definitions

//-------------------------
StrictCategPhrasePairFilter::StrictCategPhrasePairFilter(void)
{
  categorySet.insert(DIGIT_CATEG);
  categorySet.insert(NUMBER_CATEG);
  categorySet.insert(ALPHANUM_CATEG);
}

//-------------------------
bool StrictCategPhrasePairFilter::phrasePairIsOk(std::vector<std::string> s_,
                                                 std::vector<std::string> t_)
{
      // Initialize category maps
  std::map<std::string,unsigned int> srcCategMap;
  std::map<std::string,unsigned int> trgCategMap;

  std::set<std::string>::const_iterator categSetIter;
  for(categSetIter=categorySet.begin();categSetIter!=categorySet.end();++categSetIter)
  {
    srcCategMap[*categSetIter]=0;
    trgCategMap[*categSetIter]=0;
  }

      // Collect category information for source
  bool srcContainsCategTags=false;
  for(unsigned int i=0;i<s_.size();++i)
  {
    if(categorySet.find(s_[i])!=categorySet.end())
    {
      srcCategMap[s_[i]]+=1;
      srcContainsCategTags=true;
    }
  }
  
      // Collect category information for target
  bool trgContainsCategTags=false;
  for(unsigned int i=0;i<t_.size();++i)
  {
    if(categorySet.find(t_[i])!=categorySet.end())
    {
      trgCategMap[t_[i]]+=1;
      trgContainsCategTags=true;
    }
  }
  
      // Check phrase pair
  if(srcContainsCategTags || trgContainsCategTags)
  {
        // Phrase pair does contain categories: it is ok only if both
        // phrases are identical
    if(s_==t_)
      return true;
    else
      return false;
  }
  else
  {
        // Phrase pair does not contain categories
    return true;
  }
}

//-------------------------
