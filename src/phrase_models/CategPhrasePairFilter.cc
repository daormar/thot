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
/* Module: CategPhrasePairFilter                                    */
/*                                                                  */
/* Definitions file: CategPhrasePairFilter.cc                       */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "CategPhrasePairFilter.h"

//--------------- Function definitions

//-------------------------
CategPhrasePairFilter::CategPhrasePairFilter(void)
{
  categorySet.insert("<digit>");
  categorySet.insert("<alfanum>");
  categorySet.insert("<number>");
}

//-------------------------
bool CategPhrasePairFilter::phrasePairIsOk(Vector<std::string> s_,
                                           Vector<std::string> t_)
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
  for(unsigned int i=0;i<s_.size();++i)
  {
    if(categorySet.find(s_[i])!=categorySet.end())
    {
      srcCategMap[s_[i]]+=1;
    }
  }
  
      // Collect category information for target
  for(unsigned int i=0;i<t_.size();++i)
  {
    if(categorySet.find(t_[i])!=categorySet.end())
    {
      trgCategMap[t_[i]]+=1;
    }
  }

      // Check if phrase pair is OK
  std::map<std::string,unsigned int>::const_iterator srciter;
  for(srciter=srcCategMap.begin();srciter!=srcCategMap.end();++srciter)
  {
    std::string src_categ=srciter->first;
    std::map<std::string,unsigned int>::const_iterator trgiter=trgCategMap.find(src_categ);

    if(trgiter==trgCategMap.end())
      return false;
    else
    {
      if(srciter->second!=trgiter->second)
        return false;
    }
  }
  
  return true;
}

//-------------------------
