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
 

#include "StrProcUtils.h"

namespace StrProcUtils
{
  //---------------
  std::vector<std::string> charItemsToVector(const char *ch)
  {
    unsigned int i=0;
    std::string s;	
    std::vector<std::string> v;
    
    while(ch[i]!=0)
    {
      s=""; 
      while(ch[i]==' ' && ch[i]!=0) {++i;}	  
      while(ch[i]!=' ' && ch[i]!=0) {s=s+ch[i]; ++i;}
      if(s!="") v.push_back(s);  
    }	  
    
    return v;	
  }

  //---------------
  std::vector<std::string> stringToStringVector(std::string s)
  {
    std::vector<std::string> vs;	
    std::string aux;
    unsigned int i=0;	
    bool end=false;
	
    while(!end)
    {
      aux="";	 
      while(s[i]!=' ' && s[i]!='	' && i<s.size()) 
      {
        aux+=s[i];
        ++i;		   
      }
      if(aux!="") vs.push_back(aux);	   
      while((s[i]==' ' || s[i]=='	') && i<s.size()) ++i;	 
      if(i>=s.size()) end=true;	  
    }
    
    return vs;	
  }

  //---------------
  std::string stringVectorToString(std::vector<std::string> svec)
  {
    if(svec.size()==0) return "";
    else
    {
      std::string result;
      
      result=svec[0];
      for(unsigned int i=1;i<svec.size();++i)
      {
        result=result+" "+svec[i];
      }
      return result;
    }
  }

  //---------------
  std::string stringVectorToStringWithoutSpaces(std::vector<std::string> svec)
  {
    if(svec.size()==0) return "";
    else
    {
      std::string result;

      result=svec[0];
      for(unsigned int i=1;i<svec.size();++i)
      {
        result=result+svec[i];
      }
      return result;
    }
  }

  //---------------
  bool isPrefix(std::string str1,
                std::string str2)
  {
        // returns true if string str1 is a prefix of string str2
    if(str1.size()>str2.size()) return false;
    else
    {
      for(unsigned int i=0;i<str1.size();++i)
      {
        if(str1[i]!=str2[i])
          return false;
      }
      return true;
    }
  }

  //---------------
  bool isPrefixStrVec(std::vector<std::string> strVec1,
                      std::vector<std::string> strVec2)
  {
        // returns true if string vector strVec1 is a prefix of string
        // vector strVec2
    if(strVec1.size()>strVec2.size()) return false;

    for(unsigned int i=0;i<strVec1.size();++i)
    {
      if(strVec1[i]!=strVec2[i])
      {
        if(i==strVec1.size()-1)
        {
          if(!StrProcUtils::isPrefix(strVec1[i],strVec2[i]))
            return false;
        }
        else return false;
      }
    }
    return true;
  }

  //---------------
  std::string getLastWord(std::string str)
  {
    if(str.size()==0) return "";
    else
    {
      std::string result;
      unsigned int i;
      
      i=str.size()-1;
      while(i>0 && str[i]==' ') --i;
      while(i>0 && str[i]!=' ') 
      {
        result=str[i]+result;
        --i;
      }
      return result;
    }
  }

  //---------------
  bool lastCharIsBlank(const std::string& str)
  {
    if(str.size()>0)
    {
      if(str[str.size()-1]==' ')
        return true;
      else return false;
    }
    else return false;
  }

  //---------------
  std::string removeLastBlank(std::string str)
  {
    if(str.size()>0)
    {
      if(str[str.size()-1]==' ')
      {
        std::string::iterator strIter=str.end();
        --strIter;
        str.erase(strIter);
      }
      return str;
    }
    else return str;
  }

  //---------------
  std::string addBlank(std::string str)
  {
    str.push_back(' ');
    return str;
  }

  //---------------
  std::vector<float> strVecToFloatVec(std::vector<std::string> strVec)
  {
    std::vector<float> floatVec;

    for(unsigned int i=0;i<strVec.size();++i)
    {
      float value;
      sscanf(strVec[i].c_str(), "%f", &value);
      floatVec.push_back(value);
    }
    
    return floatVec;
  }

  //---------------
  std::string expandLibDirIfFound(std::string soFileName)
  {
        // Store variable name
    std::string libDirVarNameVal=getLibDirVarNameValue();
    
    if(soFileName.size()<libDirVarNameVal.size())
    {
          // soFileName is shorter that libDirVarName, so it does not contain it
      return soFileName;
    }
    else
    {
      if(libDirVarNameVal==soFileName.substr(0,libDirVarNameVal.size()))
      {
            // libDirVarName contained in soFileName
        
            // Expand soFileName
        std::string expandedSoFileName=THOT_LIBDIR;
        expandedSoFileName+=soFileName.substr(libDirVarNameVal.size(),soFileName.size()-libDirVarNameVal.size());
        
        return expandedSoFileName;
      }
      else
      {
            // libDirVarName not contained in soFileName
        return soFileName;
      }
    }
  }

  //---------------
  std::string getLibDirVarNameValue(void)
  {
    std::string libDirVarNameVal="$(";
    libDirVarNameVal+=THOT_LIBDIR_VARNAME;
    libDirVarNameVal+=")";

    return libDirVarNameVal;
  }
  
}
